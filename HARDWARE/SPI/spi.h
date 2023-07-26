#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

#define LCD_CS_Clr() 	GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define LCD_CS_Set()	GPIO_SetBits(GPIOB, GPIO_Pin_3)

void SPI2_Init(void);			 //初始化SPI2口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI2速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI2总线读写一个字节

void SPI1_Init(void);   //初始化SPI1口

void LCD_Writ_Bus(u8 dat);
		 
#endif

