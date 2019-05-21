#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "gimbal.h"


#define HIGHEST_X 3371.0
#define LOWEST_X 503.0
#define HIGHEST_Y 3165.0
#define LOWEST_Y 193.0

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

#define POS_P 2.0

void vGimbalTask (void *pvParameters){
    gimbal_struct *data = (gimbal_struct*) pvParameters;

    for ( ;; ){
        sample_gimbal(data);
        if(data->printflag)
            print_gimbal(data);
        send_gimbal(data);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}


void init_gimbal(gimbal_struct* data){
    //Configurate ADC
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_12Bit));
    ESP_ERROR_CHECK(adc1_config_channel_atten(data->x_pin_adc, ADC_ATTEN_11db)); //Full scale to 3.9V
    ESP_ERROR_CHECK(adc1_config_channel_atten(data->y_pin_adc, ADC_ATTEN_11db)); //Full scale to 3.9V

    //Start task
    BaseType_t err = xTaskCreate ( vGimbalTask, "Gimbal Task", 4096, data, tskIDLE_PRIORITY + 1, &data->handle);
    if (err != pdPASS){
        ESP_LOGE(__func__,"Gimbal Task initiation error. ");
    }
    configASSERT(data->handle);

}

void sample_gimbal(gimbal_struct* data){
    data->x = adc1_get_raw(data->x_pin_adc);
    data->y = adc1_get_raw(data->y_pin_adc);
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

void send_gimbal(gimbal_struct* data){
    static float pos = 0.0;
    float diff;
    static int send = 1;
    if(data->x < 1835){
        diff = ((1835 - data->x)/(1835 - LOWEST_X)) * -POS_P;
        send = 1;
    }
    else if(data->x >1885){
        diff = ((data->x - 1885)/(HIGHEST_X - 1885)) * POS_P;
        send = 1;
    }
    else{
        diff = 0.0;
    }

    if(( diff != 0.00 )||( send == 1 )){
        pos += diff;
        printf("\rPos: %7.2f\t Diff: %5.2f", pos, diff);
        fflush(stdout);
    }
    else{
        send = 0;
    }
        

}