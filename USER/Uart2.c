#include "Uart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "os.h"

extern OS_SEM Uart_SEM;
extern OS_SEM GPS_DATA_SEM;


/*uart2  MD2503s module*/


//���ڽ��ջ����� 	
uint8_t USART2_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
uint8_t  USART2_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�


u16 USART2_RX_STA=0;       //����״̬���	  






void Uart2_init(u32 bound)
{
	uint8_t ClearTmp = 0;
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 	USART_DeInit(USART2);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.03
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//�����ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	USART_GetITStatus(USART2, USART_IT_RXNE);
	ClearTmp =USART_ReceiveData(USART2);
}




void USART2_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	OS_ERR err = OS_ERR_NONE;
#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		
//		if((USART2_RX_STA&0x8000)==0)//����δ���
			{
//				if(USART2_RX_STA&0x4000)//���յ���0x0d
//				{
//					if(Res!=0x0a)
//						USART2_RX_STA=0;//���մ���,���¿�ʼ
//					else 
//					{
//						USART2_RX_STA|=0x8000;	//���������
////						OSSemPost(&Uart_SEM,OS_OPT_POST_1,&err);
//					}						
//				}
//			else //��û�յ�0X0D
//				{	
//					if(Res==0x0d)USART2_RX_STA|=0x4000;
//				else
					{
						USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
						USART2_RX_STA++;
						if(USART2_RX_STA>(USART3_MAX_RECV_LEN-1))
							USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
//				}
			}   		 
     } 
#ifdef SYSTEM_SUPPORT_OS	 
	OSIntExit();  											 
#endif
} 




void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}





