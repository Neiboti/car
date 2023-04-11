#ifndef __DRIVER_UDP_H__
#define __DRIVER_UDP_H__
#include "APP_control.h"

#define Client_PORT 8011
#define SERVER_PORT 8011

#define Client_IP "192.168.144.76"
#define zizhu_IP "192.168.144.7"

#define udp_int 1
#define udp_out 0



//UDP异常查询码
#define UDP_abnormal_init 1
#define UDP_abnormal_RX 2
#define UDP_abnormal_TX 3


//UDP驱动结构体
typedef struct DRR_UDP
{
	int8_t init_ok;				//初始化标志位：0未初始化、1初始化成功、-1初始化失败
	uint8_t vada;				//累加和 效验开关
	uint16_t vada_sss;			//累加和

	uint64_t cumulative_reception;			//累计接收量
	uint64_t cumulative_transmission;		//累计发送量

	sockaddr_in	Init;						//UDP初始化参数
	int32_t server_fd;						//本地IP相关


	sockaddr_in	receive;					//接收到数据的地址信息
	socklen_t addr_rece;					//长度

	sockaddr_in	sending;					//待发送数据的地址信息
	socklen_t addr_send;					//长度


	socklen_t addr_len;

	//对外异常状态数据参数
	uint8_t abnormal_init;					//初始化状态，0未初始化、1成功、2失败
	uint8_t abnormal_RX;					//接收状态，0未初始化、1成功、2失败
	uint8_t abnormal_TX;					//发送状态，0未初始化、1成功、2失败

}DRR_UDP;



//滑动发射器相关
#define UDP_slide_pool_max		128						//滑动队列大小
#define UDP_pool_buff_max		64						//最大储存大小

//滑动接收器相关
#define UDP_rx_buff_max			128						//滑动队列大小

typedef struct UDP_Data//UDP数据结构体定义
{
	uint64_t IP;								//IP地址
	uint16_t port;								//端口号
	uint8_t len;//帧长度
	uint8_t data[UDP_pool_buff_max];//数据
	uint8_t mark;//标识位1.0
}UDP_Data;


uint64_t IP_UDP_TX_init(const char* IP);		//发送默认IP初始化

uint64_t PORT_UDP_TX_init(uint16_t PORT);			//发送默认端口初始化


void driver_UDP_init(void);
int32_t udp_socket_init(void);
void udp_recive(void);
void udp_send(void);
void UDP_pool_input(uint64_t ip, uint16_t PorT, uint8_t len, uint8_t* DatAbf);
void UDP_BUFF_read(void);
//UDP状态打印
void UDP_state_printf();

//UDP异常查询
uint8_t UDP_abnormal_query(uint8_t code);


#endif
