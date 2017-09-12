#ifndef COMMON__H__
#define COMMON__H__


#include "stm32f10x.h"
#include "os.h"


#define CMD_ACK_TIMEOUT		800   //发送一组命令后等待时间


typedef enum{
	EXIT_SLEEP = 0,     //退出休眠
	ENTER_SLEEP,		//进入休眠
	OPEN_GPS,				//打开定位 
	CLOSE_GPS,			//关闭定位
	FIX_GLONASS_ONLY,		//只用GLONASS定位
	FIX_GPS_ONLY,				//只用GPS定位
	FIX_BD_ONLY,				//只用北斗定位
	FIX_GPS_AND_BD,			//GPS+北斗定位
	FIX_GPS_AND_GLONASS,			//GPS+GLONASS定位
	FIX_GPS_GLONASS_BD_ALL,		//GPS+GLONASS+北斗定位
	TEST_SIM_OK,							//测试SIM卡有没有装好
	LOOK_IMEI,								//查看IMEI号
	POWER_DOWN,								//关机
	REALM_NAME_PRASE,					//域名解析
	PDP_CONFIG,								//配置PDP
	PDP_ACTIVITE,							//激活PDP连接
	SOCKET_ESTABLISH,					//socket建立
	ENTER_TRANSFER,						//进入透传模式
	EXIT_TRANSFER,						//退出透传模式	
	SOCKET_CLOSE,							//socket关闭
	ENTER_NORMAL_MODE,				//2503进入正常模式
	ENTER_FLY_MODE,						//2503进入飞行模式
	READ_VERSION,							//读取版本号
}CMD_FUN;



extern OS_SEM Uart_SEM;
extern OS_SEM GPS_DATA_SEM;


extern const char *CMD_2503S[30];
extern OS_ERR CheckUart2ACK(char *RightStr,uint16_t *RxLen);






#endif
