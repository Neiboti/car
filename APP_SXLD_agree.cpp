#include "APP_SXLD_agree.h"
#include "driver_CAN.h"
#include "APP_abnormal.h"


SXLD_Data LD_actual[2] = { 0 };

extern ControL control;//中控函数结构体
extern AbnormaL state;

//-------------------------------------------------------------------------------------
//函数功能：滑动输入器
//函数说明；配合滑动发射器使用，可以在短时间内发送大量数据，不必再等待数据发送，妈妈再也不用担心程序的时序辣~~~~
//函数输入：功能码，CanID，请求地址，寄存器数（最大个数为3,16bit），数据1，数据2，数据3
//-------------------------------------------------------------------------------------
void SXLD_slide_pool(uint8_t Fun, uint8_t ID, uint8_t Addr, uint8_t Regs, uint16_t Data_1, uint16_t Data_2, uint16_t Data_3)
{
	uint16_t id = 0;
	uint8_t ExiD = 0;
	uint8_t len = 0;
	uint8_t DatAbf[8] = { 0 };

	id = (Fun << 8) | ID;
	DatAbf[0] = Addr;
	DatAbf[1] = Regs;
	if (Regs == 1)
	{
		DatAbf[2] = Data_1 & 0XFF;
		DatAbf[3] = Data_1 >> 8;
		len = 4;
	}
	if (Regs == 2)
	{
		DatAbf[2] = Data_1 & 0XFF;
		DatAbf[3] = Data_1 >> 8;
		DatAbf[4] = Data_2 & 0XFF;
		DatAbf[5] = Data_2 >> 8;
		len = 6;
	}
	if (Regs == 3)
	{
		DatAbf[2] = Data_1 & 0XFF;
		DatAbf[3] = Data_1 >> 8;
		DatAbf[4] = Data_2 & 0XFF;
		DatAbf[5] = Data_2 >> 8;
		DatAbf[6] = Data_3 & 0XFF;
		DatAbf[7] = Data_3 >> 8;
		len = 8;
	}
	CAN_pool_input(id, ExiD, len, DatAbf);
}

//-------------------------------------------------------------------------------------
//函数功能：周期性请求数据
//函数说明；系统时序优化，定期发送请求数据指令
//函数输入：系统时间
//-------------------------------------------------------------------------------------
void SXLD_cycle_request_data(uint16_t time)
{


	if (time % 200 == 100)SXLD_slide_pool(0x02, 0x09, 0x78, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 102)SXLD_slide_pool(0x02, 0x0A, 0x78, 0x02, 0x0000, 0x0000, 0x0000);		//读取系统标志位1-2

	if (time % 200 == 104)SXLD_slide_pool(0x02, 0x09, 0x7A, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 106)SXLD_slide_pool(0x02, 0x0A, 0x7A, 0x02, 0x0000, 0x0000, 0x0000);		//读取系统标志位3-4


	if (time % 100 == 0)SXLD_slide_pool(0x02, 0x09, 0x84, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 100 == 2)SXLD_slide_pool(0x02, 0x0A, 0x84, 0x02, 0x0000, 0x0000, 0x0000);		//读取速度

	if (time % 100 == 10)SXLD_slide_pool(0x02, 0x09, 0x8e, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 100 == 14)SXLD_slide_pool(0x02, 0x0A, 0x8e, 0x02, 0x0000, 0x0000, 0x0000);		//读取电流

	if (time % 200 == 120)SXLD_slide_pool(0x02, 0x09, 0x90, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 124)SXLD_slide_pool(0x02, 0x0A, 0x90, 0x02, 0x0000, 0x0000, 0x0000);		//读取功率

	if (time % 200 == 130)SXLD_slide_pool(0x02, 0x09, 0x92, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 134)SXLD_slide_pool(0x02, 0x0A, 0x92, 0x02, 0x0000, 0x0000, 0x0000);		//读取温度

	if (time % 200 == 140)SXLD_slide_pool(0x02, 0x09, 0x94, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 144)SXLD_slide_pool(0x02, 0x0A, 0x94, 0x02, 0x0000, 0x0000, 0x0000);		//读取温度

	if (time % 200 == 150)SXLD_slide_pool(0x02, 0x09, 0x9e, 0x02, 0x0000, 0x0000, 0x0000);
	if (time % 200 == 154)SXLD_slide_pool(0x02, 0x0A, 0x9e, 0x02, 0x0000, 0x0000, 0x0000);		//读取扭矩

	//if (time % 200 == 164)printf("T1=%4.2f   T2=%4.2f   T3=%4.2f   T4=%4.2f\n", LD_actual[0].tempH_1, LD_actual[0].tempL_1, LD_actual[1].tempH_1, LD_actual[1].tempL_1);
	//if (time % 200 == 174)printf("V1=%4.1d   V2=%4.1d   V3=%4.1d   V4=%4.1d\n", LD_actual[0].speed_L, LD_actual[0].speed_R, LD_actual[1].speed_L, LD_actual[1].speed_R);
	//if (time % 200 == 184)printf("I1=%4.1d   I2=%4.1d   I3=%4.1d   I4=%4.1d\n", LD_actual[0].current_L, LD_actual[0].current_R, LD_actual[1].current_L, LD_actual[1].current_R);
	//if (time % 200 == 194)printf("error1=%d   error2=%d\n\n", LD_actual[0].sys_state_1, LD_actual[1].sys_state_1);
}

//-------------------------------------------------------------------------------------
//函数功能：对接收到的can数据进行分类储存
//函数说明；将数据存储到结构体对应的位置
//函数输入：CAN_Data* DatA----CAN接收数据结构体
//-------------------------------------------------------------------------------------
void SXLD_can_data_recive_storage(CAN_Data* DatA)
{
	uint8_t id = 0;
	uint8_t idh = 0;
	uint8_t idl = 0;
	uint16_t data1 = 0, data2 = 0, data3 = 0;

	idh = (DatA->frame.can_id) >> 8;
	idl = (DatA->frame.can_id) & 0xFF;

	if (idh == LD_command_set)						//应答回复“请求设置寄存器”的功能码
	{
		if ((idl - 0x80) == LD_drive_id_0x09)
		{
			state.motor1_heart_interval_time = 0;		//复位驱动器离线检测时间
			id = 0;										//确定驱动器ID
		}
		if ((idl - 0x80) == LD_drive_id_0x0A)
		{
			state.motor2_heart_interval_time = 0;
			id = 1;
		}
	}

	if (idh == LD_command_rx)						//应答回复“请求读取寄存器”的功能码
	{
		if ((idl - 0x80) == LD_drive_id_0x09)
		{
			state.motor1_heart_interval_time = 0;		//复位驱动器离线检测时间
			id = 0;										//确定驱动器ID
		}
		if ((idl - 0x80) == LD_drive_id_0x0A)
		{
			state.motor2_heart_interval_time = 0;
			id = 1;
		}

		data1 = DatA->frame.data[2] | (DatA->frame.data[3] << 8);
		data2 = DatA->frame.data[4] | (DatA->frame.data[5] << 8);
		data3 = DatA->frame.data[6] | (DatA->frame.data[7] << 8);

		if (DatA->frame.data[1] == 2)
		{
			switch (DatA->frame.data[0])												//根据识别码判断返回数据
			{

			case (LD_actual_sys_state1_2):											//识别码为120，数据为系统标志位1-2
				LD_actual[id].sys_state_1 = data1;									//装载
				LD_actual[id].sys_state_2 = data2;
				break;
			case (LD_actual_sys_state3_4):											//识别码为120，数据为系统标志位1-2
			{
				LD_actual[id].state3.sys_inti = (data1 & bit_0);
				LD_actual[id].state3.sys_run_end = (data1 & bit_1) >> 1;
				//LD_actual[id].state3.mot_sp_ze_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_sp_ze_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_sp_at_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_sp_at_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.sys_run_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.sys_run_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_dir_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_dir_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_pwm_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_pwm_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_lim_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_lim_1 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_pwm_int_2 = (data1 & bit_15) >> 15;
				//LD_actual[id].state3.mot_pwm_int_1 = (data1 & bit_15) >> 15;
			}

				break;
			case (LD_actual_speed):												//识别码为132，数据为实际转速
				LD_actual[id].speed_L = data1;										//装载
				LD_actual[id].speed_R = data2;
				control.act.v1 = LD_actual[0].speed_L;
				control.act.v2 = LD_actual[0].speed_R;
				control.act.v3 = LD_actual[1].speed_L;
				control.act.v4 = LD_actual[1].speed_R;
				break;

			case (LD_actual_current):												//识别码为142，数据为实际电流
				LD_actual[id].current_L = data1;									//装载
				LD_actual[id].current_R = data2;
				control.act.i1 = LD_actual[0].current_L;
				control.act.i2 = LD_actual[0].current_R;
				control.act.i3 = LD_actual[1].current_L;
				control.act.i4 = LD_actual[1].current_R;
				break;

			case (LD_actual_power):												//识别码为144，数据为实际功率
				LD_actual[id].power_L = data1;									//装载
				LD_actual[id].power_R = data2;
				break;

			case (LD_actual_tempH):												//识别码为146，数据为实际温度1-2
				LD_actual[id].tempH_1 = data1 * 0.1;										//装载
				LD_actual[id].tempH_2 = data2 * 0.1;
				control.act.t1 = (LD_actual[0].tempH_1 + LD_actual[0].tempL_1)/2;
				control.act.t2 = (+LD_actual[0].tempH_2 + LD_actual[0].tempL_2)/2;

				break;

			case (LD_actual_tempL):												//识别码为148，数据为实际温度3-4
				LD_actual[id].tempL_1 = data1 * 0.1;										//装载
				LD_actual[id].tempL_2 = data2 * 0.1;
				control.act.t3 = (LD_actual[1].tempH_1 + LD_actual[1].tempL_1) / 2;
				control.act.t4 = (+LD_actual[1].tempH_2 + LD_actual[1].tempL_2) / 2;
				break;

			case (LD_actual_torque):												//识别码为158，数据为实际扭矩
				LD_actual[id].torque_L = data1;										//装载
				LD_actual[id].torque_R = data2;
				break;
			}
		}
	}
}


//-------------------------------------------------------------------------------------
//函数功能：电机使能
//函数输入：无
//函数输出：无
//-------------------------------------------------------------------------------------
uint8_t SXLD_mot_enable(uint16_t cyc,uint8_t tery)
{
	if (cyc % 200 == 0)
	{
		if (tery == 1)
		{
			SXLD_slide_pool(0x06, 0x09, 0xA4, 0x02, 0XFF00, 0xFF00, 0);
			SXLD_slide_pool(0x06, 0x0A, 0xA4, 0x02, 0XFF00, 0xFF00, 0);
		}
		if (tery == 0)
		{
			SXLD_slide_pool(0x06, 0x09, 0xA4, 0x02, 0X00FF, 0x00FF, 0);
			SXLD_slide_pool(0x06, 0x0A, 0xA4, 0x02, 0X00FF, 0x00FF, 0);
		}
	}
	if ((SXLD_abnormal_uninit(0) == 1) && (SXLD_abnormal_uninit(1) == 1))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//-------------------------------------------------------------------------------------
//函数功能：电机速度发送
//函数输入：期望速度
//-------------------------------------------------------------------------------------
void SXLD_motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4)
{
	static uint8_t titt = 0;
	//函数输入：功能码，CanID，请求地址，寄存器数（最大个数为3,16bit），数据1，数据2，数据3
	SXLD_slide_pool(0x06, 0x09, 0x5A, 0x02, v2, v1, 0);
	SXLD_slide_pool(0x06, 0x0A, 0x5A, 0x02, v4, v3, 0);
	titt++;
	if (titt >= 4)
	{
		titt = 0;
		SXLD_slide_pool(0x06, 0x09, 0xA4, 0x02, 0XFF00, 0xFF00, 0);
		SXLD_slide_pool(0x06, 0x0A, 0xA4, 0x02, 0XFF00, 0xFF00, 0);
	}

}


//山西零度电驱异常：未初始化返回0，初始化返回1
uint8_t SXLD_abnormal_uninit(uint8_t id)
{
	//电驱未初始化未使能
	if ((LD_actual[id].state3.sys_inti==0)||(LD_actual[id].state3.sys_run_end == 0))
	{
		return 0;
	}
	//电驱已初始化、已使能
	else
	{
		return 1;
	}
}