#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"

#define Lin1 PBout(12)
#define Lin2 PBout(13)

#define Rin1 PBout(14)
#define Rin2 PBout(15)

void Motor_GPIO_Init(void);
void Motor_PWM_Limit(int *motoL, int *motoR);
void Motor_PWM_Load(int moto1, int moto2);

#endif

