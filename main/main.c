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
#include "driver/adc.h"
#include "driver/gpio.h"

#define X_PIN GPIO_NUM_32
#define Y_PIN GPIO_NUM_33

#define X_PIN_ADC ADC1_CHANNEL_4 //GPIO32 == channel 4
#define Y_PIN_ADC ADC1_CHANNEL_5 //GPIO33 == channel 5

#define HIGHEST_X 3371.0
#define LOWEST_X 503.0
#define HIGHEST_Y 3165.0
#define LOWEST_Y 193.0

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

typedef struct {
    uint16_t x;
    uint16_t y;
} gimbal_struct;

void init_gimbal(){
    /*
    // Configurate inputs
    gpio_config_t conf;
    conf.intr_type = GPIO_INTR_DISABLE;
    conf.mode = GPIO_MODE_INPUT;
    conf.pin_bit_mask = 1ULL << X_PIN;
    conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&conf);

    conf.pin_bit_mask = 1ULL << Y_PIN;
    gpio_config(&conf);
    */

    //Configurate ADC
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(X_PIN_ADC, ADC_ATTEN_11db); //Full scale to 3.9V
    adc1_config_channel_atten(Y_PIN_ADC, ADC_ATTEN_11db); //Full scale to 3.9V
}

void sample_gimbal(gimbal_struct* data){
    data->x = adc1_get_raw(X_PIN_ADC);
    data->y = adc1_get_raw(Y_PIN_ADC);
}

void printBar (char* prefix,double percentage)
{
    float val = (float) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("%s: %5.2f%% [%.*s%*s]\t",prefix, val, lpad, PBSTR, rpad, "");
}

void printReturn()
{
    printf("\r");
}

void printFlush(){
    fflush (stdout);
}

double invertProcent(double procentage){
    return 1.0 - procentage;
}

void print_gimbal(gimbal_struct* data){
    double x_procent = (float) (data->x - LOWEST_X) / (HIGHEST_X-LOWEST_X);
    double y_procent = (float) (data->y - LOWEST_Y) / (HIGHEST_Y-LOWEST_Y);

    y_procent = invertProcent(y_procent);

    printReturn();
    printBar ("X", x_procent);
    printBar ("Y", y_procent);
    printFlush();
}

void app_main()
{
    printf("Hello Gimbal\n");
    init_gimbal();
    gimbal_struct data;
    while(1)
    {
        sample_gimbal(&data);
        print_gimbal(&data);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}
