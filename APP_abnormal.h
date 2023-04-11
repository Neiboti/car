#ifndef __APP_ABNORMAL_H__
#define __APP_ABNORMAL_H__
#include "APP_control.h"



//-------------------------------------异常编号宏定义---------------------------------------------------

#define code_HMAC_off_line		0X0001					//上位机离线

#define code_motor1_off_line	0X0002					//前驱离线				
#define code_motor2_off_line	0X0003					//后驱离线

#define code_ECU_off_line		0X0004					//ECU离线

#define code_handbrake_lock		0X0005					//抱闸未打开

#define code_motor_uninit		0X0006					//驱动器未初始化
#define code_motor1_uninit		0X0007					//前驱 未初始化			
#define code_motor2_uninit		0X0008					//后驱 未初始化

#define code_scene_handle_unoffline		0X0009		//现场手柄离线

#define code_UDP_breakdown		0X000A					//UDP故障


//警报、错误状态结构体
typedef struct AbnormaL
{
	uint8_t whole;							//整体状态
	uint8_t warning;						//警报数量
	uint8_t error;							//错误数量

	uint16_t abnormal_check_cyc;			//异常点扫描周期
	uint32_t abnormal_time;					//异常计时

	uint16_t HMAC_heart_interval_time;		///上位机心跳间隔时间
	uint8_t  HMAC_offline;					//上位机心跳状态：0未知、1在线、2离线


	uint16_t motor1_heart_interval_time;	//电机驱动心跳间隔时间
	uint16_t motor2_heart_interval_time;	//电机驱动心跳间隔时间
	uint8_t motor1_offline;					//电驱状态：0未知、1安全、2离线
	uint8_t motor2_offline;					//电驱状态：0未知、1安全、2离线

	uint8_t motor_uninit;					//电驱就绪状态标志位：0未知、1就绪、2未就绪
	uint8_t motor1_uninit;					//前驱就绪状态标志位：0未知、1就绪、2未就绪
	uint8_t motor2_uninit;					//后驱就绪状态标志位：0未知、1就绪、2未就绪


	uint8_t handbrake;						//抱闸：1打开、2锁上

	uint16_t ECU_heart_interval_time;		//ECU驱动心跳间隔时间
	uint8_t ECU_offline;					//ECU驱动心跳状态：0未知、1在线、2离线

	uint8_t scene_handle;					//现场手柄状态

	uint8_t UDP_state;						//UDP状态：0未知、1正常、2错误


}AbnormaL;

//现场手柄离线检测
void APP_scene_handle_offline_testing();


//异常模块初始化
void APP_abnormal_init();

//异常扫描、显示、处理模块
void APP_abnormal();

//异常状态查询
uint8_t APP_abnormal_consult();

//异常模块
void APP_abnormal();
//上位机离线处理
void APP_HMAC_offline_solve();

//UDP状态检测
void APP_UDP_state_testing(uint16_t cyc);

#endif
