#include "socket.h"
#include "Common.h"
#include "Uart2.h"


uint8_t SocketStatus = 0;



void SocketConfig()
{
	OS_ERR err = OS_ERR_NONE;
	uint16_t Rlen = 0;
	//����PDP
	u2_printf((char*)CMD_2503S[PDP_CONFIG]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
			;
	}
	
		//����PDP����
	u2_printf((char*)CMD_2503S[PDP_ACTIVITE]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
			;
	}
	
	//��������������socket����  ,�˴��Ļظ�������OK
	u2_printf((char*)CMD_2503S[SOCKET_ESTABLISH]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
			;
	}
	
	//����͸��ģʽ  ,�˴��Ļظ�������OK
	u2_printf((char*)CMD_2503S[ENTER_TRANSFER]);
	OSSemPend(&Uart_SEM,CMD_ACK_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
	if(err != OS_ERR_NONE)
		;
	else
	{
		if(!CheckUart2ACK("OK",&Rlen))
			;
	}
	
	
	
	
}

