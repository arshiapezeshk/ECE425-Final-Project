#include "TM4C123GH6PM.h"

void Motor_Init(uint16_t period_constant, uint16_t duty_cycle);

void Motor_Forward(uint16_t duty_cycle);

void Motor_Backward(uint16_t duty_cycle);

void Motor_Right_Turn(uint16_t duty_cycle);

void Motor_Left_Turn(uint16_t duty_cycle);

void Motor_Stop();