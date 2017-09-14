#include "Common.h"
#include "usart.h"
#include "Uart2.h"
#include "string.h"	

OS_SEM Uart_SEM;
OS_SEM GPS_DATA_SEM;


const char *CMD_2503S[30] = {
									"AT+ESLP=0\r\n",  //��ֹ˯��
									"AT+ESLP=1\r\n",  //����˯��
									"AT+EGPSC=0\r\n", //�ر�GPS
									"AT+EGPSC=1\r\n", //��GPS
									"AT+EGPSS=\"PMTK353,1,0,0,0,1*2B\"\r\n",  //Search GLONASS satellites only
									"AT+EGPSS=\"PMTK353,1,0,0,0,0*2A\"\r\n",	// Search GPS satellites only
									"AT+EGPSS=\"PMTK353,0,0,0,0,1*2A\"\r\n",	//Search BEIDOU satellites only
									"AT+EGPSS=\"PMTK353,1,0,0,0,1*2B\"\r\n",	// Search GPS and BEIDOU satellites
									"AT+EGPSS=\"PMTK353,1,1,0,0,0*2B\"\r\n",	//Search GPS and GLONASS satellites
									"AT+EGPSS=\"PMTK353,1,1,1,0,0*2A\"\r\n",	//Search GPS GLONASS, GALILEO satellites
									"AT+ICCID\r\n", //����SIM����û��װ��,û�з���һ������˵��SIM��û��ʶ��
									"AT+EGMR=0,7\r\n",  //��ѯIMEI�� 
									"AT+POWEROFF\r\n",  //�ػ�
									"AT+EDNS=0,\"my realm name \"\r\n", //��������  
									/*GPRS ����*/
									"AT+EGDCONT=0,\"IP","CMNET\"\r\n",  //1. ����PDP
									"AT+ETCPIP=1,0\r\n",  //2. ����PDP����
									"AT+ETL=1,0,0,\"122.228.19.57\",29329\r\n",  //3. ��������������SOCKET����
									"AT+ETLTS=0\r\n",  //4. ��������ӵ�͸��ģʽ�����������ݽ���socket���ص�ID��ִ�С�##ע�⣺����ָ����������֣��Ǵӵ�3��ָ��Ļظ��е����ģ�
									"+++",  //�����˳�͸��ģʽ
									"AT+ETL=0,N\r\n",  // �ر�socket(N��socket����)
									/*GPRS ����end*/
									"AT+CFUN=1\r\n",  //����ģʽ
									"AT+CFUN=4\r\n",	//����ģʽ
									"AT+CGMR\r\n",   //��ȡ�汾��
								};


														
														
														
//�����������ã��ȴ����ڷ���
OS_ERR CheckUart2ACK(char *RightStr,uint16_t *RxLen)
{
	uint16_t rxlen = 0;
	uint16_t i =0;
	USART2_RX_STA = 0;
//	if(USART2_RX_STA&0X8000)						//���յ�һ��������
	{
		rxlen=USART2_RX_STA&0X3FFF;				//�õ����ݳ���
		*RxLen = rxlen;
		for(i=0;i<rxlen;i++)
			USART1_TX_BUF[i]=USART2_RX_BUF[i];	
		USART2_RX_STA=0;		   						//������һ�ν���
		USART1_TX_BUF[i]=0;								//�Զ���ӽ�����
//		printf("\r\n%s\r\n",USART1_TX_BUF);//���ͽ��յ������ݵ�����1
		
		if(strstr((char *)USART1_TX_BUF, RightStr) != NULL)
		{
			return OS_ERR_NONE;
		}
		else		
			return OS_ERR_E;
	}
//	return OS_ERR_NONE;
}
																	
														
