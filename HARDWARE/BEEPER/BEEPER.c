#include "BEEPER.h"
#include "stm32f4xx.h"                  // Device header
#include "Random.h"
#include "math.h"

/*创建Beeper的handle*/
BEEPER_Tag Beeper0;

/*C4-B8的音调对应的频率大小*/
const uint16_t MusicNoteFrequency[] = {
//rest_note
    0,
//   C    C#   D   Eb    E    F    F#   G    G#   A   Bb    B
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
//check_note
    0,
};

/*全局TONE结构体指针，用于定时器TIM4中断函数中*/
TONE *MySound;

/*乐曲*/
TONE BEEPER_KEYPRESS[] ={
{NOTE_C6, 50},
{CHECK_NOTE, 0},//检查位
};

TONE BEEPER_TRITONE[] ={
{NOTE_B5, 50},
{NOTE_D6, 50},
{NOTE_F6, 50},
{CHECK_NOTE, 0},//检查位
};

TONE BEEPER_WARNING[] ={
{NOTE_F4, 50},
{REST_NOTE, 50},
{NOTE_F4, 50},
{CHECK_NOTE, 0},//检查位
};



TONE BEEP1[] = {
{NOTE_C5, 110},
{REST_NOTE, 20},//休止符
{NOTE_C5, 110},
{REST_NOTE, 20},
{NOTE_G5, 110},
{REST_NOTE, 20},
{NOTE_G5, 110},
{REST_NOTE, 20},
{NOTE_A5, 110},
{REST_NOTE, 20},
{NOTE_A5, 110},
{REST_NOTE, 20},
{NOTE_G5, 220},
{REST_NOTE, 20},
{NOTE_F6, 110},
{REST_NOTE, 20},
{NOTE_F6, 110},
{REST_NOTE, 20},
{NOTE_E7, 110},
{REST_NOTE, 20},
{NOTE_E7, 110},
{REST_NOTE, 20},
{NOTE_D8, 110},
{REST_NOTE, 20},
{NOTE_D8, 110},
{REST_NOTE, 20},
{NOTE_C4, 220},
{CHECK_NOTE, 0},//检查位
};



TONE BEEP2[] = {
    {REST_NOTE, 200},
    {REST_NOTE, 200},
    {REST_NOTE, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},

    {NOTE_A4, 200},
    {NOTE_E5, 400},
    {NOTE_C5, 100},
    {NOTE_A4, 100},

    {NOTE_B4, 200},
    {NOTE_F5, 200},
    {NOTE_E5, 100},
    {NOTE_D5, 300},

    {NOTE_C5, 100},
    {NOTE_D5, 100},
    {NOTE_C5, 100},
    {NOTE_D5, 100},
    {NOTE_E5, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},

    {NOTE_A4, 200},
    {NOTE_D5, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},
    {REST_NOTE, 100},
    {NOTE_A4, 50},
    {NOTE_B4, 50},

    {NOTE_C5, 200},
    {NOTE_A4, 200},
    {NOTE_E5, 200},
    {NOTE_C5, 200},

    {NOTE_D5, 200},
    {NOTE_A5, 200},
    {NOTE_G5, 200},
    {NOTE_F5, 100},
    {NOTE_E5, 50},
    {NOTE_D5, 50},

    {NOTE_E5, 800},

    {CHECK_NOTE, 0},//检查位
};


void TIM1_PWM_Init(u16 arr)
{
    /*初始化结构体*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /*开启rcc时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    /*初始化TIM1*/
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = arr - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 168 - 1;//使得频率剩下1000kHz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	// TIM_ARRPreloadConfig(TIM1, ENABLE);
	
    /*TIM1配置PWM模式*/
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = arr/2;//占空比
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, DISABLE);//先关掉防止蜂鸣器怪叫
}

void Beeper_Init(void)
{
    /*初始化结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启rcc时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*初始化PA8-TIM1_CH1*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);

    /*初始化定时器TIM1*/
    TIM1_PWM_Init(370);

    /* BEEPER使能标志位 */
    Beeper0.Beeper_Enable = 1;

    Beeper0.Sound_Loud = 98;

}

/*计算对应的预重装值ARR（psc = 10 - 1）//用1000kHz / 音调频率*/
uint16_t Set_Musical_Note(uint16_t frq)
{
    /*防止休止符时蜂鸣器怪叫*/
    if(frq == 0) return 0;
    float temp = 0;
    temp = 1000000.0f / (float)frq;
    return (uint16_t)temp;
}


/**
  * @brief Beeper的应用层函数
  * @param  TONE *Sound 传入结构体数组
  * @retval 无
  */
void Beeper_Perform(TONE *Sound)
{
    /*该变量用于计算结构体数组的长度*/
    uint16_t Note_Length;
    TIM_Cmd(TIM1, DISABLE);
    
    /*让全局结构体指针指向传入的乐曲*/
    MySound = Sound;

    /*通过寻找检查位CHECK_NOTE来计算传入的结构体长度//因为sizeof是在编译中完成的所以这里没法用*/
    for(Note_Length = 0; MySound[Note_Length].Note != CHECK_NOTE; Note_Length++);
    
    /*赋予长度大小*/
    Beeper0.Sound_Size = Note_Length;
    /*把音符表清零*/
    Beeper0.Beep_Play_Schedule = 0;
    /*开启蜂鸣器使能标志位*/
    Beeper0.Beeper_Set_Flag = 1;

}

/* 用于1ms定时器中断进行循环 */
void Beeper_Proc(void)
{
    /*判断是否继续*/
    if(Beeper0.Beeper_Continue_Flag && Beeper0.Beeper_Enable)
    {
        /*判断音符表走完没*/
        if(Beeper0.Beep_Play_Schedule < Beeper0.Sound_Size)
        {
            /*给预重装载值赋值，改变音调*/
            TIM1->ARR = (uint16_t)Set_Musical_Note(MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note]);
            /*给PWM占空比赋值，改变音量*/
            TIM1->CCR1 =(uint16_t)TIM1->ARR/(100 - Beeper0.Sound_Loud);
            /*时间减短1ms*/
            Beeper0.Beeper_Count --;
            TIM_Cmd(TIM1, ENABLE);
        }
        /*失能蜂鸣器，清空标志位*/
        else {TIM_Cmd(TIM1,DISABLE); Beeper0.Beeper_Continue_Flag = 0; GPIO_ResetBits(GPIOA, GPIO_Pin_8);} 
        /*这个操作的意思是如果count = 65535时，意思就是延时结束了，这个音符演完了*/
        if(!(Beeper0.Beeper_Count < 65535))
        {
            /*音符表走到下一个音符*/
            Beeper0.Beep_Play_Schedule ++;
            /*赋值新的延时长度给count*/
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }else{
			TIM_Cmd(TIM1,DISABLE);
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		}
    /*判断是否开启蜂鸣器*///感觉这步不太需要了//但是缺一不可
    if(Beeper0.Beeper_Set_Flag && Beeper0.Beeper_Enable)
    {
        /*先清空标志位*/
        Beeper0.Beeper_Set_Flag = 0;
        /*判断音符表是否为零//这步不能拿到继续标志位中判断，否则蜂鸣器会持续长鸣*/
        if(Beeper0.Beep_Play_Schedule == 0) 
        {
            /*开启蜂鸣器继续标志位*/
            Beeper0.Beeper_Continue_Flag = 1;
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }
}











