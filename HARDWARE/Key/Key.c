#include "Delay.h"
#include "stm32f4xx.h" // Device header
#include "BEEPER.h"
#include "key.h"
#include "led.h"

static int scanner_period = 10;				 // 按键的扫描周期
static int key_press_time[KEY_NUMBER] = {0}; // 按键信号持续时长
static key_state_enum key_state[KEY_NUMBER]; // 按键状态
//(‵▽′)///////////////////////////////////// 底层代码 ////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键初始化
// 参数说明     period          按键扫描周期 以毫秒为单位
// 返回参数     void
// 使用示例     key_init(10);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void key_init(int period)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	scanner_period = period;
}

void get_key_gpio(char *keyVal)
{
	keyVal[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
}
//(‵▽′)///////////////////////////////////// 应用层 //////////////////////////////////////////////
/*下面是格式*/

// void key_(void)
// {
// 	key_scanner();
// 	for (int i = 0; i < KEY_NUMBER; i++)
// 	{
// 		key[i].state = key_get_state(i);
// 		if (key[i].state == KEY_SHORT_PRESS)
// 		{
// 			static uint8_t go = 1;

// 			if (go++ % 2)
// 				Motor_PWM_Load(9000, -9000);
// 			// car.speed_set = 0.5;
// 			else
// 				Motor_PWM_Load(0, 0);
// 			// car.speed_set = 0;

// 			printf("key%d short press%d\r\n", i, go);
// 		}
// 		else if (key[i].state == KEY_LONG_PRESS)
// 		{

// 			if (key[i].lastState != KEY_LONG_PRESS)
// 			{
// 				printf("key%d long press\r\n", i);
// 				// flag_G.wave = flag_G.wave ^ 1;
// 			}
// 		}
// 		else if (key[i].state == KEY_RELEASE)
// 		{

// 			if (key[i].lastState != KEY_RELEASE)
// 			{
// 				printf("key%d KEY_RELEASE\r\n", i);
// 			}
// 		}
// 		key[i].lastState = key[i].state;
// 	}
// }

//(‵▽′)///////////////////////////////////// 中间层 ////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键状态扫描
// 参数说明     void
// 返回参数     void
// 使用示例     key_scanner();
// 备注信息     这个函数放在主循环或者 PIT 中断中
//-------------------------------------------------------------------------------------------------------------------
void key_scanner(void)
{
	char i = 0, keyVal[KEY_NUMBER];
	get_key_gpio(keyVal);
	//-----(‵▽′)/---------------------- 扫描按键电平 -----------------------------//
	for (i = 0; i < KEY_NUMBER; i++)
	{
		if (!keyVal[i]) // 按键按下
		{
			key_press_time[i]++;
			// printf("t%d=%d\r\n", keyVal[0], key_press_time[i]);
			if (key_press_time[i] >= KEY_LONG_PRESS_PERIOD / scanner_period)
			{
				key_state[i] = KEY_LONG_PRESS;
			}
		}
		else // 按键释放
		{
			if (key_state[i] != KEY_LONG_PRESS && key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
			{
				key_state[i] = KEY_SHORT_PRESS;
			}
			else
			{
				key_state[i] = KEY_RELEASE;
			}
			key_press_time[i] = 0;
		}

		// printf("key%d=%d\r\n", i, key_state[i]);
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取按键状态
// 参数说明     key_n           按键索引
// 返回参数     key_state_enum  按键状态
// 使用示例     key_get_state(KEY_1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
key_state_enum key_get_state(key_index_enum key_n)
{
	return key_state[key_n];
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     清除指定按键状态
// 参数说明     key_n           按键索引
// 返回参数     void            无
// 使用示例     key_clear_state(KEY_1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void key_clear_state(key_index_enum key_n)
{
	key_state[key_n] = KEY_RELEASE;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     清除所有按键状态
// 参数说明     void            无
// 返回参数     void            无
// 使用示例     key_clear_all_state();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void key_clear_all_state(void)
{
	for (int i = 0; i < KEY_NUMBER; i++)
	{
		key_state[i] = KEY_RELEASE;
	}
}
