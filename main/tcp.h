#ifndef TCP_H
#define TCP_H

void start_wifi();
esp_err_t connect_socket();
esp_err_t send_message(const char * msg, int length);

#endif