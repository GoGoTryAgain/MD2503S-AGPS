#include "GPS.h"
#include "PraseGPS.h"
#include "usart.h"
#include "Uart2.h"
#include "os.h"
#include "string.h"	

uint8_t GPS_Status = GPS_OFF;

											
//GPS����														
void GpsConfig(void)
{
	OS_ERR err = OS_ERR_NONE;
	uint16_t Rlen = 0;
	/*�ر�����*/
	u2_printf((char*)CMD_2503S[EXIT_SLEEP]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
		{
			printf("exit sleep OK\r\n");
		}
	}

	//�ȹر�GPS
	u2_printf((char*)CMD_2503S[CLOSE_GPS]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
		{
			printf("CLOSE GPS OK\r\n");
		}
	}
	
	//�ٲ�ѯ�汾��
	u2_printf((char*)CMD_2503S[READ_VERSION]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
			;
	}
	
	//��GPS
//////	u2_printf((char*)CMD_2503S[OPEN_GPS]);
//////	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
//////	if(err != OS_ERR_NONE)
//////		;
//////	else
//////	{
//////		if(!CheckUart2ACK("OK",&Rlen))
//////			printf("OPEN gps ok");
//////	}
	
	//ѡ��λϵͳ
//////	u2_printf((char*)CMD_2503S[FIX_GPS_ONLY]);
//////	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
//////	if(err != OS_ERR_NONE)
//////		;
//////	else
//////	{
//////		if(!CheckUart2ACK("OK",&Rlen))
//////		{
//////			printf("OPEN gps ok");
//////			GPS_Status = GPS_ON;
//////			OS_TaskResume(&GPSTaskTCB,&err);//����GPS��������
//////		}
//////			
//////	}
	
}

//GPS���ݵȴ�������task
void GPS_task(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	uint16_t i = 0;
	uint16_t rxlen = 0;
	GpsConfig();
	while(1)
	{
		i = 0;
		OSSemPend(&GPS_DATA_SEM,0,OS_OPT_PEND_BLOCKING,0,&err);
		for(i=0;i<rxlen;i++)
			USART1_TX_BUF[i]=USART2_RX_BUF[i];	   
		USART2_RX_STA=0;		   	//������һ�ν���
		USART1_TX_BUF[i]=0;			//�Զ���ӽ�����		
		GPS_Analysis(&gpsx_info,USART1_TX_BUF);
	}
	
	
}






