/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "gimbal.h"
#include "tcp.h"

void app_main()
{
    printf("Hello Gimbal\n");
    init_gimbal();
    start_wifi();
    //connect_socket();

    gimbal_struct data;
    while(1)
    {
        sample_gimbal(&data);
        //print_gimbal(&data);
        send_gimbal(&data);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
