#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f4xx.h"

void PWM_TIM1_Init(void);

void PWM_SetServo1(uint16_t Compare);

void PWM_SetServo2(uint16_t Compare);

void Servo1_SetAngle(float Angle);

void Servo2_SetAngle(float Angle);

#endif
