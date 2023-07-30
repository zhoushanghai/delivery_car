/*
 * @Author: hz 1281989921@qq.com
 * @Date: 2023-07-21 17:52:01
 * @LastEditors: hz 1281989921@qq.com
 * @LastEditTime: 2023-07-25 11:35:32
 * @FilePath: \USERe:\桌面\送药小车（v1.2 优化了一下key\SYSTEM\sys\sys.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __SYS_H
#define __SYS_H
#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdint.h>

// 0,不支持ucos
// 1,支持ucos
#define SYSTEM_SUPPORT_OS 0 // 定义系统文件夹是否支持UCOS

// 位带操作,实现51类似的GPIO控制功能
// 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
// IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 20) // 0x40020014
#define GPIOB_ODR_Addr (GPIOB_BASE + 20) // 0x40020414
#define GPIOC_ODR_Addr (GPIOC_BASE + 20) // 0x40020814
#define GPIOD_ODR_Addr (GPIOD_BASE + 20) // 0x40020C14
#define GPIOE_ODR_Addr (GPIOE_BASE + 20) // 0x40021014
#define GPIOF_ODR_Addr (GPIOF_BASE + 20) // 0x40021414
#define GPIOG_ODR_Addr (GPIOG_BASE + 20) // 0x40021814
#define GPIOH_ODR_Addr (GPIOH_BASE + 20) // 0x40021C14
#define GPIOI_ODR_Addr (GPIOI_BASE + 20) // 0x40022014

#define GPIOA_IDR_Addr (GPIOA_BASE + 16) // 0x40020010
#define GPIOB_IDR_Addr (GPIOB_BASE + 16) // 0x40020410
#define GPIOC_IDR_Addr (GPIOC_BASE + 16) // 0x40020810
#define GPIOD_IDR_Addr (GPIOD_BASE + 16) // 0x40020C10
#define GPIOE_IDR_Addr (GPIOE_BASE + 16) // 0x40021010
#define GPIOF_IDR_Addr (GPIOF_BASE + 16) // 0x40021410
#define GPIOG_IDR_Addr (GPIOG_BASE + 16) // 0x40021810
#define GPIOH_IDR_Addr (GPIOH_BASE + 16) // 0x40021C10
#define GPIOI_IDR_Addr (GPIOI_BASE + 16) // 0x40022010

// IO口操作,只对单一的IO口!
// 确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入

#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr, n) // 输出
#define PHin(n) BIT_ADDR(GPIOH_IDR_Addr, n)  // 输入

#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr, n) // 输出
#define PIin(n) BIT_ADDR(GPIOI_IDR_Addr, n)  // 输入

// 以下为汇编函数
void WFI_SET(void);      // 执行WFI指令
void INTX_DISABLE(void); // 关闭所有中断
void INTX_ENABLE(void);  // 开启所有中断
void MSR_MSP(u32 addr);  // 设置堆栈地址

///////////////////////////////////////////car data//////////////////////////////////////////////

#define WAVE 0

typedef enum // car status
{
    straight,
    turn,
    stop
} CAR_STATUS;

typedef struct // 小车数据
{
    short gyro_x, gyro_y, gyro_z;                    // 三轴陀螺仪数据      gyro (陀螺仪)
    short acc_x, acc_y, acc_z;                       // 三轴加速度计数据    acc (accelerometer 加速度计)
    int l_encoder, r_encoder, l_distanc, r_distanc;  // 编码器值
    float pitch, roll, yaw, speed, r_speed, l_speed; // 俯仰角 横滚角 偏航角
    float yawPlus;                                   // 无边界yaw
    float fspeedBal, fspeedTurn;

    CAR_STATUS status; // 小车状态
    float speed_set, speedSet_M, turn_set, dis_set;

    short tofVal; // mm

    int8_t dirOPEN; // 0 stop   1 left    2 right

    float XgyOFFSET, YgyOFFSET, ZgyOFFSET; // 陀螺仪零飘
    uint8_t GrayscaleData[8];              // 灰度数据
    int GrayVal;
    int servo1_set, servo2_set; // 舵机输出值
} CAR;

typedef struct
{
    int8_t oled;
    int8_t key;
    int8_t wave;
    int8_t k210;

} GLO_FLAG;

typedef struct
{
    // int8_t x, y;
    float x, y;

} K210;
//(‵▽′)/////////////////////////////////////  ////////////////////////////////////////////

#define CHECK_FLAG(flag) (*(flag) ? (*(flag) = 0, 1) : 0)

//-----(‵▽′)/---------------------- global -----------------------------//
extern CAR car;
extern GLO_FLAG flag_G;
extern char str[20];

#define DIS_EN 0.000259f // 1/3860
#define EN2VAL DIS_EN * 100.0f

#endif
