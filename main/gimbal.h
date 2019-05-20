#ifndef GIMBAL_H
#define GIMBAL_H

typedef struct {
    uint16_t x;
    uint16_t y;
} gimbal_struct;

void init_gimbal();
void print_gimbal(gimbal_struct* data);
void sample_gimbal(gimbal_struct* data);
void send_gimbal(gimbal_struct* data);
#endif //GIMBAL_H