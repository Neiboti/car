#ifndef __driver_handle_H__
#define __driver_handle_H__

#include <fcntl.h>  
#include <errno.h>  
#include <linux/input.h>  
#include <linux/joystick.h>  

#include "APP_control.h"

#define XBOX_TYPE_BUTTON    0x01  
#define XBOX_TYPE_AXIS      0x02  

#define XBOX_BUTTON_A       0x00  
#define XBOX_BUTTON_B       0x01  
#define XBOX_BUTTON_X       0x03  
#define XBOX_BUTTON_Y       0x04  
#define XBOX_BUTTON_LB      0x06  
#define XBOX_BUTTON_RB      0x07  
#define XBOX_BUTTON_START   0x0b  
#define XBOX_BUTTON_BACK    0x0a  
#define XBOX_BUTTON_HOME    0x09  
#define XBOX_BUTTON_LO      0x02    /* 左摇杆按键 */  
#define XBOX_BUTTON_RO      0x05    /* 右摇杆按键 */  

#define XBOX_BUTTON_ON      0x01  
#define XBOX_BUTTON_OFF     0x00  

#define XBOX_AXIS_LX        0x00    /* 左摇杆X轴 */  
#define XBOX_AXIS_LY        0x01    /* 左摇杆Y轴 */  
#define XBOX_AXIS_RX        0x02    /* 右摇杆X轴 */  
#define XBOX_AXIS_RY        0x03    /* 右摇杆Y轴 */  
#define XBOX_AXIS_LT        0x05  
#define XBOX_AXIS_RT        0x04  
#define XBOX_AXIS_XX        0x06    /* 方向键X轴 */  
#define XBOX_AXIS_YY        0x07    /* 方向键Y轴 */  

#define XBOX_AXIS_VAL_UP        -32767  
#define XBOX_AXIS_VAL_DOWN      32767  
#define XBOX_AXIS_VAL_LEFT      -32767  
#define XBOX_AXIS_VAL_RIGHT     32767  

#define XBOX_AXIS_VAL_MIN       -32767  
#define XBOX_AXIS_VAL_MAX       32767  
#define XBOX_AXIS_VAL_MID       0x00  


typedef struct xbox_map
{
	int     time;
	int     a;
	int     b;
	int     x;
	int     y;
	int     lb;
	int     rb;
	int     start;
	int     back;
	int     home;
	int     lo;
	int     ro;
	short int     lx;
	short int     ly;
	short int     rx;
	short int     ry;
	short int     lt;
	short int     rt;
	short int     xx;
	short int     yy;
}xbox_map_t;










//手柄服务
void handle_service();





#endif
