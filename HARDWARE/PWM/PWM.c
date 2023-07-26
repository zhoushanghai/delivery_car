#include "PWM.h"
#include "stm32f4xx.h"

void PWM_TIM5_Init(uint16_t arr, uint16_t psc)
{
    /*初始化结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*引脚配置 PA2&PA3*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*引脚重映射*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);

    /*初始化定时器TIM5*/
    TIM_DeInit(TIM5);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = arr;    // arr
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc; // psc
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);
    TIM_CtrlPWMOutputs(TIM5, ENABLE);

    /*配置定时器TIM5为OC_PWM (oc3&oc4)*/
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC3Init(TIM5, &TIM_OCInitStructure);
    TIM_OC4Init(TIM5, &TIM_OCInitStructure);

    TIM_ARRPreloadConfig(TIM5, ENABLE); // ARR影子寄存器配置

    TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable); // 输出比较影子寄存器配置
    TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);

    /*开启定时器TIM5*/
    TIM_Cmd(TIM5, ENABLE);
}
