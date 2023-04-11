#include "driver_UDP.h"

DRR_UDP UDP_DRR = { 0 };									//UDP参数

//滑动发射器
UDP_Data UDP_slide_data[UDP_slide_pool_max] = { 0 };		//发送缓冲区
uint8_t UDP_slide_int = 0;									//滑入坐标
uint8_t UDP_slide_out = 0;									//滑出坐标

//接收缓冲区
UDP_Data UDP_rx_buff[UDP_rx_buff_max] = { 0 };				//接收缓冲区
uint8_t UDP_buff_int = 0;									//滑入坐标
uint8_t UDP_buff_out = 0;									//滑出坐标


//UDP初始化参数设定
void driver_UDP_init(void)
{
	//初始化本地UDP相关参数
	UDP_DRR.Init.sin_family = AF_INET;
	UDP_DRR.Init.sin_addr.s_addr = htonl(INADDR_ANY);	//IP地址，需要进行网络序转换，INADDR_ANY：本地地址
	UDP_DRR.Init.sin_port = htons(SERVER_PORT);			//端口号，需要网络序转换
	//初始化UDP发送相关参数
	udp_socket_init();									//初始化地址、端口	


	if (UDP_DRR.abnormal_init == 1)
	{
		printf("UDP初始化成功\n");
	}
	else
	{
		printf("UDP初始化失败\n");
	}
}

//发送默认IP初始化
uint64_t IP_UDP_TX_init(const char* IP)
{
	uint64_t ip = 0;

	inet_pton(AF_INET, IP, (void*)&ip);

	return ip;
}

//发送默认端口初始化
uint64_t PORT_UDP_TX_init(uint16_t PORT)
{
	uint64_t port = 0;

	port = htons(PORT);

	return port;
}



//UDP初始化
int32_t udp_socket_init(void)
{
	int ret;
	UDP_DRR.server_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
	if (UDP_DRR.server_fd < 0)
	{
		//printf("create socket fail!\n");
		UDP_DRR.abnormal_init = 2;
		return -1;
	}
	else
	{
		UDP_DRR.abnormal_init = 1;
	}

	ret = bind(UDP_DRR.server_fd, (struct sockaddr*)&UDP_DRR.Init, sizeof(UDP_DRR.Init));
	if (ret < 0)
	{
		//printf("socket bind fail!\n");
		//printf("UDP Socket Init Failed!\n");
		UDP_DRR.abnormal_init = 2;
		return -1;
	}
	else
	{
		UDP_DRR.abnormal_init = 1;
	}
	return UDP_DRR.server_fd;
}

//发送数据时调用此函数
//滑动输入器
void UDP_pool_input(uint64_t ip, uint16_t PorT, uint8_t len, uint8_t* DatAbf)
{
	UDP_slide_data[UDP_slide_int].mark = 0;							//更改前将标志位置0，表示不可读
	UDP_slide_data[UDP_slide_int].IP = ip;							//装填IP
	UDP_slide_data[UDP_slide_int].port = PORT_UDP_TX_init(8011);	//装填端口号
	UDP_slide_data[UDP_slide_int].len = len;

	DatAbf[4] = len;											//装填长度

	DatAbf[5] = (UDP_DRR.vada_sss)|0xFF;						//装填累加和

	UDP_DRR.vada_sss = (UDP_DRR.vada_sss + 1)%0xFF;				//限幅


	for (int pul = 0; pul < len - 1; pul++)						//装填校验和
	{
		DatAbf[len - 1] = DatAbf[len - 1] + DatAbf[pul];
	}

	for (int lup = 0; lup < len; lup++)							//转移数据
	{
		UDP_slide_data[UDP_slide_int].data[lup] = DatAbf[lup];
	}
	UDP_slide_data[UDP_slide_int].mark = 1;						//更改前将标志位置1，表示可读
	UDP_slide_int = (UDP_slide_int + 1) % UDP_slide_pool_max;	//输入循环

}

//UDP数据接收线程
void udp_recive(void)
{
	uint8_t CheckSum = 0;				//校验和
	int32_t count = 0;					//数据接受是否成功

	UDP_DRR.addr_rece = sizeof(UDP_DRR.receive);
	count = recvfrom(UDP_DRR.server_fd, UDP_rx_buff[UDP_buff_int].data, 36, 0, (struct sockaddr*)&UDP_DRR.receive, &UDP_DRR.addr_rece);  //recvfrom是拥塞函数，没有数据就一直拥塞
	if (count == -1)
	{
		printf("Recieve Data Failed!\n");
		UDP_DRR.abnormal_RX = 2;
	}
	else
	{
		UDP_DRR.abnormal_RX = 1;
		for (int i = 0; i < UDP_rx_buff[UDP_buff_int].data[4] - 1; i++)//计算校验和
		{
			CheckSum = CheckSum + UDP_rx_buff[UDP_buff_int].data[i];
		}
		//若开启效验
		if (UDP_DRR.vada == 1)
		{
			//效验和不通过
			if (CheckSum != UDP_rx_buff[UDP_buff_int].data[UDP_rx_buff[UDP_buff_int].data[4] - 1])
			{
				UDP_rx_buff[UDP_buff_int].IP = UDP_DRR.receive.sin_addr.s_addr;				//记录发送者IP
				UDP_rx_buff[UDP_buff_int].port = UDP_DRR.receive.sin_port;					//记录发送者端口号
				UDP_rx_buff[UDP_buff_int].mark = 2;											//标志数据效验和出错			
			}
			else
			{
				UDP_rx_buff[UDP_buff_int].IP = UDP_DRR.receive.sin_addr.s_addr;				//记录发送者IP
				UDP_rx_buff[UDP_buff_int].port = UDP_DRR.receive.sin_port;					//记录发送者端口号
				UDP_rx_buff[UDP_buff_int].mark = 1;											//标志数据已更新
				UDP_buff_int = (UDP_buff_int + 1) % UDP_rx_buff_max;							//输入循环
			}
		}
		else
		{
			UDP_rx_buff[UDP_buff_int].IP = UDP_DRR.receive.sin_addr.s_addr;				//记录发送者IP
			UDP_rx_buff[UDP_buff_int].port = UDP_DRR.receive.sin_port;					//记录发送者端口号

			//uint16_t iii = 0;
			//int16_t i = 0, ii = 0;


			//iii = (UDP_rx_buff[UDP_buff_int].data[0] << 8) | UDP_rx_buff[UDP_buff_int].data[1];
			//i = (UDP_rx_buff[UDP_buff_int].data[6] << 8) | UDP_rx_buff[UDP_buff_int].data[7];
			//ii = (UDP_rx_buff[UDP_buff_int].data[8] << 8) | UDP_rx_buff[UDP_buff_int].data[9];

			//if (iii == 0XFFBA)
			//{
			//	printf("bit:%d   VX:%d   VY:%d\n", UDP_DRR.cumulative_reception, i, ii);
			//}

			UDP_rx_buff[UDP_buff_int].mark = 1;		//标志数据已更新
			UDP_buff_int = (UDP_buff_int + 1) % UDP_rx_buff_max;							//输入循环	



		}
		//接收总量计算
		if (UDP_DRR.cumulative_reception < 0xFFFFFFFFFFFFFFFE)UDP_DRR.cumulative_reception = UDP_DRR.cumulative_reception + 1;
		else UDP_DRR.cumulative_reception = 0;
	}

}

//UDP数据发送线程
void udp_send()
{
	int32_t count = 0;

	UDP_DRR.addr_send = sizeof(UDP_DRR.sending);
	if (UDP_slide_data[UDP_slide_out].mark == 1)				//数据待发送
	{
		if ((UDP_slide_data[UDP_slide_out].IP != 0) && (UDP_slide_data[UDP_slide_out].port != 0))				//目标IP与端口号不为0
		{
			UDP_DRR.sending.sin_addr.s_addr = UDP_slide_data[UDP_slide_out].IP;									//设定目标IP
			UDP_DRR.sending.sin_port = UDP_slide_data[UDP_slide_out].port;										//设定目标端口
		}

		count = sendto(UDP_DRR.server_fd, UDP_slide_data[UDP_slide_out].data, UDP_slide_data[UDP_slide_out].len, 0, (struct sockaddr*)&UDP_DRR.sending, UDP_DRR.addr_send);		//发送数据
		if (count == -1)
		{
			printf("Send Data Failed!\n");
			UDP_DRR.abnormal_TX = 2;
		}
		else
		{
			UDP_DRR.abnormal_TX = 1;
			UDP_slide_data[UDP_slide_out].mark = 0;							//更改前将标志位置0，表示已发送
			UDP_slide_out = (UDP_slide_out + 1) % UDP_slide_pool_max;		//输出循环
			//发送总量计算
			if (UDP_DRR.cumulative_transmission < 0xFFFFFFFFFFFFFFFE)UDP_DRR.cumulative_transmission = UDP_DRR.cumulative_transmission + 1;
			else UDP_DRR.cumulative_transmission = 0;
		}
	}

}

//UDP数据解析线程
//滑动读取器
void UDP_BUFF_read()
{
	if (UDP_rx_buff[UDP_buff_out].mark == 1)
	{
		//--------------------------------------------UDP数据读取处理区------------------------------------------------------





		//--------------------------------------------UDP数据读取处理区------------------------------------------------------
		UDP_rx_buff[UDP_buff_out].mark = 0;					//标志读取完成
		UDP_buff_out = (UDP_buff_out + 1) % UDP_rx_buff_max;
	}
}

//UDP状态打印
void UDP_state_printf()
{
	float load1 = 0, load2 = 0;
	//打印工作负载
		for (int ii = 0; ii < UDP_rx_buff_max; ii++)
		{
			if (UDP_rx_buff[ii].mark == 1)
			{
				load1++;
			}
		}
		load1 = (load1 / UDP_rx_buff_max)*100;
		for (int ii = 0; ii < UDP_slide_pool_max; ii++)
		{
			if (UDP_slide_data[ii].mark == 1)
			{
				load2++;
			}
		}
		load2 = (load2 / UDP_slide_pool_max)*100;
		printf("UDP:缓冲区负载：RX：%.2f%   TX:%.2f%  累计发送：%d,累计接收：%d\n", load2, load1, UDP_DRR.cumulative_transmission, UDP_DRR.cumulative_reception);

}


//UDP异常查询
uint8_t UDP_abnormal_query(uint8_t code)
{
	//用于UDP初始化、接收失败、发送失败
	//初始化code：1  成功：1，失败：0
	//接收失败code：  2  成功：1，失败：0
	//发送失败code：  3  成功：1，失败：0
	uint8_t ret = 0;
	switch (code)
	{
		//查询UDP发送初始化是否成功
		case (UDP_abnormal_init):;
		{
			ret = UDP_DRR.abnormal_init;
			break;
		}
		//查询UDP发送初始化是否成功
		case (UDP_abnormal_RX):;
		{
			ret = UDP_DRR.abnormal_RX;
			break;
		}
		//查询UDP发送初始化是否成功
		case (UDP_abnormal_TX):;
		{
			ret = UDP_DRR.abnormal_TX;
			break;
		}
	}
	return ret;
}
