/*
 * @Author: hz 1281989921@qq.com
 * @Date: 2023-07-26 20:01:05
 * @LastEditors: hz 1281989921@qq.com
 * @LastEditTime: 2023-07-26 21:11:17
 * @FilePath: \USERe:\桌面\送药小车\HARDWARE\Motor\pid.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __PID_H__
#define __PID_H__
#include "sys.h"
//(‵▽′)///////////////////////////////////// PID参数 ////////////////////////////////////////////
#define SPEED_P -9000.0f
#define SPEED_I -2100.0f
#define SPEED_D -1.0f

#define DIS_P 0.01f
// #define DIS_I DIS_P / 200.0f
#define DIS_I 0.003f
#define DIS_D 0.00001f

#define ANGLE_P 0.035f
#define ANGLE_I 0.0f
#define ANGLE_D 0.04f

#define ANGSPE_P -0.00007f
#define ANGSPE_I -0.0f
#define ANGSPE_D -0.000007f

// #define GRAY_P 300.0f
// #define GRAY_I -0.0f
// #define GRAY_D 100.0f
#define GRAY_P -0.08f
#define GRAY_I -0.0f
#define GRAY_D -0.01f

#define SEVOR_P -0.67f
#define SEVOR_I -0.45df
#define SEVOR_D -0.08f

//(‵▽′)///////////////////////////////////// 自定义结构体 ////////////////////////////////////////////
typedef enum
{
    PI,
    PD,
    P,
    PIDALL
} PIDTYPE;

typedef struct // 增量式PID
{
    float kp, ki, kd;                   // 三个系数
    float error, lastError, lastError2; // 误差、上次误差
    float integral, maxIntegral;        // 积分、积分限幅
    float output, maxOutput;            // 输出、输出限幅
    PIDTYPE type;
} PID;

typedef struct // 飞轮串级PID
{
    PID speed_r, speed_l; // 角度环
    PID angle;            // 速度换
    PID angSpe;           // 角加速度环
    PID dis;              // 位移环
    PID gray;             // 灰度环
    PID sevor1;           // 舵机1环
    PID sevor2;           // 舵机2环
    int output;           // 串级输出，等于inner.output
} CAR_PID;

//(‵▽′)///////////////////////////////////// Function ////////////////////////////////////////////
void PID_set(void);
float PID_Incremental(PID *pid, float target, float feedback);
float PID_Positional(PID *pid, float target, float feedback);

#endif
