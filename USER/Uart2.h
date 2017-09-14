#ifndef UART2__H_
#define UART2__H_


#include "stm32f10x.h"
#include "os.h"


#define USART3_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.

#define USART2_REC_LEN  			200  	//�����������ֽ��� 200




extern uint8_t  USART2_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern uint8_t  USART2_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern uint16_t USART2_RX_STA;   						//��������״̬
extern OS_TMR Uart_timeout_TMR;
extern OS_SEM SemTimerUart;  //�����ж����������ö�ʱ��



void Uart2_init(u32 bound);
void u2_printf(char* fmt,...);  


#endif
