#include "Encoder.h"
#include "stm32f4xx.h"

/**
 * @brief 为左编码器配置定时器TIM2
 *
 */
void Encoder_TIM2_Init(void)
{
    /*初始化结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*引脚配置 PA0&PA1*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*引脚重映射*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

    /*初始化定时器 TIM2*/
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65535;    // arr
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1; // psc
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /*配置定时器输入捕获*/
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10; // 输入捕获滤波器
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    /*配置编码器*/
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_SetCounter(TIM2, 0); // 把CNT置0

    /*开启定时器TIM2*/
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief 为右编码器配置定时器TIM3
 *
 */
void Encoder_TIM3_Init(void)
{
    /*初始化结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /*引脚配置 PB4&PB5*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 因为编码器输入数据，所以选择浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*引脚重映射*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

    /*初始化定时器 TIM3*/
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65535;    // arr
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1; // psc
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    /*配置定时器输入捕获*/
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10; // 输入捕获滤波器
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /*配置编码器*/
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    /*开启定时器TIM3*/
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief 编码器读取电机速度函数
 *
 * @param TIMx 定时器x (2or3)
 * @return int 定时器x 的CNT读数
 */
int Encoder_Read_Speed(int TIMx)
{
    int cnt_value; // 返回一段时间内，定时器的计数值

    // 返回计数值并清零定时器计数值
    switch (TIMx)
    {
    case 2: // 左编码器
        cnt_value = (short)TIM_GetCounter(TIM2);
        TIM_SetCounter(TIM2, 0);
        break;
    case 3: // 右编码器
        cnt_value = (short)TIM_GetCounter(TIM3);
        TIM_SetCounter(TIM3, 0);
        break;
    default:
        cnt_value = 0;
    }
    return cnt_value;
}

