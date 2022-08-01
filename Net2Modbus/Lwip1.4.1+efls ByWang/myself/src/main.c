/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eth.h"
#include "netconf.h"
#include "main.h"
#include "filesystem.h"
#include "ds18b20.h"
#include "flash_if.h"  


#ifdef USED_MODBUS
#include "stm32f10x_it.h"
#include "usart.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;

/* Private function prototypes -----------------------------------------------*/
void System_Periodic_Handle(void);
void InitTIM(void);
void TIM2_IRQHandler(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

extern void ModbusSenttset(uint32_t Time);
extern void UDPSendData(uint32_t Time);
extern void SendDataToSever(uint32_t Time);  
extern void tcp_sever_test(void);
extern void FileSystemInit(void);
extern void FileSystemThread(void); 
extern void httpd_init(void);
extern int IAP_main(void);
extern void initsi4432(void);
extern  void  Si4432Thread(uint32_t t);

char poll_net_data_lock=0;

uint32_t flash_net_buf[4];
uint8_t ipaddr_sever[4];
uint16_t port_sever;
uint32_t iap_mode;//�Ƿ����IAPģʽ��

int main(void)
{
  
		   {		
				  //��������FLASH ����
		    GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
            ipaddr_sever[0]=flash_net_buf[0]>>24;//ȡ�����IPλ
			ipaddr_sever[1]=flash_net_buf[0]>>16;
			ipaddr_sever[2]=flash_net_buf[0]>>8;
			ipaddr_sever[3]=flash_net_buf[0]>>0;//ȡ�����IPλ
			port_sever     =flash_net_buf[1];
			iap_mode	   =flash_net_buf[2];//ȡ����IAP mode
		
		   }


		 #ifdef USE_IAP_TFTP
/*
		 
		iap_mode==0xAAAAAAAA ���� iap_mode==0xFFFFFFFF 
		�����ʾ�ϵ����Ҫ��ת��APP���������У�Ĭ������ת��APP���ģ������תʧ���������IAP��
		
		iap_mode==0x55555555		
		�����ʾ�ϵ������IAP�����ȴ���������
		 
*/
		if((iap_mode==0xAAAAAAAA))
		{
		  // ��ת��APP���������У�����APP����
		 IAP_EndSetFlash();
		 //�����ȷ��ִ��������Զ���ᵽ���������ȥ��Ӧ�ó��������У����ǵ�������
		 //��ʾAPPӦ�ó��������ǿյģ������ϴ���д����ȷ����ʱ���������IAP���У��ȴ�������
		  
		}
		else
		if((iap_mode==0x55555555)|(iap_mode==0xFFFFFFFF))
		{
			//����IAP�������� �ȴ���������
		
		}

		#endif
	 /* Setup STM32 system (clocks, Ethernet, GPIO, NVIC) */
		  System_Setup();
		 

		  #ifdef USED_SI4432
		   initsi4432();
		   rx_data();
		  #endif
		  	 
	      InitTIM();//systick ���� ��TIM���� systick ר����DS18B20��US��ʱ����
			 
	      #ifdef USED_FILESYSTEM
		   FileSystemInit(); 
	       FileSystemThread();    
		  #endif 
		     
		  /* Initilaize the LwIP satck */
		  LwIP_Init();

		 // netbios_init();
		 
		  //tftpd_init();

		  //tcp_sever_test();

		  #ifdef USED_FILESYSTEM && USED_HTTP
		  /*Infinite loop */
		  httpd_init();
		  #endif

		  #ifdef USED_SMTP
		  my_smtp_test("The Board is Power up...");
		  #endif

		 
		  #ifdef USED_WATCHDOG
		  WatchDogInit();
          #endif

		   #ifdef USE_IAP_TFTP
		  IAP_main();
		  #endif


		  while (1)
		  {   

		   
			  TcpTestThread(LocalTime);
		      SendDataToSever(LocalTime);

			  #ifdef UDPBOARDCAST_ENABLE 
		      UDPSendData(LocalTime);
			  #endif

		      LwIP_Periodic_Handle(LocalTime);

			  #ifdef USED_MODBUS
			  ModbusSenttset(LocalTime); //����Ƿ���TCP���ܵ����ݣ�����з��� PASS RTU
			 (void)eMBMasterPoll();//���м�� modbus RTU ����
			  #endif

			  #ifdef USED_WATCHDOG
              KeepWatchDogLive();
              #endif
			 
		  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
 // LocalTime += SYSTEMTICK_PERIOD_MS
}

 /*
		Systick ����DS18B20�ľ�ȷ��ʱ�ϡ�
		TIM2���ڱ��ص�ʱ�����
		20151020

  */

 extern void SynruntimHook(uint32_t Time);
void TIM2_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);	     //���жϱ��
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	 //�����ʱ��TIM2����жϱ�־λ

		LocalTime += SYSTEMTICK_PERIOD_MS;

		SynruntimHook(LocalTime);//ͬ��ʱ��
  	
	}
	
}	


void InitTIM(void)
{	
    uint16_t 	usPrescalerValue;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//====================================ʱ�ӳ�ʼ��===========================
	//ʹ�ܶ�ʱ��2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//====================================��ʱ����ʼ��===========================
	//��ʱ��ʱ�������˵��
	//HCLKΪ72MHz��APB1����2��ƵΪ36MHz
	//TIM2��ʱ�ӱ�Ƶ��Ϊ72MHz��Ӳ���Զ���Ƶ,�ﵽ���
	//TIM2�ķ�Ƶϵ��Ϊ3599��ʱ���Ƶ��Ϊ72 / (1 + Prescaler) = 20KHz,��׼Ϊ50us
	//TIM������ֵΪusTim1Timerout50u	
	usPrescalerValue = (uint16_t) (72000000 / 20000) - 1;
	//Ԥװ��ʹ��
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	//====================================�жϳ�ʼ��===========================
	//����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//�������жϱ�־λ
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	//��ʱ��2����жϹر�
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	//��ʱ��3����
	TIM_Cmd(TIM2, DISABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = usPrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (uint16_t)(10 * 1000 / 50);//10ms
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	   
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, ENABLE);	


}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


#ifdef USED_WATCHDOG
void WatchDogInit(void)
{
 
	/*	
	
		Min/max IWDG timeout period (in ms) at 40 kHz (LSI)
				Min timeout RL[11:0]= 0x00   Max timeout RL[11:0]= 0xFF
IWDG_Prescaler_4:             min 0.1ms  ~       max 409.6ms
IWDG_Prescaler_8:             min 0.2ms  ~       max 819.2ms 
IWDG_Prescaler_16:            min 0.4ms  ~       max 1638.4ms 
IWDG_Prescaler_32:            min 0.8ms  ~       max 3276.8ms 
IWDG_Prescaler_64:            min 1.6ms  ~       max 6553.6ms 
IWDG_Prescaler_128:           min 3.2ms  ~       max 13107.2ms 
IWDG_Prescaler_256:           min 6.4ms  ~       max 26214.4ms  
	*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32); //���ó�ʱʱ��Ϊ409.6ms
	IWDG_SetReload(0xFFF);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	IWDG_Enable();
}
 
 void KeepWatchDogLive(void)
 {

 	IWDG_ReloadCounter();
 	
 }

#endif





/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
