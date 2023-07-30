/*
 * @Author: hz 1281989921@qq.com
 * @Date: 2023-07-10 10:31:29
 * @LastEditors: hz 1281989921@qq.com
 * @LastEditTime: 2023-07-10 13:49:25
 * @FilePath: \USERe:\桌面\Posture _Glove\HARDWARE\MPU6050\mpuiic.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK MiniSTM32F103开发板
// MPU6050 IIC驱动 代码
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2015/4/18
// 版本：V1.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2009-2019
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

// IO方向设置
#define MPU_SDA_IN()                     \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)8 << 28; \
    }
#define MPU_SDA_OUT()                    \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)3 << 28; \
    }
// #define MPU_SDA_IN()                     \
//     {                                    \
//         GPIOB->CRL &= 0XFF0FFFFF;        \
//         GPIOB->CRL |= (uint32_t)8 << 20; \
//     }
// #define MPU_SDA_OUT()                    \
//     {                                    \
//         GPIOB->CRL &= 0XFF0FFFFF;        \
//         GPIOB->CRL |= (uint32_t)3 << 20; \
//     }
// 0的位置
#define MPU_SDA_IN_2()                   \
    {                                    \
        GPIOB->CRL &= 0XFF0FFFFF;        \
        GPIOB->CRL |= (uint32_t)8 << 20; \
    }
#define MPU_SDA_OUT_2()                  \
    {                                    \
        GPIOB->CRL &= 0XFF0FFFFF;        \
        GPIOB->CRL |= (uint32_t)3 << 20; \
    }

// IO操作函数

extern u8 scl, sda;

// #define MPU_IIC_SCL PBout(scl) // SCL
// #define MPU_IIC_SDA PBout(sda) // SDA
// #define MPU_READ_SDA PBin(sda) // SDA

// #define MPU_IIC_SCL PBout(6) // SCL
// #define MPU_IIC_SDA PBout(7) // SDA
// #define MPU_READ_SDA PBin(7) // SDA

// PB4 好像是有一点问题的

// #define MPU_IIC_SCL PBout(6) // SCL
// #define MPU_IIC_SDA PBout(7) // SDA
// #define MPU_READ_SDA PBin(7) // SDA
#define MPU_IIC_SCL PBout(6) // SCL
#define MPU_IIC_SDA PBout(7) // SDA
#define MPU_READ_SDA PBin(7) // SDA

#define MPU_IIC_SCL_2 PBout(4) // SCL
#define MPU_IIC_SDA_2 PBout(5) // SDA
#define MPU_READ_SDA_2 PBin(5) // SDA

// IIC所有操作函数
void MPU_IIC_Delay(void);                // MPU IIC延时函数
void MPU_IIC_Init(void);                 // 初始化IIC的IO口
void MPU_IIC_Start(void);                // 发送IIC开始信号
void MPU_IIC_Stop(void);                 // 发送IIC停止信号
void MPU_IIC_Send_Byte(u8 txd);          // IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack); // IIC读取一个字节
u8 MPU_IIC_Wait_Ack(void);               // IIC等待ACK信号
void MPU_IIC_Ack(void);                  // IIC发送ACK信号
void MPU_IIC_NAck(void);                 // IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr, u8 addr);

void MPU_IIC_SCL_switch(void);
void MPU_IIC_SDA_switch(void);
void MPU_READ_SDA_switch(void);

// #define MPU_IIC_SCL MPU_IIC_SCL_switch()   // SCL
// #define MPU_IIC_SDA MPU_IIC_SDA_switch()   // SDA
// #define MPU_READ_SDA MPU_READ_SDA_switch() // SDA

#endif
