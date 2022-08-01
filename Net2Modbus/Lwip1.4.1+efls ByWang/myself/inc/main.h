																			   /**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f107.h"
#include "stm32f10x_wwdg.h"
#include "stm32f10x_iwdg.h"
//MYSELFBOARD�����˱�ʾʹ�ÿ����塣�������ʾʹ�����Լ������ǰ���
//���һ������1820��IO�ں�SD����CS���Ų�һ����
//#define MYSELFBOARD
//ʹ��SMTP
//#define USED_SMTP


//��������ʹ�ܲ���Ӧ�����ݷ��͹㲥
//#define UDPBOARDCAST_ENABLE 

//��������ʹ��MODBUS RTU TX/RX�ײ��շ� (ע��Ӧ�ò�û��ʹ�á���ΪӦ�ò���㽻��������������߽�����RTU͸��)
#define USED_MODBUS		1

/*���������ʹ�÷������·�FFBB����ָ��������ú��Զ���ѵ485�豸*/
#define RS485_MASTER_CALL

//���������ʹ��SD���ļ�ϵͳ
//#define USED_FILESYSTEM

//���������ʹ��HTTP 
//#define USED_HTTP

//���������ʹ�� DS18B20 �ɼ��¶�
//#define USED_DS18B20


//���������ʹ�ÿ��Ź� 
//#define USED_WATCHDOG
/*
�������ΪIAP���򣬹رպ���ΪӦ�ó���
�ر�ע�⣺
IAP������뿪ʼ��ַ��0x08000000
APP������뿪ʼ��ַ��0x08010000  */
#define USE_IAP_TFTP   /* enable IAP using TFTP */

#define   MAIN_SN        0      //�����
#define   SECOND_SN      9     //�����
#define   SUB_SN         1    //�����

/*����֮������ʹ��433����*/
//#define   USED_SI4432

/* Exported function prototypes ----------------------------------------------*/
void Time_Update(void);
void Delay(uint32_t nCount);


#ifdef USED_WATCHDOG
void WatchDogInit(void);
void KeepWatchDogLive(void);
#endif






#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

