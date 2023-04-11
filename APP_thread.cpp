#include "APP_thread.h"
#include <stdio.h>
#include <pthread.h>

#include "driver_CAN.h"
#include "APP_SXLD_agree.h"
#include "APP_BDW_agree.h"
#include "driver_UDP.h"
#include "APP_UDP_agree.h"
#include "driver_handle.h"
#include "APP_ECU_agree.h"
#include "APP_abnormal.h"
#include "APP_FOL.h"
#include "APP_LMAC.h"


//CAN接收缓冲区
extern CAN_Data CAN_rx_buff[CAN_rx_buff_max];					//接收缓冲区
extern uint8_t CAN_buff_int;									//滑入坐标
extern uint8_t CAN_buff_out;									//滑出坐标

//UDP接收缓冲区
extern UDP_Data UDP_rx_buff[UDP_rx_buff_max];					//接收缓冲区
extern uint8_t UDP_buff_int;									//滑入坐标
extern uint8_t UDP_buff_out;									//滑出坐标
//中控结构体
extern ControL control;



//CAN接收线程
void* thread_CAN_recive(void* ptr)
{
	printf("CAN接收线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		CAN_recive();
	}
	pthread_exit(0);
}
//CAN发送线程
void* thread_CAN_send(void* ptr)
{
	printf("CAN发送线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		CAN_send();
	}
	pthread_exit(0);
}
//CAN报文解析读取线程
void* thread_CAN_BUFF_read(void* ptr)
{
	printf("CAN报文解析线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		if (CAN_rx_buff[CAN_buff_out].mark == 1)
		{
			//--------------------------------------------CAN数据读取处理区------------------------------------------------------

			//ECU报文读取
			ECU_CAN_read(&CAN_rx_buff[CAN_buff_out]);

			//驱动器报文读取
			switch (control.sys.ESC)
			{
			case (0):
			{
				SXLD_can_data_recive_storage(&CAN_rx_buff[CAN_buff_out]);
				BDW_can_data_recive_storage(&CAN_rx_buff[CAN_buff_out]);
			}
			break;
			case (ESC_SXLD):SXLD_can_data_recive_storage(&CAN_rx_buff[CAN_buff_out]); break;
			case (ESC_BDW):	BDW_can_data_recive_storage(&CAN_rx_buff[CAN_buff_out]);  break;
			}
			CAN_rx_buff[CAN_buff_out].mark = 0;					//标志读取完成
			CAN_buff_out = (CAN_buff_out + 1) % CAN_rx_buff_max;
		}

	}
	pthread_exit(0);
}
//UDP接收线程
void* thread_UDP_recive(void* ptr)
{
	printf("UDP接收线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		udp_recive();
	}
	pthread_exit(0);
}
//UDP发送线程
void* thread_UDP_send(void* ptr)
{
	printf("UDP发送线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		udp_send();
	}
	pthread_exit(0);
}
//UDP报文解析读取线程
void* thread_UDP_BUFF_read(void* ptr)
{
	printf("UDP报文解析线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		if (UDP_rx_buff[UDP_buff_out].mark == 1)
		{
			//--------------------------------------------UDP数据读取处理区------------------------------------------------------
			UDP_agree_read(UDP_rx_buff[UDP_buff_out].data, UDP_rx_buff[UDP_buff_out].IP, UDP_rx_buff[UDP_buff_out].port);

			//--------------------------------------------UDP数据读取处理区------------------------------------------------------
			UDP_rx_buff[UDP_buff_out].mark = 0;					//标志读取完成
			UDP_buff_out = (UDP_buff_out + 1) % UDP_rx_buff_max;
		}
	}
	pthread_exit(0);
}

//手柄线程
void* thread_handle_service(void* ptr)
{
	printf("手柄线程--->启动成功\n");
	pthread_detach(pthread_self());
	while (1)
	{
		handle_service();
	}
	pthread_exit(0);
}

void* thread_ECU_APP(void* ptr)
{
	printf("ECU线程--->启动成功\n\n\n");
	pthread_detach(pthread_self());
	while (1)
	{
		APP_ECU();
	}
	pthread_exit(0);
}

void* thread_PING_PING(void* ptr)
{
	printf("PING线程--->启动成功\n\n\n");
	pthread_detach(pthread_self());
	while (1)
	{
		int ping = 0;
		//ping = system("ping 192.168.144.76");
		//printf("\n%d\n",ping);
		sleep(1);
	}
	pthread_exit(0);
}




//启动线程
int thread_creat(void)
{
	pthread_t CANRC, CANSE, CANRE, UDPRC, UDPSE, UDPRE, HANDLE, ECUM, PING;
	//CAN接收线程
	int ret1 = pthread_create(&CANRC, NULL, thread_CAN_recive, NULL);
	if (ret1) {
		printf("Create thread_CAN_recive error!\n");
		return 1;
	}
	//CAN发送线程
	int ret2 = pthread_create(&CANSE, NULL, thread_CAN_send, NULL);
	if (ret2) {
		printf("Create thread_CAN_send error!\n");
		return 1;
	}
	//CAN报文解析读取线程
	int ret3 = pthread_create(&CANRE, NULL, thread_CAN_BUFF_read, NULL);
	if (ret3) {
		printf("Create thread_CAN_BUFF_read error!\n");
		return 1;
	}
	//UDP接收线程
	int ret4 = pthread_create(&UDPRC, NULL, thread_UDP_recive, NULL);
	if (ret4) {
		printf("Create thread_UDP_recive error!\n");
		return 1;
	}
	//UDP发送线程
	int ret5 = pthread_create(&UDPSE, NULL, thread_UDP_send, NULL);
	if (ret5) {
		printf("Create thread_UDP_send error!\n");
		return 1;
	}
	//UDP报文解析读取线程
	int ret6 = pthread_create(&UDPRE, NULL, thread_UDP_BUFF_read, NULL);
	if (ret6) {
		printf("Create thread_UDP_BUFF_read error!\n");
		return 1;
	}
	//手柄线程
	int ret7 = pthread_create(&HANDLE, NULL, thread_handle_service, NULL);
	if (ret7) {
		printf("Create thread_handle_service error!\n");
		return 1;
	}
	//ECU线程
	int ret8 = pthread_create(&ECUM, NULL, thread_PING_PING, NULL);
	if (ret8) {
		printf("Create thread_ECU_APP error!\n");
		return 1;
	}

	//线程
	int ret9 = pthread_create(&PING, NULL, thread_ECU_APP, NULL);
	if (ret9) {
		printf("Create thread_ECU_APP error!\n");
		return 1;
	}
}

//初始化总函数
void APP_init(void)
{
	printf("上电成功，正在进行初始化\n");
	APP_CONTROL_init();												//
	driver_can_init();
	driver_UDP_init();
	ECU_init();
	APP_abnormal_init();
	APP_FOL_Init();
	APP_HMAC_TX_init();												//UDP上报

}