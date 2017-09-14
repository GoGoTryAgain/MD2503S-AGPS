#include "Common.h"
#include "usart.h"
#include "Uart2.h"
#include "string.h"	

OS_SEM Uart_SEM;
OS_SEM GPS_DATA_SEM;


const char *CMD_2503S[30] = {
									"AT+ESLP=0\r\n",  //禁止睡眠
									"AT+ESLP=1\r\n",  //开启睡眠
									"AT+EGPSC=0\r\n", //关闭GPS
									"AT+EGPSC=1\r\n", //打开GPS
									"AT+EGPSS=\"PMTK353,1,0,0,0,1*2B\"\r\n",  //Search GLONASS satellites only
									"AT+EGPSS=\"PMTK353,1,0,0,0,0*2A\"\r\n",	// Search GPS satellites only
									"AT+EGPSS=\"PMTK353,0,0,0,0,1*2A\"\r\n",	//Search BEIDOU satellites only
									"AT+EGPSS=\"PMTK353,1,0,0,0,1*2B\"\r\n",	// Search GPS and BEIDOU satellites
									"AT+EGPSS=\"PMTK353,1,1,0,0,0*2B\"\r\n",	//Search GPS and GLONASS satellites
									"AT+EGPSS=\"PMTK353,1,1,1,0,0*2A\"\r\n",	//Search GPS GLONASS, GALILEO satellites
									"AT+ICCID\r\n", //测试SIM卡有没有装好,没有返回一串数字说明SIM卡没有识别到
									"AT+EGMR=0,7\r\n",  //查询IMEI号 
									"AT+POWEROFF\r\n",  //关机
									"AT+EDNS=0,\"my realm name \"\r\n", //域名解析  
									/*GPRS 入网*/
									"AT+EGDCONT=0,\"IP","CMNET\"\r\n",  //1. 配置PDP
									"AT+ETCPIP=1,0\r\n",  //2. 激活PDP连接
									"AT+ETL=1,0,0,\"122.228.19.57\",29329\r\n",  //3. 建立到服务器的SOCKET连接
									"AT+ETLTS=0\r\n",  //4. 进入此连接的透传模式，这个命令根据建立socket返回的ID号执行。##注意：这条指令里面的数字，是从第3条指令的回复中得来的！
									"+++",  //主动退出透传模式
									"AT+ETL=0,N\r\n",  // 关闭socket(N是socket数字)
									/*GPRS 入网end*/
									"AT+CFUN=1\r\n",  //正常模式
									"AT+CFUN=4\r\n",	//飞行模式
									"AT+CGMR\r\n",   //读取版本号
								};


														
														
														
//当发送完配置，等待串口返回
OS_ERR CheckUart2ACK(char *RightStr,uint16_t *RxLen)
{
	uint16_t rxlen = 0;
	uint16_t i =0;
	USART2_RX_STA = 0;
//	if(USART2_RX_STA&0X8000)						//接收到一次数据了
	{
		rxlen=USART2_RX_STA&0X3FFF;				//得到数据长度
		*RxLen = rxlen;
		for(i=0;i<rxlen;i++)
			USART1_TX_BUF[i]=USART2_RX_BUF[i];	
		USART2_RX_STA=0;		   						//启动下一次接收
		USART1_TX_BUF[i]=0;								//自动添加结束符
//		printf("\r\n%s\r\n",USART1_TX_BUF);//发送接收到的数据到串口1
		
		if(strstr((char *)USART1_TX_BUF, RightStr) != NULL)
		{
			return OS_ERR_NONE;
		}
		else		
			return OS_ERR_E;
	}
//	return OS_ERR_NONE;
}
																	
														
