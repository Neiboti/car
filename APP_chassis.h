#ifndef __APP_chassis_H__
#define __APP_chassis_H__

#include "APP_control.h"



#define Wheel_D 1.7 	                  //轮子直径
#define Reduction_ratio 0.05              //减速比
#define Track 1.2						  //轮距
#define Pi 3.1415						  //π

#define Ten_thousandth_ratio_MAX 12000		//万分比最大值
#define Ten_thousandth_ratio_MIN -12000		//万分比最小值

#define tyre_rmp_max 3600				  //单轮转速上限单位：RMP/min
#define tyre_rmp_min -3600				  //单轮转速上限单位：RMP/min

#define linear_speed_max_ms 5.f				  //线速度上限单位：5m/s

#define linear_angular_max_as 120.f			  //角速度上限，120°/s				


typedef struct chassis					//底盘输出结构体
{
	int32_t v1;							//轮1速度
	int32_t v2;							//轮2速度
	int32_t v3;							//轮3速度
	int32_t v4;							//轮4速度



}chassis;

void Chassis_Given_Speed(float Vx, float Vw, uint16_t cyc, uint16_t TimE);

void motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4);

void Chassis_Given_Curvature(float Vx, float Vc, uint16_t cyc, uint16_t TimE);

void request_data(uint16_t TimE);

uint8_t chassis_motor_init(uint16_t TimE);		//电机驱动初始化



//X-W式带滤波版本
void Chassis_Given_Speed_FOL(float Vx, float Vw, uint16_t cyc, uint16_t TimE);

float Speed_conversion_MS(int rpm);

//X-W式带滤波、带失速修正版
void Chassis_Given_Speed_FOL_revise(float Vx, float Vw, uint16_t cyc, uint16_t TimE);

//X-W式带失速修正版-2
void Chassis_Given_Speed_revise_table(float Vx, float Vw, uint16_t cyc, uint16_t TimE);



#endif




