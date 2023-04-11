#ifndef __APP_UDP_AGREE_H__
#define __APP_UDP_AGREE_H__
#include "APP_control.h"



//类型



//-------------------------------------自定义功能--------------------------------------------------
//帧头

#define UDP_LPC_ask_RX 0xFFAA					//下位机接受请求帧头
#define UDP_LPC_set_RX 0xFFBA					//下位机接受设置帧头
#define UDP_LPC_ask_TX 0xFFA9					//下位机回应请求帧头
#define UDP_LPC_set_TX 0xFFB9					//下位机回应设置帧头


//帧类型
#define set_handle_control 0x0001			//手柄数据
#define ask_connection 0x0002				//请求连接
#define ask_HPC_heart 0x0003				//上位机心跳报文
#define ask_speed 0x0004					//请求速度
#define ask_current 0x0005					//请求电流
#define ask_temp 0x0006						//请求温度

#define set_ECU_sth 0x0007					//设置ECU开关
#define set_control_mod 0x0008				//设置控制模式

#define ask_electrical_information 0x0009	//请求电气信息

#define set_speed_angular 0x000A			//设置线速度-角速度
#define set_speed_curvature 0x000B			//设置线速度-曲率

#define set_ball_head_XY 0x000D				 //设置云台XY速度
#define ask_axis_lock 0x000F				//请求开关闸

//远程手柄按键

#define key_Y 0X80								//按键Y
#define key_B 0X40								//按键B
#define key_A 0X20								//按键A
#define key_X 0X10								//按键X

#define key_start 0X08							//按键start
#define key_back 0X04							//按键back
#define key_LB 0X02								//按键LB
#define key_RB 0X01								//按键RB

//连接帧
#define connection_max	16						//最大同时连接上位机数

//控制数据类型
#define speed_angular_remote		0X01		//按键Y
#define speed_angular_autonomy		0X02		//按键B
#define speed_curvature_remote		0X03		//按键A
#define speed_curvature_autonomy	0X04		//按键X





//协议手柄控制帧
typedef struct UDP_Agree_handle
{
	//手柄控制数据、功能帧0x0001
	int16_t LX_1;									//左摇杆X轴、16位
	int16_t LY_1;									//左摇杆Y轴、16位
	int16_t RX_1;									//右摇杆X轴、16位
	int16_t RY_1;									//右摇杆Y轴、16位
	uint8_t cross_key;								//十字按键
	uint8_t rule_key;								//普通按键

}UDP_Agree_handle;

//----------------------------------------------------------------------请求连接帧

//连接帧主结构体
typedef struct UDP_Agree_connection
{

	uint32_t IP;									//IP
	uint16_t prot;									//端口号
	uint16_t code;									//当前控制者编号
	uint8_t state;									//上位机状态帧
	uint32_t heart_interval_time;					//心跳间隔

}UDP_Agree_connection;

//协议心跳帧
typedef struct UDP_Agree_heart
{
	//协议心跳帧0x0003
	uint32_t set_cmd_time;				//上位机时间

	uint16_t reality_ECU_state;			//当前ECU开关状态
	uint16_t reality_alert_number;		//警报数量
	uint16_t reality_error_number;		//错误数量
	uint16_t reality_battery_level;		//电池电量
	uint16_t reality_control_mod;		//当前控制模式
	uint16_t reality_heart_code;		//本次心跳编号

}UDP_Agree_heart;

//请求速度帧
typedef struct UDP_Agree_speed
{
	//请求速度帧0x0004
	uint32_t set_format;							//数据格式/单位
	uint16_t v1;									//速度1
	uint16_t v2;									//速度2
	uint16_t v3;									//速度3
	uint16_t v4;									//速度4
}UDP_Agree_speed;

//请求电流帧
typedef struct UDP_Agree_current
{
	//请求电流帧0x0005
	uint32_t set_format;							//数据格式/单位
	uint16_t i1;									//电流1
	uint16_t i2;									//电流2
	uint16_t i3;									//电流3
	uint16_t i4;									//电流4
}UDP_Agree_current;

//请求温度帧
typedef struct UDP_Agree_temp
{
	//请求温度帧0x0006
	uint32_t set_format;							//数据格式/单位
	uint16_t t1;									//温度1
	uint16_t t2;									//温度2
	uint16_t t3;									//温度3
	uint16_t t4;									//温度4
}UDP_Agree_temp;

//ECU控制帧
typedef struct UDP_Agree_ECU_set
{
	//协议心跳帧0x0007

	uint32_t sth_code;			//ECU开关编号
	uint32_t sth_time;			//ECU闪烁时间
	uint8_t make;				//标志位

}UDP_Agree_ECU_set;

//控制模式帧
typedef struct UDP_Agree_control_mod
{
	//协议心跳帧0x0008

	uint32_t mod;				//控制模式编号
	uint16_t code;				//设备编号

}UDP_Agree_control_mod;

//请求电气信息
typedef struct UDP_Agree_electrical
{
	//请求电流帧0x0005
	uint32_t set_cyc;								//自动上报时间，0关、单位MS
	uint32_t set_format;							//数据格式/单位
	uint16_t reality_program_drive;					//程序版本号及电驱型号
	uint16_t reality_electrical;					//电气代号
}UDP_Agree_electrical;

//曲率控制
typedef struct UDP_auto_curvature
{
	int16_t speed;									//设定速度M/S，最小分辨率0.01M/S
	int16_t curvature;								//设定曲率，最小分辨率0.0001
	int32_t latitude;								//纬度
	int32_t longitude;								//经度
	int32_t height;									//高度
	int16_t course;									//航向

}UDP_curvature;

//线速度角速度控制
typedef struct UDP_angular_velocity
{
	int16_t speed;								//设定速度M/S，最小分辨率0.01M/S
	int16_t angular;								//设定角速度，最小分辨率0.0001

}UDP_angular_velocity;

//UDP上报
typedef struct UDP_Agree_upload
{
	uint64_t IP;				//IP
	uint16_t prot;				//端口号
	uint8_t mark;				//标志，0关闭，1开启

}UDP_Agree_upload;

typedef struct UDP_Agree							//UDP数据结构体定义
{

	UDP_Agree_handle handle;						//手柄帧
	UDP_Agree_connection connection;				//连接帧
	UDP_Agree_heart heart;							//心跳帧
	UDP_Agree_speed speed;							//速度帧
	UDP_Agree_current current;						//电流帧
	UDP_Agree_temp temp;							//温度帧
	UDP_Agree_ECU_set ECU;							//ECU设置帧
	UDP_Agree_control_mod control_mod;				//控制方式设置帧

	uint16_t control_mark;							//控制数据类型
	UDP_auto_curvature cte;							//曲率控制结构体
	UDP_angular_velocity angular_velocity;			//线速度角速度结构体


}UDP_Agree;

//UDP接收数据解析总函数
void UDP_agree_read(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//请求帧
void custom_agree_ask(uint16_t Type, uint8_t* readbuff, uint32_t IP, uint16_t prot);

//设置帧
void custom_agree_set(uint16_t Type, uint8_t* readbuff, uint32_t IP, uint16_t prot);


#endif