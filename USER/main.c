#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "GPS.h"
#include "Uart2.h"


//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()



OS_Q Distribute_Msg; 

OS_TMR Uart_timeout_TMR;

OS_SEM SemTimerUart;  //串口中断中用于启用定时器


void OSResourceInit(void);
void CreateUserTask(void);

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define LED0_TASK_PRIO		4
//任务堆栈大小	
#define LED0_STK_SIZE 		128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//任务优先级
#define LED1_TASK_PRIO		5
//任务堆栈大小	
#define LED1_STK_SIZE 		128
//任务控制块
OS_TCB Led1TaskTCB;
//任务堆栈	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *p_arg);

//任务优先级
#define FLOAT_TASK_PRIO		6
//任务堆栈大小
#define FLOAT_STK_SIZE		128
//任务控制块
OS_TCB	FloatTaskTCB;
//任务堆栈
__align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//任务函数
void float_task(void *p_arg);

//任务优先级
#define GPS_TASK_PRIO		15
//任务堆栈大小
#define GPS_STK_SIZE		256
//任务控制块
OS_TCB	GPSTaskTCB;
//任务堆栈
__align(8) CPU_STK	GPS_TASK_STK[GPS_STK_SIZE];
//任务函数
//extern void GPS_task(void *p_arg);


//任务优先级
#define DISTRIBUTE_TASK_PRIO		14
//任务堆栈大小
#define DISTRIBUTE_STK_SIZE		128
//任务控制块
OS_TCB	DistributeTaskTCB;
//任务堆栈
__align(8) CPU_STK	DISTRIBUTE_TASK_STK[DISTRIBUTE_STK_SIZE];
//任务函数
 void Distribute_task(void *p_arg);


//任务优先级
#define TIMER_UART_TASK_PRIO		10
//任务堆栈大小
#define TIMER_UART_STK_SIZE		64
//任务控制块
OS_TCB	Timer_Uart_TaskTCB;
//任务堆栈
__align(8) CPU_STK	TimerUart_TASK_STK[TIMER_UART_STK_SIZE];
//任务函数
 void TimerUart_task(void *p_arg);


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();       //延时初始化
	delay_ms(100);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init();    //串口波特率设置
	LED_Init();         //LED初始化
	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	CreateUserTask();
	OSResourceInit();							 

								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0=0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED0=1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//led1任务函数
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1=~LED1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//浮点测试任务
void float_task(void *p_arg)
{
	CPU_SR_ALLOC();
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		OS_CRITICAL_ENTER();	//进入临界区
		printf("float_num的值为: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//退出临界区
		delay_ms(500);			//延时500ms
	}
}

//串口定时器启用任务
 void TimerUart_task(void *p_arg)
{
	OS_ERR err = OS_ERR_NONE;
	OSTmrStart(&Uart_timeout_TMR,&err);
}


void CreateUserTask(void)
{
	OS_ERR err = OS_ERR_NONE;	
				 
//GPS 解析任务
	OSTaskCreate((OS_TCB 	* )&GPSTaskTCB,		
				 (CPU_CHAR	* )"GPS prase task", 		
                 (OS_TASK_PTR )GPS_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )GPS_TASK_PRIO,     	
                 (CPU_STK   * )&GPS_TASK_STK[0],	
                 (CPU_STK_SIZE)GPS_STK_SIZE/10,	
                 (CPU_STK_SIZE)GPS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			
								 
//串口数据分配任务
	OSTaskCreate((OS_TCB 	* )&DistributeTaskTCB,		
				 (CPU_CHAR	* )"distriute data task", 		
                 (OS_TASK_PTR )Distribute_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )DISTRIBUTE_TASK_PRIO,     	
                 (CPU_STK   * )&DISTRIBUTE_TASK_STK[0],	
                 (CPU_STK_SIZE)DISTRIBUTE_STK_SIZE/10,	
                 (CPU_STK_SIZE)DISTRIBUTE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);					
								 
//定时器启动任务
	OSTaskCreate((OS_TCB 	* )&Timer_Uart_TaskTCB,		
				 (CPU_CHAR	* )"start uart timer task", 		
                 (OS_TASK_PTR )TimerUart_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TIMER_UART_TASK_PRIO,     	
                 (CPU_STK   * )&TimerUart_TASK_STK[0],	
                 (CPU_STK_SIZE)TIMER_UART_STK_SIZE/10,	
                 (CPU_STK_SIZE)TIMER_UART_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);					
								 
}

//启用分发任务
void TimerUart()
{
	OS_ERR err = OS_ERR_NONE;
	while(1)
	{
		OSSemPend(&SemTimerUart,0,OS_OPT_PEND_BLOCKING,0,&err);
		OSQPost(&Distribute_Msg,(uint8_t*)USART2_RX_BUF,USART2_RX_STA,OS_OPT_POST_FIFO,&err);
	}

}


void TaskSemInit()
{
	OS_ERR err = OS_ERR_NONE;
	//串口配置类数据ASK信号量
	OSSemCreate(&Uart_SEM,"uart_rec_sem",0,&err);
	//创建gps的信号量
	OSSemCreate(&GPS_DATA_SEM,"Gps_rec_sem",1,&err);
}

void QueueMsgInit()
{
	OS_ERR err = OS_ERR_NONE;
	OSQCreate((OS_Q*)&Distribute_Msg,"uart data distribute",10,&err);
}


/*串口接收空闲超时定时器*/
void TimerInit()
{
	OS_ERR err = OS_ERR_NONE;
	OSTmrCreate((OS_TMR*)(&Uart_timeout_TMR),
							"uart time out tmr",
							0,2000,OS_OPT_TMR_ONE_SHOT,
							(OS_TMR_CALLBACK_PTR)TimerUart,
							0,
							&err);
	
	
	
}

//* 信号量、消息队列，定时器初始化  //
void OSResourceInit(void)
{
	TaskSemInit();
	TimerInit();
	QueueMsgInit();
}



//根据数据来源，启用对应的任务
App_tag CheckDataFrom(uint8_t *data,uint32_t len)
{
	uint8_t *tmp = NULL;
	tmp=(u8*)strstr((const char *)data,"+EGPSC");
	if(tmp != NULL)
		return GPS_DATA_TAG;
	else
		return CONFIG_TAG;
	//else if()
}



//根据串口数据，分发相应的数据到各个处理任务
 void Distribute_task(void *p_arg)
 {
	 	 
	 OS_MSG_SIZE size;
	 uint8_t *MsgData ;
	 OS_ERR err; 
	 App_tag Tag;
	 while(1)
	 {
		 MsgData = OSQPend((OS_Q*)&Distribute_Msg,
											 (OS_TICK)  0,
											 (OS_OPT)  OS_OPT_PEND_BLOCKING,
											 (OS_MSG_SIZE *)&size,
											 (CPU_TS*)  0,
											 (OS_ERR *)&err);
		 Tag = CheckDataFrom(MsgData,size);
		 switch(Tag)
		 {
			 case CONFIG_TAG:
				 OSSemPost(&Uart_SEM,OS_OPT_POST_1,&err);
				break;
			 case SOCKET_DATA_TAG: 
				break;
			 case GPS_DATA_TAG:
				 OSSemPost(&GPS_DATA_SEM,OS_OPT_POST_1,&err);
				break;
			 
		 }
	 }

	 
	 
 }

