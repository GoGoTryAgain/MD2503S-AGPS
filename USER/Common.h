#ifndef COMMON__H__
#define COMMON__H__


#include "stm32f10x.h"
#include "os.h"


#define CMD_ACK_TIMEOUT		800   //����һ�������ȴ�ʱ��


typedef enum{
	EXIT_SLEEP = 0,     //�˳�����
	ENTER_SLEEP,		//��������
	OPEN_GPS,				//�򿪶�λ 
	CLOSE_GPS,			//�رն�λ
	FIX_GLONASS_ONLY,		//ֻ��GLONASS��λ
	FIX_GPS_ONLY,				//ֻ��GPS��λ
	FIX_BD_ONLY,				//ֻ�ñ�����λ
	FIX_GPS_AND_BD,			//GPS+������λ
	FIX_GPS_AND_GLONASS,			//GPS+GLONASS��λ
	FIX_GPS_GLONASS_BD_ALL,		//GPS+GLONASS+������λ
	TEST_SIM_OK,							//����SIM����û��װ��
	LOOK_IMEI,								//�鿴IMEI��
	POWER_DOWN,								//�ػ�
	REALM_NAME_PRASE,					//��������
	PDP_CONFIG,								//����PDP
	PDP_ACTIVITE,							//����PDP����
	SOCKET_ESTABLISH,					//socket����
	ENTER_TRANSFER,						//����͸��ģʽ
	EXIT_TRANSFER,						//�˳�͸��ģʽ	
	SOCKET_CLOSE,							//socket�ر�
	ENTER_NORMAL_MODE,				//2503��������ģʽ
	ENTER_FLY_MODE,						//2503�������ģʽ
	READ_VERSION,							//��ȡ�汾��
}CMD_FUN;



extern OS_SEM Uart_SEM;
extern OS_SEM GPS_DATA_SEM;


extern const char *CMD_2503S[30];
extern OS_ERR CheckUart2ACK(char *RightStr,uint16_t *RxLen);






#endif
