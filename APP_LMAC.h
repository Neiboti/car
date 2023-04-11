#ifndef __APP_LMAC_H__
#define __APP_LMAC_H__
#include "APP_control.h"
#include "APP_UDP_agree.h"


void APP_HMAC_TX_main();							//上位机休眠计时

void APP_HMAC_TX_init();							//UDP上报函数初始化

void APP_set_handle_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);	//远程手柄控制帧数据处理

void LMC_set_handle_key(UDP_Agree* agree);			//远程手柄按键响应

void LMC_set_handle_rocker(UDP_Agree* agree);		//远程手柄摇杆响应

//请求控制权操作数据处理
void APP_ask_connection_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//控制权数据反馈
void APP_ask_connection_TX(uint8_t ok, uint32_t IP, uint16_t prot);

//心跳报文帧数据处理
void APP_ask_HPC_heart_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//心跳报文数据反馈
void APP_ask_HPC_heart_TX(uint32_t IP, uint16_t prot);

//请求速度帧数据处理
void APP_ask_speed_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//速度数据反馈
void APP_ask_speed_TX(uint32_t format, uint32_t IP, uint16_t prot);

//请求电流帧数据处理
void APP_ask_current_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//电流数据反馈
void APP_ask_current_TX(uint32_t format, uint32_t IP, uint16_t prot);

//请求温度帧数据处理
void APP_ask_temp_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//温度数据反馈
void APP_ask_temp_TX(uint32_t format, uint32_t IP, uint16_t prot);

//设置ECU开关数据处理
void APP_set_ECU_sth_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//设置ECU开关帧回应
void APP_set_ECU_sth_TX(uint32_t ok, uint32_t IP, uint16_t prot);

//设置控制方式数据处理
void APP_set_control_mod_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//设置控制方式帧回应
void APP_set_control_mod_TX(uint32_t ok, uint32_t IP, uint16_t prot);

//设置 曲率控制 数据处理
void APP_set_curvature_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

///设置 线速度-角速度控制 数据处理
void APP_set_angular_velocity_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//请求开关抱闸数据处理
void APP_ask_ask_axis_lock_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot);

//请求开关抱闸回应
void APP_ask_ask_axis_lock_TX(uint8_t ok, uint32_t IP, uint16_t prot);

#endif
