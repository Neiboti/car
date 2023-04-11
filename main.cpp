#include <cstdio>
#include "APP_control.h"
#include "APP_thread.h"





int main()
{
	//初始化
	APP_init();
	sleep(1);
	//各线程启动
	thread_creat();

	while (1)
	{

		APP_CONTROL();		//中控主函数

	}

	return 0;
}