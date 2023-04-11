#include "APP_UDP_agree.h"
#include "driver_UDP.h"
#include "APP_control.h"
#include "APP_LMAC.h"
#include "APP_ECU_agree.h"
#include "APP_abnormal.h"
#include "APP_LMAC.h"

UDP_Agree Agree = { 0 };					//UDP解析后的数据

extern AbnormaL state;
extern ControL control;



//UDP帧内容读取
void UDP_agree_read(uint8_t* readbuff,uint32_t IP,uint16_t prot)
{
	uint16_t HeadeR = 0, TypE = 0;            //帧头、功能帧

	HeadeR = (readbuff[0] << 8) | readbuff[1];
	TypE = (readbuff[2] << 8) | readbuff[3];

	switch (HeadeR)
	{

		//收到请求帧：0XFFAA
	case (UDP_LPC_ask_RX):  custom_agree_ask(TypE, readbuff,IP,prot);
		break;

		//收到设置帧：0XFFBA
	case (UDP_LPC_set_RX):  custom_agree_set(TypE, readbuff, IP, prot);
		break;

	}

}







//请求帧
void custom_agree_ask(uint16_t Type, uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	switch (Type)
	{
		//请求获取控制权
	case (ask_connection):APP_ask_connection_RX(readbuff, IP, prot); break;
		//心跳报文
	case (ask_HPC_heart):APP_ask_HPC_heart_RX(readbuff, IP, prot); break;
		//请求速度数据处理
	case(ask_speed):APP_ask_speed_RX(readbuff, IP, prot); break;
		//请求电流数据处理
	case(ask_current):APP_ask_current_RX(readbuff, IP, prot); break;
		//请求温度数据处理
	case(ask_temp):APP_ask_temp_RX(readbuff, IP, prot); break;
		//请求抱闸
	case(ask_axis_lock):APP_ask_ask_axis_lock_RX(readbuff, IP, prot); break;

	}
}

//设置帧
void custom_agree_set(uint16_t Type, uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//检验此用户是否拥有UDP控制权
	if ((IP == Agree.connection.IP))
	{
		switch (Type)
		{

			//手柄控制 数据处理
		case (set_handle_control): APP_set_handle_control_RX(readbuff, IP, prot); break;
			//设置 ECU 数据处理
		case(set_ECU_sth):APP_set_ECU_sth_RX(readbuff, IP, prot); break;
			//设置 控制模式 数据处理
		case(set_control_mod):APP_set_control_mod_RX(readbuff, IP, prot); break;
			//设置 线速度-角速度控制 数据处理
		case(set_speed_angular):APP_set_angular_velocity_control_RX(readbuff, IP, prot); break;
			//设置 曲率控制 数据处理
		case(set_speed_curvature): break;
			//设置 云台控制 数据处理
		case(set_ball_head_XY): break;

		}
	}
	else
	{
		//无控制权
		APP_ask_connection_TX(0, IP, prot);
	}



}
