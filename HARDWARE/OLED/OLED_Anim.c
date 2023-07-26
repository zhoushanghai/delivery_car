#include "sys.h"

void RightComeOut_Refresh(void)
{
	uint8_t i, n, j;//做ui特效要用到的变量
	for(n = 0; n < 5 && j == 0; n+=2)	//出场特效
		{
			for(i = 0; i <128 && j == 0; i++)
			{
				
				OLED_PartialRefresh(128 - i, i,n, n+2);
				
//				delay_us(i*10+1+n);

					
			}
			if(n == 4)
				{
					j = 1;
				}
		}
}

void RightToLeft_Clear(void)
{
	uint8_t i, n;//做ui特效要用到的变量
	for(n = 0; n < 3; n+=2)	//消退//清屏特效
		{
			for(i = 0; i < 128; i++)
			{
				OLED_GRAM[i][n] = 0;
				OLED_GRAM[i][n+1] = 0;
				
				OLED_PartialRefresh(127-i,i,n,n+2);
				
//				delay_us(i*10+1+n);
				
				
			}
		}
}

void Screensaver(void)//展示屏保
{
	if(Time_msForAnim > 10000)
		{
			uint8_t i, n;
			OLED_Clear();
			delay_ms(300);
			
			OLED_ShowPicture(0,0,128,32,BMP1,1);
			
			for(i = 0; i < 128; i += 15)
			{
				if(Key_KeyNumber == 1 || Key_KeyNumber == 2) break;//按键按下屏保解除
				if((i / 15) % 2 == 0) n = 2;
				else n = 0;
				if(i > 128) i = 128;
				OLED_PartialRefresh(0,i+15<128?i+15:128,n,n+2);
				delay_ms(300);
				if(Key_KeyNumber == 1 || Key_KeyNumber == 3) break;//按键按下屏保解除
			}	
		}
}

void test2(void)
{
	if(KeyNum == 2)	//长按测试
	{
		Time_msForAnim = 0;
		OLED_ShowPicture(0,0,128,32,BMP2[1],1);
		OLED_Refresh();
	}

	if(KeyNum == 3)	//双击测试
	{
		Time_msForAnim = 0;
		OLED_ShowPicture(0,0,128,32,BMP2[0],1);
		OLED_Refresh();
	}
}




//		OLED_ShowString(0,0,"Hello!",16,1);	//从上到下刷新
//		for(n = 0; n < 32; n++)
//		{
//			for(i = 0; i < 8*strlen("Hello!"); i++)
//			{
//				OLED_BUFF[i][n/8] |= OLED_GRAM[i][n/8]&(0x01<<(n%8));
//				
//			}
//			OLED_PartialRefreshForBuff(0,8*strlen("Hello!"),0,4);
////			delay_ms(1);
//		}










