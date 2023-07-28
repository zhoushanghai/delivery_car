#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "Key.h"
// #include "malloc.h"
// #include "w25qxx.h"
#include "ST7789_Init.h"
#include "BEEPER.h"
#include "Random.h"

#include "Encoder.h"
#include "Motor.h"
#include "PWM.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "pid.h"
#include "gw_gray_serial.h"
#include "Servo.h"
//-----(‵▽′)/----------------------  -----------------------------//

char str[20];
int distanc_R = 0, distanc_L = 0;

CAR car = {
	.l_distanc = 0,
	.r_distanc = 0,
	.dis_set = 0,
	.status = straight,
	.XgyOFFSET = 0,
	.YgyOFFSET = 0,
	.ZgyOFFSET = 0};
GLO_FLAG flag_G = {.oled = 0};
KEY key[KEY_NUMBER] = { // 按键
	{KEY_RELEASE, KEY_RELEASE}};

extern CAR_PID pid;

//-----(‵▽′)/---------------------- fun -----------------------------//
void data_show(void);
void key_(void);
void software_init(void);
void wave(void);

void RX_data_deal(void);
float yaw_in180(float yaw);
float abs_float(float a);
//(‵▽′)/////////////////////////////////////////////////////////////////////////////////
float angle = 90;
int main(void)
{
	software_init();
	PWM_SetServo1(1600);
	PWM_SetServo2(1595);
	while (1)
	{
		if (CHECK_FLAG(&flag_G.oled))
		{
			// flag_G.oled = 0;
			if (flag_G.wave)
				wave();
			RX_data_deal();
			data_show();
		}
		if (CHECK_FLAG(&flag_G.key))
		{
			// flag_G.key = 0;
			key_();
			// delay_us(5);
		}
	}
}

void wave(void)
{

	// printf("R:%d L:%d\r\n", car.r_encoder, car.l_encoder);
	// printf("R:%d L:%d\r\n", distanc_R, distanc_L);
	// printf("R:%f L:%f\r\n", car.r_speed, car.l_speed);
	// printf("set speed:%.2f\r\n", car.speed_set);
	// printf("L:%d R:%d\r\n", (int)pid.speed_l.output, (int)pid.speed_r.output);
	// printf("%.2f,%.2f,%.2f,%d\r\n", car.speed_set, car.r_speed, car.l_speed, pid.speed_l.type);
	// printf("%.2f,%.2f,%.2f,%d\r\n", pid.dis.output, car.r_speed, car.l_speed, car.l_distanc);

	// mpu_dmp_get_data(&car.roll, &car.pitch, &car.yaw);
	// printf("%.2f,%.2f,%.2f\r\n", car.pitch, car.roll, car.yaw);

	// car.GrayscaleData = gw_gray_serial_read(GW_GRAY_SERIAL_GPIO_GROUP, GW_GRAY_SERIAL_GPIO_CLK, GW_GRAY_SERIAL_GPIO_DAT);
	// printf("%d\r\n", car.GrayscaleData);
	// get_gray_data();
	// 灰度值
	printf("gray:%d\r\n", car.GrayVal);

	// printf("%d,%d,%.2f,%.2f\r\n", car.gyro_x, car.gyro_y, (float)car.gyro_z - car.ZgyOFFSET, -pid.speed_l.output + pid.angSpe.output);
	// printf("%d,%.2f,%.2f\r\n", car.status, car.turn_set, car.dis_set);

	// 巡线数据
	// printf("XX%f\r\n", pid.gray.output);
}

#define FILTER 0.4f
void TIM7_IRQHandler(void)
{
	static float yaw_m;
	static int i = 0;
	static uint8_t en_cnt = 0, gray_cnt = 0;

	static float speed_l_M = 0, speed_r_M = 0;
	float yaw_PID;
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		if (++en_cnt >= 2)
		{
			// static int out_r = 0, out_l = 0;
			// encoder
			en_cnt = 0;
			car.l_encoder = -Encoder_Read_Speed(2);
			car.r_encoder = Encoder_Read_Speed(3);
			car.l_distanc += car.l_encoder;
			car.r_distanc += car.r_encoder;
			car.r_speed = (float)car.r_encoder * EN2VAL;
			car.l_speed = (float)car.l_encoder * EN2VAL;

			switch (car.status)
			{
			case straight:

				if (++gray_cnt >= 2) // 50ms
				{
					gray_cnt = 0;
					get_gray_data();
					PID_Positional(&pid.gray, 0, (float)car.GrayVal);
				}
				// mpu6050
				MPU_Get_Gyroscope(&car.gyro_x, &car.gyro_y, &car.gyro_z);

				// pid cal
				PID_Positional(&pid.dis, car.dis_set, (float)(car.l_distanc + car.r_distanc) / 2.0f);
				// turn
				// PID_Positional(&pid.angSpe, pid.gray.output, (float)car.gyro_z - car.ZgyOFFSET);
				PID_Positional(&pid.angSpe, 0, (float)car.gyro_z - car.ZgyOFFSET);
				// 一阶滤波
				speed_l_M += FILTER * (pid.dis.output + pid.angSpe.output + pid.gray.output - speed_l_M);
				speed_r_M += FILTER * (pid.dis.output - pid.angSpe.output - pid.gray.output - speed_r_M);

				// PID_Incremental(&pid.speed_l, pid.dis.output + pid.angSpe.output + pid.gray.output, car.l_speed);
				// PID_Incremental(&pid.speed_r, pid.dis.output - pid.angSpe.output - pid.gray.output, car.r_speed);

				PID_Incremental(&pid.speed_l, speed_l_M, car.l_speed);
				PID_Incremental(&pid.speed_r, speed_r_M, car.r_speed);

				// pid.speed_l.output += pid.gray.output;
				// pid.speed_r.output -= pid.gray.output;
				printf("%.2f,%.2f\r\n", speed_l_M, speed_r_M);

				if (abs_float(car.dis_set) <= abs_float((float)(car.l_distanc + car.r_distanc) / 2.0f))
				{
					car.status = stop;
					car.l_distanc = 0;
					car.r_distanc = 0;
					car.dis_set = 0;
				}
				break;

			case turn:

				mpu_dmp_get_data(&car.roll, &car.pitch, &car.yaw);
				++i;
				if (i < 2) // 刚开始转向
				{
					yaw_m = car.yaw;
				}

				yaw_PID = yaw_in180(car.yaw - yaw_m);
				printf("P%.2f s%.2f y%.2f\r\n", yaw_PID, yaw_m, car.yaw);
				PID_Positional(&pid.angle, car.turn_set, yaw_PID);

				PID_Incremental(&pid.speed_l, -pid.angle.output, car.l_speed);
				PID_Incremental(&pid.speed_r, pid.angle.output, car.r_speed);

				if (abs_float(yaw_PID - car.turn_set) < 1)
				{
					car.status = stop;
					car.turn_set = 0;
					i = 0;
				}

				break;

			case stop:

				i = 0;

				PID_Incremental(&pid.speed_l, 0, car.l_speed);
				PID_Incremental(&pid.speed_r, 0, car.r_speed);

				if (car.turn_set)
					car.status = turn;
				if (car.dis_set)
					car.status = straight;

				break;
			}

			// motor load
			Motor_PWM_Load((int)(-pid.speed_l.output), (int)(pid.speed_r.output));
		}
	}
}

void data_show(void)
{
	mpu_dmp_get_data(&car.roll, &car.pitch, &car.yaw);
	sprintf(str, "pit:%5.2f roll:%5.2f yaw:%5.2f", car.pitch, car.roll, car.yaw);
	LCD_ShowString(0, 0, str, WHITE, BLACK, 16, 0);
}

void software_init(void)
{
	flag_G.wave = 1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组2
	delay_init(168);								// 初始化延时函数

	uart_init(115200); // 初始化串口波特率为115200
	LED_Init();		   // 初始化LED

	MPU_Init();
	mpu_dmp_init();

	PWM_TIM1_Init(); // 20ms 50Hz 舵机
	PWM_TIM5_Init(8400 - 1, 1 - 1);
	Encoder_TIM2_Init();
	Encoder_TIM3_Init();
	Motor_GPIO_Init();

	Motor_PWM_Load(0, 0);

	//	W25QXX_Init(); // 初始化W25Q128
	//	my_mem_init(SRAMIN); // 初始化内部内存池

	// Key_Init(); // 按键初始化
	key_init(10); // 按键初始化

	//	Beeper_Init(); // 初始化蜂鸣器

	ST7789_Init();						  // 屏幕初始化
	LCD_Color_Fill(0, 0, 240, 240, CYAN); // 让屏幕刷青色
	LCD_ShowString(1, 1, "hello world", WHITE, BLACK, 16, 0);

	PID_set();
	gyroOffset_init(); // 陀螺仪零飘初始化
	spl_gw_gray_serial_init();
	//(‵▽′)/////////////////////////////////////  ////////////////////////////////////////////

	TIM4_Random_Init(1000 - 1, 84 - 1); // ms定时器
	TIM7_Random_Init(5000 - 1, 84 - 1); // 5ms定时器
}
void key_(void)
{
	key_scanner();
	for (int i = 0; i < KEY_NUMBER; i++)
	{
		key[i].state = key_get_state(i);
		if (key[i].state == KEY_SHORT_PRESS)
		{
			static uint8_t go = 1;

			if (go++ % 2)
				Motor_PWM_Load(9000, -9000);
			// car.speed_set = 0.5;
			else
				Motor_PWM_Load(0, 0);
			// car.speed_set = 0;

			printf("key%d short press%d\r\n", i, go);
		}
		else if (key[i].state == KEY_LONG_PRESS)
		{

			if (key[i].lastState != KEY_LONG_PRESS)
			{
				printf("key%d long press\r\n", i);
				// flag_G.wave = flag_G.wave ^ 1;
			}
		}
		else if (key[i].state == KEY_RELEASE)
		{

			if (key[i].lastState != KEY_RELEASE)
			{
				printf("key%d KEY_RELEASE\r\n", i);
			}
		}
		key[i].lastState = key[i].state;
	}
}

float yaw_in180(float yaw)
{
	if (yaw > 180)
	{
		yaw -= 360;
	}
	else if (yaw < -180)
	{
		yaw += 360;
	}
	return yaw;
}

float abs_float(float a)
{
	if (a < 0)
		return -a;
	else
		return a;
}

uint8_t check_flag(uint8_t *flag)
{
	if (*flag)
	{
		*flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
