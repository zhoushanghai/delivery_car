#include "Motor.h"
#include "stm32f4xx.h"  

/**
 * @brief 电机初始化函数
 * 
 */
void Motor_GPIO_Init(void)
{
    /*初始化结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /*引脚配置 PB12&PB13&PB14&PB15*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief 载入电机的PWM的限幅函数
 *
 * @param motoL 载入左电机的的PWM_CCR
 * @param motoR 载入右电机的PWM_CCR
 */
int PWM_MAX = 9000, PWM_MIN = -7200; // PWM_CCR限幅变量
void Motor_PWM_Limit(int *motoL, int *motoR)
{
    if (*motoL > PWM_MAX)
    {
        *motoL = PWM_MAX;
    }
    if (*motoL < PWM_MIN)
    {
        *motoL = PWM_MIN;
    }

    if (*motoR > PWM_MAX)
    {
        *motoR = PWM_MAX;
    }
    if (*motoR < PWM_MIN)
    {
        *motoR = PWM_MIN;
    }
}

/**
 * @brief 取绝对值函数
 * @param 要取绝对值的数
 * @retval 取绝对值后的数
 */
int GFP_abs(int p) // 绝对值函数
{
    int q;
    q = p > 0 ? p : (-p); 
    return q;
}

/**
 * @brief PWM_CCR载入电机函数 // 入口参数：PID运算完成的最终PWM值
 * @param  moto1,moto2,左电机PWM_CCR赋值，右电机PWM_CCR赋值
 * @retval 无
 */
void Motor_PWM_Load(int moto1, int moto2)
{
    if (moto1 > 0) // 让左电机正转
    {
        Lin1 = 1, Lin2 = 0;
    }
    else // 让左电机反转
    {
        Lin1 = 0, Lin2 = 1;
    }                                    
    TIM_SetCompare3(TIM5, GFP_abs(moto1)); // 因为此函数只能写正数，所以要取绝对值

    if (moto2 > 0) // 让右电机正转
    {
        Rin1 = 1, Rin2 = 0;
    }
    else // 让左电机反转
    {
        Rin1 = 0, Rin2 = 1;
    }
    TIM_SetCompare4(TIM5, GFP_abs(moto2)); // 因为此函数只能写正数，所以要取绝对值
}
