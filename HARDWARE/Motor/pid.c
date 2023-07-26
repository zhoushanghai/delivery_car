#include "pid.h"
#define INTEGRAL_CALCULATION_H 0 // PID 进行积分计算

static void PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOut);
static void Limit(float *val, float max);

CAR_PID pid;

///////////////////////////////////init///////////////////////////////
void PID_set(void)
{
    PID_Init(&pid.speed_l, SPEED_P, SPEED_I, SPEED_D, 0.0, 10000);
    PID_Init(&pid.speed_r, SPEED_P, SPEED_I - 200.0f, SPEED_D, 0.0, 10000);
    PID_Init(&pid.dis, DIS_P, DIS_I, DIS_D, 0.0, 0.3);

    PID_Init(&pid.angle, ANGLE_P, ANGLE_I, ANGLE_D, 0.0, 0.3);

    PID_Init(&pid.angSpe, ANGSPE_P, ANGSPE_I, ANGSPE_D, 0.0, 0.6);

    PID_Init(&pid.gray, GRAY_P, GRAY_I, GRAY_D, 0.0, 0.8);
}

//(‵▽′)///////////////////////////////////// 计算函数 ////////////////////////////////////////////
/*****************
增量式PID控制器
******************/
float PID_Incremental(PID *pid, float target, float feedback)
{
    pid->lastError2 = pid->lastError;
    pid->lastError = pid->error;
    pid->error = target - feedback;
    switch (pid->type)
    {
    case PI:
        pid->output += (pid->error - pid->lastError) * pid->kp + pid->error * pid->ki;
        break;
    case PD:
        pid->output += (pid->error - pid->lastError) * pid->kp + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;
        break;
    case P:
        pid->output += (pid->error - pid->lastError) * pid->kp;
        break;
    case PIDALL:
        pid->output += (pid->error - pid->lastError) * pid->kp + pid->error * pid->ki + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;
        break;
    }
    Limit(&pid->output, pid->maxOutput);

    return pid->output;
}

/*****************
位置式PID控制器
******************/
float PID_Positional(PID *pid, float target, float feedback)
{
    pid->lastError = pid->error;
    pid->error = target - feedback;
    switch (pid->type)
    {
    case PI:
        pid->integral += pid->error * pid->ki;
        Limit(&pid->integral, pid->maxIntegral);
        pid->output = pid->error * pid->kp + pid->integral;
        break;
    case PD:
        pid->output = pid->output = pid->error * pid->kp + (pid->error - pid->lastError) * pid->kd;
        break;
    case P:
        pid->output = pid->error * pid->kp;
        break;

    case PIDALL:
        pid->integral += pid->error * pid->ki;
        Limit(&pid->integral, pid->maxIntegral);
        pid->output = pid->error * pid->kp + pid->integral + (pid->error - pid->lastError) * pid->kd;
        break;
    }
    Limit(&pid->output, pid->maxOutput);

    return pid->output;
}
//(‵▽′)///////////////////////////////////// 底层代码 ////////////////////////////////////////////
static void Limit(float *val, float max)
{
    if (*val > max)
        *val = max;
    else if (*val < -max)
        *val = -max;
}

// 用于初始化pid参数的函数
static void PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOut)
{
    pid->kp = p;
    pid->ki = i;
    pid->kd = d;
    pid->maxIntegral = maxI;
    pid->maxOutput = maxOut;
    // 判断pid类型
    if (i == 0 && d == 0)
    {
        pid->type = P;
    }
    else if (i == 0)
    {
        pid->type = PD;
    }
    else if (d == 0)
    {
        pid->type = PI;
    }
    else
    {
        pid->type = PIDALL;
    }
}

/*****************
速度环PI控制器：Kp*Ek+Ki*Ek_S(Ek_S：偏差的积分)
******************/
void Velocity(PID *pid, int target, float feedback)
{
    pid->lastError2 = pid->lastError;
    pid->lastError = pid->error;
    pid->error = target - feedback;

    pid->output += (pid->error - pid->lastError) * pid->kp + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;
    Limit(&pid->output, pid->maxOutput);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// /*****************
//  角速度环 PD控制器
// 入口：Med:机械中值(期望角度)，Angle:真实角度，gyro_Y:真实角速度
// 出口：直立环输出
// ******************/
// void TRV_pid_angspe(PID *pid, float reference, float feedback)
// {

//     /*有低通滤波的*/
//     // static float a = 0.7;
//     // pid->error = feedback - reference;
//     // pid->error = pid->error * a + pid->lastError * (1 - a);
//     // pid->output += (pid->error - pid->lastError) * pid->kp + pid->error * pid->ki;
//     // pid->lastError = pid->error;

//     /*有低通滤波的*/
//     pid->lastError2 = pid->lastError;
//     pid->lastError = pid->error;
//     pid->error = feedback - reference;
//     pid->output += (pid->error - pid->lastError) * pid->kp + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;

//     // 输出限幅
//     Limit(&pid->output, pid->maxOutput);
// }

// /*****************
// 直立环PD控制器：Kp*Ek+Kd*Ek_D

// 入口：Med:机械中值(期望角度)，Angle:真实角度，gyro_Y:真实角速度
// 出口：直立环输出
// ******************/
// void Vertical(PID *pid, float Med, float Angle)
// {
//     pid->lastError2 = pid->lastError;
//     pid->lastError = pid->error;
//     pid->error = Angle - Med;
//     pid->output += (pid->error - pid->lastError) * pid->kp + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;
// }

// /*****************
// 速度环PI控制器：Kp*Ek+Ki*Ek_S(Ek_S：偏差的积分)
// ******************/
// void Velocity(PID *pid, float Target, float encoder)
// {
//     // 定义成静态变量，保存在静态存储器，使得变量不丢掉
//     static float Encoder_Err, EnC_Err_Lowout, EnC_Err_Lowout_last;
//     float a = 0.6;

//     // 1.计算速度偏差
//     // 舍去误差--我的理解：能够让速度为"0"的角度，就是机械中值。
//     Target = -Target;
//     Encoder_Err = (float)(Target - encoder);
//     // 2.对速度偏差进行低通滤波
//     // low_out = (1-a)*Ek+a*low_out_last
//     EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_last; // 使得波形更加平滑，滤除高频干扰，放置速度突变

//     pid->integral += EnC_Err_Lowout * pid->ki;
//     pid->integral = pid->integral > pid->maxIntegral ? pid->maxIntegral : (pid->integral < (-pid->maxIntegral) ? (-pid->maxIntegral) : pid->integral);

//     // 5.速度环控制输出
//     pid->output = pid->kp * EnC_Err_Lowout + pid->integral + pid->kd * (EnC_Err_Lowout - EnC_Err_Lowout_last);
//     EnC_Err_Lowout_last = EnC_Err_Lowout;

//     /////////////////////////////////////////直接给////////////////////////////////////////////////////
//     // Target = -Target;
//     // pid->error = Target - encoder;
//     // // 4.积分限幅
//     // pid->integral += pid->error * pid->ki;
//     // pid->integral = pid->integral > pid->maxIntegral ? pid->maxIntegral : (pid->integral < (-pid->maxIntegral) ? (-pid->maxIntegral) : pid->integral);

//     // // 5.速度环控制输出
//     // pid->output = pid->kp * pid->error + pid->integral + pid->kd * (pid->error - pid->lastError);
//     // pid->lastError = pid->error;
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////
// void flypid_angspe(PID *pid, float reference, float feedback)
// {
//     static float angspe_iout;
//     //-----(‵▽′)/---------------------- 低通滤波 -----------------------------//
//     // err = reference - feedback; // 计算新error
//     // pid->error = (1 - a) * err + a * pid->lastError;
//     // // lastErr = EnC_Err_Lowout; // 将旧error存起来
//     // // pid->error = EnC_Err_Lowout;

//     pid->error = reference - feedback;
//     angspe_iout = pid->error * pid->ki;
//     pid->integral += angspe_iout;
//     pid->output += (pid->error - pid->lastError) * pid->kp + angspe_iout;
//     Limit(&pid->output, pid->maxOutput);
//     pid->lastError = pid->error;
// }

// void flypid_angle(PID *pid, float reference, float feedback)
// {
//     // static float lastErr, err;
//     // err = reference - feedback;
//     // pid->output = (err)*pid->kp - (err - lastErr) * pid->kd;
//     // lastErr = err;
//     // pid->output += (pid->error - pid->lastError) * pid->kp + (gyro - lastGyro) * pid->kd;
//     pid->lastError2 = pid->lastError;
//     pid->lastError = pid->error;
//     pid->error = reference - feedback;
//     pid->output += (pid->error - pid->lastError) * pid->kp + (pid->error - 2 * pid->lastError + pid->lastError2) * pid->kd;
// }

// void flypid_Velocity(PID *pid, float Target, float encoder)
// {
//     // 定义成静态变量，保存在静态存储器，使得变量不丢掉
//     static float Encoder_Err, EnC_Err_Lowout, EnC_Err_Lowout_last;
//     float a = 0.7;

//     // 1.计算速度偏差
//     // 舍去误差--我的理解：能够让速度为"0"的角度，就是机械中值。
//     Encoder_Err = (float)(Target - encoder);
//     // 2.对速度偏差进行低通滤波
//     // low_out = (1-a)*Ek+a*low_out_last
//     EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_last; // 使得波形更加平滑，滤除高频干扰，放置速度突变
//     EnC_Err_Lowout_last = EnC_Err_Lowout;                             // 防止速度过大影响直立环的正常工作
//     // 3.对速度偏差积分出位移
//     pid->integral += EnC_Err_Lowout * pid->ki;
//     // Encoder_S += EnC_Err_Lowout * pid->ki;
//     // 4.积分限幅
//     pid->integral = pid->integral > pid->maxIntegral ? pid->maxIntegral : (pid->integral < (-pid->maxIntegral) ? (-pid->maxIntegral) : pid->integral);

//     // 5.速度环控制输出
//     pid->output = pid->kp * EnC_Err_Lowout + pid->integral;
// }

// void turn_angspe(PID *pid, float reference, float feedback)
// {
//     pid->error = reference - feedback;
//     pid->output = pid->error * pid->kp + (pid->error - pid->lastError) * pid->kd;
//     pid->lastError = pid->error;
//     Limit(&pid->output, pid->maxOutput);
// }

// void turn_midLin(PID *pid, float tra, float angle)
// {

//     pid->error = tra - angle;
//     pid->output = pid->error * pid->kp + (pid->error - pid->lastError) * pid->kd;
//     pid->lastError = pid->error;
// }

// void turn_Velocity(PID *pid, float Target, float encoder)
// {
//     // 定义成静态变量，保存在静态存储器，使得变量不丢掉
//     static float Encoder_Err, EnC_Err_Lowout, EnC_Err_Lowout_last;
//     float a = 0.7;

//     // 1.计算速度偏差
//     Encoder_Err = (float)(Target - encoder);
//     // 2.对速度偏差进行低通滤波
//     // low_out = (1-a)*Ek+a*low_out_last
//     EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_last; // 使得波形更加平滑，滤除高频干扰，放置速度突变
//     EnC_Err_Lowout_last = EnC_Err_Lowout;                             // 防止速度过大影响直立环的正常工作
//     // 3.对速度偏差积分出位移
//     // pid->integral += EnC_Err_Lowout * pid->ki;
//     // Encoder_S += EnC_Err_Lowout * pid->ki;
//     // 4.积分限幅
//     pid->integral = pid->integral > pid->maxIntegral ? pid->maxIntegral : (pid->integral < (-pid->maxIntegral) ? (-pid->maxIntegral) : pid->integral);

//     // 5.速度环控制输出
//     pid->output = pid->kp * EnC_Err_Lowout;
// }
