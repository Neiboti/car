#ifndef __APP_ECU_agree_H__
#define __APP_ECU_agree_H__

#include "APP_control.h"
#include "driver_CAN.h"

#define Communication_time    100  //ECU通信时间，用于设置周期性的发送ECU控制信号，一般为100MS


//------------------ECU通信参数
//#define ECU_ID 0x11000002
//#define ECU_ID_cyc 0x11000001		//ECU定时上报ID

#define ECU_ID 0x11000002
#define ECU_ID_cyc 0x91000002		//ECU定时上报ID

//------------------灯控宏定义
#define axis_lock_off 0					//抱闸关
#define axis_lock_on  1					//抱闸开

#define power_motor_off 2				//驱动供电关
#define power_motor_on  3				//驱动供电开

#define power_headlight_off 4			//前车灯关
#define power_headlight_on  5			//前车灯开
#define power_headlight_flash 6			//前车灯闪

#define power_rearlights_off 7			//后车灯关
#define power_rearlights_on  8			//后车灯开
#define power_rearlights_flash  9		//后车灯闪

#define power_L_lamp_off	10			//左车灯关
#define power_L_lamp_on		11			//左车灯开
#define power_L_lamp_flash  12			//左车灯闪

#define power_R_lamp_off	13			//右车灯关
#define power_R_lamp_on		14			//右车灯开
#define power_R_lamp_flash  15			//右车灯闪

#define powre_clearance_lamp_off 16		//示廓灯关
#define powre_clearance_lamp_on 17		//示廓灯开
#define powre_clearance_lamp_flash 18	//示廓灯闪

#define powre_upper_supply_off 19		//上装供电关
#define powre_upper_supply_on 20		//上装供电开



typedef struct ECU_DRR
{
	uint8_t ECU_init;				//ECU初始化标志
	uint16_t com_time;				//ECU通信时间，用于设置周期性的发送ECU控制信号，一般为100MS
	uint32_t switch_light;			//ECU开关标志位，
	uint8_t ECU_data0;				//备用数据0

	uint32_t ECU_sete;				//ECU开关状态



	uint8_t axis_lock;				//抱闸开关：0关、1开
	uint8_t power_motor;			//前电机驱动供电：0关、1开
	uint8_t powre_upper_supply;		//上装供电：0关、1开

	uint8_t power_headlight;		//前车灯供电：0关、1开
	uint32_t power_headlight_time;	//前车灯闪烁时间：0不闪烁、1----65536闪烁间隔

	uint8_t power_rearlight;		//后车灯供电：0关、1开
	uint32_t power_rearlight_time;	//后车灯闪烁时间：0不闪烁、1----65536闪烁间隔

	uint8_t power_L_lamp;			//左车灯供电：0关、1开
	uint32_t power_L_lamp_time;		//左车灯闪烁时间：0不闪烁、1----65536闪烁间隔

	uint8_t power_R_lamp;			//右车灯供电：0关、1开
	uint32_t power_R_lamp_time;		//右车灯闪烁时间：0不闪烁、1----65536闪烁间隔

	uint8_t powre_clearance_lamp;			//示廓灯供电：0关、1开
	uint32_t powre_clearance_lamp_time;		//示廓灯闪烁时间：0不闪烁、1----65536闪烁间隔


}ECU_DRR;









void ECU_init();
void ECU_light(uint32_t code, uint32_t FlasH);
void ECU_light_flash(uint32_t TimE);
void ECU_slide_pool(uint32_t IiggT, uint8_t data0);
void APP_ECU();


uint8_t ECU_handbrake_check();				//ECU抱闸检测

void ECU_CAN_read(CAN_Data* DatA);			//ECU报文读取

//获取ECU粗略状态
uint16_t get_ECU_rough_state_16bit();
//获取ECU粗略状态
uint8_t get_ECU_rough_state_8bit();

#endif
