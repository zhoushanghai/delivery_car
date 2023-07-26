#ifndef __ENCODER_H
#define __ENCODER_H
void Encoder_TIM2_Init(void);

void Encoder_TIM3_Init(void);

int Encoder_Read_Speed(int TIMx);

#endif
