#include "driver_CAN.h"

DRR_CAN Init_can = { 0 };

//滑动发射器
CAN_Data CAN_slide_data[CAN_slide_pool_max] = { 0 };		//发送缓冲区
uint8_t CAN_slide_int = 0;									//滑入坐标
uint8_t CAN_slide_out = 0;									//滑出坐标

//接收缓冲区
CAN_Data CAN_rx_buff[CAN_rx_buff_max] = { 0 };				//接收缓冲区
uint8_t CAN_buff_int = 0;									//滑入坐标
uint8_t CAN_buff_out = 0;									//滑出坐标


//CAN初始化
uint8_t driver_can_init(void)
{
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame[1] = { 0 };
	Init_can.SocketCan0 = socket(PF_CAN, SOCK_RAW, CAN_RAW);					//创建套接字
	strcpy(ifr.ifr_name, "can0");
	ioctl(Init_can.SocketCan0, SIOCGIFINDEX, &ifr);								//指定 can0 设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(Init_can.SocketCan0, (struct sockaddr*)&addr, sizeof(addr));			//将套接字与 can0 绑定
	//fcntl(SocketCanFd, F_SETFL, FNDELAY);								//用于设置read函数的非阻塞
	printf("CAN初始化成功\n");
	return Init_can.SocketCan0;
}


//发送数据时调用此函数
//滑动输入器
void CAN_pool_input(uint32_t ID, uint8_t ExiD, uint8_t len, uint8_t* DatAbf)
{
	CAN_slide_data[CAN_slide_int].mark = 0;						//更改前将标志位置0，表示不可读
	CAN_slide_data[CAN_slide_int].len = len;
	CAN_slide_data[CAN_slide_int].frame.can_dlc = len;

	for (int lup = 0; lup < len; lup++)							//转移数据
	{
		CAN_slide_data[CAN_slide_int].frame.data[lup] = DatAbf[lup];
	}

	if (ExiD == 1)//使用扩展帧
	{
		CAN_slide_data[CAN_slide_int].frame.can_id = ID | 0x80000000; //第31bit置1
	}
	else//使用标准帧
	{
		CAN_slide_data[CAN_slide_int].frame.can_id = ID;
	}

	CAN_slide_data[CAN_slide_int].mark = 1;						//更改前将标志位置1，表示可读
	CAN_slide_int = (CAN_slide_int + 1) % CAN_slide_pool_max;	//输入循环

}

//CAN数据接收线程
void CAN_recive(void)
{
	int32_t count = 0;					//数据接受是否成功
	count = read(Init_can.SocketCan0, &CAN_rx_buff[CAN_buff_int].frame, sizeof(CAN_rx_buff[CAN_buff_int].frame)); //接收报文 
	if (count > 0)
	{

		//printf("RX=id=%x   ", CAN_rx_buff[CAN_buff_int].frame.can_id);
		//for (int asd = 0; asd < 8; asd++)
		//{
		//	printf("%x", CAN_rx_buff[CAN_buff_int].frame.data[asd]);
		//}
		//printf("\n\n");

		CAN_rx_buff[CAN_buff_int].mark = 1;
		CAN_buff_int = (CAN_buff_int + 1) % CAN_rx_buff_max;	//输入循环

		//接收总量计算
		if (Init_can.cumulative_reception < 0xFFFFFFFFFFFFFFFE)Init_can.cumulative_reception = Init_can.cumulative_reception + 1;
		else Init_can.cumulative_reception = 0;


	}
	else printf("Recive Err\n");
}
//CAN数据发送线程
void CAN_send(void)
{
	int32_t nbytes = 0;					//数据发送是否成功
	if (CAN_slide_data[CAN_slide_out].mark == 1)
	{
		nbytes = write(Init_can.SocketCan0, &CAN_slide_data[CAN_slide_out].frame, sizeof(CAN_slide_data[CAN_slide_out].frame));
		if (nbytes != sizeof(CAN_slide_data[CAN_slide_out].frame))
		{
			printf("Send Error frame[0] nbytes=%d \n", nbytes);//正常nbytes=16
		}
		else
		{
			//printf("TX=id=%x   ", CAN_slide_data[CAN_slide_out].frame.can_id);
			//for (int asd = 0; asd < 8; asd++)
			//{
			//	printf("%x", CAN_slide_data[CAN_slide_out].frame.data[asd]);
			//}
			//printf("\n\n");

			CAN_slide_data[CAN_slide_out].mark = 0;
			CAN_slide_out = (CAN_slide_out + 1) % CAN_slide_pool_max;	//输入循环
			//发送总量计算
			if (Init_can.cumulative_transmission < 0xFFFFFFFFFFFFFFFE)Init_can.cumulative_transmission = Init_can.cumulative_transmission + 1;
			else Init_can.cumulative_transmission = 0;
		}
	}
	usleep(1500);						//延时us
}

////CAN数据解析线程
////滑动读取器
//void CAN_BUFF_read()
//{
//	if (CAN_rx_buff[CAN_buff_out].mark == 1)
//	{
//		//--------------------------------------------CAN数据读取处理区------------------------------------------------------
//
//
//
//
//
//		//--------------------------------------------CAN数据读取处理区------------------------------------------------------
//		CAN_rx_buff[CAN_buff_out].mark = 0;					//标志读取完成
//		CAN_buff_out = (CAN_buff_out + 1) % CAN_rx_buff_max;
//	}
//}
//CAN状态打印
void CAN_state_printf()
{
	float load1 =0.f, load2 =0.f;
	//打印工作负载
	for (int ii = 0; ii < CAN_rx_buff_max; ii++)
	{
		if (CAN_rx_buff[ii].mark == 1)
		{
			load1++;
		}
	}
	load1 = (load1 / CAN_rx_buff_max) * 100;
	for (int ii = 0; ii < CAN_slide_pool_max; ii++)
	{
		if (CAN_slide_data[ii].mark == 1)
		{
			load2++;
		}
	}
	load2 = (load2 / CAN_slide_pool_max) * 100;
	//printf("CAN:缓冲区负载：RX：%.2f%   TX:%.2f%  累计发送：%8d,累计接收：%8d\n", load2, load1, Init_can.cumulative_transmission, Init_can.cumulative_reception);

}