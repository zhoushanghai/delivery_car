#ifndef __KEY_H__
#define __KEY_H__

#define SHORT_PRESS 1  // 单击
#define LONG_PRESS 2   // 长按
#define DOUBLE_PRESS 3 // 双击

typedef struct
{
	uint16_t Time_1;			  // Time_1判断长按时长
	uint16_t Time_2;			  // Time_2判断双击时长
	uint8_t FirstPress_Flag;	  // 第一次按下标志
	uint8_t SecondPress_Flag;	  // 第二次按下标志
	uint8_t LongPressOpen_Flag;	  // 是否打开长按标志(1打开/0关闭)
	uint8_t DoublePressOpen_Flag; // 是否打开双击标志(1打开/0关闭)
} Key_Tag;

/*底层函数*/
void Key_Init(void);

void TIM2_Key_Init(void);
void Key_Loop(void);
uint8_t Key_GetState(void);
/*应用层函数*/
uint8_t Key_GetNum(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define KEY_RELEASE_LEVEL (GPIO_HIGH) // 按键的默认状态 也就是按键释放状态的电平
#define KEY_MAX_SHOCK_PERIOD (20)	  // 按键消抖检测时长 单位毫秒 低于这个时长的信号会被认为是杂波抖动
#define KEY_LONG_PRESS_PERIOD (1000)  // 最小长按时长 单位毫秒 高于这个时长的信号会被认为是长按动作
//==================================================定义 按键 基本配置================================================

//==================================================定义 按键 参数结构体===============================================

typedef enum
{
	KEY_1,
	KEY_NUMBER,
} key_index_enum; // 按键索引 对应上方定义的按键引脚个数 默认定义四个按键

typedef enum
{
	KEY_RELEASE,	 // 按键释放状态
	KEY_SHORT_PRESS, // 按键短按状态
	KEY_LONG_PRESS,	 // 按键长按状态
} key_state_enum;

typedef struct
{
	key_state_enum state;
	key_state_enum lastState;
} KEY;
//==================================================定义 按键 参数结构体===============================================

//==================================================声明 按键 基础函数===============================================
void key_scanner(void);								// 按键状态扫描
key_state_enum key_get_state(key_index_enum key_n); // 获取按键状态
void key_clear_state(key_index_enum key_n);			// 清除指定按键状态
void key_clear_all_state(void);						// 清除所有按键状态
void key_init(int period);							// 按键初始化
//==================================================声明 按键 基础函数===============================================

#endif
