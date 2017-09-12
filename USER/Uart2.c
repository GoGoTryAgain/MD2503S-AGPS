#include "Uart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "os.h"

extern OS_SEM Uart_SEM;
extern OS_SEM GPS_DATA_SEM;


/*uart2  MD2503s module*/


//串口接收缓存区 	
uint8_t USART2_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
uint8_t  USART2_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART2_MAX_SEND_LEN字节


u16 USART2_RX_STA=0;       //接收状态标记	  






void Uart2_init(u32 bound)
{
	uint8_t ClearTmp = 0;
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 	USART_DeInit(USART2);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.03
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	USART_GetITStatus(USART2, USART_IT_RXNE);
	ClearTmp =USART_ReceiveData(USART2);
}




void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	OS_ERR err = OS_ERR_NONE;
#ifdef SYSTEM_SUPPORT_OS	 	
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res =USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		
//		if((USART2_RX_STA&0x8000)==0)//接收未完成
			{
//				if(USART2_RX_STA&0x4000)//接收到了0x0d
//				{
//					if(Res!=0x0a)
//						USART2_RX_STA=0;//接收错误,重新开始
//					else 
//					{
//						USART2_RX_STA|=0x8000;	//接收完成了
////						OSSemPost(&Uart_SEM,OS_OPT_POST_1,&err);
//					}						
//				}
//			else //还没收到0X0D
//				{	
//					if(Res==0x0d)USART2_RX_STA|=0x4000;
//				else
					{
						USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
						USART2_RX_STA++;
						if(USART2_RX_STA>(USART3_MAX_RECV_LEN-1))
							USART2_RX_STA=0;//接收数据错误,重新开始接收	  
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
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}





