#include "APP_ECU_agree.h"
#include "driver_CAN.h"
#include "APP_abnormal.h"



ECU_DRR ECU = { 0 };			//ECU结构体

extern ControL control;//中控函数结构体
extern AbnormaL state;

//ECU参数初始化
void ECU_init()
{
	ECU.com_time = Communication_time;			//设定ECU速度发送周期
	ECU.axis_lock = 1;							//抱闸开
	ECU.power_motor = 0;						//电机驱动供电开
	ECU.power_headlight = 0;					//前灯开
	//ECU_light(power_headlight_flash, 500);	//前灯闪烁，500MS
	//ECU_light(power_R_lamp_flash, 500);			//右灯闪烁，500MS
	//ECU_light(power_L_lamp_flash, 500);			//左灯闪烁，500MS

	ECU.powre_upper_supply = 1;				//上装供电

	printf("ECU初始化成功\n");
}

void ECU_CAN_read(CAN_Data* DatA)
{
	uint32_t id = 0;

	if (DatA->frame.can_id == ECU_ID_cyc)
	{
		state.ECU_heart_interval_time = 0;
		ECU.ECU_sete = DatA->frame.data[3] | (DatA->frame.data[2] << 8) | (DatA->frame.data[1] << 16) | (DatA->frame.data[0] << 24);
		//printf("ECU_SETE=%x\n", ECU.ECU_sete);
	}

}





//ECU灯控相关
//指令码，闪烁时间，0为不闪烁
void ECU_light(uint32_t code, uint32_t FlasH)
{
	switch (code)
	{
	case (axis_lock_off):ECU.axis_lock = 0x00; break;
	case (axis_lock_on):ECU.axis_lock = 0x01; break;
	case (power_motor_off):ECU.power_motor = 0x00; break;
	case (power_motor_on):ECU.power_motor = 0x01; break;

	case (power_headlight_off):ECU.power_headlight = 0x00; ECU.power_headlight_time = 0; break;
	case (power_headlight_on):ECU.power_headlight = 0x01; ECU.power_headlight_time = 0; break;
	case (power_headlight_flash):ECU.power_headlight_time = FlasH; break;

	case (power_rearlights_off):ECU.power_rearlight = 0x00; ECU.power_rearlight_time = 0; break;
	case (power_rearlights_on):ECU.power_rearlight = 0x01; ECU.power_rearlight_time = 0; break;
	case (power_rearlights_flash):ECU.power_rearlight_time = FlasH; break;

	case (power_L_lamp_off):ECU.power_L_lamp = 0x00; ECU.power_L_lamp_time = 0; break;
	case (power_L_lamp_on):ECU.power_L_lamp = 0x01; ECU.power_L_lamp_time = 0; break;
	case (power_L_lamp_flash):ECU.power_L_lamp_time = FlasH; break;

	case (power_R_lamp_off):ECU.power_R_lamp = 0x00; ECU.power_R_lamp_time = 0; break;
	case (power_R_lamp_on):ECU.power_R_lamp = 0x01; ECU.power_R_lamp_time = 0; break;
	case (power_R_lamp_flash):ECU.power_R_lamp_time = FlasH; break;
	}
}
//
void ECU_light_flash(uint32_t TimE)
{
	if (ECU.power_headlight_time != 0)
	{
		if ((TimE % ECU.power_headlight_time) == 0)
		{
			ECU.power_headlight = (ECU.power_headlight + 1) % 2;
		}
	}

	if (ECU.power_rearlight_time != 0)
	{
		if ((TimE % ECU.power_rearlight_time) == 0)
		{
			ECU.power_rearlight = (ECU.power_rearlight + 1) % 2;
		}
	}

	if (ECU.power_L_lamp_time != 0)
	{
		if ((TimE % ECU.power_L_lamp_time) == 0)
		{
			ECU.power_L_lamp = (ECU.power_L_lamp + 1) % 2;
		}
	}

	if (ECU.power_R_lamp_time != 0)
	{
		if ((TimE % ECU.power_R_lamp_time) == 0)
		{
			ECU.power_R_lamp = (ECU.power_R_lamp + 1) % 2;
		}
	}

}

//ECU抱闸检测
uint8_t ECU_handbrake_check()
{
	uint8_t mark = 0;
	if ((ECU.power_motor == 1) && (ECU.axis_lock == 1))mark = 1;
	if ((ECU.power_motor == 0) && (ECU.axis_lock == 0))mark = 0;
	return mark;
}




//
void ECU_slide_pool(uint32_t IiggT, uint8_t data0)
{
	uint32_t ID = 0;
	uint8_t ExiD = 0;
	uint8_t len = 0;
	uint8_t DatAbf[8] = { 0 };

	ID = ECU_ID;		//设定ECUID
	ExiD = 1;				//标记为扩展帧
	len = 8;				//数据长度

	//DatAbf[0] = (IiggT & 0XF000) >> 12;
	//DatAbf[1] = (IiggT & 0X0F00) >> 8;
	//DatAbf[2] = (IiggT & 0X00F0) >> 4;
	//DatAbf[3] = (IiggT & 0X000F);
	//DatAbf[4] = data0;
	//DatAbf[0] = 0xF;
	//DatAbf[1] = 0xF;
	//DatAbf[2] = 0xF;
	//DatAbf[3] = 0xF;
	DatAbf[0] = (IiggT & 0XFF00) >> 8;
	DatAbf[1] = (IiggT & 0X00FF);


	CAN_pool_input(ID, ExiD, len, DatAbf);
}



//ECU主线程
void APP_ECU()
{
	uint32_t TimE = 0;

	while (1)
	{

		if (((TimE % ECU.com_time) == 0) && (TimE != 0))
		{
			ECU_light_flash(TimE);

			ECU.switch_light = (0x3000 * ECU.power_L_lamp) + (0x0C00 * ECU.power_R_lamp) + /*(0x00C0 * ECU.power_motor)*/ + (0x000C * ECU.axis_lock) + (0x0003 * ECU.power_headlight) + (0x0030 * ECU.powre_upper_supply);

			ECU_slide_pool(ECU.switch_light, ECU.ECU_data0);
			//ECU_slide_pool(0xFFFF, ECU.ECU_data0);
		}
		TimE = (TimE + 1) % 0xFFFFFFFE;
		usleep(1000);
	}


}





//K8--------K7--------K6--------K5--------K4--------K3--------K2--------K1
//空--------左灯------右灯------警报灯-----驱供电-----上装------抱闸-------前大灯
//0xC000----0x3000----0x0C00----0x0300----0x00c0----0x0030----0x000c----0x0003


//------------------------模块对外接口部分-------------------------------------------

//获取ECU粗略状态
uint16_t get_ECU_rough_state_16bit()
{
	//共计8个开关，每个开关占两位，从左到右：k8->k1
	//00为关闭、01为常亮、11为闪烁。
	uint16_t state = 0;
	uint8_t k[8] = { 0 };

	if (ECU.power_headlight_time == 0)k[0] = ECU.power_headlight;
	if ((ECU.power_headlight_time != 0) && (ECU.power_headlight == 1))k[0] = 0x11;

	k[1] = ECU.axis_lock;
	k[2] = ECU.powre_upper_supply;
	k[3] = ECU.power_motor;
	k[4] = 0;

	if (ECU.power_R_lamp_time == 0)k[5] = ECU.power_R_lamp;
	if ((ECU.power_R_lamp_time != 0) && (ECU.power_R_lamp == 1))k[5] = 0x11;
	if (ECU.power_L_lamp_time == 0)k[6] = ECU.power_L_lamp;
	if ((ECU.power_L_lamp_time != 0) && (ECU.power_L_lamp == 1))k[6] = 0x11;
	k[7] = 0;

	state = (k[7] << 14) | (k[6] << 12) | (k[5] << 10) | (k[4] << 8) | (k[3] << 6) | (k[2] << 4) | (k[1] << 2) | k[0];
	return state;
}

//获取ECU粗略状态
uint8_t get_ECU_rough_state_8bit()
{
	//共计8个开关，每个开关占1位，从左到右：k8->k1
	//0为关闭、1为亮、
	uint8_t state = 0;
	uint8_t k[8] = { 0 };

	k[0] = ECU.power_headlight;
	k[1] = ECU.axis_lock;
	k[2] = ECU.powre_upper_supply;
	k[3] = ECU.power_motor;
	k[4] = 0;
	k[5] = ECU.power_R_lamp;
	k[6] = ECU.power_L_lamp;
	k[7] = 0;

	state = (k[7] << 7) | (k[6] << 6) | (k[5] << 5) | (k[4] << 4) | (k[3] << 3) | (k[2] << 2) | (k[1] << 1) | k[0];
	return state;
}
