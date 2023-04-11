#ifndef __APP_SXLD_agree_H__
#define __APP_SXLD_agree_H__
#include "APP_control.h"
#include "driver_CAN.h"

#define LD_command_rx 0x01 	                      //应答回复“请求读取寄存器”的功能码
#define LD_command_set 0x05						  //应答回复“请求设置寄存器”的功能码

#define LD_drive_id_0x09   0x09				      //驱动ID
#define LD_drive_id_0x0A   0x0A				      //驱动ID

#define LD_slide_rx_max   32					  //滑动池大小上限

//识别码宏定义
#define LD_actual_sys_state1_2 0x78				  //系统状态字1-2
#define LD_actual_sys_state3_4 0x7A				  //系统状态字3-4
#define LD_actual_speed      0x84				  //实际转速L-R
#define LD_actual_current    0x8E				  //实际电流L-R				
#define LD_actual_power      0x90				  //实际功率L-R	
#define LD_actual_tempH      0x92				  //实际温度1-2					
#define LD_actual_tempL      0x94				  //实际温度1-2	
#define LD_actual_torque     0X9E				  //实际扭矩L-R	

typedef struct  SXLD_Data_state3					//系统状态字3
{
	uint8_t sys_inti;								//系统初始化标志位、0未初始化、1已初始化
	uint8_t sys_run_end;								//系统使能标志位，0未使能、1已使能
	uint8_t mot_sp_ze_2;							//电机2是否处于零速，						
	uint8_t mot_sp_ze_1;							//电机1是否处于零速，		
	uint8_t mot_sp_at_2;							//电机2是否处于加速，						
	uint8_t mot_sp_at_1;							//电机1是否处于加速，
	uint8_t sys_run_2;								//电机2是否处于工作状态
	uint8_t sys_run_1;								//电机1是否处于工作状态
	uint8_t mot_dir_2;								//电机2是否处于正转，						
	uint8_t mot_dir_1;								//电机1是否处于正转，
	uint8_t mot_pwm_2;								//是否发送PWM波，						
	uint8_t mot_pwm_1;									
	uint8_t mot_lim_2;								//是否限流						
	uint8_t mot_lim_1;
	uint8_t mot_pwm_int_2;							//DIN8端子是否有输入					
	uint8_t mot_pwm_int_1;							//DIN7端子是否有输入

} SXLD_Data_state3;




typedef struct  SXLD_Data					//山西零度电机驱动返回数据结构体定义
{
	SXLD_Data_state3 state3;				//系统标志位3




	uint16_t sys_state_1;					//系统状态字1
	uint16_t sys_state_2;					//系统状态字1

	int16_t speed_L;						//实际转速L
	int16_t speed_R;						//实际转速R

	uint16_t current_L;						//实际电流L
	uint16_t current_R;						//实际电流R

	uint16_t power_L;						//实际功率L
	uint16_t power_R;						//实际功率R

	float tempH_1;						    //实际温度1
	float tempH_2;						    //实际温度2

	float tempL_1;					    	//实际温度3
	float tempL_2;						    //实际温度4

	uint16_t torque_L;						//实际扭矩L
	uint16_t torque_R;						//实际扭矩R



} SXLD_Data;








uint8_t SXLD_mot_enable(uint16_t cyc, uint8_t tery);						//电驱使能/初始化

void SXLD_motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4);

void SXLD_cycle_request_data(uint16_t time);
//CAN报文数据处理
void SXLD_can_data_recive_storage(CAN_Data* DatA);


uint8_t SXLD_abnormal_uninit(uint8_t id);					//山西零度电驱异常：未初始化返回0，初始化返回1



#endif