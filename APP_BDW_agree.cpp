#include "APP_BDW_agree.h"
#include "driver_CAN.h"
#include "APP_abnormal.h"

BDW_pool BDW_actual[2];

extern ControL control;//中控函数结构体
extern AbnormaL state;

//-------------------------------------------------------------------------------------
//函数功能：数据处理函数
//函数输入：CAN_Data* DatA----CAN接收数据结构体
//-------------------------------------------------------------------------------------
void BDW_can_data_recive_storage(CAN_Data* DatA)
{
	int16_t COBID = 0;
	uint8_t id = 0;
	COBID = DatA->frame.can_id;
	if ((COBID == BDW_RX_cobid_1) || (COBID == BDW_RX_cobid_2))					//返回数据
	{
		if (COBID == BDW_RX_cobid_1)id = 0;
		if (COBID == BDW_RX_cobid_2)id = 1;
		switch (DatA->frame.data[0])
		{
		case (BDW_answer_read_8bit):rx_8bit(DatA->frame.data, id);				//接收8位数据
			break;
		case (BDW_answer_read_16bit):rx_16bit(DatA->frame.data, id);			//接收16位数据
			break;
		case (BDW_answer_read_32bit):rx_32bit(DatA->frame.data, id);			//接收32位数据
			break;
		case (BDW_answer_error):BDW_read_write_error(DatA->frame.data, id);		//读写错误报文
			break;
		}
	}
	if ((COBID == BDW_hearbeat_ID1) || (COBID == BDW_hearbeat_ID2))				//心跳报文响应
	{
		//待升级，应将时间变量定义移动到BDW中
		if (COBID == BDW_hearbeat_ID1)motor_heart_echo(0);
		if (COBID == BDW_hearbeat_ID2)motor_heart_echo(1);
	}
}

//-------------------------------------------------------------------------------------
//函数功能：8bit数据处理函数
//函数输入：
// uint8_t* DatA----CAN接收数据
// uint8_t id-------驱动器ID
//-------------------------------------------------------------------------------------
void rx_8bit(uint8_t* DatA, uint8_t id)
{
	DatA[0] = 0;
}

//-------------------------------------------------------------------------------------
//函数功能：16bit数据处理函数
//函数输入：
// uint8_t* DatA----CAN接收数据
// uint8_t id-------驱动器ID
//-------------------------------------------------------------------------------------
void rx_16bit(uint8_t* DatA, uint8_t id)
{
	uint32_t index = 0;
	uint16_t data0 = 0;
	index = (DatA[3]) | (DatA[2] << 16) | (DatA[1] << 8);
	data0 = (DatA[4]) | (DatA[5] << 8);
	switch (index)
	{
	case (BDW_digital_io_input_state):BDW_actual[id].Digital_io_input_state = data0;            //数字IO输入状态
		break;
	case (BDW_digital_io_output_state):BDW_actual[id].Digital_io_output_state = data0;			//数字IO输出状态
		break;

	case (BDW_actual_current_L):BDW_actual[id].current_L = data0;								//实际电流
		break;
	case (BDW_actual_current_R):BDW_actual[id].current_R = data0;								//实际电流
		break;

	case (BDW_actual_error1_L):BDW_actual[id].error1_L = data0;									//故障1
		break;
	case (BDW_actual_error1_R):BDW_actual[id].error1_R = data0;									//故障1
		break;

	case (BDW_actual_state1_L):BDW_actual[id].state1_L = data0;									//状态1
		break;
	case (BDW_actual_state1_R):BDW_actual[id].state1_R = data0;									//状态1
		break;

	case (BDW_actual_error2_L):BDW_actual[id].error2_L = data0;									//故障2
		break;
	case (BDW_actual_error2_R):BDW_actual[id].error2_R = data0;									//故障2
		break;

	case (BDW_actual_state2_L):BDW_actual[id].state2_L = data0;									//状态2
		break;
	case (BDW_actual_state2_R):BDW_actual[id].state2_R = data0;									//状态2
		break;

	case (BDW_actual_temp_heat_sink):
		BDW_actual[id].temp_heat_sink = data0;						//散热片温度
		break;
	case (BDW_actual_hall):BDW_actual[id].hall = data0;											//霍尔状态
		break;
	case (BDW_actual_tempe_mcu):BDW_actual[id].tempe_mcu = data0;								//mcu温度
		break;

	case (BDW_actual_current_set_L):BDW_actual[id].current_set_L = data0;						//给定电流
		break;
	case (BDW_actual_current_set_R):BDW_actual[id].current_set_R = data0;						//给定电流
		break;

	case (BDW_analog_input0_state):BDW_actual[id].Analog_input0_state = data0;					//模拟输入0电压
		break;
	case (BDW_analog_input1_state):BDW_actual[id].Analog_input1_state = data0;					//模拟输入1电压
		break;

	case (BDW_actual_current_total):BDW_actual[id].current_total = data0;						//母线电流
		break;
	case (BDW_actual_power):BDW_actual[id].power = data0;										//输入功率
		break;

	case (BDW_actual_power_L):BDW_actual[id].power_L = data0;									//功率
		break;
	case (BDW_actual_power_R):BDW_actual[id].power_R = data0;									//功率
		break;
	}
}

//-------------------------------------------------------------------------------------
//函数功能：32bit数据处理函数
//函数输入：
// uint8_t* DatA----CAN接收数据
// uint8_t id-------驱动器ID
//-------------------------------------------------------------------------------------
void rx_32bit(uint8_t* DatA, uint8_t id)
{
	uint32_t index = 0;
	int32_t data0 = 0;
	index = (DatA[3]) | (DatA[2] << 16) | (DatA[1] << 8);
	data0 = ((uint32_t)DatA[4]) | ((uint32_t)DatA[5] << 8) | ((uint32_t)DatA[6] << 16) | ((uint32_t)DatA[7] << 24);

	switch (index)
	{
	case (BDW_actual_speed_L):
		BDW_actual[id].speed_L = data0;									//实际速度
		break;
	case (BDW_actual_speed_R):BDW_actual[id].speed_R = data0;									//实际速度
		break;

	case (BDW_actual_position_L):BDW_actual[id].position_L = data0;								//实际位置
		break;
	case (BDW_actual_position_R):BDW_actual[id].position_R = data0;								//实际位置
		break;

	case (BDW_actual_speed_set_L):BDW_actual[id].speed_set_L = data0;							//给定速度
		break;
	case (BDW_actual_speed_set_R):BDW_actual[id].speed_set_R = data0;							//给定速度
		break;
	}
}

//-------------------------------------------------------------------------------------
//函数功能：32bit数据处理函数
//函数输入：
// uint8_t* DatA----CAN接收数据
// uint8_t id-------驱动器ID
//-------------------------------------------------------------------------------------
void BDW_read_write_error(uint8_t* DatA, uint8_t id)
{
	uint32_t data0 = 0;
	data0 = DatA[6] | DatA[7] << 8 | DatA[8] << 16 | DatA[9] << 24;
	//RW_error = data0;
}

//-------------------------------------------------------------------------------------
//函数功能：滑动输入器
//函数说明；在短时间内发送大量数据
//函数输入：CanID，数据类型、功能码、数据
//-------------------------------------------------------------------------------------
void BDW_slide_pool(uint16_t Cob_iD, uint8_t CommanD, uint32_t IndeX, uint32_t DatA)
{
	uint32_t ID = 0;
	uint8_t ExiD = 0;
	uint8_t len = 0;
	uint8_t DatAbf[8] = { 0 };
	ExiD = 0;
	ID = Cob_iD;

	DatAbf[0] = CommanD;
	DatAbf[1] = IndeX >> 16;
	DatAbf[2] = (IndeX & 0xFF00) >> 8;
	DatAbf[3] = IndeX & 0xFF;
	switch (CommanD)
	{
	case (BDW_command_write_8bit):
		DatAbf[4] = DatA;
		DatAbf[5] = 0;
		DatAbf[6] = 0;
		DatAbf[7] = 0;
		len = 5;
		break;

	case (BDW_command_write_16bit):
		DatAbf[4] = (DatA & 0xFF00) >> 8;
		DatAbf[5] = DatA & 0xFF;
		DatAbf[6] = 0;
		DatAbf[7] = 0;
		len = 6;
		break;
	case (BDW_command_write_32bit):
		DatAbf[4] = (DatA & 0xFF000000) >> 24;
		DatAbf[5] = (DatA & 0xFF0000) >> 16;
		DatAbf[6] = (DatA & 0xFF00) >> 8;
		DatAbf[7] = (DatA & 0xFF);
		len = 8;
		break;
	case (BDW_command_read):
		DatAbf[4] = 0;
		DatAbf[5] = 0;
		DatAbf[6] = 0;
		DatAbf[7] = 0;
		len = 4;
		break;
	}
	CAN_pool_input(ID, ExiD, len, DatAbf);
}

//-------------------------------------------------------------------------------------
//函数功能：速度控制初始化，配置电机驱动
//函数输入：uint8_t* DatA
//-------------------------------------------------------------------------------------
uint8_t BDW_speed_control_init(uint16_t time, uint8_t cycle)
{
	uint8_t mark = 0;				//0:未完成、1：完成
	static uint16_t CounteE = 0;

	if (time % cycle == 0)
	{
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state1_L, 0);			//请求左电机状态1
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state1_R, 0);			//请求左电机状态2
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state1_L, 0);			//请求右电机状态1
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state1_R, 0);			//请求右电机状态2
	}
	if ((time % cycle) == (cycle / 2))
	{
		if ((BDW_actual[0].state1_L & 0xF == 0xF) || (BDW_actual[1].state1_L & 0xF == 0xF))
		{
			//BDW_inti = 1;						//标记初始化完成
			mark = 1;
			printf("八达威------>驱动初始化成功\n");
		}
		else
		{
			CounteE++;
			if (CounteE >= 10)
			{
				CounteE = 0;
				printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");			//清屏
				printf("重试初始化电机驱动\n");
			}
			//驱动1未初始化成功
			if ((BDW_actual[0].state1_L == 0x1D) || (BDW_actual[0].state1_R == 0x1D))
			{
				BDW_slide_pool(BDW_cobid_1, BDW_command_write_8bit, BDW_CiA402_operationmode, BDW_mode_velocity);			//配置控制模式为速度控制
				BDW_slide_pool(BDW_cobid_1, BDW_command_write_16bit, BDW_CiA402_control_word, BDW_byte_reset);				//复位错误
				BDW_slide_pool(BDW_cobid_1, BDW_command_write_16bit, BDW_CiA402_control_word, BDW_byte_enable_on);			//使能开
				printf("驱动1复位中\n");
			}
			//驱动2未初始化成功
			if ((BDW_actual[1].state1_L == 0x1D) || (BDW_actual[1].state1_R == 0x1D))
			{
				BDW_slide_pool(BDW_cobid_2, BDW_command_write_8bit, BDW_CiA402_operationmode, BDW_mode_velocity);			//配置控制模式为速度控制
				BDW_slide_pool(BDW_cobid_2, BDW_command_write_16bit, BDW_CiA402_control_word, BDW_byte_reset);				//复位错误
				BDW_slide_pool(BDW_cobid_2, BDW_command_write_16bit, BDW_CiA402_control_word, BDW_byte_enable_on);			//使能开
				printf("驱动2复位中\n");
			}
		}
		printf("%4X       %4X       %4X      %4X\n\n", BDW_actual[0].state1_L, BDW_actual[0].state1_R, BDW_actual[1].state1_L, BDW_actual[1].state1_R);
		//清空旧数据
		BDW_actual[0].state1_L = 0;
		BDW_actual[0].state1_R = 0;
		BDW_actual[1].state1_L = 0;
		BDW_actual[1].state1_R = 0;
	}
	return mark;
}

//-------------------------------------------------------------------------------------
//函数功能：速度控制参数发送
//函数输入：
//函数输出：无
//-------------------------------------------------------------------------------------
void BDW_motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4)
{
	if (v1 > 3500)v1 = 2500;
	if (v1 < -3500)v1 = -2500;
	if (v2 > 3500)v2 = 2500;
	if (v2 < -3500)v2 = -2500;
	if (v3 > 3500)v3 = 2500;
	if (v3 < -3500)v3 = -2500;
	if (v4 > 3500)v4 = 2500;
	if (v4 < -3500)v4 = -2500;
	BDW_slide_pool(BDW_cobid_1, BDW_command_write_32bit, BDW_CiA402_target_velocity_L, v1);		//发送电机速度1
	BDW_slide_pool(BDW_cobid_1, BDW_command_write_32bit, BDW_CiA402_target_velocity_R, v2);		//发送电机速度2
	BDW_slide_pool(BDW_cobid_2, BDW_command_write_32bit, BDW_CiA402_target_velocity_L, v3);		//发送电机速度3
	BDW_slide_pool(BDW_cobid_2, BDW_command_write_32bit, BDW_CiA402_target_velocity_R, v4);		//发送电机速度4
}

//-------------------------------------------------------------------------------------
//函数功能：电流控制参数发送
//函数输入：
//函数输出：无
//-------------------------------------------------------------------------------------
void BDW_motor_current_out(int32_t I1, int32_t I2, int32_t I3, int32_t I4)
{
	BDW_slide_pool(BDW_cobid_1, BDW_command_write_32bit, BDW_CiA402_target_torque_L, I1);		//发送电机速度1
	BDW_slide_pool(BDW_cobid_1, BDW_command_write_32bit, BDW_CiA402_target_torque_R, I2);		//发送电机速度2
	BDW_slide_pool(BDW_cobid_2, BDW_command_write_32bit, BDW_CiA402_target_torque_L, I3);		//发送电机速度3
	BDW_slide_pool(BDW_cobid_2, BDW_command_write_32bit, BDW_CiA402_target_torque_R, I4);		//发送电机速度4
}

//-------------------------------------------------------------------------------------
//函数功能：周期性请求数据
//函数说明；系统时序优化，定期发送请求数据指令
//函数输入：系统时间
//-------------------------------------------------------------------------------------
void BDW_cycle_request_data(uint16_t time)
{
	if (time % 100 == 0)
	{
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_speed_L, 0);	//请求速度
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_speed_R, 0);	//请求速度
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_speed_L, 0);	//请求速度
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_speed_R, 0);	//请求速度
	}
	if (time % 100 == 16)
	{
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_current_L, 0);	//请求电流
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_current_R, 0);	//请求电流
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_current_L, 0);	//请求电流
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_current_R, 0);	//请求电流
	}

	//if (time % 200 == 32)
	//{
	//	BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_power_L, 0);			//请求功率
	//	BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_power_R, 0);			//请求功率
	//	BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_power_L, 0);			//请求功率
	//	BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_power_R, 0);			//请求功率
	//}

	if (time % 500 == 48)
	{
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_temp_heat_sink, 0);	//请求散热片温度
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_tempe_mcu, 0);			//请求mcu温度
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_temp_heat_sink, 0);	//请求散热片温度
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_tempe_mcu, 0);			//请求mcu
		//printf("\n    T1=%4d          T2=%4d           T3=%4d          T4=%4d\n\n", BDW_actual[0].temp_heat_sink, BDW_actual[0].tempe_mcu, BDW_actual[1].temp_heat_sink, BDW_actual[1].tempe_mcu);
	}

	if (time % 500 == 64)																//请求错误、状态
	{
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state1_L, 0);			//请求左电机状态1
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state2_L, 0);			//请求左电机状态2
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state1_R, 0);			//请求右电机状态1
		BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_state2_R, 0);			//请求右电机状态2

		//BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_error1_L, 0);			//请求左电机状态1
		//BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_error2_L, 0);			//请求左电机状态2
		//BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_error1_R, 0);			//请求右电机状态1
		//BDW_slide_pool(BDW_cobid_1, BDW_command_read, BDW_actual_error2_R, 0);			//请求右电机状态2

		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state1_L, 0);			//请求左电机状态1
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state2_L, 0);			//请求左电机状态2
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state1_R, 0);			//请求右电机状态1
		BDW_slide_pool(BDW_cobid_2, BDW_command_read, BDW_actual_state2_R, 0);			//请求右电机状态2
	}


}

//驱动器心跳报文响应
void motor_heart_echo(uint8_t id)
{
	if (id == 0)state.motor1_heart_interval_time = 0;							//重置时间
	if (id == 1)state.motor2_heart_interval_time = 0;							//重置时间
}