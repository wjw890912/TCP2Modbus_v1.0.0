/**
  ******************************************************************************
  * @file    client.c
  * @author  MCD Application Team
  * @version wangjunwei
  * @date    20150717
  * @brief   A sample TCP client  Test
 */

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/tcp_impl.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "filesystem.h"
#include "stm32f10x.h"
#include "flash_if.h"


#define TCP_PORT      40096
//5s һ��TCP����	 (250ms*4)*5
#define TCP_CREATTRM_INTERVAL  (250*4)*5
//ϵͳ����ʱ��1S����һ��
#define SYSRUN_TIMER_MSECS         500
//2s	SENT TCP DATA
#define TCPSent_CREATTRM_INTERVAL (250*4)*2 
//500ms	SENT UDP DATA
#define UDPSent_CREATTRM_INTERVAL (250*2)
/* Private function prototypes -----------------------------------------------*/
void udp_client_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
void tcp_client_err(void *arg, err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

void  tcp_client_callback(void);
char Tcplink=1;	//TCP���� 1�Ǵ���������� 0��ʾ�����Ե�ǰ�Ѿ�����������
char Udplink=0;	//UDP���� 0�Ǵ���������� 1��ʾ�����Ե�ǰ�Ѿ�����������
struct tcp_pcb *wpcb; 
uint32_t retry_TCP_connect=0;
uint32_t SystemRunTime=0;
uint32_t TCPCreatTrm =0;
uint32_t RunSecTimer = 0;
struct udp_pcb *upcb;
u8 Udpbuf[10];
char UID_STM32[16];

extern uint32_t flash_net_buf[2];
extern uint8_t ipaddr_sever[4];	//sever IP
extern uint16_t port_sever;		//sever PORT
void udp_client_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	  	    struct pbuf *ps;
			char data[1024];
   if(p != NULL)
   {
	   if(p->tot_len<1024)
		{
			           // data = p->payload;	

						memcpy(data,p->payload,p->tot_len);
						 
						 pbuf_free(p); //��һʱ���ͷ�pbuf


		                if (strncmp(data, "getid", 5) == 0)
						{

						   uint32_t id;

						  UID_STM32[0]=0xAB;//mark

						  UID_STM32[1]=0x00;//����

						  id=(UID->UID0_31+UID->UID32_63+UID->UID64_95);//sum ID
						  
				 		  UID_STM32[2]=id;
						  UID_STM32[3]=id>>8;//
						  UID_STM32[4]=id>>16; // build ID 	lost The Hight 8bit data save the low 3 byte data as it ID

						  UID_STM32[5]=0xCD; //mark

						  UID_STM32[6]=0x24;
						  UID_STM32[7]=0x0d;  //efl
						  UID_STM32[8]=0x0a;

						  ps = pbuf_alloc(PBUF_TRANSPORT, 9, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&UID_STM32[0],9);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);

		                  }
						  else if (strncmp(data, "getrun", 6) == 0)
					     {
						  UID_STM32[0]=0xAA;
						  UID_STM32[4]=retry_TCP_connect;
						  UID_STM32[3]=retry_TCP_connect>>8;
						  UID_STM32[2]=retry_TCP_connect>>16;	  //��������
						  UID_STM32[1]=retry_TCP_connect>>24;

						  UID_STM32[8]=SystemRunTime;
						  UID_STM32[7]=SystemRunTime>>8;		  //����ʱ��
						  UID_STM32[6]=SystemRunTime>>16;
						  UID_STM32[5]=SystemRunTime>>24;
									
						  UID_STM32[9]=0xBB;
						  UID_STM32[10]=0x0d;
						  UID_STM32[11]=0x0a;
					      ps = pbuf_alloc(PBUF_TRANSPORT, 12, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&UID_STM32[0],12);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);


					     }
						 else if (strncmp(data, "getiap", 6) == 0)
						 {
						 	   //��ȡIAP��ǰ������
							char iap[10];
							  GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
							  iap[0] = 0xAB;
							  iap[1] = flash_net_buf[2]>>24;//�߰�λ
							  iap[2] = flash_net_buf[2]>>16;
							  iap[3] = flash_net_buf[2]>>8;
							  iap[4] = flash_net_buf[2]>>0;//�Ͱ�λ
							  iap[5]=0xCD; //mark
						      iap[6]=0x24;
						      iap[7]=0x0d;  //efl
						      iap[8]=0x0a;
						
						  ps = pbuf_alloc(PBUF_TRANSPORT, 10, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&iap[0],17);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);
						 }
						 else if (strncmp(data, "setiap", 6) == 0)
						 {

						 		 //�����ϵ�IAP��������							   
								 flash_net_buf[2]=0x55555555;
								WriteIpToFlash(flash_net_buf,3);
					
								ps = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_RAM);
								if(ps!=(0))
						       {
								memcpy((u8*)ps->payload,"OK\r\n",4);
								udp_sendto(upcb, ps,addr, port);
								}
								pbuf_free(ps);

						 
						 }
						  else if (strncmp(data, "setapp", 6) == 0)
						 {
						        //�����ϵ�APP��������
						 	   						   
								flash_net_buf[2]=0xAAAAAAAA;
								WriteIpToFlash(flash_net_buf,3);
					
								ps = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_RAM);
								if(ps!=(0))
						        {
								memcpy((u8*)ps->payload,"OK\r\n",4);
								udp_sendto(upcb, ps,addr, port);
								}
								pbuf_free(ps);
						 
						 }
						  else if ((strncmp(data, "setchl", 6) == 0))
							{
								/*����Ƶ��*/
								   flash_net_buf[3]=data[6];
								WriteIpToFlash(flash_net_buf,3);

							ps = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_RAM);
							  if(ps!=(0))
							  {
							  memcpy((u8*)ps->payload,"OK\r\n",4);
							  udp_sendto(upcb, ps,addr, port);
							  }
							  pbuf_free(ps);

							}
						 else if ((strncmp(data, "getchl", 6) == 0))
							{
							char str[11];
							  GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
							  str[0] = 0xAB;
							  str[1] = 'C';
							  str[2] = 'H';
							  str[3] = flash_net_buf[3]>>24;//�߰�λ
							  str[4] = flash_net_buf[3]>>16;
							  str[5] = flash_net_buf[3]>>8;
							  str[6] = flash_net_buf[3]>>0;//�Ͱ�λ
							  str[7]=0xCD; //mark
						      str[8]=0x24;
						      str[9]=0x0d;  //efl
						      str[10]=0x0a;
							   ps = pbuf_alloc(PBUF_TRANSPORT, 12, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&str[0],11);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);
						   	
							}
						 else if (strncmp(data, "getconfig", 9) == 0)
						 {
							  char str[17];
							  GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
							  str[0] = 0xAB;
							  str[1] = 'I';
							  str[2] = 'P';
							  str[3] = flash_net_buf[0]>>24;//�߰�λ
							  str[4] = flash_net_buf[0]>>16;
							  str[5] = flash_net_buf[0]>>8;
							  str[6] = flash_net_buf[0]>>0;//�Ͱ�λ
							  str[7] = 'P';
							  str[8] = 'O';
							  str[9] = 'R';
							  str[10] = 'T';
							  str[11] = flash_net_buf[1]>>8;//��8λ
							  str[12] = flash_net_buf[1];//��8λ
							  str[13]=0xCD; //mark
						      str[14]=0x24;
						      str[15]=0x0d;  //efl
						      str[16]=0x0a;
						
						   ps = pbuf_alloc(PBUF_TRANSPORT, 17, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&str[0],17);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);
						}
						else if ((strncmp(data, "setconfig", 9) == 0)&&
						    ((strncmp(&data[17], ":",1) == 0)))
		  				 {

								ipaddr_sever[0]=data[10];//ȡ�����IPλ
								ipaddr_sever[1]=data[12];
								ipaddr_sever[2]=data[14];
								ipaddr_sever[3]=data[16];

								port_sever     =data[18];
								port_sever<<=8;
								port_sever     +=data[19];


					
								flash_net_buf[0]=((uint32_t)(ipaddr_sever[0]<<24))+
								                 ((uint32_t)(ipaddr_sever[1]<<16))+
												 ((uint32_t)(ipaddr_sever[2]<<8))+
												 ((uint32_t)(ipaddr_sever[3]<<0));
						
								flash_net_buf[1]= port_sever;
								
								WriteIpToFlash(flash_net_buf,3);
					
								ps = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_RAM);
								if(ps!=(0))
						       {
								memcpy((u8*)ps->payload,"OK\r\n",4);
								udp_sendto(upcb, ps,addr, port);
								}
								pbuf_free(ps);

										//[setconfig.1.2.3.4:56] ascii �ַ�
										//1.2.3.4�ֱ��ʾIP��ַ��4���ֽڣ��ö��Ÿ�������λ��ǰ��λ�ں�
										//56����ʾ�˿ںţ���λ��ǰ��λ�ں��ã�����
										//73 65 74 63 6F 6E 66 69 67 2E 31 2E 32 2E 33 2E 34 3A 35 36  HEX��ʾ
							/*
										 ���ӣ�
										 ��Ҫ���÷�������IPΪ192.168.0.4���˿�2301��
										 ת��ʮ������Ϊ
										 IP��ַ192.168.0.4���˿�2301��-> HEX IP��ַ��0xc0,0xa8,0x00,0x04 �˿ڣ�0x08fd
										 ���Ҫ������������
										 {73 65 74 63 6F 6E 66 69 67 2E  C0  2E  A8  2E  00  2E  04  3A  08  FD } 
										 {[0][1][2][3][4][5][6][7][8][9][10][11][12][13][14][15][16][17][18][19]}

										 {73 65 74 63 6F 6E 66 69 67 2E  70  2E  4A  2E  8E  2E  E5  3A  21  CB }
										 
							*/
							
							
							
							

						   }
						    else
						   if (strncmp(data, "getver", 6) == 0)
						   {
								//��ȡ�汾��
						   unsigned char const DataStr[]=__DATE__; //12
						   unsigned char const TimeStr[]=__TIME__; //9
						   unsigned char vesion[40];
						   		
						   memcpy(&vesion[0],"version=",8);
							vesion[8]=' ';
							vesion[9]=MAIN_SN+0x30;//�����
							vesion[10]='.';
							vesion[11]=SECOND_SN+0x30;//�����
							vesion[12]='.';
							vesion[13]=SUB_SN+0x30;//�����
							vesion[14]=' ';
						   
						   memcpy(&vesion[15],DataStr,12);
						   memcpy(&vesion[27],TimeStr,9);
						   vesion[26]=' ';

					      ps = pbuf_alloc(PBUF_TRANSPORT, 40, PBUF_RAM);
						  if(ps!=(0))
						  {
						  memcpy((u8*)ps->payload,(u8*)&vesion[0],40);
						  udp_sendto(upcb, ps,addr, port);
						  }
						  pbuf_free(ps);

						   }
						   else
						   if (strncmp(data, "reboot", 6) == 0)
						   {
						   	   //ϵͳ����

								   #ifndef USED_WATCHDOG
								  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);//ʹ�ܿ��Ź�
								 /*
								    @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
							  *     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
							  *     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
							  *     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
								 */
								  WWDG_SetPrescaler(WWDG_Prescaler_1);//���ÿ��Ź�ʱ�ӷ�Ƶϵ��
								  WWDG_SetWindowValue(0x40);//���ô���ʱ��
								  WWDG_SetCounter(0x7F);//ι��
								  WWDG_Enable(0x7F);//�������Ź�
								  while(1);//WAITE :system will RESET
								  #else
								  while(1);//WAITE :system will RESET
						          #endif


						   
						   }



		
		}//if(p->tot_len<1024)
		else
		{
		 pbuf_free(p);
		}



	//���չ̶����ȵĹ㲥�����ݲ�ת���㲥��ȥ test data len >10

	/*
	 if(p->tot_len<10){
	    memcpy((u8*)&Udpbuf[0],(u8*)p->payload,p->tot_len);
		ps = pbuf_alloc(PBUF_TRANSPORT, p->tot_len, PBUF_RAM);
		memcpy((u8*)ps->payload,(u8*)&Udpbuf[0],p->tot_len);
		udp_sendto(upcb, ps,IP_ADDR_BROADCAST, 21223);
		     pbuf_free(ps);
	  				  }

	   pbuf_free(p); */

	   //pbuf_free(p);

   }//if(p != NULL)	  
	  
}

void udp_client_callback_app(void)
{
                          
   /* Create a new UDP control block  */
   upcb = udp_new();   
   upcb->so_options |= SOF_BROADCAST;
 
   /* Bind the upcb to any IP address and the UDP_PORT port*/
   udp_bind(upcb, IP_ADDR_ANY, 21228);  
   /* Set a receive callback for the upcb */
   udp_recv(upcb, udp_client_callback, NULL);

}
  char cansent=0;
  extern char* Getmeminf();
  extern uint32_t smtp_Tcp_count[10];
   char TcpRecvBuf[1500];
   uint32_t TcpRecvLenth=0;
   char Data[2]={0xFF,0xAA};
   char Data1[2]={0xFF,0xBB};
   char Statues_MB=0x00;
/*���������µ�Э������ 
Э��֡��ʽ
֡ͷ+����+������+\r\n
֡ͷ��
һ֡���غͷ����������Ŀ�ʼͬʱ�������ж������ϴ������·�������
��XFF+0X55������ʾ�����ϴ���������
��0XFF+0XAA��: ��ʾ�������·�������
���ͣ�
0x01:��ʾ������ʪ�ȴ�����
0x02��ʾ���մ�����
0x03 ��ʾPH ֵ������
0x04 ��ʾ��������������
������
��ͬ�����͵Ĵ�������������������ǳ����ṩ��MODBUS-RTU��Э��ֱ�Ӵ����ϡ�

 ���������ͣ�FF AA + ���� +��modbus RTU ������ ��
 ���ػظ�  ��FF 55 + ID + ���� +��modbus RTU ������ ��+55 FF +\r\n

*/



	uint8_t SeverIP[4] = {0};
	uint32_t SeverPORT = 0;


#ifdef   RS485_MASTER_CALL
extern uint8_t TCP2RS485CMDLIST[1400];	//�����������Է���������ѵ��������֡
extern uint32_t DevIndex; //���������ϱ��е��豸ָ�
#endif

  err_t  tcp_client_reciver(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{

				static char Tdata[1500]; //����̫����Ϊ��̬
					 int tcplen;
			
 if (err == ERR_OK && p != NULL)
  {

    /* Inform TCP that we have taken the data. */
    tcp_recved(tpcb, p->tot_len);
		// data = p->payload;
		tcplen=p->tot_len;	 
		 if(tcplen<1500) 
		 {
		memcpy(Tdata,p->payload,p->tot_len);//ץ�����Ƴ�������
		 }	    				 
	            pbuf_free(p);//��һʱ���ͷ�pbuf	
		

					if (strncmp(Tdata, Data, 2) == 0)
					 {
						 						 
							//char *ptr=(char*)p->payload;
							char *ptr=(char*)Tdata;
							  
								ptr+=3;	//jump FF AA and other there the ptr is point to MODBUS-RTU   
							 // if(p->tot_len<1500) //asseter the buffer lenth 
							 // {
	
							 TcpRecvLenth=tcplen-5-3;//upadta the recive data lenth	     AA FF  24 0D 0A =5 BYTES
							   
							 memcpy(TcpRecvBuf,ptr,TcpRecvLenth); // data copy to appliction data buffer 											 s
							   //}


				  	   }
					    else
					   #ifdef   RS485_MASTER_CALL
					   	if (strncmp(Tdata, Data1, 2) == 0)
						{

						  	char *ptr=(char*)Tdata;
							   
								ptr+=2;	//jump FF BB and other there the ptr is point to MODBUS-RTU   
							  //if(p->tot_len<1500) //asseter the buffer lenth 
							 // {
	
							 TcpRecvLenth=tcplen-5-2;//upadta the recive data lenth	     BB FF  24 0D 0A =5 BYTES
							 memset(TCP2RS485CMDLIST, 0, 1024);//�����BUFF 
							 memcpy(TCP2RS485CMDLIST,ptr,TcpRecvLenth); // data copy to appliction data buffer 											 s
							   	DevIndex=0;//������ÿ�θ��µ�ʱ�򶼱����ã���ͷ��ʼ
							   	TcpRecvLenth=0;//�����������ݲ���Ҫ��������485��modbus_main.c������Զ���������ֻҪ����������ȥ����

						    tcp_write(tpcb, "FFBBsave\r\n", sizeof( "FFBBsave\r\n"), 1);
						    tcp_output(tpcb);

							  // }
					   
					   }
					  else
					  #endif
					if (strncmp(Tdata, "mem", 3) == 0)
					 {
						   char *ptr;
						   uint16_t len;

		                 ptr=Getmeminf();
						 len=strlen(ptr);
						 tcp_write(tpcb, ptr, len, 1);
						 tcp_output(tpcb);
					  }
					  else
						if (strncmp(Tdata, "start", 5) == 0)
					 {
							cansent=1;
						 tcp_write(tpcb, "startsent\r\n", sizeof( "startsent\r\n"), 1);
						 tcp_output(tpcb);
					 }
					 else
					  	if (strncmp(Tdata, "stop", 4) == 0)
					 {
							cansent=0;
						 tcp_write(tpcb, "startsent\r\n", sizeof( "startsent\r\n"), 1);
						 tcp_output(tpcb);
					 }
					 else
		             if (strncmp(Tdata, "getrun", 6) == 0)
					 {
						  UID_STM32[0]=0xAA;
						  UID_STM32[4]=retry_TCP_connect;
						  UID_STM32[3]=retry_TCP_connect>>8;
						  UID_STM32[2]=retry_TCP_connect>>16;	  //��������
						  UID_STM32[1]=retry_TCP_connect>>24;

						  UID_STM32[8]=SystemRunTime;
						  UID_STM32[7]=SystemRunTime>>8;		  //����ʱ��
						  UID_STM32[6]=SystemRunTime>>16;
						  UID_STM32[5]=SystemRunTime>>24;
									
						  UID_STM32[9]=0xBB;
						  UID_STM32[10]=0x0d;
						  UID_STM32[11]=0x0a;
					 tcp_write(tpcb, &UID_STM32, 12, 1);
							 tcp_output(tpcb);
					 }
					 else
					 if (strncmp(Tdata, "getid", 5) == 0)
						{

						   uint32_t id;
						   id=(UID->UID0_31+UID->UID32_63+UID->UID64_95);//sum ID
						   //FFAA 01 01 000001 01 0001 000000(�豸id���ĸ���) 00��������00���棩 AAFF 240D0A 

						  UID_STM32[0]=0xFF;//mark
						  UID_STM32[1]=0xAA;

						  UID_STM32[2]=0x01;

						  UID_STM32[3]=0x01;

				 		  UID_STM32[4]=id;
						  UID_STM32[5]=id>>8;//
						  UID_STM32[6]=id>>16; // build ID 	lost The Hight 8bit data save the low 3 byte data as it ID

						  UID_STM32[7]=0x01;

						  UID_STM32[8]=0x00;
						  UID_STM32[9]=0x26; //485תTCP����

						  UID_STM32[10]=0;
						  UID_STM32[11]=0;//
						  UID_STM32[12]=0; // dev ID  =0x000000

						  UID_STM32[13]=0x00;
				
						  UID_STM32[14]=0xAA; //mark
						  UID_STM32[15]=0xFF; //mark

						  UID_STM32[16]=0x24;
						  UID_STM32[17]=0x0d;  //efl
						  UID_STM32[18]=0x0a;
							 if( tcp_write(tpcb, &UID_STM32, 19, 1)==ERR_OK)
							  {

							  tcp_output(tpcb);

							  }
							  else
							  {
							  
							  	   #ifndef USED_WATCHDOG
								  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);//ʹ�ܿ��Ź�
								 /*
								    @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
							  *     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
							  *     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
							  *     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
								 */
								  WWDG_SetPrescaler(WWDG_Prescaler_1);//���ÿ��Ź�ʱ�ӷ�Ƶϵ��
								  WWDG_SetWindowValue(0x40);//���ô���ʱ��
								  WWDG_SetCounter(0x7F);//ι��
								  WWDG_Enable(0x7F);//�������Ź�
								  while(1);//WAITE :system will RESET
								  #else
								  while(1);//WAITE :system will RESET
						          #endif
							  }


		                  }
						  else if ((strncmp(Tdata, "setchl", 6) == 0))
							{
								/*����Ƶ��*/
							   flash_net_buf[3]=Tdata[6];
							   	WriteIpToFlash(flash_net_buf,3);
							  	tcp_write(tpcb,"OK��\r\n",sizeof("OK��\r\n"), 1);
							 tcp_output(tpcb);
							}
						 else if ((strncmp(Tdata, "setconfig", 9) == 0)&&
						    ((strncmp(&Tdata[17], ":",1) == 0)))
		  				 {

								ipaddr_sever[0]=Tdata[10];//ȡ�����IPλ
								ipaddr_sever[1]=Tdata[12];
								ipaddr_sever[2]=Tdata[14];
								ipaddr_sever[3]=Tdata[16];

								port_sever     =Tdata[18];
								port_sever<<=8;
								port_sever     +=Tdata[19];


					
								flash_net_buf[0]=((uint32_t)(ipaddr_sever[0]<<24))+
								                 ((uint32_t)(ipaddr_sever[1]<<16))+
												 ((uint32_t)(ipaddr_sever[2]<<8))+
												 ((uint32_t)(ipaddr_sever[3]<<0));
						
								flash_net_buf[1]= port_sever;
								
								WriteIpToFlash(flash_net_buf,0);
					
						  	tcp_write(tpcb,"OK��\r\n",sizeof("OK��\r\n"), 1);
							 tcp_output(tpcb);
						
						}
						else if ((strncmp(Tdata, "getchl", 6) == 0))
							{
							char str[11];
							  GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
							  str[0] = 0xAB;
							  str[1] = 'C';
							  str[2] = 'H';
							  str[3] = flash_net_buf[3]>>24;//�߰�λ
							  str[4] = flash_net_buf[3]>>16;
							  str[5] = flash_net_buf[3]>>8;
							  str[6] = flash_net_buf[3]>>0;//�Ͱ�λ
							  str[7]=0xCD; //mark
						      str[8]=0x24;
						      str[9]=0x0d;  //efl
						      str[10]=0x0a;

						   	tcp_write(tpcb,str,11, 1);
							 tcp_output(tpcb);
							}
						else if (strncmp(Tdata, "getconfig", 9) == 0)
						  {
							  char str[17];
							  GetIpFromFlash(flash_net_buf);//��ȡ2���ֵ�buf��
							  str[0] = 0xAB;
							  str[1] = 'I';
							  str[2] = 'P';
							  str[3] = flash_net_buf[0]>>24;//�߰�λ
							  str[4] = flash_net_buf[0]>>16;
							  str[5] = flash_net_buf[0]>>8;
							  str[6] = flash_net_buf[0]>>0;//�Ͱ�λ
							  str[7] = 'P';
							  str[8] = 'O';
							  str[9] = 'R';
							  str[10] = 'T';
							  str[11] = flash_net_buf[1]>>8;//��8λ
							  str[12] = flash_net_buf[1];//��8λ
							  str[13]=0xCD; //mark
						      str[14]=0x24;
						      str[15]=0x0d;  //efl
						      str[16]=0x0a;

						   	tcp_write(tpcb,str,17, 1);
							 tcp_output(tpcb);

						  }	  
						else if (strncmp(Tdata, "reboot", 6) == 0)
						   {
						   	   //ϵͳ����


						  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);//ʹ�ܿ��Ź�
						 /*
						    @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
					  *     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
					  *     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
					  *     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
						 */
						  WWDG_SetPrescaler(WWDG_Prescaler_1);//���ÿ��Ź�ʱ�ӷ�Ƶϵ��
						  WWDG_SetWindowValue(0x40);//���ô���ʱ��
						  WWDG_SetCounter(0x7F);//ι��
						  WWDG_Enable(0x7F);//�������Ź�
						  while(1);//WAITE :system will RESET
						   }
						   else
						   {
						   
							char *ptr=(char*)Tdata;
							  	
							 TcpRecvLenth=tcplen;
							   
							 memcpy(TcpRecvBuf,ptr,TcpRecvLenth); // data copy to appliction data buffer 											 s
							 	   
						   }

						 
	 
   }


if ((err == ERR_OK && p == NULL)||(err<0))
  {
	Tcplink=1;
    tcp_close(wpcb);

  }		 
  
	return ERR_OK;
} 
 void tcp_client_err(void *arg, err_t err)
 {
	  if(err==ERR_RST) //������λ
	  {
		 //host well be free the PCB dont close PCB

	    Tcplink=1;
	  	return ;  //DO nothing..
	  }

 	  if(err==ERR_ABRT)
	  {
 	 Tcplink=1;
  // tcp_close(wpcb);
   	  	return ;  //DO nothing..
 	   }
	 
	 //   tcp_close(wpcb);
	   	 Tcplink=1;

	 
 }

void tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{


}

 extern  void ERR_Connect(void);//�ͷ�������tcppcbs 
 void  tcp_client_callback(void)
  {	

	  	struct ip_addr ip_addr;	

			wpcb= (struct tcp_pcb*)0;
			  /* Create a new TCP control block  */
			  wpcb = tcp_new();
			
			  /* Assign to the new pcb a local IP address and a port number */
			 if(tcp_bind(wpcb, IP_ADDR_ANY, TCP_PORT)!=ERR_OK)
			   {
			   		//failed

				
					 return ;
			   
			   }
			  //115.28.168.92 
			  //IP4_ADDR(&ip_addr, 192,168,0,163);
		      //IP4_ADDR(&ip_addr,114,215,155,179 );//���·�����
			  IP4_ADDR(&ip_addr,ipaddr_sever[0],ipaddr_sever[1],ipaddr_sever[2],ipaddr_sever[3]);//�����ͻ��ķ�������IP��ַ
		
			tcp_err( wpcb,tcp_client_err);  //register err

			tcp_recv(wpcb,tcp_client_reciver);  //register recv

		   //tcp_poll(wpcb,tcp_client_poll,2);

			/* Connect to the server: send the SYN *///TCP_PORT
		  if( tcp_connect(wpcb, &ip_addr, port_sever, tcp_client_connected)!=ERR_OK)
		  {

			  /*#20180903 ʹ��udp 21228 �˿ڿ�getrun,Ȼ�󲻶ϵ����������i�·���
			  ����������������20180903��debug�������⡣����Ϊconnct���ӷ���ʧ��
			  pcbû����ȷ�Ľ���tcp_active_pcbs�����С������ٴν���newD��ʱ��ռ�ڴ��ʧ��
			  ʵ�ʴ�ʱ��pcbפ����tcp_bound_pcbs�������˺���bindʧ�ܴӶ����ء�Ȼ���ٴν���TCP new
			  �ٴη���ʧ���ٴ�bindʧ�ܣ��ٴη��أ�һ�������ͳ���������*/
		  	   //if connect failed then we will free 
		  	    ERR_Connect();


		  
		  }

			
		

				 
}

/**
  * @brief  This function is called when the connection with the remote 
  *         server is established
  * @param arg user supplied argument
  * @param tpcb the tcp_pcb which received data
  * @param err error value returned by the tcp_connect 
  * @retval error value
  */

err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  
  	   Tcplink=0;
 	   retry_TCP_connect++;
	tpcb->so_options |= SOF_KEEPALIVE;
    tpcb->keep_idle = 1000;// ms
    tpcb->keep_intvl = 1000;// ms
    tpcb->keep_cnt = 2;// report error after 2 KA without response

    tcp_write(tpcb, "XWDZ202208011826", 16, 1);	//����ע��ID
	tcp_output(tpcb);
  
  return ERR_OK;
}


 
void TcpTestThread(uint32_t Time)
{

if (Time - TCPCreatTrm >= TCP_CREATTRM_INTERVAL)
  {
    TCPCreatTrm =  Time;
	 
	 if(Tcplink)//if link =1,will be connectting to sever ..
	 {
	
	  FS_Write(1,"TCP_Retry\r\n", sizeof("TCP_Retry\r\n"));
	  tcp_client_callback();
	 
	 }
	 if(Udplink==0)//just actived once 
	 {
	   Udplink=1; 
	   FS_Write(1,"UDP_Retry\r\n", sizeof("UDP_Retry\r\n"));
	   udp_client_callback_app();
	   

	 }
    
  }






}

void SynruntimHook(uint32_t Time)
{
 		 
 if ( Time  - RunSecTimer  >= SYSRUN_TIMER_MSECS)
  {
		RunSecTimer =  Time;

		SystemRunTime++;  //  ϵͳ����ʱ�䵥λ1sһ��
	  	
  }

}

uint8_t Mb2TcpBuff[256];
uint32_t Mb2TcpLenth=0;
char num=0,num1=0;
uint32_t TCPSentCreatTrm=0;
 void SendDataToSever(uint32_t Time)
 {			err_t erro;
	
  

 		 if((Tcplink==0)&&(Mb2TcpLenth>0)/*&&((Time - TCPSentCreatTrm >= TCPSent_CREATTRM_INTERVAL)*/)
		 {
		        //TCPSentCreatTrm =  Time;
		   //���ӳɹ������Է�������


				   erro= tcp_write(wpcb, Mb2TcpBuff, Mb2TcpLenth, 1);
					  Mb2TcpLenth=0; //д��tcpbuff���ͷŵ�������ȼ���
				 if(erro==ERR_OK )
			        {
				
				
				         if(tcp_output(wpcb)==ERR_OK)
						 {
						 
							//�ɹ��ķ�����һ��
						 }
						 else
						 {
						   //	Tcplink=1;
   							// tcp_close(wpcb);
						 }
					}
					else
					{
						  num++;
						  if(num>=50)
						  {	   num=0;
									  switch(erro)
									  {
								/*	  case -1:{FS_Write(1,"Out of memory error\r\n", sizeof("Out of memory error\r\n"));break;}
									  case -2:{FS_Write(1,"Buffer error", sizeof("Buffer error"));break;}
									  case -3:{FS_Write(1,"Timeout", sizeof("Timeout"));break;}
									  case -4:{FS_Write(1,"Routing problem", sizeof("Routing problem"));break;}
									  case -5:{FS_Write(1,"Operation in progress", sizeof("Operation in progress"));break;}
									  case -6:{FS_Write(1,"Illegal value", sizeof("Illegal value"));break;}
									  case -7:{FS_Write(1,"Operation would block", sizeof("Operation would block"));break;}
									  case -8:{FS_Write(1,"Address in use", sizeof("Address in use"));break;}
									  case -9:{FS_Write(1,"Already connected", sizeof("Already connected"));break;}
									  case -10:{FS_Write(1,"Connection aborted", sizeof("Connection aborted"));break;}
									  case -11:{FS_Write(1,"Connection reset", sizeof("Connection reset"));break;}
									  case -12:{FS_Write(1,"Connection closed", sizeof("Connection closed"));break;}
									  case -13:{FS_Write(1,"Not connected", sizeof("Not connected"));break;}
									  case -14:{FS_Write(1,"Illegal argument", sizeof("Illegal argument"));break;}
									  case -15:{FS_Write(1,"Low-level netif error", sizeof("Low-level netif error"));break;}
									   */
									  }
									  
									   if(erro!=(-1))
									   {
									 //  	Tcplink=1;
				  					 //  tcp_close(wpcb);
									   }

						}
					}

		 }
		 else
		 if(Mb2TcpLenth>0)
		 {
		  
			 Mb2TcpLenth=0;// if no TCP connection but we recived a farme of usart.	we just make the lenth to "0" other do nothing .

		 }
		 else
		 {
		 	 //none succefull connection and no usart data ��do nothing ...
		 
		 }
 
 
 }


  #ifdef USED_SI4432
extern uint8_t RX_Buf[64];
extern uint8_t RxSi4432Len;
char num2=0;
void Si4432SendDataToSever(uint32_t Time)
 {			err_t erro;
			uint8_t Rf2TcpBuff[100];
			uint16_t Rf2TcpLenth;

 		 if((Tcplink==0)&&(RxSi4432Len>0))
		 {
		        //TCPSentCreatTrm =  Time;
		   //���ӳɹ������Է�������
			Rf2TcpLenth=RxSi4432Len;//���½��ܵ������ݳ��ȣ�׼������TCP����
			 Rf2TcpBuff[0]=0xFF;
			 Rf2TcpBuff[1]=0xAA;          //֡ͷ
			
			 Rf2TcpBuff[2]=UID_STM32[2];         //�ϴ����ݵ���������ʾ
			
			 Rf2TcpBuff[3]=UID_STM32[3];         //��ϵͳId��ҵ����ϵͳ��ID�� ,z��ʱδ�ã�ֵĬ��Ϊ01����
			
			
			 Rf2TcpBuff[4]=UID_STM32[4];
			 Rf2TcpBuff[5]=UID_STM32[5];
			 Rf2TcpBuff[6]=UID_STM32[6]; //����ID(3���ֽڣ��豸���ò���)
			
			 Rf2TcpBuff[7]=0x02;         //�豸���ࣨ1���ֽ� 01���ء�02�豸��03ֱ�� д����
			
			
			 Rf2TcpBuff[8]=UID_STM32[8];					  
			 Rf2TcpBuff[9]=UID_STM32[9];         //�豸���ͣ�2���ֽڡ��豸���ò�����
			
			
			 Rf2TcpBuff[10]=0;
			 Rf2TcpBuff[11]=0;
			 Rf2TcpBuff[12]=0; //�豸id(3���ֽڣ��豸���ò���),
			
			memcpy(&Rf2TcpBuff[13],RX_Buf,Rf2TcpLenth);//rf���ݸ��ƽ�TCP����buf��
			
			
			 Rf2TcpBuff[Rf2TcpLenth+13+0]=0xAA;
			 Rf2TcpBuff[Rf2TcpLenth+13+1]=0xFF;	 //֡β
			
			 Rf2TcpBuff[Rf2TcpLenth+13+2]=0x24;	
			 Rf2TcpBuff[Rf2TcpLenth+13+3]=0x0D;
			 Rf2TcpBuff[Rf2TcpLenth+13+4]=0x0A;	//�����	
			  
			 Rf2TcpLenth+=18;//����֡�ܳ��ȣ�������Ϊ���ͱ�ʾ���͵�TCP buf
			

				   erro= tcp_write(wpcb, Rf2TcpBuff, Rf2TcpLenth, 1);
					  RxSi4432Len=0; //д��tcpbuff���ͷŵ�������ȼ���
				 if(erro==ERR_OK )
			        {
				
				
				         if(tcp_output(wpcb)==ERR_OK)
						 {
						 
							//�ɹ��ķ�����һ��
						 }
						 else
						 {
						   //	Tcplink=1;
   							// tcp_close(wpcb);
						 }
					}
					else
					{
						  num2++;
						  if(num2>=50)
						  {	   num2=0;
									  switch(erro)
									  {
								/*	  case -1:{FS_Write(1,"Out of memory error\r\n", sizeof("Out of memory error\r\n"));break;}
									  case -2:{FS_Write(1,"Buffer error", sizeof("Buffer error"));break;}
									  case -3:{FS_Write(1,"Timeout", sizeof("Timeout"));break;}
									  case -4:{FS_Write(1,"Routing problem", sizeof("Routing problem"));break;}
									  case -5:{FS_Write(1,"Operation in progress", sizeof("Operation in progress"));break;}
									  case -6:{FS_Write(1,"Illegal value", sizeof("Illegal value"));break;}
									  case -7:{FS_Write(1,"Operation would block", sizeof("Operation would block"));break;}
									  case -8:{FS_Write(1,"Address in use", sizeof("Address in use"));break;}
									  case -9:{FS_Write(1,"Already connected", sizeof("Already connected"));break;}
									  case -10:{FS_Write(1,"Connection aborted", sizeof("Connection aborted"));break;}
									  case -11:{FS_Write(1,"Connection reset", sizeof("Connection reset"));break;}
									  case -12:{FS_Write(1,"Connection closed", sizeof("Connection closed"));break;}
									  case -13:{FS_Write(1,"Not connected", sizeof("Not connected"));break;}
									  case -14:{FS_Write(1,"Illegal argument", sizeof("Illegal argument"));break;}
									  case -15:{FS_Write(1,"Low-level netif error", sizeof("Low-level netif error"));break;}
									   */
									  }
									  
									   if(erro!=(-1))
									   {
									 //  	Tcplink=1;
				  					 //  tcp_close(wpcb);
									   }

						}
					}

		 }
		 else
		 if(RxSi4432Len>0)
		 {
		  
			 RxSi4432Len=0;// if no TCP connection but we recived a farme of usart.	we just make the lenth to "0" other do nothing .

		 }
		 else
		 {
		 	 //none succefull connection and no usart data ��do nothing ...
		 
		 }
 
 
 }

 #endif

 #ifdef UDPBOARDCAST_ENABLE 
 extern  u8  temp[33]; //�¶��ַ���DS1820
  uint8_t Udpbf[1024]={0x55};
  uint32_t UDPSentCreatTrm=0;
 
 void UDPSendData(uint32_t Time)
 {
 
   	     struct pbuf *Udps;
		 ip_addr_t dst_ip_addr;

		 IP4_ADDR(&dst_ip_addr, 192,168,0,163);
		if((Udplink)&&(cansent)&&((Time - UDPSentCreatTrm >= UDPSent_CREATTRM_INTERVAL)))
		{ 
			   UDPSentCreatTrm=	Time ;
		Udps = pbuf_alloc(PBUF_TRANSPORT, 32, PBUF_RAM);
				if(Udps!=NULL)
				{
				   
							
				memcpy((u8*)Udps->payload,(u8*)&temp[0],32);
		
				udp_sendto(upcb,Udps,IP_ADDR_BROADCAST/*&dst_ip_addr*//* IP_ADDR*/,8080);
				
				 pbuf_free(Udps);
				}
				else
				{
					   num1++;
				   if(num1==50)
				   {	
				   num1=0;
			//	   UART_Write(UART0,"Oups is NULL\r\n", sizeof("Oups is NULL\r\n"));


				   }
				
				}

	 	}
 
 }


#endif

 err_t Tcp_recv(void *arg, struct tcp_pcb *pcb,  struct pbuf *p, err_t err)
{
	 if (err == ERR_OK && p != NULL)
  {
  



	   pbuf_free(p);

  }
  if ((err == ERR_OK && p == NULL)||(err<0))
  {
	 
	 tcp_close(pcb);
	
  }


}

static void conn_err(void *arg, err_t err)
{
 
}
err_t Tcp_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{ 

  pcb->so_options |= SOF_KEEPALIVE;
  pcb->keep_idle = 500;// ms
  pcb->keep_intvl = 500;// ms
  pcb->keep_cnt = 2;// report error after 2 KA without response
  //tcp_arg(pcb, pcb);
  tcp_recv(pcb, Tcp_recv);
  tcp_err(pcb, conn_err);
// tcp_poll(pcb, http_poll, 10);
  return ERR_OK;
}


 void tcp_sever_test(void)
{
  struct tcp_pcb *pcb;
  /*create new pcb*/
  pcb = tcp_new();
  
  if (!pcb)
  {
    return ;
  }
  /* bind PORT traffic to pcb */
  tcp_bind(pcb, IP_ADDR_ANY, 8080);
  /* start listening on port 80 */
  pcb = tcp_listen(pcb);
  /* define callback function for TCP connection setup */
  tcp_accept(pcb, Tcp_accept);
}



/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
