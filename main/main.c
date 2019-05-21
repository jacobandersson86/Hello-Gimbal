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

#define X_PIN_ADC ADC1_CHANNEL_4 //GPIO32 == channel 4
#define Y_PIN_ADC ADC1_CHANNEL_5 //GPIO33 == channel 5

void app_main()
{
    printf("Hello Gimbal\n");

    gimbal_struct data = {
        .x = 0,
        .y = 0,
        .x_pin_adc = X_PIN_ADC,
        .y_pin_adc = Y_PIN_ADC,
        .printflag = false,
        .handle = NULL
    };

    init_gimbal(&data);
    start_wifi();
    connect_socket();


    while(1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
