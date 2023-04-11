#include "APP_chassis.h"
#include "APP_BDW_agree.h"
#include "APP_SXLD_agree.h"
#include "APP_ECU_agree.h"
#include "APP_FOL.h"

chassis css_drr = { 0 };				//底盘控制结构体

extern ControL control;


//-------------------------------------------------------------------------------------
//函数功能：码盘信号转轮上线速度
//函数输入：码盘转速 RPM
//函数输出：轮上线速度 M/S
//-------------------------------------------------------------------------------------
float Speed_conversion_MS(int rpm)
{
	float v = 0.0, rpm_f = 0.f;
	rpm_f = rpm;											//保证下一步精度		
	v = (rpm_f / 60.0) * Reduction_ratio * Wheel_D;			//计算轮上线速度; 霍尔速度转成转每秒 *  减速比  *  轮胎周长
	return v;
}

//-------------------------------------------------------------------------------------
//函数功能：底盘电机速度给定_开环
//函数输入：x方向速度，w自转角速度,顺时针转动为正，轮子转速为M/S
//函数输出：无
//-------------------------------------------------------------------------------------
void Chassis_Given_Speed(float Vx, float Vw, uint16_t cyc, uint16_t TimE)
{
	if ((TimE % cyc) == 0)
	{
		control.set.v1 = Vx + Vw;
		control.set.v2 = -Vx + Vw;
		control.set.v3 = -Vx + Vw;
		control.set.v4 = Vx + Vw;

		if (control.set.v1 > 4000)control.set.v1 = 4000;
		if (control.set.v1 < -4000)control.set.v1 = -4000;
		if (control.set.v2 > 4000)control.set.v2 = 4000;
		if (control.set.v2 < -4000)control.set.v2 = -4000;
		if (control.set.v3 > 4000)control.set.v3 = 4000;
		if (control.set.v3 < -4000)control.set.v3 = -4000;
		if (control.set.v4 > 4000)control.set.v4 = 4000;
		if (control.set.v4 < -4000)control.set.v4 = -4000;

		motor_speed_out(control.set.v1, control.set.v2, control.set.v3, control.set.v4);
	}
}

//X-W式带滤波版本
void Chassis_Given_Speed_FOL(float Vx, float Vw, uint16_t cyc, uint16_t TimE)
{
	if ((TimE % cyc) == 0)
	{
		float x = 0, w = 0;
		Vx = APP_FOL_handle(Vx, 0);
		Vw = APP_FOL_handle(Vw, 1);
		control.set.v1 = Vx + Vw;
		control.set.v2 = -Vx + Vw;
		control.set.v3 = -Vx + Vw;
		control.set.v4 = Vx + Vw;
		motor_speed_out(control.set.v1, control.set.v2, control.set.v3, control.set.v4);
	}
}

//X-W式带滤波、带失速修正版
void Chassis_Given_Speed_FOL_revise(float Vx, float Vw, uint16_t cyc, uint16_t TimE)
{
	if ((TimE % cyc) == 0)
	{

		float x = 0, w = 0;
		int16_t rmp1 = 0, rmp2 = 0;
		float aefa = 0;
		//限幅
		if (Vx > Ten_thousandth_ratio_MAX)Vx = Ten_thousandth_ratio_MAX;
		if (Vx < Ten_thousandth_ratio_MIN)Vx = Ten_thousandth_ratio_MIN;
		if (Vw > Ten_thousandth_ratio_MAX)Vw = Ten_thousandth_ratio_MAX;
		if (Vw < Ten_thousandth_ratio_MIN)Vw = Ten_thousandth_ratio_MIN;

		x = APP_FOL_handle(Vx, 0) * 0.0706 * linear_speed_max_ms;
		w = APP_FOL_handle(Vw, 1) * 0.0030 * linear_angular_max_as;

		rmp1 = x + w;
		rmp2 = -x + w;

		////限速
		//if (rmp1 > tyre_rmp_max)
		//{
		//	rmp1 = tyre_rmp_max;
		//}
		//if (rmp1 < tyre_rmp_min)
		//{
		//	rmp1 = tyre_rmp_min;
		//}
		//if (rmp2 > tyre_rmp_max)
		//{
		//	rmp2 = tyre_rmp_max;
		//}
		//if (rmp2 < tyre_rmp_min)
		//{
		//	rmp2 = tyre_rmp_min;
		//}

		//优先照顾转向性能
		if (rmp1 > tyre_rmp_max)
		{
			aefa = rmp1 / tyre_rmp_max;
			rmp1 = tyre_rmp_max;
			aefa = rmp2 / aefa;
			rmp2 = aefa;
		}
		if (rmp1 < tyre_rmp_min)
		{
			aefa = rmp1 / tyre_rmp_min;
			rmp1 = tyre_rmp_min;
			aefa = rmp2 / aefa;
			rmp2 = aefa;
		}
		if (rmp2 > tyre_rmp_max)
		{
			aefa = rmp2 / tyre_rmp_max;
			rmp2 = tyre_rmp_max;
			aefa = rmp1 / aefa;
			rmp1 = aefa;
		}
		if (rmp2 < tyre_rmp_min)
		{
			aefa = rmp2 / tyre_rmp_min;
			rmp2 = tyre_rmp_min;
			aefa = rmp1 / aefa;
			rmp1 = aefa;
		}

		//printf("Vx：%f，Vw：%f\n", x, w);
		//printf("VR：%d，VL：%d\n",rmp1,rmp2);

		control.set.v1 = rmp1;
		control.set.v2 = rmp2;
		control.set.v3 = rmp2;
		control.set.v4 = rmp1;

		motor_speed_out(control.set.v1, control.set.v2, control.set.v3, control.set.v4);
	}
}

float rmp_a[25] = { 0, };
float rmp_b[25] = { 0, };

//X-W式带失速修正版-2
void Chassis_Given_Speed_revise_table(float Vx, float Vw, uint16_t cyc, uint16_t TimE)
{
	if ((TimE % cyc) == 0)
	{

		int16_t rmp1 = 0, rmp2 = 0;
		float x = 0, w = 0;
		int vxx = 0, vww = 0;
		x = Vx * 0.0001f * linear_speed_max_ms;
		w = Vw * 0.0001f * linear_angular_max_as;

		if ((Vx < 100) && (Vx > -100))
		{
			x = 0;
		}
		else
		{
			if (Vx > 0)
			{
				x = 250 + 1700 * x;
			}
			if (Vx < 0)
			{
				x = -250 + 1700 * x;
			}
		}

		//if ((Vw < 400) && (Vw > -400))
		//{
		//	if (Vw > 0)
		//	{
		//		//700=16000a+400b;
		//		//0=640000a+800b;
		//		//2800=640000a+1600b
		//		// 2800=800b
		//		//b=3.5;
		//		//a=-0.004375;
		//		w = -0.004375f * w * w + 3.5f * w;
		//	}
		//	if (Vw < 0)
		//	{
		//		//-300=40000a-200b;
		//		//0=160000a-400b;
		//		//-1200=160000a-800b;

		//		w = 0.004375f * w * w + 3.5 * w;
		//	}


		//}
		//else
		//{
		//	if (Vw > 0)
		//	{
		//		w = 300 + w * 67;
		//	}
		//	if (Vw < 0)
		//	{
		//		w = -300 + w * 67;
		//	}
		//}

		//立得仓库内72
		//立得粗糙路面

		w = w * 72;


		rmp1 = x + w;
		rmp2 = -x + w;

		control.set.v1 = rmp1;
		control.set.v2 = rmp2;
		control.set.v3 = rmp2;
		control.set.v4 = rmp1;

		motor_speed_out(control.set.v1, control.set.v2, control.set.v3, control.set.v4);
	}
}



//-------------------------------------------------------------------------------------
//函数功能：x_w控制——开环
//函数输入：x线速度，w转向角速度,顺时针转动为正，轮子转速为M/S
//函数输出：无
//-------------------------------------------------------------------------------------
void Chassis_Given_velocity_angular(float Vx, float Vw)
{
	float vl = 0, vr = 0, vd = 0;
	Vx = Vx * 705.88f;
	Vw = (Vw * 1.2f * 3.1416f) * 3.93f;
	vl = Vx + Vw;
	vr = -Vx + Vw;
	if (vl >= 4000)
	{
		vr = vr * (4000 / vl);
		vl = 4000;
	}
	if (vl <= -4000)
	{
		vr = vr * (-4000 / vl);
		vl = -4000;
	}
	if (vr >= 4000)
	{
		vl = vl * (4000 / vr);
		vr = 4000;
	}
	if (vr <= -4000)
	{
		vl = vl * (-4000 / vr);
		vr = -4000;
	}
	css_drr.v1 = vl;
	css_drr.v2 = vr;
	css_drr.v3 = vr;
	css_drr.v4 = vl;
}

//-------------------------------------------------------------------------------------
//函数功能：速度、曲率控制（未修复BUG）
//函数输入：
//函数输出：无
//-------------------------------------------------------------------------------------
void Chassis_Given_Curvature(float Vx, float Vc, uint16_t cyc, uint16_t TimE)
{
	if ((TimE % cyc) == 0)
	{
		float vl = 0, vr = 0, r = 0;
		if (((Vc == 1) || (Vc == -1)) && (Vx == 0))
		{
			vl = -Vc;
			vr = Vc;
		}
		r = 1.f / Vc;
		vl = Vx * ((r + 0.6f) / (2 * r));
		vr = Vx * ((r - 0.6f) / (2 * r));
	}

}


//设定电机速度
void motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4)
{
	int ca = v1 + v2;

	int ca1 = v1 - v2;

	if (ca1 >= 0)
	{
		if (ca >= 300)
		{
			ECU_light(power_R_lamp_flash, 500);			//右灯闪烁，500MS
			ECU_light(power_L_lamp_on, 0);			//右灯闪烁，500MS
		}
		if (ca <= -300)
		{
			ECU_light(power_L_lamp_flash, 500);			//右灯闪烁，500MS
			ECU_light(power_R_lamp_on, 0);			//右灯闪烁，500MS
		}

		if ((ca < 300) && (ca > -300))
		{
			ECU_light(power_R_lamp_on, 0);			//右灯闪烁，500MS
			ECU_light(power_L_lamp_on, 0);			//右灯闪烁，500MS
		}
	}
	if (ca1 < 0)
	{
		if (ca >= 300)
		{
			ECU_light(power_L_lamp_flash, 500);			//右灯闪烁，500MS
			ECU_light(power_R_lamp_on, 0);			//右灯闪烁，500MS
		}
		if (ca <= -300)
		{
			ECU_light(power_R_lamp_flash, 500);			//右灯闪烁，500MS
			ECU_light(power_L_lamp_on, 0);			//右灯闪烁，500MS
		}

		if ((ca < 300) && (ca > -300))
		{
			ECU_light(power_R_lamp_on, 0);			//右灯闪烁，500MS
			ECU_light(power_L_lamp_on, 0);			//右灯闪烁，500MS
		}
	}



	switch (control.sys.ESC)
	{
	case (ESC_SXLD):SXLD_motor_speed_out(v1, v2, v3, v4); break;
	case (ESC_BDW):BDW_motor_speed_out(v1, v2, v3, v4); break;
	}
}


//请求数据
void request_data(uint16_t TimE)
{
	switch (control.sys.ESC)
	{
	case (ESC_SXLD):SXLD_cycle_request_data(TimE); break;
	case (ESC_BDW):BDW_cycle_request_data(TimE); break;
	}
}




//电机初始化
uint8_t chassis_motor_init(uint16_t TimE)
{
	uint8_t mark = 0;													//初始化标志位，0：未完成，1：完成
	switch (control.sys.ESC)
	{
	case (ESC_SXLD):mark = SXLD_mot_enable(TimE, 1); break;				//山西零度驱动器不需要初始化
	case (ESC_BDW):mark = BDW_speed_control_init(TimE, 200); break;		//八达威电驱初始化，0：未完成，1：完成
	}

	return mark;
}