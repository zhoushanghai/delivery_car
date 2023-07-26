#include "control.h"

extern CAR_PID pid;


// #include "control.h"
// extern flywheelPID pid;
// extern travelPID travel_pid;
// // extern eulerianAngles_t eulerAngle; // 欧拉角
// extern gyro_param_t GyroOffset;
// extern CAMERA camera;

// extern float abs_float(float a);

// typedef struct
// {
//     uint8_t count;
//     uint8_t flag;
// } FLAG;

// void flypid_Velocity(PID *pid, float Target, float encoder);
// void flypid_angle(PID *pid, float reference, float feedback);
// void flypid_angspe(PID *pid, float reference, float feedback);
// void Limit(int *val, int max);
// float speed_smooth(float speed_set);
// float set_turnVal(float camErr, int8_t dir);

// float speedT_set = 0;

// void set_TRVPID_flatGround(void) // 平地 行进轮 pid
// {
//     PID_Init(&travel_pid.speed, travel_SPEED_P, travel_SPEED_I, travel_SPEED_D, 5, MAX_PWM_travel);           // 初始化串级PID角速度环
//     PID_Init(&travel_pid.angle, travel_ANGLE_P, travel_ANGLE_I, travel_ANGLE_D, ANGSPE_MXAI, MAX_PWM_travel); // 初始化串级PID角度环
//     PID_Init(&travel_pid.angSpe, travel_ANGSPE_P, travel_ANGSPE_I, travel_ANGSPE_D, 0, MAX_PWM_travel);
// }

// void set_TRVPID_slope(void) // 斜坡 行进轮 pid
// {
//     // PID_Init(&travel_pid.speed, travel_SPEED_P, travel_SPEED_I, travel_SPEED_D, 5, MAX_PWM_travel);           // 初始化串级PID角速度环
//     // PID_Init(&travel_pid.angle, travel_ANGLE_P, travel_ANGLE_I, travel_ANGLE_D, ANGSPE_MXAI, MAX_PWM_travel); // 初始化串级PID角度环
//     PID_Init(&travel_pid.angSpe, 2, travel_M_ANGSPE_I, travel_M_ANGSPE_D, 0, MAX_PWM_travel);
// }

// // void track_check(void)
// //{
// //     static TRACK lastTRA;
// //     // 接收来自摄像头的数据
// //     // car.track = straight;
// //     // 结合tof数据进行判断
// //     if (car.tofVal < 200)
// //     {
// //         car.track = slope;
// //         if (lastTRA != slope) // buzzer tips
// //         {
// //             flag_G.buzzer = 1;
// //             printf("tof track\n");
// //         }
// //     }
// //     else
// //     {
// //         car.track = straight;
// //     }
// //
// //     lastTRA = car.track;
// // }
// /********************************
//  检测设定速度是否变化，如果变化则改变pid参数

//  分为静止和运动两种情况，运动需要保证加速时不会角度不会超调
// ********************************/
// void TRVPID_change(float speedSet)
// {
//     static float lastSpeedSet = 0;
//     if (speedSet != lastSpeedSet) // 设定速度改变
//     {
//         lastSpeedSet = speedSet;
//         if (speedSet == 0)
//         {
//             set_TRVPID_flatGround();
//         }
//         else
//         {
//             set_TRVPID_slope();
//         }
//     }
// }

// void pitch1_control(void) // 控制算法的函数
// {
//     static FLAG angSpe_f = {0, 0}, angle_f = {0, 0}, speed_f = {0, 0}, angSpe_t = {0, 0}, angle_t = {0, 0}, speed_t = {0, 0}, speed_turn = {0, 0}, midLin_turn = {0, 0}, angSpe_turn = {0, 0};
//     //    static float trvSpeed_output = 0;
//     static int trv_output;
//     static float turnTar, turnVal;

//     // float turnVal;
//     if (++angSpe_f.count == 1)
//     {
//         angSpe_f.count = 0;
//         angSpe_f.flag = 1;
//     }
//     if (++angle_f.count == 5)
//     {
//         angle_f.count = 0;
//         angle_f.flag = 1;
//     }
//     if (++speed_f.count == 10)
//     {
//         speed_f.count = 0;
//         speed_f.flag = 1;
//     }
//     if (++angSpe_t.count == 2)
//     {
//         angSpe_t.count = 0;
//         angSpe_t.flag = 1;
//     }
//     if (++angle_t.count == 10)
//     {
//         angle_t.count = 0;
//         angle_t.flag = 1;
//     }
//     if (++speed_t.count == 20)
//     {
//         speed_t.count = 0;
//         speed_t.flag = 1;
//     }
//     if (++speed_turn.count == 30)
//     {
//         speed_turn.count = 0;
//         speed_turn.flag = 1;
//     }
//     if (++midLin_turn.count == 10)
//     {
//         midLin_turn.count = 0;
//         midLin_turn.flag = 1;
//     }
//     if (++angSpe_turn.count == 2)
//     {
//         angSpe_turn.count = 0;
//         angSpe_turn.flag = 1;
//     }

//     //    static int trv_output = 0;
//     ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //-----(‵▽′)/----------------------数据采集-----------------------------//
//     if (speed_f.flag) // 飞轮速度
//     {
//         car.left_encoder_data = -encoder_get_count(LEFT_FLYWHEEL_ENCODER_INDEX);  // 获取左侧编码器值
//         encoder_clear_count(LEFT_FLYWHEEL_ENCODER_INDEX);                         // 清除编码器计数 方便下次采集
//         car.right_encoder_data = encoder_get_count(RIGHT_FLYWHEEL_ENCODER_INDEX); // 获取右侧编码器值
//         encoder_clear_count(RIGHT_FLYWHEEL_ENCODER_INDEX);                        // 清除编码器计数 方便下次采集
//         car.fspeedBal = (float)(car.left_encoder_data + car.right_encoder_data) / 40;

//         // car.fspeed_l = (float)(car.left_encoder_data) / 20;
//         // car.fspeed_r = (float)(car.right_encoder_data) / 20;
//     }
//     /*
//     姿态解算 陀螺仪数据
//     5ms 解算一次pitch和roll
//     1ms解算一次yaw 提高更新频率，减少漂移
//     */

//     // if (angle_f.flag || angle_t.flag) // angle
//     //     flag_G.par = 1;
//     // else
//     //     flag_G.par = 0;

//     // Attitude_Algorithm();
//     if (angle_f.flag || angle_t.flag) // angle
//     {
//         Attitude_Algorithm();
//     }
//     else
//     {
//         icm20602_get_gyro(); // 读取陀螺仪数据
//         car.gyro_x = icm20602_gyro_x - (int)(GyroOffset.Xdata);
//         car.gyro_y = icm20602_gyro_y - (int)(GyroOffset.Ydata);
//         car.gyro_z = icm20602_gyro_z - (int)(GyroOffset.Zdata);
//     }
//     // Angular velocity low-pass filtering
//     static int16 lastGyro;
//     static float a = 0.6;
//     car.gyro_y = (1 - a) * car.gyro_y + a * lastGyro;
//     lastGyro = car.gyro_y;

//     if (speed_t.flag) // travel wheel speed
//     {
//         car.travel_encoder_data = encoder_get_count(ENCODER_1); // 获取编码器计数
//         car.speed = (float)(car.travel_encoder_data) * EN2V;
//         encoder_clear_count(ENCODER_1); // 清空编码器计数s
//     }

// #if FLYWHEEL
//     //-----(‵▽′)/---------------------- 飞轮转向 -----------------------------//
//     /*
//     速度环          30ms
//     角度换（误差环） 10ms
//     角速度环        2ms
//     */
//     // 速度环
//     // if (speed_turn.flag)
//     // {
//     //     car.fspeedTurn = (float)(car.left_encoder_data - car.right_encoder_data) / 40; // 计算转向差速
//     //     turn_Velocity(&pid.speed_turn, 0, car.fspeedTurn);
//     // }

//     // 转向 中线误差环
//     /*****************************
//      * 转向的值  为了保证路障通过的准确性（摄像头数据刷新不稳定，所以每2ms固定更新一次数据，而不是依靠摄像头刷新数据）
//      * ******************************/
//     // if (angSpe_turn.flag)
//     // {
//     //     turnVal = set_turnVal(camera.conErr); // 转向的值
//     // }

//     /*
//   飞轮转弯的角度换 v （误差环）的计算是取决于摄像头程序的刷新速度，所以不是固定时间
//   v2.8版本的摄像头程序刷新速度为13ms （75Hz）*/

//     if (flag_G.fps_fresh) // 摄像头刷新
//     {
//         flag_G.fps_fresh = 0;
//         turnTar = set_turnVal(-camera.fonErr, car.dirOPEN); // 转向的值
//         // printf("%f,", turnTar);
//         // turnVal += (turnTar - turnVal) * 0.7; // 一阶低通滤波

//         turnVal = turnTar;
//         float max_turnVal = 45, min_turnVal = -45;

//         if (turnVal > max_turnVal)
//         {
//             turnVal = max_turnVal;
//         }
//         else if (turnVal < min_turnVal)
//         {
//             turnVal = min_turnVal;
//         }
//         printf("%f\n", turnVal);
//         turn_midLin(&pid.midLin_turn, 0, turnVal);
//         // track_check();
//     }
//     // 转向 角速度环
//     if (angSpe_turn.flag)
//     {
//         turn_angspe(&pid.angSpe_turn, pid.midLin_turn.output, (float)(car.gyro_z));
//         // turn_angspe(&pid.angSpe_turn, 0, (float)(car.gyro_z));
//     }

//     //-----(‵▽′)/---------------------- 飞轮平衡 -----------------------------//
//     // fly wheel PID calculate
//     /*
//     速度环           10ms
//     角度换（误差环）  5ms
//     角速度环         1ms
//     */
//     // 速度环
//     if (speed_f.flag)
//     {
//         flypid_Velocity(&pid.speed, 0, car.fspeedBal);
//     }

//     // 角度环
//     if (angle_f.flag)
//     {
//         flypid_angle(&pid.angle, Mechanical_median_roll + pid.speed.output, car.roll);
//     }

//     // 角速度环
//     flypid_angspe(&pid.angSpe, pid.angle.output, car.gyro_y);

//     // 是否加入转向环
//     // #if ONLY_FORWARD
//     //     pid.output_r = pid.angSpe.output;
//     //     pid.output_l = pid.angSpe.output;
//     // #else
//     //     pid.output_r = pid.angSpe.output - pid.midLin_turn.output;
//     //     pid.output_l = pid.angSpe.output + pid.midLin_turn.output;
//     // #endif

//     if (flag_G.cam)
//     {
//         pid.output_r = pid.angSpe.output - pid.angSpe_turn.output;
//         pid.output_l = pid.angSpe.output + pid.angSpe_turn.output;
//     }
//     else
//     {
//         pid.output_r = pid.angSpe.output;
//         pid.output_l = pid.angSpe.output;
//     }

//     Limit(&pid.output_l, MAX_PWM); // 输出限幅
//     Limit(&pid.output_r, MAX_PWM);

//     if (pid.output_r > 0) // 根据占空比切换方向引脚
//     {
//         gpio_set_level(RIGHT_FLYWHEEL_DIR, RIGHT_FLYWHEEL_CLOCKWISE);
//     }
//     else
//     {
//         gpio_set_level(RIGHT_FLYWHEEL_DIR, !RIGHT_FLYWHEEL_CLOCKWISE);
//         pid.output_r = -pid.output_r;
//     }

//     if (pid.output_l > 0) // 根据占空比切换方向引脚
//     {
//         gpio_set_level(LEFT_FLYWHEEL_DIR, LEFT_FLYWHEEL_CLOCKWISE);
//     }
//     else
//     {
//         gpio_set_level(LEFT_FLYWHEEL_DIR, !LEFT_FLYWHEEL_CLOCKWISE);
//         pid.output_l = -pid.output_l;
//     }

//     if (car.status == CAR_OK || car.status == CAR_ONLYFLY)
//     {
//         pwm_set_duty(LEFT_FLYWHEEL_PWM, pc(pid.output_l)); // 输出占空比 占空比必须为正值 因此此处使用绝对值
//         pwm_set_duty(RIGHT_FLYWHEEL_PWM, pc(pid.output_r));
//     }
//     else
//     {
//         gpio_set_level(LEFT_FLYWHEEL_BRAKE, 0);
//         gpio_set_level(RIGHT_FLYWHEEL_BRAKE, 0);
//     }

// #endif
//     //-----(‵▽′)/---------------------- travel wheel -----------------------------//

// #if TRV
//     // travel wheel PID calculate
//     if (speed_t.flag) // travel wheel speed
//     {
//         // TRVPID_change(car.speed_set); // 检查是否需要改变PID参数
//         /*平滑改变速度 一阶滤波*/
//         if (car.speedSet_M != car.speed_set) // 是否改变
//         {
//             if (abs_float(car.speedSet_M - car.speed_set) > 0.002)
//                 car.speedSet_M += (car.speed_set - car.speedSet_M) * 0.04;
//             else
//                 car.speedSet_M = car.speed_set;
//         }

// #if 0
//         speedT_set = speed_smooth(car.speed_set);//使速度平滑变换
//         Velocity(&travel_pid.speed, speedT_set, car.speed);
// #else
//         Velocity(&travel_pid.speed, car.speedSet_M, car.speed); // 直接给速度
// #endif
//     }

//     if (angle_t.flag) // travel wheel angle
//     {
//         Vertical(&travel_pid.angle, Mechanical_median_pitch + travel_pid.speed.output, car.pitch);
//     }
//     if (angSpe_t.flag) // travel wheel speed
//     {
//         TRV_pid_angspe(&travel_pid.angSpe, -travel_pid.angle.output, car.gyro_x);

//         travel_pid.output = (int)(travel_pid.angSpe.output);
//         trv_output = travel_pid.output;
//         Limit(&trv_output, MAX_PWM_travel);
//         // travel wheel PWM output
//         if (trv_output > 0)
//         {
//             gpio_set_level(TRV_DIR, TRV_1_CLOCKWISE);
//         }
//         else
//         {
//             gpio_set_level(TRV_DIR, !TRV_1_CLOCKWISE);
//             trv_output = -trv_output;
//         }
//         if (car.status == CAR_OK || car.status == CAR_ONLYTRV)
//         {
//             // PWM out

//             pwm_set_duty(TRV_PWM, (trv_output + DAED_ZONE_travel));
//             // pwm_set_duty(TRV_PWM, trv_output);
//         }
//         else
//         {
//             pwm_set_duty(TRV_PWM, (0));
//         }
//     }

// #endif
//     //-----(‵▽′)/----------------------  -----------------------------//
//     if (angSpe_f.flag)
//         angSpe_f.flag = 0;
//     if (angle_f.flag)
//         angle_f.flag = 0;
//     if (speed_f.flag)
//         speed_f.flag = 0;
//     if (angSpe_t.flag)
//         angSpe_t.flag = 0;
//     if (angle_t.flag)
//         angle_t.flag = 0;
//     if (speed_t.flag)
//         speed_t.flag = 0;
//     if (midLin_turn.flag)
//         midLin_turn.flag = 0;
//     if (angSpe_turn.flag)
//         angSpe_turn.flag = 0;
//     if (speed_turn.flag)
//         speed_turn.flag = 0;
// }

// void Limit(int *val, int max)
// {
//     if (*val > max)
//         *val = max;
//     else if (*val < -max)
//         *val = -max;
// }

// float speed_smooth(float speed_set)
// {
// #define STEP_SPEED 0.05
//     static float speed_now;
//     if (speed_set > speed_now)
//     {
//         speed_now += STEP_SPEED;
//         if (speed_set < speed_now)
//         {
//             speed_now = speed_set;
//         }
//     }
//     else if (speed_set < speed_now)
//     {
//         speed_now -= STEP_SPEED;
//         if (speed_set > speed_now)
//         {
//             speed_now = speed_set;
//         }
//     }
//     else
//     {
//         speed_now = speed_set;
//     }
//     return speed_now; // 速度反向的
// }
// /*
// 开环控制
// 四个1/8圆环组成
// 恒定的误差 |13|
// 2ms刷新
// */
// #define TURN_STACK_ERR 30
// #define TURN_STACK_TIME 25

// typedef enum
// {
//     cam,
//     track,
//     crossroad,
//     breakroad
// } NAV;

// float yaw_in180(float yaw)
// {
//     if (yaw > 180)
//     {
//         yaw -= 360;
//     }
//     else if (yaw < -180)
//     {
//         yaw += 360;
//     }
//     return yaw;
// }

// float set_turnVal(float camErr, int8_t dir)
// {
//     static int8_t crossroad_Flag = 0, breakroad_Flag = 0, breakstate = 0;
//     static int16_t i = 0;
//     static float err, errSet = 0;
//     static NAV nav = cam, lastNav = cam;
//     static float yaw_m;
//     // printf("nav:%s\n",nav);
//     switch (nav)
//     {
//     case cam: // 摄像头循迹控制
//         if (car.tofVal < 100)
//         {
//             nav = track;
//             err = camErr;
//             flag_G.buzzer = 1;
//         }
//         else
//         {
//             err = camErr;
//         }
//         if (camera.stateFlag[1] == 1)
//         {
//             car.dirOPEN = camera.xDir[camera.xCount];
//             // flag_G.buzzer = 1;
//         }
//         else
//         {
//             dir = 0;
//         }
//         if (dir) // 判断是否为十字路口转弯  0 没有转向 1 向右  2向左
//         {
//             nav = crossroad;
//         }
//         if (camera.stateFlag[5] == 1)
//         {
//             nav = breakroad;
//         }
//         // err = 0;

//         break;
//     case track: // 路障开环控制（tof）
//         if (lastNav != track)
//         {
//             // flag_G.buzzer = 1;
//             printf("n%d\r\n", nav);
//         }
//         // flag_G.buzzer = 1;

//         switch (++i)
//         {
//         case 1:
//             err = TURN_STACK_ERR;
//             break;
//         case TURN_STACK_TIME * 1:
//             err = 0;
//             break;
//         case TURN_STACK_TIME + 5:
//             err = 0;
//             break;
//         case TURN_STACK_TIME * 2 + 5:
//             err = -TURN_STACK_ERR;
//             break;
//         // case TURN_STACK_TIME * 3:
//         //     err = TURN_STACK_ERR;
//         //     break;
//         case TURN_STACK_TIME * 3: // over
//             nav = cam;
//             i = 0;
//             err = camErr;
//             break;
//         }
//         break;

//     case crossroad: // 十字路口转弯
//         pidChange(1);
//         ++i;
//         car.speed_set = 2.6;
//         if (i < 2) // 刚开始转向
//         {
//             yaw_m = car.yaw;
//             // printf("@%.1f\r\n", yaw_m);
//         }

//         if (!crossroad_Flag)
//         {
//             switch (dir)
//             {
//             case 1:
//                 errSet = -TURN_STACK_ERR;
//                 break;
//             case 2:
//                 errSet = TURN_STACK_ERR;
//                 break;
//             }
//             // 一阶滤波
//             err += (errSet - err) * 0.15;
//             float ywaoo = abs_float(yaw_in180(car.yaw - yaw_m));
//             // printf("y%.0f\r\n", ywaoo);

//             if (ywaoo > 70) // 转向结束 偏航角辅助判断 不准确
//             {
//                 car.dirOPEN = 0;
//                 crossroad_Flag = 1;
//                 camera.stateFlag[1] = 0;
//                 camera.xCount += 1;
//                 car.speed_set = 4;
//             }
//         }
//         else
//         {
//             errSet = camErr;
//             err += (errSet - err) * 0.15;
//             if (abs_float(err - errSet) < 6)
//             {
//                 nav = cam;
//                 i = 0;
//                 crossroad_Flag = 0;
//                 pidChange(0);
//                 // printf("y%f\r\n", yaw_in180(car.yaw - yaw_m));
//             }
//         }

//         break;
//     case breakroad: // 断路
//         pidChange(3);
//         /***********方法一：原路返回************/

//         if (++i < 5)
//         {
//             yaw_m = car.yawPlus;
//         }
//         if (!breakroad_Flag)
//         {

//             car.speed_set = 2.8;
//             if (abs_float(car.yawPlus - yaw_m) < 75 && breakstate == 0)
//             {
//                 err = -20;
//             }
//             else
//             {
//                 breakstate = 1;
//             }
//             if (breakstate == 1)
//             {
//                 err = 35;
//                 // car.speed_set = 3.5;
//             }
//             if (abs_float(car.yawPlus - yaw_m) > 210)
//             {
//                 err = 5;
//                 breakstate = 2;
//             }
//             if (camera.image_arr[115][MT9V03X_W / 2] == 255 && breakstate == 2)
//             {
//                 breakroad_Flag = 1;
//             }
//         }
//         else
//         {
//             errSet = camErr;
//             err += (errSet - err) * 0.15;
//             if (abs_float(err - errSet) < 6)
//             {
//                 nav = cam;
//                 camera.stateFlag[5] = 0;
//                 i = 0;
//                 breakroad_Flag = 0;
//                 car.speed_set = 4;
//                 breakstate = 0;
//                 pidChange(0);
//                 // printf("y%f\r\n", yaw_in180(car.yaw - yaw_m));
//             }
//         }

//         ////////////////////////////////////方法2：按键设定转特定的方向/////////////////////////////////////////
//         pidChange(1);
//         ++i;
//         car.speed_set = 2.6;
//         if (i < 2) // 刚开始转向
//         {
//             yaw_m = car.yaw;
//             // printf("@%.1f\r\n", yaw_m);
//         }

//         if (!crossroad_Flag)
//         {
//             switch (camera.BreakDir) // 设定转动方向
//             {
//             case 1:
//                 errSet = -TURN_STACK_ERR;
//                 break;
//             case 2:
//                 errSet = TURN_STACK_ERR;
//                 break;
//             }
//             // 一阶滤波
//             err += (errSet - err) * 0.15;
//             float ywaoo = abs_float(yaw_in180(car.yaw - yaw_m));
//             // printf("y%.0f\r\n", ywaoo);

//             if (ywaoo > camera.BreakAngle) //   设定角度时时正数，只有大小没有方向  转向结束 偏航角辅助判断 不准确
//             {
//                 car.dirOPEN = 0;
//                 crossroad_Flag = 1;
//                 camera.stateFlag[1] = 0;
//                 camera.xCount += 1;
//                 car.speed_set = 4;
//             }
//         }
//         /*我不知道这里还需不需要，因为按理来说，断路之后应该就没有摄像头信息了*/
//         else
//         {
//             errSet = camErr;
//             err += (errSet - err) * 0.15;
//             if (abs_float(err - errSet) < 6)
//             {
//                 nav = cam;
//                 i = 0;
//                 crossroad_Flag = 0;
//                 pidChange(0);
//                 // printf("y%f\r\n", yaw_in180(car.yaw - yaw_m));
//             }
//         }
//         break;
//     }
//     if (lastNav != nav)
//     {
//         // printf("n%d\r\n", nav);
//         flag_G.buzzer = 1;
//     }
//     printf("%d\r,%.2f,%d\n", nav, abs_float(car.yawPlus - yaw_m), breakroad_Flag);
//     lastNav = nav;
//     return err;
// }

// void breakRoad_setAngle(void)
// {
//     // static float yaw_m;
//     // pidChange(1);
//     // ++i;
//     // car.speed_set = 2.6;
//     // if (i < 2) // 刚开始转向
//     // {
//     //     yaw_m = car.yaw;
//     //     // printf("@%.1f\r\n", yaw_m);
//     // }

//     // if (!crossroad_Flag)
//     // {
//     //     switch (camera.BreakAngle)
//     //     {
//     //     case 1:
//     //         errSet = -TURN_STACK_ERR;
//     //         break;
//     //     case 2:
//     //         errSet = TURN_STACK_ERR;
//     //         break;
//     //     }
//     //     // 一阶滤波
//     //     err += (errSet - err) * 0.15;
//     //     float ywaoo = abs_float(yaw_in180(car.yaw - yaw_m));
//     //     // printf("y%.0f\r\n", ywaoo);

//     //     if (ywaoo > camera.BreakAngle) // 转向结束 偏航角辅助判断 不准确
//     //     {
//     //         car.dirOPEN = 0;
//     //         crossroad_Flag = 1;
//     //         camera.stateFlag[1] = 0;
//     //         camera.xCount += 1;
//     //         car.speed_set = 4;
//     //     }
//     // }
//     // else
//     // {
//     //     errSet = camErr;
//     //     err += (errSet - err) * 0.15;
//     //     if (abs_float(err - errSet) < 6)
//     //     {
//     //         nav = cam;
//     //         i = 0;
//     //         crossroad_Flag = 0;
//     //         pidChange(0);
//     //         // printf("y%f\r\n", yaw_in180(car.yaw - yaw_m));
//     //     }
//     // }
// }
