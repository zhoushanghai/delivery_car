#include "sys.h"
#include "usart.h"
#include "pid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern CAR_PID pid;
extern K210 k210;

u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA = 0;			// 接收状态标记

u8 USART_RX_BUF_U1[10];	 // 接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA_U1 = 0; // 接收状态标记
//////////////////////////////////////////////////////////////////////////////////
// 如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos 使用
#endif

static void analysis_data(u8 *buf);
static void change_pid(u8 *buf);
void analysis_k210_data(u8 *buf);

//(‵▽′)/////////////////////////////////////  ////////////////////////////////////////////

void RX_k210data_deal(void)
{
	analysis_k210_data(USART_RX_BUF_U1);

	// printf("K210Data:%s\r\n", USART_RX_BUF_U1);
	// 清空缓存
	for (int i = 0; i < 10; i++)
	{
		USART_RX_BUF_U1[i] = '\0';
	}
	USART_RX_STA_U1 = 0;
}
/*解析来自K210的数据
数据格式是：
!x，y@
*/
// void analysis_k210_data(u8 *buf)
// {
// 	// Step 1: 判断数据格式是否正确
// 	if (buf[0] != '!' || buf[2] != ',' || buf[4] != '@')
// 	{
// 		printf("Invalid data format!\r\n");
// 		return;
// 	}

// 	// Step 2: 提取x和y的值
// 	k210.x = buf[1];
// 	k210.y = buf[3];
// 	// Step 3: 处理x和y的值（在这里你可以根据需要进行其他操作）
// 	printf("x: %c, y: %c\r\n", k210.x, k210.y);

// }
#define K210_FILTER 0.3f

void analysis_k210_data(u8 *buf)
{
	char *start_ptr = strchr((char *)buf, '!'); // 找到'!'的位置
	char *comma_ptr = strchr((char *)buf, ','); // 找到','的位置
	char *end_ptr = strchr((char *)buf, '@');	// 找到'@'的位置

	if (start_ptr != NULL && comma_ptr != NULL && end_ptr != NULL)
	{
		// 检查数据包格式是否正确
		if (comma_ptr > start_ptr && end_ptr > comma_ptr)
		{
			// 提取x和y
			char x_str[10];
			char y_str[10];
			memset(x_str, 0, sizeof(x_str));
			memset(y_str, 0, sizeof(y_str));

			// 复制x和y的字符串部分
			strncpy(x_str, start_ptr + 1, comma_ptr - start_ptr - 1);
			strncpy(y_str, comma_ptr + 1, end_ptr - comma_ptr - 1);

			// 将提取到的x和y转换成数值
			int8_t x = 112 - (int8_t)(atoi(x_str));
			int8_t y = 112 - (int8_t)(atoi(y_str));
			// 低通滤波
			k210.x += K210_FILTER * ((float)x - k210.x);
			k210.y += K210_FILTER * ((float)y - k210.y);

			// printf("x = %.2f, y = %.2f\n", k210.x, k210.y);
		}
	}
}

void RX_data_deal(void)
{
	// printf("RX_data_deal\r\n");
	analysis_data(USART_RX_BUF);

	// 清空缓存
	for (int i = 0; i < 32; i++)
	{
		USART_RX_BUF[i] = '\0';
	}
	USART_RX_STA = 0;
}

/*
设定速度：v1.5
显示波形：wa
*/
static void analysis_data(u8 *buf)
{

	static float speed = 0.2;
	if (buf[0] != '\0')
	{
		printf("RX data:%s\r\n", USART_RX_BUF);
		if (buf[1] != '\0')
		{
			if (buf[0] == 'v')
			{
				speed = atof((char *)buf + 1);
				car.speed_set = speed;
				printf("//////////////////////////\r\nspeed:%f\r\n\r\n", speed);
			}
			else if (buf[0] == 'w' && buf[1] == 'a') // 显示波形
			{
				flag_G.wave ^= 1;
			}
			else if (buf[0] == 'a') // 转向
			{
				car.turn_set = atof((char *)buf + 1);
				printf("//////////////////////////\r\nturn:%f\r\n\r\n", car.turn_set);
			}
			else if (buf[1] == 's' && buf[2] == 'e') // 舵机
			{
				if (buf[0] == '1')
				{
					car.servo1_set = atoi((char *)buf + 3);
					printf("//////////////////////////\r\nservo1:%d\r\n\r\n", car.servo1_set);
				}
				else if (buf[0] == '2')
				{
					car.servo2_set = atoi((char *)buf + 3);
					printf("//////////////////////////\r\nservo2:%d\r\n\r\n", car.servo2_set);
				}
			}
			else if (buf[0] == '*')
				change_pid(buf);
		}
		else
		{
			u8 speChange_flag = 0;
			switch (buf[0])
			{
			case 'G':
				car.speed_set = speed;
				speChange_flag = 1;
				break;
			case 'S': // 停止
				car.speed_set = 0;
				speChange_flag = 1;

				car.status = stop;
				car.turn_set = 0;
				break;

			case 'Q': // 停止
				// car.dis_set = 4280.0f;
				car.dis_set = -3860.0f * 4;
				car.l_distanc = 0;
				car.r_distanc = 0;

				break;

			case 'E': // 停止
				car.turn_set = 90.0f;
				break;
				// case 'K':
				// 	car.speed_set = -speed;
				// 	break;
				// case 'D':
				// 	car.speed_set += 1;
				// 	break;
				// case 'E':
				// 	car.speed_set -= 1;
				// 	break;
				// case 'F':
				// 	car.status = CAR_STOP;
				// 	break;
				// case 'R':
				// 	// car.turnSet -= 20;
				// 	car.dirOPEN = 2;
				// 	// flag_G.buzzer = 1;
				// 	break;
				// case 'L':
				// 	// car.turnSet += 20;
				// 	car.dirOPEN = 1;
				// 	break;
				// default:
				// 	break;
			}
			if (speChange_flag)
			{
				speChange_flag = 0;
				printf("set spe:%f \r\n", car.speed_set);
			}
		}
	}
}

/*******************************
 * 修改PID
 *******************************/
static void change_pid(u8 *buf)
{
	static float *val;
	// static float *val_l;

	int8_t pidSet = 20;
	PID *SetPID;

	switch (buf[1])
	{
	case 'v':
		SetPID = &pid.speed_r;
		pidSet = 0;
		switch (buf[2])
		{
		case 'p':
			val = &pid.speed_r.kp;
			// val_l = &pid.speed_l.kp;
			break;
		case 'i':
			val = &pid.speed_r.ki;
			// val_l = &pid.speed_l.ki;
			break;
		case 'd':
			val = &pid.speed_r.kd;
			// val_l = &pid.speed_l.kd;
			break;
		}
		break;

	case 'a':
		SetPID = &pid.angle;
		pidSet = 0;
		switch (buf[2])
		{
		case 'p':
			val = &pid.angle.kp;

			break;
		case 'i':
			val = &pid.angle.ki;

			break;
		case 'd':
			val = &pid.angle.kd;

			break;
		}
		break;
	}

	if (pidSet != 20)
	{
		// buf[0] = '\0';
		// buf[1] = '\0';
		*val = atof((char *)buf + 3);
		// *val_l = atof((char *)buf + 3);
		printf("//////////////////////////\r\nPID set P:%f I:%f D:%f\r\n\r\n", SetPID->kp, SetPID->ki, SetPID->kd);
	}
}

//(‵▽′)///////////////////////////////////// 初始化 ////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

// 加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
// 重定义fputc函数
int fputc(int ch, FILE *f)
{
	// while ((USART1->SR & 0X40) == 0)
	// 	; // 循环发送,直到发送完毕
	// USART1->DR = (u8)ch;
	// return ch;

	while ((USART3->SR & 0X40) == 0)
		; // 循环发送,直到发送完毕
	USART3->DR = (u8)ch;
	return ch;
}
#endif

#if EN_USART1_RX // 如果使能了接收
// 串口1中断服务程序
// 注意,读取USARTx->SR能避免莫名其妙的错误

// 初始化IO 串口1
// bound:波特率
void uart_init(u32 bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // 使能USART1时钟

	// 串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  // GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // GPIOA10复用为USART1

	// USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 初始化PA9，PA10

	// USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(USART1, &USART_InitStructure);										// 初始化串口1

	USART_Cmd(USART1, ENABLE); // 使能串口1

	// USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启相关中断

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		  // 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器、

#endif

	//-----(‵▽′)/---------------------- init UART3 -----------------------------//

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // 使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 使能USART3时钟

	// 串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART2); // GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART2); // GPIOB10复用为USART3

	// USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; // GPIOB11与GPIOB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			 // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			 // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			 // 上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 // 初始化

	// USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(USART3, &USART_InitStructure);										// 初始化串口1

	USART_Cmd(USART3, ENABLE); // 使能串口3

	// USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启相关中断

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  // 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器、
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS // 如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART1); //(USART1->DR);	//读取接收到的数据
		if (Res == '@')
		{
			flag_G.k210 = 1;
			// printf("2ss\r\n");
		}
		if ((USART_RX_STA_U1 & 0x8000) == 0) // 接收未完成
		{
			if (USART_RX_STA_U1 & 0x4000) // 接收到了0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA_U1 = 0; // 接收错误,重新开始
				else
					USART_RX_STA_U1 |= 0x8000; // 接收完成了
			}
			else // 还没收到0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA_U1 |= 0x4000;
				else
				{
					USART_RX_BUF_U1[USART_RX_STA_U1 & 0X3FFF] = Res;
					USART_RX_STA_U1++;
					if (USART_RX_STA_U1 > (USART_REC_LEN - 1))
						USART_RX_STA_U1 = 0; // 接收数据错误,重新开始接收
				}
			}
		}
	}
#if SYSTEM_SUPPORT_OS // 如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();
#endif
}
#endif

void USART3_IRQHandler(void) // 串口3中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS // 如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART3); //(USART3->DR);	//读取接收到的数据

		if ((USART_RX_STA & 0x8000) == 0) // 接收未完成
		{
			if (USART_RX_STA & 0x4000) // 接收到了0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA = 0; // 接收错误,重新开始
				else
					USART_RX_STA |= 0x8000; // 接收完成了
			}
			else // 还没收到0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; // 接收数据错误,重新开始接收
				}
			}
		}
	}
#if SYSTEM_SUPPORT_OS // 如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();
#endif
}
