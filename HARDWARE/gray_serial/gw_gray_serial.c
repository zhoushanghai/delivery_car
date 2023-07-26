/*
 * @Author: hz 1281989921@qq.com
 * @Date: 2023-07-25 11:24:14
 * @LastEditors: hz 1281989921@qq.com
 * @LastEditTime: 2023-07-26 15:42:25
 * @FilePath: \USERe:\桌面\送药小车（v1.2 优化了一下key\HARDWARE\gray_serial\gw_gray_serial.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "gw_gray_serial.h"
#include "delay.h"
#include "usart.h"

#define GW_GRAY_SERIAL_GPIO_GROUP GPIOB
#define GW_GRAY_SERIAL_GPIO_CLK GPIO_Pin_6
#define GW_GRAY_SERIAL_GPIO_DAT GPIO_Pin_7

static uint8_t gw_gray_serial_read(GPIO_TypeDef *GPIOx, uint16_t gpio_clk, uint16_t gpio_dat);
//(‵▽′)///////////////////////////////////// 灰度数字数据处理模块 ////////////////////////////////////////////
void get_gray_data(void)
{
	uint8_t grayData;
	grayData = gw_gray_serial_read(GW_GRAY_SERIAL_GPIO_GROUP, GW_GRAY_SERIAL_GPIO_CLK, GW_GRAY_SERIAL_GPIO_DAT);
	// printf("%d\r\n", grayData);
	car.GrayVal = 0;
	for (int i = 0; i < 8; i++)
	{
		if (GET_NTH_BIT(grayData, i + 1))
		{
			car.GrayscaleData[i] = 1;
		}
		else
		{
			car.GrayscaleData[i] = 0;
		}

		if (i < 4)
		{
			car.GrayVal += car.GrayscaleData[i] * (4 - i);
		}
		else
		{
			car.GrayVal -= car.GrayscaleData[i] * (i - 3);
		}
	}
}

//(‵▽′)///////////////////////////////////// 初始化 ////////////////////////////////////////////
/*
 * 初始化gpio
 */
void spl_gw_gray_serial_init(void)
{
	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GW_GRAY_SERIAL_GPIO_DAT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GW_GRAY_SERIAL_GPIO_GROUP, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GW_GRAY_SERIAL_GPIO_CLK;
	GPIO_Init(GW_GRAY_SERIAL_GPIO_GROUP, &GPIO_InitStructure);
}
//(‵▽′)///////////////////////////////////// 底层函数 ////////////////////////////////////////////

static uint8_t gw_gray_serial_read(GPIO_TypeDef *GPIOx, uint16_t gpio_clk, uint16_t gpio_dat)
{
	uint8_t ret = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i)
	{
		/* 输出时钟下降沿 */
		GPIO_ResetBits(GPIOx, gpio_clk);

		delay_us(5); // 有外部上拉源(4k ~ 10k电阻) 可不加此行

		ret |= GPIO_ReadInputDataBit(GPIOx, gpio_dat) << i;

		/* 输出时钟上升沿,让传感器更新数据*/
		GPIO_SetBits(GPIOx, gpio_clk);

		/* 延迟需要在5us左右 */
		delay_us(5);
	}

	return ret;
}
