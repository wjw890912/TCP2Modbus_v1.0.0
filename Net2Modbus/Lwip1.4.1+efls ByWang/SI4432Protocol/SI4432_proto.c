// File Name:SI4432_proto.C
// Author:СARM�˲�&����׳
// Date: 2012��
 //Q Q:925295580


#include <stdio.h>
#include <string.h>
#include "SI4432.H"
#include "SI4432_proto.h"
#include "stm32f10x.h"
#include "main.h"

#ifdef USED_SI4432
 uint8_t TX_buf[5]; 
/*��������Э��֡����*/
unsigned char RecvCmdFromSI4432(uint8_t *pBuf,uint8_t leg)
{		 
	 	 uint32_t ID_RX=0;
		  uint16_t temp;


		if(leg!=5)return;	 //���ݰ���������ȷ
		
	
		
}		   

#endif
