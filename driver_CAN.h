#ifndef __DRIVER_CAN_H__
#define __DRIVER_CAN_H__
#include "APP_control.h"
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

//UDP驱动结构体
typedef struct DRR_CAN
{
	int8_t init_ok;							//初始化标志位：0未初始化、1初始化成功、-1初始化失败
	int32_t SocketCan0;						//帧头
	uint64_t cumulative_reception;			//累计接收量
	uint64_t cumulative_transmission;		//累计发送量
}DRR_CAN;


#define can_int 1
#define can_out 0


//滑动发射器相关
#define CAN_slide_pool_max		64						//滑动队列大小
#define CAN_pool_buff_max		32						//最大储存大小

//滑动接收器相关
#define CAN_rx_buff_max			64						//滑动队列大小

typedef struct CAN_Data//can数据结构体定义
{
	//uint8_t data[CAN_pool_buff_max];//数据

	//uint32_t id;
	can_frame frame;			//can帧数据
	uint8_t EXID;				//扩展帧标志位
	uint8_t mark;				//标识位1.0
	uint8_t len;				//帧长度


}CAN_Data;



uint8_t driver_can_init(void);														//初始化
void CAN_pool_input(uint32_t ID, uint8_t ExiD, uint8_t len, uint8_t* DatAbf);//滑动输入器
void CAN_recive(void);														//CAN数据接收线程
void CAN_send(void);															//CAN数据发送线程
//void CAN_BUFF_read(void);														//CAN数据解析线程滑动读取器
//CAN状态打印
void CAN_state_printf();

#endif