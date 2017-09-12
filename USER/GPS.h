#ifndef GPS__H_
#define GPS__H_

#include "stm32f10x.h"
#include "os.h"
#include "Common.h"

typedef enum{
	GPS_OFF = 0,
	GPS_ON  = 0,
}Gps_Status;




typedef enum {
	CONFIG_TAG,
	SOCKET_DATA_TAG,
	GPS_DATA_TAG,
}App_tag;






extern OS_TCB	GPSTaskTCB;




void GPS_task(void *p_arg);





#endif
