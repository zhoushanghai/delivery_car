#include "mpuiic.h"
#include "delay.h"

extern u8 mpu_sel;
/**************************************************************************
 作  者 ：大鱼电子
 淘宝地址：https://shop119207236.taobao.com
 微信公众号【大鱼机器人】
 后台回复【平衡小车】：获取平衡小车全套DIY资料
 后台回复【电子开发工具】：获取电子工程师必备开发工具
 后台回复【电子设计资料】：获取电子设计资料包
 知乎：张巧龙
**************************************************************************/
// MPU IIC 延时函数
void MPU_IIC_Delay(void)
{
	delay_us(2);
}

// 初始化IIC
void MPU_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);										  // 使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;											  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;											  // 50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);														  // 根据设定参数初始化GPIOB
}
// 产生IIC起始信号
void MPU_IIC_Start(void)
{
	if (mpu_sel)
	{
		MPU_SDA_OUT(); // sda线输出
		MPU_IIC_SDA = 1;
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SDA = 0; // START:when CLK is high,DATA change form high to low
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0; // 钳住I2C总线，准备发送或接收数据
	}
	else
	{
		MPU_SDA_OUT_2(); // sda线输出
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SDA_2 = 0; // START:when CLK is high,DATA change form high to low
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0; // 钳住I2C总线，准备发送或接收数据
	}
}
// 产生IIC停止信号
void MPU_IIC_Stop(void)
{
	if (mpu_sel)
	{
		MPU_SDA_OUT(); // sda线输出
		MPU_IIC_SCL = 0;
		MPU_IIC_SDA = 0; // STOP:when CLK is high DATA change form low to high
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_SDA = 1; // 发送I2C总线结束信号
		MPU_IIC_Delay();
	}
	else
	{
		MPU_SDA_OUT_2(); // sda线输出
		MPU_IIC_SCL_2 = 0;
		MPU_IIC_SDA_2 = 0; // STOP:when CLK is high DATA change form low to high
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_SDA_2 = 1; // 发送I2C总线结束信号
		MPU_IIC_Delay();
	}
}
// 等待应答信号到来
// 返回值：1，接收应答失败
//         0，接收应答成功
u8 MPU_IIC_Wait_Ack(void)
{
	if (mpu_sel)
	{
		u8 ucErrTime = 0;
		MPU_SDA_IN(); // SDA设置为输入
		MPU_IIC_SDA = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		while (MPU_READ_SDA)
		{
			ucErrTime++;
			if (ucErrTime > 250)
			{
				MPU_IIC_Stop();
				return 1;
			}
		}
		MPU_IIC_SCL = 0; // 时钟输出0
		return 0;
	}
	else
	{
		u8 ucErrTime = 0;
		MPU_SDA_IN_2(); // SDA设置为输入
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		while (MPU_READ_SDA_2)
		{
			ucErrTime++;
			if (ucErrTime > 250)
			{
				MPU_IIC_Stop();
				return 1;
			}
		}
		MPU_IIC_SCL_2 = 0; // 时钟输出0
		return 0;
	}
}
// 产生ACK应答
void MPU_IIC_Ack(void)
{
	if (mpu_sel)
	{
		MPU_IIC_SCL = 0;
		MPU_SDA_OUT();
		MPU_IIC_SDA = 0;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0;
	}
	else
	{
		MPU_IIC_SCL_2 = 0;
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2 = 0;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0;
	}
}
// 不产生ACK应答
void MPU_IIC_NAck(void)
{
	if (mpu_sel)
	{
		MPU_IIC_SCL = 0;
		MPU_SDA_OUT();
		MPU_IIC_SDA = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0;
	}
	else
	{
		MPU_IIC_SCL_2 = 0;
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0;
	}
}
// IIC发送一个字节
// 返回从机有无应答
// 1，有应答
// 0，无应答
void MPU_IIC_Send_Byte(u8 txd)
{
	if (mpu_sel)
	{
		u8 t;
		MPU_SDA_OUT();
		MPU_IIC_SCL = 0; // 拉低时钟开始数据传输
		for (t = 0; t < 8; t++)
		{
			MPU_IIC_SDA = (txd & 0x80) >> 7;
			txd <<= 1;
			MPU_IIC_SCL = 1;
			MPU_IIC_Delay();
			MPU_IIC_SCL = 0;
			MPU_IIC_Delay();
		}
	}
	else
	{
		u8 t;
		MPU_SDA_OUT_2();
		MPU_IIC_SCL_2 = 0; // 拉低时钟开始数据传输
		for (t = 0; t < 8; t++)
		{
			MPU_IIC_SDA_2 = (txd & 0x80) >> 7;
			txd <<= 1;
			MPU_IIC_SCL_2 = 1;
			MPU_IIC_Delay();
			MPU_IIC_SCL_2 = 0;
			MPU_IIC_Delay();
		}
	}
}
// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	if (mpu_sel)
	{
		unsigned char i, receive = 0;
		MPU_SDA_IN(); // SDA设置为输入
		for (i = 0; i < 8; i++)
		{
			MPU_IIC_SCL = 0;
			MPU_IIC_Delay();
			MPU_IIC_SCL = 1;
			receive <<= 1;
			if (MPU_READ_SDA)
				receive++;
			MPU_IIC_Delay();
		}
		if (!ack)
			MPU_IIC_NAck(); // 发送nACK
		else
			MPU_IIC_Ack(); // 发送ACK
		return receive;
	}
	else
	{
		unsigned char i, receive = 0;
		MPU_SDA_IN_2(); // SDA设置为输入
		for (i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_2 = 0;
			MPU_IIC_Delay();
			MPU_IIC_SCL_2 = 1;
			receive <<= 1;
			if (MPU_READ_SDA_2)
				receive++;
			MPU_IIC_Delay();
		}
		if (!ack)
			MPU_IIC_NAck(); // 发送nACK
		else
			MPU_IIC_Ack(); // 发送ACK
		return receive;
	}
}
