#include "APP_abnormal.h"
#include "APP_control.h"
#include "APP_ECU_agree.h"
#include "APP_chassis.h"
#include "APP_SXLD_agree.h"
#include "APP_handle.h"
#include "driver_UDP.h"


AbnormaL state = { 0 };				//异常状态结构体
extern ControL control;				//中控函数结构体

//异常模块初始化
void APP_abnormal_init()
{
	state.abnormal_check_cyc = 1000;

	state.motor_uninit = state_error;

	printf("车身自检系统初始化完成。\n");
}

//异常状态查询
uint8_t APP_abnormal_consult()
{
	return state.whole;
}


//异常点信息输入（异常代码，标志）
void APP_abnormal_int(uint16_t abnl, uint8_t mark)
{
	switch (abnl)
	{
		//上位机离线
	case (code_HMAC_off_line):state.HMAC_offline = mark; break;
		//前驱离线
	case (code_motor1_off_line):state.motor1_offline = mark; break;
		//后驱离线
	case (code_motor2_off_line):state.motor2_offline = mark; break;
		//ECU离线
	case (code_ECU_off_line):state.ECU_offline = mark; break;
		//抱闸未打开
	case (code_handbrake_lock):state.handbrake = mark; break;
		//驱动未初始化
	case (code_motor_uninit):state.motor_uninit = mark; break;
		//前驱未就绪
	case (code_motor1_uninit):state.motor1_uninit = mark; break;
		//后驱未就绪
	case (code_motor2_uninit):state.motor2_uninit = mark; break;
		//现场手柄离线
	case (code_scene_handle_unoffline):state.scene_handle = mark; break;
		//UDP故障
	case (code_UDP_breakdown):state.UDP_state = mark; break;

	}
}


//抱闸检测
void APP_handbrake_testing()
{
	if (ECU_handbrake_check() == 1)
	{
		//抱闸已都打开
		APP_abnormal_int(code_handbrake_lock, state_safe);
	}
	else
	{
		//错误：抱闸未打开
		APP_abnormal_int(code_handbrake_lock, state_error);
	}
}

//上位机离线检测
void APP_HMAC_heart_interval()
{
	//control.state.HMAC_heart_interval_time再此函数内以1MS为周期+1
	//在APP_LMAC内的心跳报文响应中置0
	//control.HMAC_heart_interval_warning在此函数内如果time>=最大离线时间，则置1
	//在APP_LMAC内的心跳报文响应中置0
	//触发心跳报文最大间隔警报后，将在警报处理程序进行响应。
	if (HMAC_heart_interval_enable == 1)//上位机心跳检测开
	{
		if (state.HMAC_heart_interval_time < 0xFFFE)state.HMAC_heart_interval_time++;//时间累加
		//间隔超过心跳间隔最大时间,判定上位机离线
		if (state.HMAC_heart_interval_time >= HMAC_heart_interval_max)
		{
			//远程控制时，不允许上位机离线，会产生错误
			if (control.sys.mod == mod_remote_on)
			{
				APP_abnormal_int(code_HMAC_off_line, state_error);
			}
			//其他模式时，允许上位机离线，会产生警报
			if (control.sys.mod != mod_remote_on)
			{
				APP_abnormal_int(code_HMAC_off_line, state_warning);
			}
		}
		//上位机在线
		else
		{
			APP_abnormal_int(code_HMAC_off_line, state_safe);
		}
	}
	//如果不开启上位机离线检测，上位机一直处于在线状态
	if (HMAC_heart_interval_enable == 0)
	{
		APP_abnormal_int(code_HMAC_off_line, state_safe);
	}

}

//ECU离线检测
void APP_ECU_offline_testing()
{
	//
	if (ECU_heart_interval_enable == 1)									//ECU心跳检测开
	{
		if (state.ECU_heart_interval_time < 0xFFFF)state.ECU_heart_interval_time++;									//时间累加
		if (state.ECU_heart_interval_time >= ECU_heart_interval_max)		//间隔超过心跳间隔最大时间
		{
			APP_abnormal_int(code_ECU_off_line, state_error);			//ECU离线
		}
		else
		{
			APP_abnormal_int(code_ECU_off_line, state_safe);			//ECU错误
		}
	}
	//如果关闭ECU离线检测，ECU一直在线
	if (ECU_heart_interval_enable == 0)
	{
		APP_abnormal_int(code_ECU_off_line, state_safe);
	}
}

//电机驱动离线检测
void APP_motor_offline_testing()
{
	//

	if (motor_heart_interval_enable == 1)									//电驱心跳检测开
	{
		if (ECU_handbrake_check() == 1)										//电驱供电，打开抱闸
		{
			if (state.motor1_heart_interval_time < 0xFFFF)state.motor1_heart_interval_time++;								//时间累加
			if (state.motor2_heart_interval_time < 0xFFFF)state.motor2_heart_interval_time++;								//时间累加
			if (state.motor1_heart_interval_time >= motor_heart_interval_max)	//间隔超过心跳间隔最大时间
			{
				APP_abnormal_int(code_motor1_off_line, state_error);
			}
			else
			{
				APP_abnormal_int(code_motor1_off_line, state_safe);
			}
			if (state.motor2_heart_interval_time >= motor_heart_interval_max)	//间隔超过心跳间隔最大时间
			{
				APP_abnormal_int(code_motor2_off_line, state_error);
			}
			else
			{
				APP_abnormal_int(code_motor2_off_line, state_safe);
			}
		}
	}
}
//现场手柄离线检测
void APP_scene_handle_offline_testing()
{
	//离线
	if (APP_scene_handle_abnormal() != 1)
	{
		//现场控制不允许手柄离线
		if (control.sys.mod == mod_scene_on)
		{
			APP_abnormal_int(code_scene_handle_unoffline, state_error);
		}
		else
		{
			APP_abnormal_int(code_scene_handle_unoffline, state_warning);
		}
	}
	else
	{
		APP_abnormal_int(code_scene_handle_unoffline, state_safe);
	}
}




//电驱初始化检测
void APP_motor_uninit_check()
{
	if (ECU_handbrake_check() == 1)								//电驱供电，打开抱闸
	{
		switch (control.sys.ESC)
		{
			//电驱为八达威
		case (ESC_BDW):
		{
			if ((state.motor_uninit != state_safe) && (state.handbrake == state_safe))
			{
				APP_abnormal_int(code_motor1_uninit, state_error);
				APP_abnormal_int(code_motor2_uninit, state_error);
			}
			if ((state.motor_uninit == state_safe) && (state.handbrake == state_safe))
			{
				APP_abnormal_int(code_motor1_uninit, state_safe);
				APP_abnormal_int(code_motor2_uninit, state_safe);
			}
		}
		break;
		//电驱为山西零度
		case (ESC_SXLD):
		{
			//前驱未初始化
			if (SXLD_abnormal_uninit(0) == 0)
			{
				APP_abnormal_int(code_motor1_uninit, state_error);
			}
			//前驱已初始化
			else
			{
				APP_abnormal_int(code_motor1_uninit, state_safe);
			}
			//后驱未初始化
			if (SXLD_abnormal_uninit(1) == 0)
			{
				APP_abnormal_int(code_motor2_uninit, state_error);
			}
			//后驱已初始化
			else
			{
				APP_abnormal_int(code_motor2_uninit, state_safe);
			}
			if ((SXLD_abnormal_uninit(0) == 1) && (SXLD_abnormal_uninit(1) == 1))
			{
				APP_abnormal_int(code_motor_uninit, state_safe);
			}

		}
		break;

		}

	}
}

//UDP状态检测
void APP_UDP_state_testing(uint16_t cyc)
{
	static uint16_t cc = 0;
	if ((cc % cyc) == 0)
	{
		if ((UDP_abnormal_query(UDP_abnormal_init) != 1) || (UDP_abnormal_query(UDP_abnormal_TX) != 1) || (UDP_abnormal_query(UDP_abnormal_RX) != 1))
		{
			APP_abnormal_int(code_UDP_breakdown, state_warning);
		}
		else
		{
			APP_abnormal_int(code_UDP_breakdown, state_safe);
		}
	}
}





//异常点扫描
void APP_abnormal_check()
{
	APP_handbrake_testing();			//抱闸检测

	APP_HMAC_heart_interval();			//上位机离线检测

	APP_ECU_offline_testing();			//ECU离线检测

	APP_motor_offline_testing();		//电驱离线检测

	APP_motor_uninit_check();			//电驱初始化检测

	APP_scene_handle_offline_testing();	//现场手柄离线检测

	APP_UDP_state_testing(100);			//UDP状态检测
}

//异常信息统计
void APP_abnormal_count()
{
	state.error = 0;
	state.warning = 0;

	//上位机离线
	if (state.HMAC_offline == state_error)state.error = state.error + 1;
	if (state.HMAC_offline == state_warning)state.warning = state.warning + 1;
	//前驱离线
	if (state.motor1_offline == state_error)state.error = state.error + 1;
	if (state.motor1_offline == state_warning)state.warning = state.warning + 1;
	//后驱离线
	if (state.motor2_offline == state_error)state.error = state.error + 1;
	if (state.motor2_offline == state_warning)state.warning = state.warning + 1;
	//ECU离线
	if (state.ECU_offline == state_error)state.error = state.error + 1;
	if (state.ECU_offline == state_warning)state.warning = state.warning + 1;
	//抱闸未开
	if (state.handbrake == state_error)state.error = state.error + 1;
	if (state.handbrake == state_warning)state.warning = state.warning + 1;
	//电驱未初始化
	if (state.motor_uninit == state_error)state.error = state.error + 1;
	if (state.motor_uninit == state_warning)state.warning = state.warning + 1;
	//前驱未初始化
	if (state.motor1_uninit == state_error)state.error = state.error + 1;
	if (state.motor1_uninit == state_warning)state.warning = state.warning + 1;
	//后驱未初始化
	if (state.motor2_uninit == state_error)state.error = state.error + 1;
	if (state.motor2_uninit == state_warning)state.warning = state.warning + 1;
	//现场手柄离线
	if (state.scene_handle == state_error)state.error = state.error + 1;
	if (state.scene_handle == state_warning)state.warning = state.warning + 1;

	//更新系统状态
	if ((state.error == 0) && (state.warning == 0))
	{
		state.whole = state_safe;			//无警告无错误
	}
	else
	{
		if (state.error != 0)
		{
			state.whole = state_error;		//错误
		}
		else
		{
			state.whole = state_warning;	//有警告无错误
		}
	}
}

//异常点显示
void APP_abnormal_printf()
{
	if (printf_abnormal==1)
	{
		if ((state.abnormal_check_cyc != 0) && (state.abnormal_time % state.abnormal_check_cyc == 0))
		{
			//--------------------------------警告相关信息打印
			if (state.warning == 0)
			{
				printf(" 0 个警告：无\n");
			}
			else
			{
				printf(" %d 个警告：", state.warning);
				if (state.HMAC_offline == state_warning)printf("上位机离线、");
				if (state.motor1_offline == state_warning)printf("前驱离线、");
				if (state.motor2_offline == state_warning)printf("后驱离线、");
				if (state.ECU_offline == state_warning)printf("ECU离线、");
				if (state.handbrake == state_warning)printf("抱闸未打开、");
				if (state.motor_uninit == state_warning)printf("电驱未就绪、");
				if (state.motor1_uninit == state_warning)printf("前驱未就绪、");
				if (state.motor2_uninit == state_warning)printf("后驱未就绪、");
				if (state.scene_handle == state_warning)printf("现场手柄离线、");
				printf("\n");
			}
			//--------------------------------错误相关信息打印
			if (state.error == 0)
			{
				printf(" 0 个错误：无\n");
			}
			else
			{
				printf(" %d 个错误：", state.error);
				if (state.HMAC_offline == state_error)printf("上位机离线、");
				if (state.motor1_offline == state_error)printf("前驱离线、");
				if (state.motor2_offline == state_error)printf("后驱离线、");
				if (state.ECU_offline == state_error)printf("ECU离线、");
				if (state.handbrake == state_error)printf("抱闸未打开、");
				if (state.motor_uninit == state_error)printf("电驱未就绪、");
				if (state.motor1_uninit == state_error)printf("前驱未就绪、");
				if (state.motor2_uninit == state_error)printf("后驱未就绪、");
				if (state.scene_handle == state_error)printf("现场手柄离线、");
			}
			printf("\n\n");
		}
	}
	//异常点显示计时
	state.abnormal_time = (state.abnormal_time + 1) % 0xFFFFFFFF;
}

//电机未初始化错误处理
void motor_uninit_solve()
{
	if (state.motor_uninit != state_safe)					//电驱未初始化
	{
		if (ECU_handbrake_check() == 1)								//电驱供电，打开抱闸
		{

			if (chassis_motor_init(control.sys.time_ms) == 1)
			{
				printf("电驱初始化完成\n");
				APP_abnormal_int(code_motor_uninit, state_safe);
				APP_abnormal_int(code_motor1_uninit, state_safe);
				APP_abnormal_int(code_motor2_uninit, state_safe);
			}

		}
	}
}

//异常处理
void APP_abnormal_solve()
{
	motor_uninit_solve();			//点击未初始化处理
	APP_HMAC_offline_solve();		//上位机离线处理
}

//上位机离线处理
void APP_HMAC_offline_solve()
{
	if (state.HMAC_offline == state_error)
	{
		//停止车辆
		Chassis_Given_Speed(0, 0, 100, control.sys.time_ms);
		//清空控制数据
		control.remote.vx = 0;
		control.remote.vw = 0;
	}
}







//异常模块
void APP_abnormal()
{
	APP_abnormal_check();		//异常扫描

	APP_abnormal_count();		//异常信息统计

	APP_abnormal_printf();		//异常信息显示

	APP_abnormal_solve();		//异常处理
}