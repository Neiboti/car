#include "driver_handle.h"

#include "APP_handle.h"



extern HandlE handle_data;
bool HandleOn;
int xbox_len, xbox_fd;                  //HandleOn =1开启手柄控制 =0关闭手柄控制
xbox_map_t map;



//
int xbox_open(const char* file_name)
{
	int xbox_fd;

	xbox_fd = open(file_name, O_RDONLY);//  非阻塞| O_NONBLOCK
	if (xbox_fd < 0)
	{
		return -1;
	}

	return xbox_fd;
}

//
int xbox_map_read(int xbox_fd, xbox_map_t* map)
{
	int len, type = 0, number, value;
	struct js_event js;

	len = read(xbox_fd, &js, sizeof(struct js_event));
	if (len < 0)
	{
		printf("手柄数据读取失败！\n");
		return -1;
	}

	type = js.type;
	number = js.number;
	value = js.value;

	map->time = js.time;

	if (type == JS_EVENT_BUTTON)
	{
		switch (number)
		{
		case XBOX_BUTTON_A:
			map->a = value;
			break;

		case XBOX_BUTTON_B:
			map->b = value;
			break;

		case XBOX_BUTTON_X:
			map->x = value;
			break;

		case XBOX_BUTTON_Y:
			map->y = value;
			break;

		case XBOX_BUTTON_LB:
			map->lb = value;
			break;

		case XBOX_BUTTON_RB:
			map->rb = value;
			break;

		case XBOX_BUTTON_START:
			map->start = value;
			break;

		case XBOX_BUTTON_BACK:
			map->back = value;
			break;

		case XBOX_BUTTON_HOME:
			map->home = value;
			break;

		case XBOX_BUTTON_LO:
			map->lo = value;
			break;

		case XBOX_BUTTON_RO:
			map->ro = value;
			break;

		default:
			break;
		}
	}
	else if (type == JS_EVENT_AXIS)
	{
		switch (number)
		{
		case XBOX_AXIS_LX:
			map->lx = value;
			break;

		case XBOX_AXIS_LY:
			map->ly = value;
			break;

		case XBOX_AXIS_RX:
			map->rx = value;
			break;

		case XBOX_AXIS_RY:
			map->ry = value;
			break;

		case XBOX_AXIS_LT:
			map->lt = value;
			break;

		case XBOX_AXIS_RT:
			map->rt = value;
			break;

		case XBOX_AXIS_XX:
			map->xx = value;
			break;

		case XBOX_AXIS_YY:
			map->yy = value;
			break;

		default:
			break;
		}
	}
	else
	{
		/* Init do nothing */
	}

	return len;
}



//手柄服务程序---线程
void handle_service()
{
	memset(&map, 0, sizeof(xbox_map_t));
	while (1)
	{
		xbox_fd = xbox_open("/dev/input/js0");
		if (xbox_fd < 0)
		{

			close(xbox_fd);
			sleep(3);
		}
		else
		{
			while (1)
			{
				xbox_len = xbox_map_read(xbox_fd, &map);
				if (xbox_len < 0)//手柄断开连接，尝试重连
				{
					memset(&map, 0, sizeof(xbox_map_t));
					xbox_fd = xbox_open("/dev/input/js0");
					if (xbox_fd < 0)
					{
						handle_data.setat = 1;
						close(xbox_fd);
						sleep(1);
					}
				}
				else//手柄连接正常
				{
					handle_data.setat = 2;
					handle_control();                       //手柄数据处理
				}
			}
		}
	}
}

















