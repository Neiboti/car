#include "APP_LMAC.h"
#include "APP_UDP_agree.h"
#include "APP_control.h"
#include "APP_ECU_agree.h"
#include "APP_abnormal.h"
#include "driver_UDP.h"
#include "APP_ECU_agree.h"

extern ControL control;
extern AbnormaL state;
extern UDP_Agree Agree;
extern ECU_DRR ECU;			//ECU结构体



//UDP上报函数
void APP_HMAC_TX_main()
{
	//进行UDP上报

	//计时
	if (Agree.connection.state == 1)
	{
		Agree.connection.heart_interval_time = (Agree.connection.heart_interval_time + 1) % 0xFFFFFFFE;
	}
	if (Agree.connection.heart_interval_time >= 1000)
	{
		//超过5S没有收到上位机的心跳/请求
		//将当前控制者设为休眠状态
		Agree.connection.state = 0;
	}

}

//UDP上报函数初始化
void APP_HMAC_TX_init()
{
	//用于设定上报的一些参数

	//默认控制者网络地址
	Agree.connection.code = 0x1000;
	Agree.connection.state = 1;
	Agree.connection.IP = IP_UDP_TX_init(Client_IP);
	Agree.connection.prot = PORT_UDP_TX_init(Client_PORT);

}


//---------------------------------------------------------------------------------------远程手柄

//远程手柄控制帧数据处理
void APP_set_handle_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	Agree.handle.LX_1 = (readbuff[6] << 8) | readbuff[7];		//摇杆
	Agree.handle.LY_1 = (readbuff[8] << 8) | readbuff[9];
	Agree.handle.RX_1 = (readbuff[10] << 8) | readbuff[11];
	Agree.handle.RY_1 = (readbuff[12] << 8) | readbuff[13];
	Agree.handle.cross_key = readbuff[14];						//十字按键
	Agree.handle.rule_key = readbuff[15];						//普通按键

	//远程手柄数据打印
	//printf("LX:%d   LY:%d   RX:%d   RY:%d\n", Agree.handle.LX_1, Agree.handle.LY_1, Agree.handle.RX_1, Agree.handle.RY_1);
	//远程手柄按键事件
	LMC_set_handle_key(&Agree);
	//远程手柄摇杆事件
	LMC_set_handle_rocker(&Agree);
}

//远程手柄按键响应
void LMC_set_handle_key(UDP_Agree* agree)
{
	//远程端back按键按下，尝试退出远程模式
	if ((agree->handle.rule_key & key_back) == key_back)
	{
		control_mod_change(mod_remote_off);
	}
	//远程端start按键按下，尝试开启远程模式
	if ((agree->handle.rule_key & key_start) == key_start)
	{
		control_mod_change(mod_remote_on);
	}
	//远程端B键按下，合抱闸
	if ((agree->handle.rule_key & key_B) == key_B)
	{
		ECU_light(axis_lock_off, 0);								  //抱闸关
		ECU_light(power_motor_off, 0);								  //驱动器供电关


	}
	//远程端A键按下，开抱闸
	if ((agree->handle.rule_key & key_A) == key_A)
	{
		ECU_light(axis_lock_on, 0);
		ECU_light(power_motor_on, 0);								  //驱动器供电关
		state.motor_uninit = state_warning;								//标记驱动器未初始化

	}

	//远程端Y键按下，大灯开
	if ((agree->handle.rule_key & key_Y) == key_Y)
	{
		ECU_light(power_headlight_on, 0);							//大灯开

	}
	//远程端X键按下，大灯关
	if ((agree->handle.rule_key & key_X) == key_X)
	{
		ECU_light(power_headlight_off, 0);							//大灯关

	}


}

//远程手柄摇杆响应
void LMC_set_handle_rocker(UDP_Agree* agree)
{
	//若处于远程模式，修改运动设定值
	if (control.sys.mod == mod_remote_on)
	{
		control.remote.vx = agree->handle.LY_1;
		control.remote.vw = agree->handle.RX_1;
	}
}

//---------------------------------------------------------------------------------------心跳报文

//心跳报文帧数据处理
void APP_ask_HPC_heart_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//2023-2-22，移除心跳报文帧中的ECU开关指令，替换为上位机系统时间
	//如果set_basics_cyc=0，则表示不进行自动上报，采用接受到一帧心跳帧回应一帧的方式
	//2023-3-3,移除自动上报，采用一收一发的形式
	//读取数据，

	if (control.sys.mod != mod_remote_on)
	{
		//IP为控制者IP
		state.HMAC_heart_interval_time = 0;						//重置时间
		Agree.connection.heart_interval_time = 0;				//重置上位机休眠时间
	}
	else
	{
		if (IP == Agree.connection.IP)
		{
			state.HMAC_heart_interval_time = 0;						//重置时间
			Agree.connection.heart_interval_time = 0;				//重置上位机休眠时间
		}
	}

	Agree.heart.set_cmd_time = (readbuff[6] << 24) | (readbuff[7] << 16) | (readbuff[8] << 8) | readbuff[9];	//上位机系统时间

	APP_ask_HPC_heart_TX(IP, prot);

}

//心跳报文数据反馈
void APP_ask_HPC_heart_TX(uint32_t IP, uint16_t prot)
{

	//回应数据
	uint8_t data[19] = { 0 };						//数据池
	uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧

	//获取数据
	HeadeR = UDP_LPC_ask_TX;
	TypE = ask_HPC_heart;
	Agree.heart.reality_ECU_state = get_ECU_rough_state_16bit();		//ECU  16bit 开光状态
	Agree.heart.reality_alert_number = state.warning;					//警报数量
	Agree.heart.reality_error_number = state.error;						//错误数量
	Agree.heart.reality_battery_level = 0;								//电池电量
	Agree.heart.reality_control_mod = control.sys.mod;					//当前控制模式



	Agree.heart.reality_heart_code = Agree.connection.code;//心跳编号

	//装填数据

	data[0] = (HeadeR & 0xFF00) >> 8;
	data[1] = HeadeR & 0x00FF;
	data[2] = (TypE & 0xFF00) >> 8;
	data[3] = TypE & 0x00FF;

	data[6] = (Agree.heart.reality_ECU_state & 0xFF00) >> 8;
	data[7] = (Agree.heart.reality_ECU_state & 0xFF);
	data[8] = (Agree.heart.reality_alert_number & 0xFF00) >> 8;
	data[9] = (Agree.heart.reality_alert_number & 0xFF);
	data[10] = (Agree.heart.reality_error_number & 0xFF00) >> 8;
	data[11] = (Agree.heart.reality_error_number & 0xFF);
	data[12] = (Agree.heart.reality_battery_level & 0xFF00) >> 8;
	data[13] = (Agree.heart.reality_battery_level & 0xFF);
	data[14] = (Agree.heart.reality_control_mod & 0xFF00) >> 8;
	data[15] = (Agree.heart.reality_control_mod & 0xFF);
	data[16] = (Agree.heart.reality_heart_code & 0xFF00) >> 8;
	data[17] = (Agree.heart.reality_heart_code & 0xFF);

	//printf("控制者：%X，控制模式：%X\n", Agree.heart.reality_heart_code, Agree.heart.reality_control_mod);

	//数据发送
	UDP_pool_input(IP, prot, 19, data);
}

//---------------------------------------------------------------------------------------请求链接

//请求控制权操作数据处理
void APP_ask_connection_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//判断有无控制权
	uint16_t CODE = 0, ACTION = 0;

	CODE = (readbuff[6] << 8) | readbuff[7];												//请求者编号
	ACTION = (readbuff[8] << 8) | readbuff[9];											//请求者行为


	if ((CODE >= Agree.connection.code) || (Agree.connection.state == 0))
	{
		//请求者编号大于当控制者或当前控制者离线,移交控制权
		Agree.connection.code = CODE;
		Agree.connection.IP = IP;
		Agree.connection.prot = prot;
		Agree.connection.state = 1;
		Agree.connection.heart_interval_time = 0;				//重置上位机休眠时间


		//设置为指定模式
		control_mod_change(ACTION);

		APP_ask_connection_TX(1, IP, prot);
	}
	else
	{
		APP_ask_connection_TX(0, IP, prot);
	}
}
//控制权数据反馈
void APP_ask_connection_TX(uint8_t ok, uint32_t IP, uint16_t prot)
{
	//回应数据
	uint8_t data[11] = { 0 };						//数据池
	uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧
	//获取数据
	HeadeR = UDP_LPC_ask_TX;
	TypE = ask_connection;
	//装填数据
	data[0] = (HeadeR & 0xFF00) >> 8;
	data[1] = HeadeR & 0x00FF;
	data[2] = (TypE & 0xFF00) >> 8;
	data[3] = TypE & 0x00FF;

	data[6] = (Agree.connection.code & 0xFF00) >> 8;
	data[7] = (Agree.connection.code & 0xFF);
	if (ok == 0)
	{
		data[8] = 0x00;
		data[9] = 0x00;
	}
	else
	{
		data[8] = 0xFF;
		data[9] = 0xFF;
	}
	UDP_pool_input(IP, prot, 11, data);

}

//---------------------------------------------------------------------------------------请求速度

//请求速度帧数据处理
void APP_ask_speed_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//
	//
	//
	uint32_t set_format = 0;

	set_format = (readbuff[6] << 24) | (readbuff[7] << 16) | (readbuff[8] << 8) | readbuff[9];//数据格式

	APP_ask_speed_TX(set_format, IP, prot);
}

//速度数据反馈
void APP_ask_speed_TX(uint32_t format, uint32_t IP, uint16_t prot)
{
	//格式0-四轮速度
	if (format == 0)
	{
		//回应数据
		uint8_t data[15] = { 0 };					//数据池
		uint16_t HeadeR = 0, TypE = 0;				 //帧头、功能帧

		//获取数据
		HeadeR = UDP_LPC_ask_TX;
		TypE = ask_speed;
		Agree.speed.v1 = control.act.v1;
		Agree.speed.v2 = control.act.v2;
		Agree.speed.v3 = control.act.v3;
		Agree.speed.v4 = control.act.v4;

		//装填数据
		data[0] = (HeadeR & 0xFF00) >> 8;
		data[1] = HeadeR & 0x00FF;
		data[2] = (TypE & 0xFF00) >> 8;
		data[3] = TypE & 0x00FF;

		data[6] = (Agree.speed.v1 & 0xFF00) >> 8;
		data[7] = (Agree.speed.v1 & 0xFF);
		data[8] = (Agree.speed.v2 & 0xFF00) >> 8;
		data[9] = (Agree.speed.v2 & 0xFF);
		data[10] = (Agree.speed.v3 & 0xFF00) >> 8;
		data[11] = (Agree.speed.v3 & 0xFF);
		data[12] = (Agree.speed.v4 & 0xFF00) >> 8;
		data[13] = (Agree.speed.v4 & 0xFF);

		//数据发送
		UDP_pool_input(IP, prot, 15, data);
	}
	if (format == 0x2000)
	{
		//32bit左侧速度+32bit右侧速度

		//回应数据
		uint8_t data[17] = { 0 };					//数据池
		uint16_t HeadeR = 0, TypE = 0;				 //帧头、功能帧
		int32_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;
		//获取数据
		HeadeR = UDP_LPC_ask_TX;
		TypE = ask_speed;
		v1 = control.act.ms_v1 * -10000;
		v2 = control.act.ms_v2 * 10000;

		//装填数据
		data[0] = (HeadeR & 0xFF00) >> 8;
		data[1] = HeadeR & 0x00FF;
		data[2] = (TypE & 0xFF00) >> 8;
		data[3] = TypE & 0x00FF;

		data[6] = (format & 0xFF00) >> 8;
		data[7] = format & 0x00FF;


		data[8] = (v1 & 0xFF000000) >> 24;
		data[9] = (v1 & 0xFF0000) >> 16;
		data[10] = (v1 & 0xFF00) >> 8;
		data[11] = (v1 & 0xFF);
		data[12] = (v2 & 0xFF000000) >> 24;
		data[13] = (v2 & 0xFF0000) >> 16;
		data[14] = (v2 & 0xFF00) >> 8;
		data[15] = (v2 & 0xFF);

		//数据发送
		UDP_pool_input(IP, prot, 17, data);
	}

}

//---------------------------------------------------------------------------------------请求电流

//请求电流帧数据处理
void APP_ask_current_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	uint32_t set_format = 0;
	set_format = (readbuff[6] << 24) | (readbuff[7] << 16) | (readbuff[8] << 8) | readbuff[9];//数据格式

	APP_ask_current_TX(set_format, IP, prot);
}
//电流数据反馈
void APP_ask_current_TX(uint32_t format, uint32_t IP, uint16_t prot)
{
	//格式0-
	if (format == 0)
	{
		//回应数据
		uint8_t data[19] = { 0 };						//数据池
		uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧

		//获取数据
		HeadeR = UDP_LPC_ask_TX;
		TypE = ask_current;
		Agree.current.i1 = control.act.i1;
		Agree.current.i2 = control.act.i2;
		Agree.current.i3 = control.act.i3;
		Agree.current.i4 = control.act.i4;

		//装填数据
		data[0] = (HeadeR & 0xFF00) >> 8;
		data[1] = HeadeR & 0x00FF;
		data[2] = (TypE & 0xFF00) >> 8;
		data[3] = TypE & 0x00FF;

		data[6] = (Agree.current.i1 & 0xFF00) >> 8;
		data[7] = (Agree.current.i1 & 0xFF);
		data[8] = (Agree.current.i2 & 0xFF00) >> 8;
		data[9] = (Agree.current.i2 & 0xFF);
		data[10] = (Agree.current.i3 & 0xFF00) >> 8;
		data[11] = (Agree.current.i3 & 0xFF);
		data[12] = (Agree.current.i4 & 0xFF00) >> 8;
		data[13] = (Agree.current.i4 & 0xFF);


		//数据发送
		UDP_pool_input(IP, prot, 15, data);
	}
}
//---------------------------------------------------------------------------------------请求温度

//请求温度帧数据处理
void APP_ask_temp_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	uint32_t set_format = 0;
	set_format = (readbuff[6] << 24) | (readbuff[7] << 16) | (readbuff[8] << 8) | readbuff[9];//数据格式
	APP_ask_temp_TX(set_format, IP, prot);
}
//温度数据反馈
void APP_ask_temp_TX(uint32_t format, uint32_t IP, uint16_t prot)
{
	//格式0-
	if (format == 0)
	{
		//回应数据
		uint8_t data[15] = { 0 };						//数据池
		uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧

		//获取数据
		HeadeR = UDP_LPC_ask_TX;
		TypE = ask_temp;
		Agree.temp.t1 = control.act.t1;
		Agree.temp.t2 = control.act.t2;
		Agree.temp.t3 = control.act.t3;
		Agree.temp.t4 = control.act.t4;

		//装填数据
		data[0] = (HeadeR & 0xFF00) >> 8;
		data[1] = HeadeR & 0x00FF;
		data[2] = (TypE & 0xFF00) >> 8;
		data[3] = TypE & 0x00FF;

		data[6] = (Agree.temp.t1 & 0xFF00) >> 8;
		data[7] = (Agree.temp.t1 & 0xFF);
		data[8] = (Agree.temp.t2 & 0xFF00) >> 8;
		data[9] = (Agree.temp.t2 & 0xFF);
		data[10] = (Agree.temp.t3 & 0xFF00) >> 8;
		data[11] = (Agree.temp.t3 & 0xFF);
		data[12] = (Agree.temp.t4 & 0xFF00) >> 8;
		data[13] = (Agree.temp.t4 & 0xFF);

		//数据发送
		UDP_pool_input(IP, prot, 15, data);
	}
}



//--------------------------------------------------------------------------------------------
//设置ECU开关数据处理
void APP_set_ECU_sth_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	Agree.ECU.sth_code = (readbuff[6] << 8) | (readbuff[7]);
	Agree.ECU.sth_time = (readbuff[8] << 8) | (readbuff[9]);

	//指令码，闪烁时间，0为不闪烁
	ECU_light(Agree.ECU.sth_code, Agree.ECU.sth_time);
	//设置ECU开关帧回应
	APP_set_ECU_sth_TX(1, IP, prot);
}

//设置ECU开关帧回应
void APP_set_ECU_sth_TX(uint32_t ok, uint32_t IP, uint16_t prot)
{
	//回应数据
	uint8_t data[11] = { 0 };						//数据池
	uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧
	uint16_t ECU_state = 0;
	//获取数据
	HeadeR = UDP_LPC_set_TX;
	TypE = set_ECU_sth;
	//装填数据
	data[0] = (HeadeR & 0xFF00) >> 8;
	data[1] = HeadeR & 0x00FF;
	data[2] = (TypE & 0xFF00) >> 8;
	data[3] = TypE & 0x00FF;

	ECU_state = get_ECU_rough_state_16bit();

	if (ok == 0)
	{
		data[6] = 0x00;
		data[7] = 0x00;
	}
	else
	{
		data[6] = 0xFF;
		data[7] = 0xFF;
	}

	data[8] = (ECU_state & 0xFF00) >> 8;
	data[9] = (ECU_state & 0xFF);


	UDP_pool_input(IP, prot, 11, data);
}

//设置控制方式数据处理
void APP_set_control_mod_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	Agree.control_mod.mod = (readbuff[6] << 24) | (readbuff[7] << 16) | (readbuff[8] << 8) | (readbuff[9]);
	Agree.control_mod.code = (readbuff[10] << 8) | (readbuff[11]);

	//模式切换

	control_mod_change(Agree.control_mod.mod);



	//设置控制方式帧回应
	APP_set_control_mod_TX(1, IP, prot);



}

//设置控制方式帧回应
void APP_set_control_mod_TX(uint32_t ok, uint32_t IP, uint16_t prot)
{
	//回应数据
	uint8_t data[11] = { 0 };						//数据池
	uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧
	//获取数据
	HeadeR = UDP_LPC_set_TX;
	TypE = set_ECU_sth;
	//装填数据
	data[0] = (HeadeR & 0xFF00) >> 8;
	data[1] = HeadeR & 0x00FF;
	data[2] = (TypE & 0xFF00) >> 8;
	data[3] = TypE & 0x00FF;

	data[6] = (Agree.connection.code & 0xFF00) >> 8;
	data[7] = (Agree.connection.code & 0xFF);
	if (ok == 0)
	{
		data[8] = 0x00;
		data[9] = 0x00;
	}
	else
	{
		data[8] = 0xFF;
		data[9] = 0xFF;
	}
	UDP_pool_input(IP, prot, 11, data);
}


//---------------------------------------------------------------------------------------设置曲率

//曲率控制
void APP_set_curvature_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//设置控制数据来源
	switch (control.sys.mod)
	{
		//自主模式
	case (mod_autonomy_on): Agree.control_mark = speed_curvature_autonomy; break;
		//远程模式
	case (mod_remote_on): Agree.control_mark = speed_curvature_remote; break;
	}

	Agree.cte.speed = ((readbuff[6] << 8) | (readbuff[7])) * 0.01;
	Agree.cte.curvature = ((readbuff[8] << 8) | (readbuff[9])) * 0.0001;
	Agree.cte.latitude = (readbuff[10] << 24) | (readbuff[11] << 16) | (readbuff[12] << 8) | readbuff[13];
	Agree.cte.longitude = (readbuff[14] << 24) | (readbuff[15] << 16) | (readbuff[16] << 8) | readbuff[17];
	Agree.cte.height = (readbuff[18] << 24) | (readbuff[19] << 16) | (readbuff[20] << 8) | readbuff[21];
	Agree.cte.course = ((readbuff[22] << 8) | (readbuff[23]));

}

//---------------------------------------------------------------------------------------设置线速度-角速度

//线速度-角速度控制
void APP_set_angular_velocity_control_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	//设置控制数据来源
	switch (control.sys.mod)
	{
		//自主模式
	case (mod_autonomy_on): Agree.control_mark = speed_angular_autonomy; break;
		//远程模式
	case (mod_remote_on): Agree.control_mark = speed_angular_remote; break;
	}
	Agree.angular_velocity.speed = (readbuff[6] << 8) | (readbuff[7]);
	Agree.angular_velocity.angular = (readbuff[8] << 8) | (readbuff[9]);

	//printf("VX:%d     VW:%d\n", Agree.angular_velocity.speed, Agree.angular_velocity.angular);

}

//请求开关抱闸数据处理1
void APP_ask_ask_axis_lock_RX(uint8_t* readbuff, uint32_t IP, uint16_t prot)
{
	uint16_t ECU_SwitcH = 0;

	ECU_SwitcH = (readbuff[6] << 8) | (readbuff[7]);

	if (ECU_SwitcH == 0XFFFF)
	{
		ECU_light(axis_lock_on, 0);									//抱闸开
		ECU_light(power_motor_on, 0);								//驱动器供电开
		state.motor_uninit = state_warning;							//标记驱动器未初始化

	}
	if (ECU_SwitcH == 0X0000)
	{
		ECU_light(axis_lock_off, 0);								  //抱闸关
		ECU_light(power_motor_off, 0);								  //驱动器供电关
	}


}

//请求开关抱闸回应
void APP_ask_ask_axis_lock_TX(uint8_t ok, uint32_t IP, uint16_t prot)
{
	//回应数据
	uint8_t data[11] = { 0 };						//数据池
	uint16_t HeadeR = 0, TypE = 0;				  //帧头、功能帧
	//获取数据
	HeadeR = UDP_LPC_set_TX;
	TypE = ask_axis_lock;
	//装填数据
	data[0] = (HeadeR & 0xFF00) >> 8;
	data[1] = HeadeR & 0x00FF;
	data[2] = (TypE & 0xFF00) >> 8;
	data[3] = TypE & 0x00FF;
	if (ok == 0)
	{
		data[6] = 0x00;
		data[7] = 0x00;
	}
	else
	{
		data[6] = 0xFF;
		data[7] = 0xFF;
	}
	data[8] = (Agree.connection.code & 0xFF00) >> 8;
	data[9] = (Agree.connection.code & 0xFF);

	UDP_pool_input(IP, prot, 11, data);
}