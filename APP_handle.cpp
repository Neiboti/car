#include "APP_handle.h"
#include "driver_handle.h"
#include "APP_ECU_agree.h"
#include "APP_control.h"
#include "APP_abnormal.h"

HandlE handle_data = { 0 };

extern xbox_map_t map;

extern ControL control;//中控函数结构体
extern AbnormaL state;
//
void handle_control(void)
{
	if (map.a == 1)                                                 //按下A开启抱闸
	{
		ECU_light(axis_lock_on, 0);									//抱闸开
		ECU_light(power_motor_on, 0);								//驱动器供电开

		state.motor_uninit = state_warning;				//标记驱动器未初始化

		//system("cansend can0 11000002#FFFF000000000000");			  //开抱闸
	}
	if (map.b == 1)
	{
		ECU_light(axis_lock_off, 0);								  //抱闸关
		ECU_light(power_motor_off, 0);								  //驱动器供电关


		//system("cansend can0 11000002#0000000000000000");  //关抱闸
	}
	if (map.back == 1)
	{
		control_mod_change(mod_scene_off);					//关闭现场控制模式
	}
	if (map.start == 1)
	{
		control_mod_change(mod_scene_on);					//开启现场控制模式
	}

	if (map.y == 1)                                                 //按下A开启抱闸
	{
		ECU_light(power_headlight_on, 0);							//大灯开
	}
	if (map.x == 1)
	{
		ECU_light(power_headlight_off, 0);							//大灯关
	}

	if (map.rb == 1)
	{
		control_mod_change(mod_autonomy_on);							//开启自主控制模式
	}
	if (map.lb == 1)
	{
		control_mod_change(mod_autonomy_off);							//开启自主控制模式
	}
	//手柄数据打印
	//printf("LX:%d   LY:%d   RX:%d   RY:%d\n", map.lx, map.ly, map.rx, map.ry);
}

//现场手柄异常：现场手柄离线
uint8_t APP_scene_handle_abnormal()
{
	if(handle_data.setat  !=2 )return 0;
	else return 1;
}