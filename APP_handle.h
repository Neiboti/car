#ifndef __APP_handle_H__
#define __APP_handle_H__

#include "APP_control.h"


typedef struct HandlE
{
	uint8_t enable;					//手柄使能
	uint8_t setat;					//手柄状态，0未初始化，1离线、2在线
}HandlE;



uint8_t APP_scene_handle_abnormal();                       //现场手柄异常：现场手柄离线

void handle_control(void);











#endif
