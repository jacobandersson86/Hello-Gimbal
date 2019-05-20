#include "esp_log.h"
#include "nvs_flash.h"
#include "string.h"


#include <lwip/sockets.h>
#include "esp_wifi.h"
#include "esp_event_loop.h"


#define PORT 5555
#define DEFAULT_SSID "plejd"
#define DEFAULT_PWD "Plejd247Plus"
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_RSSI -127
#define DEFAULT_AUTHMODE WIFI_AUTH_WPA2_PSK



static int socket_connection = NULL;
static int is_connected = 0;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static unsigned int socklen = sizeof(client_addr);

static SemaphoreHandle_t wifi_connected;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		ESP_LOGI(__func__, "SYSTEM_EVENT_STA_START");
		ESP_ERROR_CHECK(esp_wifi_connect());
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI(__func__, "SYSTEM_EVENT_STA_GOT_IP");
		ESP_LOGI(__func__,
			"Got IP: %s\n",
			ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		xSemaphoreGive(wifi_connected);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		ESP_LOGI(__func__, "SYSTEM_EVENT_STA_DISCONNECTED");
		ESP_ERROR_CHECK(esp_wifi_connect());
		break;
	default:
		break;
	}
	return ESP_OK;
}


void start_wifi() {
	wifi_connected = xSemaphoreCreateBinary();

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	wifi_config_t wifi_config = {
		.sta = {
		.ssid = DEFAULT_SSID,
		.password = DEFAULT_PWD,
		.scan_method = DEFAULT_SCAN_METHOD,
		.sort_method = DEFAULT_SORT_METHOD,
		.threshold.rssi = DEFAULT_RSSI,
		.threshold.authmode = DEFAULT_AUTHMODE,
	},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());
}


esp_err_t connect_socket(){
	// Don't connect socket until WiFi is connected
	xSemaphoreTake(wifi_connected, portMAX_DELAY);

	socket_connection = socket(AF_INET, SOCK_STREAM, 0);
	
	if (socket_connection < 0){
		return ESP_FAIL;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	if(inet_pton(AF_INET, "10.13.6.189", &server_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return ESP_FAIL; 
    } 
	
	if (connect(socket_connection, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    { 
        printf("\nConnection to socket Failed \n"); 
        return ESP_FAIL; 
    } 
	
	is_connected = 1;
	return ESP_OK;
}


esp_err_t send_message(const char * msg, int length)
{	
	if (!is_connected){
		return ESP_FAIL;
	}
	int to_write = length;
	int sent = 0;
	
	int flags = MSG_DONTWAIT;
	
	while (to_write > 0)
	{
		
		sent = send(connect_socket, msg + (length - to_write), to_write, flags);
		if (sent > 0) {
			to_write -= sent;
		}
		else if (sent < 0) {
			// We've had an error...
			printf("Failed to write to socket: %s\n",strerror(errno));
			switch (errno) {
			case EAGAIN: {
					printf("Socket has failed\n");
					close(connect_socket);
					is_connected = 0;
					return ESP_FAIL;
				}
			case ENOTCONN: {
					break;
				}
			case EIO: {
					break;
				}
			case ENETDOWN: {
					break;
				}
			case ECONNRESET: {
					
					return ESP_FAIL;
				}
					
			default:
				break;
			}
			
			return ESP_FAIL;
		}
	}
	return ESP_OK;
}
