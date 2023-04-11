#include "APP_FOL.h"


/*↓↓↓↓↓↓↓------------滤波结构体初始化值----------------↓↓↓↓↓↓*/
FOL_FilterTypeDef FOL_Motor_Init =
{
	.A = 0.35,
	.Output = 0,
};

FOL_FilterTypeDef FOL_handle_Init =
{
	.A = 0.50,
	.Output = 0,
};




FOL_FilterTypeDef FOL_Motor[4];
FOL_FilterTypeDef FOL_handle[2];

/*-------------------------------------------------------------------------
函数名称：float FirstOrderLowPassFilter(float ResrcData, FOL_FilterTypeDef* fol)
函数功能：一阶低通滤波
函数输入：需要滤波的值，滤波结构体
函数输出：无
--------------------------------------------------------------------------*/
float FirstOrderLowPassFilter(float ResrcData, FOL_FilterTypeDef* fol)
{

	fol->Output = (1 - fol->A) * fol->Output + fol->A * ResrcData;
	return fol->Output;
}

/*-------------------------------------------------------------------------
函数名称：void FOL_Init(void)
函数功能：滤波结构体初始化
函数输入：
函数输出：无
--------------------------------------------------------------------------*/
void APP_FOL_Init(void)
{
	FOL_Motor[0] = FOL_Motor_Init;
	FOL_Motor[1] = FOL_Motor_Init;
	FOL_Motor[2] = FOL_Motor_Init;
	FOL_Motor[3] = FOL_Motor_Init;
	FOL_handle[0] = FOL_handle_Init;
	FOL_handle[1] = FOL_handle_Init;
	printf("滤波模块初始化完成。\n");
}

/*-------------------------------------------------------------------------
函数名称：float FOL_motor_out(float ResrcData, char SL)
函数功能：码盘滤波
函数输入：ResrcData:需要滤波的数据、SL：滤波容器编号(对应FOL_Motor[4])
函数输出：无
--------------------------------------------------------------------------*/
float APP_FOL_motor_out(float ResrcData, char SL)
{

	FOL_Motor[SL].Output = (1 - FOL_Motor[SL].A) * FOL_Motor[SL].Output + FOL_Motor[SL].A * ResrcData;
	return FOL_Motor[SL].Output;
}

float APP_FOL_handle(float ResrcData, char SL)
{

	FOL_handle[SL].Output = (1 - FOL_handle[SL].A) * FOL_handle[SL].Output + FOL_handle[SL].A * ResrcData;
	return FOL_handle[SL].Output;
}



