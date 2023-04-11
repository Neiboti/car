#ifndef __APP_FOL_H__
#define __APP_FOL_H__
#include "APP_control.h"


//一阶低通滤波结构体
typedef struct
{
	float A;			//滤波系数
	float Output;		//滤波输出
}FOL_FilterTypeDef;

extern FOL_FilterTypeDef FOL_Motor[4];

void APP_FOL_Init(void);																//滤波初始化
float FirstOrderLowPassFilter(float ResrcData, FOL_FilterTypeDef* fol);				//滤波服务函数
float APP_FOL_motor_out(float ResrcData, char SL);										//码盘专用滤波器0-3
float APP_FOL_handle(float ResrcData, char SL);											//手柄滤波专用


#endif
