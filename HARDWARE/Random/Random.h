#ifndef __RANDOM_H__
#define __RANDOM_H__

void TIM4_Random_Init(uint16_t ARR, uint16_t PSC);
void TIM7_Random_Init(uint16_t ARR, uint16_t PSC);
uint32_t RandomCreate(void);

uint32_t RandomCreateAgain(uint16_t Number);
#endif


