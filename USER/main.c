#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "GPS.h"




/************************************************
 ALIENTEK ��Ӣ��STM32������UCOSʵ�� 
 ��4-1 UCOSIII UCOSIII��ֲ
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//�������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


OS_Q Distribute_Msg; 





void OSResourceInit(void);
void CreateUserTask(void);

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define LED0_TASK_PRIO		4
//�����ջ��С	
#define LED0_STK_SIZE 		128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//�������ȼ�
#define LED1_TASK_PRIO		5
//�����ջ��С	
#define LED1_STK_SIZE 		128
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *p_arg);

//�������ȼ�
#define FLOAT_TASK_PRIO		6
//�����ջ��С
#define FLOAT_STK_SIZE		128
//������ƿ�
OS_TCB	FloatTaskTCB;
//�����ջ
__align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//������
void float_task(void *p_arg);

//�������ȼ�
#define GPS_TASK_PRIO		5
//�����ջ��С
#define GPS_STK_SIZE		256
//������ƿ�
OS_TCB	GPSTaskTCB;
//�����ջ
__align(8) CPU_STK	GPS_TASK_STK[GPS_STK_SIZE];
//������
//extern void GPS_task(void *p_arg);


//�������ȼ�
#define DISTRIBUTE_TASK_PRIO		4
//�����ջ��С
#define DISTRIBUTE_STK_SIZE		128
//������ƿ�
OS_TCB	DistributeTaskTCB;
//�����ջ
__align(8) CPU_STK	DISTRIBUTE_TASK_STK[DISTRIBUTE_STK_SIZE];
//������
 void Distribute_task(void *p_arg);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();       //��ʱ��ʼ��
	delay_ms(100);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init();    //���ڲ���������
	LED_Init();         //LED��ʼ��
	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	CreateUserTask();
	OSResourceInit();							 

								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0=0;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED0=1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//led1������
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1=~LED1;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

//�����������
void float_task(void *p_arg)
{
	CPU_SR_ALLOC();
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		OS_CRITICAL_ENTER();	//�����ٽ���
		printf("float_num��ֵΪ: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//�˳��ٽ���
		delay_ms(500);			//��ʱ500ms
	}
}

void CreateUserTask(void)
{
	OS_ERR err = OS_ERR_NONE;
		//����LED0����
//	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
//				 (CPU_CHAR	* )"led0 task", 		
//                 (OS_TASK_PTR )led0_task, 			
//                 (void		* )0,					
//                 (OS_PRIO	  )LED0_TASK_PRIO,     
//                 (CPU_STK   * )&LED0_TASK_STK[0],	
//                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
//                 (CPU_STK_SIZE)LED0_STK_SIZE,		
//                 (OS_MSG_QTY  )0,					
//                 (OS_TICK	  )0,					
//                 (void   	* )0,					
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR 	* )&err);				
				 
//GPS ��������
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
								 
//�������ݷ�������
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
}


void TimerUart()
{
	
}


void TaskSemInit()
{
	OS_ERR err = OS_ERR_NONE;

	//����gps���ź���
	OSSemCreate(&GPS_DATA_SEM,"Gps_rec_sem",1,&err);
}

void QueueMsgInit()
{
	OS_ERR err = OS_ERR_NONE;
	OSQCreate((OS_Q*)&Distribute_Msg,"uart data distribute",10,&err);
}

OS_TMR Uart_timeout_TMR;

void TimerInit()
{
	OS_ERR err = OS_ERR_NONE;
	OSTmrCreate((OS_TMR*)(&Uart_timeout_TMR),
							"uart time out tmr",
							0,2000,OS_OPT_TMR_PERIODIC,
							(OS_TMR_CALLBACK_PTR)TimerUart,
							0,
							&err);
	
	
	
}


void OSResourceInit(void)
{
	TaskSemInit();
	TimerInit();
	QueueMsgInit();
}

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

//���ݴ������ݣ��ַ���Ӧ�����ݵ�������������
 void Distribute_task(void *p_arg)
 {
	 OS_MSG_SIZE size;
	 uint8_t *MsgData ;
	 OS_ERR err; 
	 App_tag Tag;
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
			 OSSemPost(&GPS_DATA_SEM,OS_OPT_POST_ALL,&err);
			break;
		 case SOCKET_DATA_TAG: 
			break;
		 case GPS_DATA_TAG:
			 OSSemPost(&GPS_DATA_SEM,OS_OPT_POST_1,&err);
			break;
		 
	 }
	 
	 
 }
