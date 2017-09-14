#ifndef UART2__H_
#define UART2__H_


#include "stm32f10x.h"
#include "os.h"


#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

#define USART2_REC_LEN  			200  	//定义最大接收字节数 200




extern uint8_t  USART2_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern uint8_t  USART2_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern uint16_t USART2_RX_STA;   						//接收数据状态
extern OS_TMR Uart_timeout_TMR;
extern OS_SEM SemTimerUart;  //串口中断中用于启用定时器



void Uart2_init(u32 bound);
void u2_printf(char* fmt,...);  


#endif
