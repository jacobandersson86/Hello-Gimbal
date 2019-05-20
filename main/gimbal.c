#include "driver/adc.h"
#include "driver/gpio.h"
#include "gimbal.h"

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

#define POS_P 2.0



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