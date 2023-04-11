#include "APP_control.h"
#include "driver_handle.h"
#include "APP_chassis.h"
#include "APP_ECU_agree.h"
#include "APP_abnormal.h"
#include "APP_FOL.h"
#include "APP_UDP_agree.h"
#include "APP_LMAC.h"
#include "driver_UDP.h"
#include "driver_CAN.h"

ControL control = { 0 };

extern xbox_map_t map;

extern UDP_Agree Agree;


//模式切换
void control_mod_change(uint32_t MarK)
{
	switch (MarK)
	{
		//设置为零模式
	case (mod_zero):control.sys.mod = mod_zero; break;
		//关闭自主模式
	case (mod_autonomy_off):if (control.sys.mod == mod_autonomy_on)control.sys.mod = mod_zero; break;
		//关闭远程模式
	case (mod_remote_off):if (control.sys.mod == mod_remote_on)control.sys.mod = mod_zero; break;
		//关闭现场模式
	case (mod_scene_off):if (control.sys.mod == mod_scene_on)control.sys.mod = mod_zero; break;
		//开启自主模式
	case (mod_autonomy_on):if (control.sys.mod == mod_zero)control.sys.mod = mod_autonomy_on; break;
		//开启远程模式
	case (mod_remote_on):if ((control.sys.mod == mod_zero) || (control.sys.mod == mod_autonomy_on))control.sys.mod = mod_remote_on; break;
		//开启现场模式
	case (mod_scene_on):if ((control.sys.mod == mod_zero) || (control.sys.mod == mod_autonomy_on) || (control.sys.mod == mod_remote_on))control.sys.mod = mod_scene_on; break;
	}

	if (control.sys.mod == mod_autonomy_on)
	{
		Agree.connection.IP = IP_UDP_TX_init(zizhu_IP);
		Agree.connection.code = 0;
	}

	if (control.sys.mod == mod_zero)
	{
		Agree.connection.IP = 0;
		Agree.connection.code = 0;
	}

}

//中控初始化
void APP_CONTROL_init()
{
	//control.sys.ESC = ESC_BDW;			//设置电机驱动为八达威
	control.sys.ESC = ESC_SXLD;				//设置电机驱动为山西零度
	control.sys.mod = mod_zero;				//设置控制模式为零模式

	if (control.sys.ESC == ESC_BDW)printf("当前驱动器型号为：八达威\n");
	if (control.sys.ESC == ESC_SXLD)printf("当前驱动器型号为：山西零度\n");

	printf("中控初始化成功\n");
}

//底盘控制
void APP_chassis_control(uint8_t mod, uint16_t TimE)
{
	//底盘控制
	switch (mod)
	{
		//零控制模式
	case (mod_zero):
	{
		Chassis_Given_Speed(0, 0, 100, TimE);
	}
	break;
	//自主控制模式
	case (mod_autonomy_on):
	{
		//Chassis_Given_Speed_revise_table(0, 417 * 6, 100, TimE);
		switch (Agree.control_mark)
		{
		case (speed_curvature_autonomy):
		{
			//Chassis_Given_Curvature(Agree.cte.speed, Agree.cte.curvature, 100, TimE);
		}
		break;
		case (speed_angular_autonomy):
		{

			Chassis_Given_Speed_revise_table(Agree.angular_velocity.speed, Agree.angular_velocity.angular, 100, TimE);
			//printf("VX:%d     VW:%d\n", Agree.angular_velocity.speed, Agree.angular_velocity.angular);
		}
		break;
		}
	}
	break;
	//远程控制模式
	case (mod_remote_on):
	{
		switch (Agree.control_mark)
		{
		case (speed_curvature_remote):
		{
			//Chassis_Given_Curvature(Agree.cte.speed, Agree.cte.curvature, 100, TimE);
			//printf("VX:%f     VW:%f\n", Agree.angular_velocity.speed, Agree.angular_velocity.angular);
		}
		break;
		case (speed_angular_remote):
		{

			Chassis_Given_Speed_FOL_revise(Agree.angular_velocity.speed, Agree.angular_velocity.angular, 100, TimE);
			//Chassis_Given_Speed_FOL_revise(Agree.angular_velocity.speed, Agree.angular_velocity.angular, 100, TimE);
			//Chassis_Given_Speed(Agree.angular_velocity.speed * 0.5, Agree.angular_velocity.angular * 0.5, 100, TimE);
			//printf("VX:%d     VW:%d\n", Agree.angular_velocity.speed, Agree.angular_velocity.angular);

		}
		break;
		}
	}
	break;
	//现场控制模式
	case (mod_scene_on):
	{
		//Chassis_Given_Speed_FOL(-0.1f * map.ly, 0.08f * map.rx, 100, TimE);
		//临时测试
		Chassis_Given_Speed_FOL_revise(-0.35f * map.ly, 0.35f * map.rx, 100, TimE);
	}
	break;
	}
}

//中控主函数
void APP_CONTROL(void)
{
	request_data(control.sys.time_ms);											//请求数据

	mathematics_data(control.sys.time_ms);									//计算数据

	APP_abnormal();																//异常处理模块

	if (APP_abnormal_consult() != state_error)									//无错误
	{
		APP_chassis_control(control.sys.mod, control.sys.time_ms);				//发送控制数据
	}
	else
	{
		//停止车辆
		Chassis_Given_Speed(0, 0, 100, control.sys.time_ms);
	}

	APP_HMAC_TX_main();															//UDP上报数据

	APP_printf(control.sys.time_ms);											//数据打印

	APP_sys_time();																//系统计时

}

//系统计时
void APP_sys_time()
{
	usleep(1000);
	control.sys.time_ms++;
	if (control.sys.time_ms >= 1000)
	{
		control.sys.time_ms = 0;
		control.sys.time_s++;
		if (control.sys.time_s >= 60)
		{
			control.sys.time_s = 0;
			control.sys.time_mis++;
			if (control.sys.time_mis >= 65535)
			{
				control.sys.time_mis = 0;
			}
		}
	}
}
int16_t ijk = 0;
//数据打印
void APP_printf(uint16_t time)
{
	//手柄数据
	//printf("LY:%d          RX:%d\n", map.ly, map.rx);

	////远程     X——W底盘控制设定值
	//if ((time % 500) == 0)
	//{
	//	printf("remotevx:%d          remotevw:%d\n", control.remote_vx, control.remote_vx);
	//	printf("mod:::%d\n", control.sys.mod);
	//}

	//控制模式显示
	if ((time == 0) && (printf_control_mod == 1))
	{
		if (control.sys.mod == mod_zero)printf("当前模式--->待命");
		if (control.sys.mod == mod_autonomy_on)printf("当前模式--->自主");
		if (control.sys.mod == mod_remote_on)printf("当前模式--->远程");
		if (control.sys.mod == mod_scene_on)printf("当前模式--->现场");

		printf("    当前控制者：%4X 控制者IP：%d", Agree.connection.code, Agree.connection.IP);


		printf("\n");
	}

	//UDP缓冲区占用率,总收发量
	if ((time == 0) && (printf_UDP_buff_capacity_query == 1))
	{
		UDP_state_printf();
		CAN_state_printf();
		printf("\n");
	}

}



//计算数据
void mathematics_data(uint16_t TimE)
{
	if (TimE % 100 == 0)
	{
		control.act.ms_v1 = Speed_conversion_MS(control.act.v1);
		control.act.ms_v2 = Speed_conversion_MS(control.act.v2);
		control.act.ms_v3 = Speed_conversion_MS(control.act.v3);
		control.act.ms_v4 = Speed_conversion_MS(control.act.v4);

		control.act.ms_vhe = (-control.act.ms_v1 + control.act.ms_v2 + control.act.ms_v3 - control.act.ms_v4) / 4;
		control.act.kmh_vhe = control.act.ms_vhe * 3.6f;

		//printf("VL:%f  VR:%f\n", control.act.ms_v1, control.act.ms_v2);

		//printf("当前速度：%.2fM/S\n", control.act.ms_vhe);
		//printf("当前速度：%.2fKM/h\n", control.act.kmh_vhe);
	}
}













