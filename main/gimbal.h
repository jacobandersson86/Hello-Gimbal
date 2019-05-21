#ifndef GIMBAL_H
#define GIMBAL_H

#include "driver/adc.h"
#include <stdbool.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    adc_channel_t x_pin_adc;
    adc_channel_t y_pin_adc;
    TaskHandle_t handle;
    bool printflag; // Will print in each sampling itteration. One can use print_gimbal in the main loop and print more slowly, approx 250 ms delay. 
    int16_t period_ms;
} gimbal_struct;

void init_gimbal(gimbal_struct* data);
void print_gimbal(gimbal_struct* data);
void sample_gimbal(gimbal_struct* data);
void send_gimbal(gimbal_struct* data);
#endif //GIMBAL_H