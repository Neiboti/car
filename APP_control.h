#ifndef __APP_control_H__
#define __APP_control_H__
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

//打印数据开关
#define printf_control_mod 1				//控制模式信息打印
#define printf_abnormal 1					//警报信息打印
#define printf_UDP_buff_capacity_query 0	//UDP相关信息
#define printf_CAN_buff_capacity_query 0	//CAN相关信息
//数据第N位宏定义

#define bit_0				0x0001				//第0位
#define bit_1				0x0002				
#define bit_2				0x0004				
#define bit_3				0x0008				
#define bit_4				0x0010				
#define bit_5				0x0020			
#define bit_6				0x0040			
#define bit_7				0x0080				
#define bit_8				0x0100				
#define bit_9				0x0200
#define bit_10				0x0400				
#define bit_11				0x0800				
#define bit_12				0x1000				
#define bit_13				0x2000				
#define bit_14				0x4000				
#define bit_15				0x8000				

//常用数据定义
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

//警报标志位定义
#define state_undefined 0				//对应开机状态，未更新警报信息
#define state_safe 1					//安全，未触发警报
#define state_warning 2					//触发警报
#define state_error 3					//触发错误

//离线标志位定义
#define state_off_line 2				//离线

//------------------模式宏定义
#define mod_zero 0				//零模式、此模式下不进行任何控制类操作（消息上报等不涉及底盘运动的程序依然会运行，包括灯控），等待其他模式抢占（优先级3）
#define mod_autonomy_on 1		//自主模式   （优先级4）
#define mod_autonomy_off 2		//自主模式   （优先级4）
#define mod_remote_on 3			//远程模式（优先级2）
#define mod_remote_off 4		//远程模式（优先级2）
#define mod_scene_on 5			//现场模式（优先级1，最高）
#define mod_scene_off 6			//现场模式（优先级1，最高）

#define mod_DEBUG_on 0x8000		//调试模式
#define mod_DEBUG_off 0x4000	//调试模式


//------------------电机驱动型号宏定义
#define ESC_SXLD 0x01 	                  //山西零度电机驱动
#define ESC_BDW  0x02 	                  //八达威电机驱动

//------------------系统相关参数宏定义

#define HMAC_heart_interval_enable 1		//上位机心跳检测：0关，1开
#define HMAC_heart_interval_max 500			//上位机心跳报文最大间隔时间，单位ms

#define motor_heart_interval_enable 1		//电驱心跳检测：0关，1开
#define motor_heart_interval_max 500		//电驱心跳报文最大间隔时间，单位ms

#define ECU_heart_interval_enable 1			//ECU心跳检测：0关，1开
#define ECU_heart_interval_max 2000			//ECU心跳报文最大间隔时间，单位ms

//中控系统参数结构体
typedef struct ControL_sys
{
	uint8_t ESC;					//电机驱动型号、1：山西零度、2：八达威
	uint8_t mod;					//控制模式：0空模式、1自主模式、3远程模式、5现场模式


	uint16_t time_ms;				//系统时间
	uint16_t time_s;
	uint16_t time_mis;

}ControL_sys;

//中控系统参数结构体_设定
typedef struct ControL_set
{
	int32_t v1;				//设定速度
	int32_t v2;				//设定速度
	int32_t v3;				//设定速度
	int32_t v4;				//设定速度

}ControL_set;

//中控系统参数结构体
typedef struct ControL_act
{
	int32_t v1;				//实际速度
	int32_t v2;				//实际速度
	int32_t v3;				//实际速度
	int32_t v4;				//实际速度

	float ms_v1;				//实际速度：单位M/S
	float ms_v2;				//实际速度：单位M/S
	float ms_v3;				//实际速度：单位M/S
	float ms_v4;				//实际速度：单位M/S

	float ms_vhe;				//实际总速度：单位M/S
	float kmh_vhe;				//实际总速度：单位kM/h
	uint32_t 	i1;				//实际电流
	uint32_t 	i2;				//
	uint32_t 	i3;				//
	uint32_t 	i4;				//

	uint32_t 	t1;				//实际温度
	uint32_t 	t2;				//
	uint32_t 	t3;				//
	uint32_t 	t4;				//

}ControL_act;

//中控系统参数结构体
typedef struct ControL_remote
{
	int32_t vx;				//远程X_W式控制的X速度
	int32_t vw;				//远程X_W式控制的W速度

}ControL_remote;

//中控数据结构体
typedef struct ControL
{
	ControL_sys sys;				//中控系统参数

	ControL_set set;				//控制变量设定值

	ControL_act act;				//控制变量实际值

	ControL_remote remote;			//远程控制数据

}ControL;



void APP_CONTROL_init(void);				//中控初始化

void control_mod_change(uint32_t MarK);		//模式切换

void APP_CONTROL();							//中控主函数


void APP_sys_time();						//计时函数

void APP_printf(uint16_t time);				//数据打印

//计算数据
void mathematics_data(uint16_t TimE);
//X-W式带失速修正版
void Chassis_Given_Speed_revise(float Vx, float Vw, uint16_t cyc, uint16_t TimE);

#endif
