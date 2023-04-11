#ifndef __APP_BDW_agree_H__
#define __APP_BDW_agree_H__
#include "APP_control.h"
#include "driver_CAN.h"

/*

命令码+索引低+索引高+子索引+数据0-数据n

*/

/*读写报文*/
#define BDW_command_read 0x40				//读命令

#define BDW_answer_read_8bit 0x4F			//答复8位数据
#define BDW_answer_read_16bit 0x4B			//答复16位数据
#define BDW_answer_read_32bit 0x43			//答复32位数据

#define BDW_command_write_8bit 0x2F			//8位写命令
#define BDW_command_write_16bit 0x2B		//16位写命令
#define BDW_command_write_32bit 0x23		//32位写命令

#define BDW_answer_write 0x60				//答复写命令

/*读写错误报文*/
#define BDW_answer_error 0x80				//表示返回的是系统错误的数据

/*32位读写指令错误码*/

#define BDW_RW_error_0  0x00000305		//切换位未交替
#define BDW_RW_error_1  0x01000405		//客户机/服务器命令说明符无效或未知--不支持对对象的访问
#define BDW_RW_error_2  0x00000106		//不支持对对象的访问
#define BDW_RW_error_3  0x01000106		//尝试读取只写对象
#define BDW_RW_error_4  0x02000106		//尝试写入只读对象
#define BDW_RW_error_5  0x00000206		//对象在对象字典中不存在
#define BDW_RW_error_6  0x41000406		//对象不能映射到PDO。
#define BDW_RW_error_7  0x42000406		//要映射的对象的数量和长度将超过PDO长度
#define BDW_RW_error_8  0x47000406		//设备的一般内部不兼容性
#define BDW_RW_error_9  0x10000706		//数据类型不匹配，服务参数长度不匹配。
#define BDW_RW_error_10 0x12000706		//数据类型不匹配，服务参数长度太高，
#define BDW_RW_error_11 0x13000706		//数据类型不匹配，服务参数长度太低，
#define BDW_RW_error_12 0x11000906		//子索引不存在
#define BDW_RW_error_13 0x43000406		//一般参数不兼容
#define BDW_RW_error_14 0x00000606		//由于硬件错误，访问失败
#define BDW_RW_error_15 0x30000906		//超出参数的值范围
#define BDW_RW_error_16 0x31000906		//写入的参数的值太高
#define BDW_RW_error_17 0x32000906		//写入的参数的值太低
#define BDW_RW_error_18 0x36000906		//最大值小于最小值
#define BDW_RW_error_19 0x20000008		//数据无法传输或存储到应用程序
#define BDW_RW_error_20 0x21000008		//由于本地控制，无法将数据传输或存储到应用程序
#define BDW_RW_error_21 0x22000008		//由于当前设备状态，无法将数据传输或存储到应用程序
#define BDW_RW_error_22 0x23000008		//没有对象词典


/*心跳报文
说明：
COB_ID          Byte0
0x700+ID        状态：

*/
#define BDW_hearbeat_ID                0x700					//心跳报文帧头
#define BDW_hearbeat_ID1               0x701					//心跳报文帧头
#define BDW_hearbeat_ID2               0x702					//心跳报文帧头
#define BDW_hearbeat_bootup            0x00						//起始心跳
#define BDW_hearbeat_stopoed           0x04						//停止心跳
#define BDW_hearbeat_operational       0x05						//操作心跳
#define BDW_hearbeat_preoperational    0x7f						//滞后操作



/*紧急事件
每次检测出故障，都会产生一个紧急信号，并存放在1003字段内
紧急对象C0B_ID是  0x81 到 0xff

紧急消息结构：  错误代码   错误寄存器  内部错误码  错误码数据1   错误码数据2
			   16 bit（↓） 8bit（↓）  8bit（0）  16bit（0）    16bit（0）   括号内为该位一般值

*/

#define BDW_error_ID_parameter_fault          0x630F01             //丢失参数故障（内存加载失败）
#define BDW_error_ID_power                    0x630F01             //功率故障
#define BDW_error_ID_error                    0x100001             //出现故障
#define BDW_error_ID_overcurrent              0x221103             //过流
#define BDW_error_ID_Current_out_of_tolerance 0x221203             //电流超差
#define BDW_error_ID_overload                 0x222103             //过载
#define BDW_error_ID_ocp                      0x231003             //过流保护
#define BDW_error_ID_phase_failure            0x313001             //电机缺相
#define BDW_error_ID_undervoltage             0x322005             //欠压
#define BDW_error_ID_overpressure             0x321005             //过压
#define BDW_error_ID_overheated               0x421008             //过热
#define BDW_error_ID_encoder                  0x730501             //编码器异常
#define BDW_error_ID_Hall                     0x730801             //霍尔反馈异常
#define BDW_error_ID_speed                    0x731101             //速度超差
#define BDW_error_ID_position                 0x732001             //位置异常
#define BDW_error_ID_Communication_time       0x813101             //通讯超时
#define BDW_error_ID_position_follow          0x861101             //位置跟随异常
#define BDW_error_ID_acceleration_max         0xFF0080             //加速度过大警告
#define BDW_error_ID_acceleration_min         0xFF0180             //加速度过小警告
#define BDW_error_ID_IGBT                     0xFF0280             //IGBT故障
#define BDW_error_ID_motor_overheating        0xFF0380             //电机过热
#define BDW_error_ID_flying_car               0xFF0480             //飞车故障
#define BDW_error_ID_zero_drift               0xFF0880             //内部零偏故障
/*

TDO预留
暂用SDO

*/


/*控制字*/

#define BDW_control           0x6040                             //控制字
#define BDW_stats             0x6041                             //控制状态字
#define BDW_operationmode     0x6060                             //设置控制模式
#define BDW_displaymode       0x6061                             //查询控制模式

#define BDW_byte_reset       0x0080                              //控制字：复位
#define BDW_byte_enable_on   0x000F                              //控制字：使能开

//



/*电机模式选择*/
#define BDW_mode_nop          -1                                 //未设置模式
#define BDW_mode_position     0x01                               //位置控制
#define BDW_mode_velocity     0x03                               //速度控制模式
#define BDW_mode_torque       0x04                               //电流控制模式
#define BDW_mode_homing       0x06                               //回零模式
#define BDW_mode_cyclic       0x08                               //周期同步位置模式

/*
回零模式

回零模式控制字+归零模式开关+运动状态+有无错误
	4位           4位       4位      4位
*/

#define BDW_homing_2_off       0x00              //归零模式关
#define BDW_homing_2_on         0x08              //归零模式开

#define BDW_homing_3_halt1    0x00                 //未达到零点
#define BDW_homing_3_halt2    0x02                 //到达零点
#define BDW_homing_3_halt3    0x08                 //减速状态
#define BDW_homing_3_halt4    0x0A                 //速度为零

#define BDW_homing_4_error1   0x00                 //未完成、无错误
#define BDW_homing_4_error2   0x04                 //未完成、有错误
#define BDW_homing_4_error3   0x08                 //完成、无错误
#define BDW_homing_4_error4   0x0C                 //完成、有错误

/*回零模式相关参数*/
#define BDW_homeposition_L    0x607C               //左原点位置
#define BDW_homeposition_R    0x687C               //右原点位置
#define BDW_homingmethod      0x6098               //回零方法
#define BDW_shom_speed        0x6099               //回零速度

/*------------------------速度模式---------------------------*/
/*
通用状态字，表示动作的完成状态
适用于：速度控制、电流控制
*/
#define BDW_halt_target1 0x00                 //未达到目标速度
#define BDW_halt_target2 0x02                 //达到目标速度
#define BDW_halt_target3 0x08                 //减速中
#define BDW_halt_target4 0x0A                 //速度为零
/*---------------------*/

#define BDW_speed_halt_slippage1 0x00                //速度不为零，未达到最大滑移
#define BDW_speed_halt_slippage2 0x04                //速度不为零，达到最大滑移
#define BDW_speed_halt_slippage3 0x08                //速度为零，未达到最大滑移
#define BDW_speed_halt_slippage4 0x0C                //速度为零，达到最大滑移

/*速度模式相关参数*/

#define BDW_speed_actual_L 0x6069                    //左电机实际速度
#define BDW_speed_targe_L  0x60FF                     //左电机设定速度
#define BDW_speed_demand_L 0x606B                    //左电机当前给定速度

#define BDW_speed_actual_R 0x6869                    //右电机实际速度
#define BDW_speed_targe_R  0x68FF                    //右电机设定速度
#define BDW_speed_demand_R 0x686B                    //右电机当前给定速度

#define BDW_speed_max      0x6069                    //最高转速

/*------------------------电流（扭矩）模式---------------------------*/
/*电流（扭矩）模式相关参数*/
#define BDW_torque_actual_L 0x6077                    //左电机实际电流
#define BDW_torque_targe_L  0x6071                    //左电机设定电流

#define BDW_torque_actual_R 0x6877                    //左电机实际电流
#define BDW_torque_targe_R  0x6871                    //左电机设定电流

/*------------------------位置控制模式---------------------------*/
/*位置模式控制字*/
#define BDW_position_control1    0x00                       //关闭位置控制
#define BDW_position_control2    0x02                       //关闭位置控制
#define BDW_position_control3    0x04                       //关闭位置控制
#define BDW_position_control4    0x06                       //关闭位置控制
#define BDW_position_control5    0x08                       //开启位置控制、连续绝对值位置控制
#define BDW_position_control6    0x0A                       //开启位置控制、连续相对值位置控住
#define BDW_position_control7    0x0C                       //开启位置控制、中断当前运动，输入新绝对值
#define BDW_position_control8    0x0E                       //开启位置控制、中断当前运动，输入新相对值
/*位置模式状态字*/
#define BDW_position_state1      0x00                       //执行中、未到达目标位置、未设定目标位置、无错误
#define BDW_position_state2      0x04                       //执行中、未到达目标位置、未设定目标位置、有错误
#define BDW_position_state3      0x08                       //执行中、未到达目标位置、已设定目标位置、无错误
#define BDW_position_state4      0x0C                       //执行中、未到达目标位置、已设定目标位置、有错误
#define BDW_position_state5      0x20                       //执行中、已到达目标位置、未设定目标位置、无错误
#define BDW_position_state6      0x24                       //执行中、已到达目标位置、未设定目标位置、有错误
#define BDW_position_state7      0x28                       //执行中、已到达目标位置、已设定目标位置、无错误
#define BDW_position_state8      0x2C                       //执行中、已到达目标位置、已设定目标位置、有错误
#define BDW_position_state9      0x80                       //停止中、减速中、 未设定目标位置、无错误
#define BDW_position_state10     0x84                       //停止中、减速中、 未设定目标位置、有错误
#define BDW_position_state11     0x88                       //停止中、减速中、 已设定目标位置、无错误
#define BDW_position_state12     0x8C                       //停止中、减速中、 已设定目标位置、有错误
#define BDW_position_state13     0xA0                       //停止中、速度为0、未设定目标位置、无错误
#define BDW_position_state14     0xA4                       //停止中、速度为0、未设定目标位置、有错误
#define BDW_position_state15     0xA8                       //停止中、速度为0、已设定目标位置、无错误
#define BDW_position_state16     0xAC                       //停止中、速度为0、已设定目标位置、有错误

/*电流（扭矩）模式相关参数*/

#define BDW_position_actual_L        0x6064                 //左电机：实际位置
#define BDW_position_targe_L         0x607A                 //左电机：设定位置
#define BDW_position_speed_L         0x6081                 //左电机：转动速度
#define BDW_position_speed_end_L     0x6082                 //左电机：到达目标位置后的转动速度
#define BDW_position_acceleration_L  0x6083                 //左电机：转动加速度
#define BDW_position_deceleration_L  0x6084                 //左电机：转动减速度（暂不支持、预留）
#define BDW_position_type_L          0x6086                 //左电机：速度曲线（仅支持梯形速度曲线）

#define BDW_position_actual_R        0x6864                 //左电机：实际位置
#define BDW_position_targe_R         0x687A                 //左电机：设定位置
#define BDW_position_speed_R         0x6881                 //左电机：转动速度
#define BDW_position_speed_end_R     0x6882                 //左电机：到达目标位置后的转动速度
#define BDW_position_acceleration_R  0x6883                 //左电机：转动加速度（20rpm/s）
#define BDW_position_deceleration_R  0x6884                 //左电机：转动减速度（暂不支持、预留）
#define BDW_position_type_R          0x6886                 //左电机：速度曲线（仅支持梯形速度曲线，值：0）

/*-----------------对象字典     含     子目录部分---------------------------------------------------*/
#define BDW_digital_io_input_state                  0x200B01               //数字IO输入状态
#define BDW_digital_io_output_state                 0x200B02               //数字IO输出状态

#define BDW_actual_current_L                        0x200B05               //左电机实际电流、单位0.1A
#define BDW_actual_current_R                        0x200B2A               //右电机实际电流

#define BDW_actual_speed_L                          0x200B06               //左电机实际速度、rmp
#define BDW_actual_speed_R                          0x200B2B               //右电机实际速度

#define BDW_actual_position_L                       0x200B07               //左电机实际位置、脉冲个数
#define BDW_actual_position_R                       0x200B2C               //右电机实际位置

#define BDW_actual_error1_L                         0x200B03               //左电机实时故障1
#define BDW_actual_state1_L                         0x200B04               //左电机实时状态1

#define BDW_actual_error1_R                         0x200B28               //右电机实时故障1
#define BDW_actual_state1_R                         0x200B29               //右电机实时状态1

#define BDW_actual_error2_L                         0x200B1C               //左电机实时故障2
#define BDW_actual_state2_L                         0x200B1D               //左电机实时状态2

#define BDW_actual_error2_R                         0x200B2D               //右电机实时故障2
#define BDW_actual_state2_R                         0x200B2E               //右电机实时状态2

#define BDW_actual_temp_heat_sink                   0x200B0C               //驱动器散热片温度
#define BDW_actual_hall                             0x200B1B               //霍尔状态
#define BDW_actual_tempe_mcu                        0x200B0E               //mcu温度

#define BDW_actual_speed_set_L                      0x200B25               //左电机给定速度
#define BDW_actual_speed_set_R                      0x200B30               //右电机给定速度

#define BDW_actual_current_set_L                    0x200B24               //左电机给定电流
#define BDW_actual_current_set_R                    0x200B2F               //右电机给定电流

#define BDW_analog_input0_state                     0x200B11               //模拟输入0电压
#define BDW_analog_input1_state                     0x200B12               //模拟输入1电压

#define BDW_actual_current_total                    0x200B31               //母线电流
#define BDW_actual_power                            0x200B32               //输入功率

#define BDW_actual_power_L                          0x200B33               //左电机功率
#define BDW_actual_power_R                          0x200B34               //右电机功率

#define BDW_CiA402_control_word                     0x604000               //控制字     、rw  、u16 
#define BDW_CiA402_status_word_L                    0x604100               //状态字     、r   、u16
#define BDW_CiA402_status_word_R                    0x684100               //状态字     、r   、u16
#define BDW_CiA402_operationmode                    0x606000               //控制模式   、rw  、8
#define BDW_CiA402_displaymode                      0x606100               //显示模式   、r   、8
#define BDW_CiA402_position_actual_value_L          0x606400               //位置实际值 、r   、32
#define BDW_CiA402_position_actual_value_R          0x686400               //位置实际值 、r   、32
#define BDW_CiA402_position_target_value_L          0x607A00               //位置设定值 、rw  、32
#define BDW_CiA402_position_target_value_R          0x687A00               //位置设定值 、rw  、32
#define BDW_CiA402_position_demand_value_L          0x606200               //位置设定值 、r   、32
#define BDW_CiA402_position_demand_value_R          0x686200               //位置设定值 、r   、32
#define BDW_CiA402_profile_velocity_L               0x608100               //轨迹速度   、rw  、u32
#define BDW_CiA402_profile_velocity_R               0x688100               //轨迹速度   、rw  、u32
#define BDW_CiA402_end_velocity_L                   0x608200               //末端速度   、rw  、u32
#define BDW_CiA402_end_velocity_R                   0x688200               //末端速度   、rw  、u32
#define BDW_CiA402_reachposition                    0x606700               //到达位置   、rw  、u32
#define BDW_CiA402_profile_acceleration_L           0x608300               //轨迹加速度 、rw  、u32
#define BDW_CiA402_profile_acceleration_R           0x688300               //轨迹加速度 、rw  、u32
#define BDW_CiA402_profile_deceleration_L           0x608400               //轨迹减速度 、rw  、u32
#define BDW_CiA402_profile_deceleration_R           0x688400               //轨迹减速度 、rw  、u32
#define BDW_CiA402_quick_stop_deceleration_L        0X608500               //快速制动   、rw  、u32
#define BDW_CiA402_quick_stop_deceleration_R        0X688500               //快速制动   、rw  、u32
#define BDW_CiA402_motion_profile_type              0x608600               //运动轨迹   、rw  、16
#define BDW_CiA402_target_velocity_L                0x60FF00               //目标速度   、rw  、32
#define BDW_CiA402_target_velocity_R                0x68FF00               //目标速度   、rw  、32
#define BDW_CiA402_velocity_sensor_actualValue_L    0X606900               //传感器速度 、r   、32
#define BDW_CiA402_velocity_sensor_actualValue_R    0X686900               //传感器速度 、r   、32
#define BDW_CiA402_sensor_select                    0x606A00               //传感器选择 、rw  、16
#define BDW_CiA402_velocity_actul_value_L           0x606C00               //实际速度   、r   、32
#define BDW_CiA402_velocity_actul_value_R           0x686C00               //实际速度   、r   、32
#define BDW_CiA402_velocity_window                  0x606D00               //速度窗口   、rw  、u16
#define BDW_CiA402_velocity_window_time             0x606E00               //窗口时间   、rw  、u16
#define BDW_CiA402_velocity_threshold               0x606F00               //速度上限   、rw  、u16
#define BDW_CiA402_velocity_threshold_time          0x607000               //速度上限时间 rw  、u16
#define BDW_CiA402_target_torque_L                  0x607100               //目标扭矩   、rw  、16
#define BDW_CiA402_target_torque_R                  0x687100               //目标扭矩   、rw  、16
#define BDW_CiA402_max_currentt_L                   0x607300               //最大电流   、r   、16
#define BDW_CiA402_max_currentt_R                   0x687300               //最大电流   、r   、16
#define BDW_CiA402_torque_actual_value_L            0x607700               //实际扭矩   、r   、16
#define BDW_CiA402_torque_actual_value_R            0x687700               //实际扭矩   、r   、16
#define BDW_CiA402_currentt_actual_value_L          0x607800               //实际电流   、r   、16
#define BDW_CiA402_currentt_actual_value_R          0x687800               //实际电流   、r   、16
#define BDW_CiA402_home_offset_L                    0x607C00               //原点偏移   、rw  、32
#define BDW_CiA402_home_offset_R                    0x687C00               //原点偏移   、rw  、32
#define BDW_CiA402_homing_method                    0x609800               //回零方法   、rw  、8
#define BDW_CiA402_high_homing_speed                0x609901               //高回零速度 、rw  、32
#define BDW_CiA402_low_homing_speed                 0x609902               //低回零速度 、rw  、32
#define BDW_CiA402_error_code_L                     0x603F00               //错误码     、r   、u8
#define BDW_CiA402_error_code_R                     0x683F00               //错误码     、r   、u8
#define BDW_CiA402_supported_drive_mode             0x650200               //支持模式   、r   、u16
#define BDW_CiA402_quick_stop_option_code           0x605A00               //快速停止选择 rw  、16
#define BDW_CiA402_Shutdown_option_code             0x605B00               //关闭选项   、rw  、16
#define BDW_CiA402_disable_operation_option_code    0x605C00               //禁止操作   、rw  、16
#define BDW_CiA402_halt_option_code                 0x605D00               //暂停选项  、 rw  、16
#define BDW_CiA402_fault_reaction_option_code       0x605E00               //故障反应选择 rw  、16


typedef struct BDW_pool					//数据池
{
	uint16_t Digital_io_input_state;        //数字IO输入状态
	uint16_t Digital_io_output_state;       //数字IO输出状态

	int16_t current_L;                      //左电机实际电流、单位0.1A
	int16_t current_R;                      //右电机实际电流

	int32_t speed_L;                        //左电机实际速度、rmp
	int32_t speed_R;                        //右电机实际速度

	int32_t position_L;                     //左电机实际位置、脉冲个数
	int32_t position_R;                     //右电机实际位置

	uint16_t error1_L;                      //左电机实时故障1
	uint16_t state1_L;                      //左电机实时状态1

	uint16_t error1_R;                      //右电机实时故障1
	uint16_t state1_R;                      //右电机实时状态1

	uint16_t error2_L;                      //左电机实时故障2
	uint16_t state2_L;                      //左电机实时状态2

	uint16_t error2_R;                      //右电机实时故障2
	uint16_t state2_R;                      //右电机实时状态2

	int16_t temp_heat_sink;                 //驱动器散热片温度
	uint16_t hall;                          //霍尔状态
	int16_t tempe_mcu;                      //mcu温度

	int32_t speed_set_L;                    //左电机给定速度
	int32_t speed_set_R;                    //右电机给定速度

	int16_t current_set_L;                  //左电机给定电流
	int16_t current_set_R;                  //右电机给定电流

	int16_t Analog_input0_state;            //模拟输入0电压
	int16_t Analog_input1_state;            //模拟输入1电压

	int16_t current_total;                  //母线电流
	int16_t power;                          //输入功率

	int16_t power_L;                        //左电机功率
	int16_t power_R;                        //右电机功率

	uint8_t heartbeat;                      //心跳状态，1为接收到心跳报文

}BDW_pool;



#define BDW_cobid_1 0x601                         //电机驱动1 ID
#define BDW_cobid_2 0x602                         //电机驱动2 ID
#define BDW_RX_cobid_1 0x581                      //电机驱动1 ID
#define BDW_RX_cobid_2 0x582                      //电机驱动2 ID


void rx_8bit(uint8_t* DatA, uint8_t id);                            //接收8bit数据
void rx_16bit(uint8_t* DatA, uint8_t id);                           //接收16bit数据
void rx_32bit(uint8_t* DatA, uint8_t id);                           //接收32bit数据
void BDW_slide_pool(uint16_t Cob_iD, uint8_t CommanD, uint32_t IndeX, uint32_t DatA);//发送报文

//void BDW_control_init(uint8_t MoD);

void BDW_read_write_error(uint8_t* DatA, uint8_t id);	//读写错误报文


uint8_t BDW_speed_control_init(uint16_t time, uint8_t cycle);			//设置电机驱动为速度模式

void BDW_motor_speed_out(int32_t v1, int32_t v2, int32_t v3, int32_t v4);
void BDW_motor_current_out(int32_t I1, int32_t I2, int32_t I3, int32_t I4);
void BDW_cycle_request_data(uint16_t time);

void BDW_can_data_recive_storage(CAN_Data* DatA);

void motor_heart_echo(uint8_t id);				//心跳报文响应
















#endif