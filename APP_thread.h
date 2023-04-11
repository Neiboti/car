#ifndef __APP_APP_thread_H__
#define __APP_APP_thread_H__

#include "APP_control.h"


void* thread_CAN_recive(void* ptr);//CAN接收线程

void* thread_CAN_send(void* ptr);//CAN发送线程

void* thread_CAN_BUFF_read(void* ptr);//CAN报文解析读取线程

void* thread_UDP_recive(void* ptr);//UDP发送线程

void* thread_UDP_send(void* ptr);//UDP发送线程

void* thread_UDP_BUFF_read(void* ptr);//UDP报文解析读取线程

void* thread_handle_service(void* ptr);//手柄线程



void APP_init(void);
int thread_creat(void);






#endif
