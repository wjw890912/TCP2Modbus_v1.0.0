#include "stm32f10x_it.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#define CLR_DIR_1WIRE                      
 // ��������Ϊ�����  
#define SET_DIR_1WIRE   
 // ��������Ϊ�����  
#define CLR_OP_1WIRE   GPIO_ResetBits(GPIOE, GPIO_Pin_3)       // ������0 
#define SET_OP_1WIRE   GPIO_SetBits(GPIOE, GPIO_Pin_3)         // ������1    
#define CHECK_IP_1WIRE GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)// �������״̬  

unsigned int ms_count; 
unsigned char wmh,wml;  
unsigned char Integer,Decimals;
unsigned char Uid[6];//DS18B20��ID��
float k;
static uint8_t  fac_us=0;//us��ʱ������
static uint16_t fac_ms=0;//ms��ʱ������


void Init_DS18B20_IO(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
  /* Configure PE3 pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

} 

 	
void Set18b20IOin(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
  /* Configure PE3 pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}  
 void Set18b20IOout(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
  /* Configure PE3 pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}  

          
//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864 
 
//��ʼ���ӳٺ���
void Delay_init(uint8_t SYSCLK)
{
 SysTick->CTRL&=0xfffffffb;//ѡ���ڲ�ʱ�� HCLK/8
 fac_us=SYSCLK/8;      
 fac_ms=(u16)fac_us*1000;
} 

  void Delay_us(uint32_t Nus)
  {
  SysTick->LOAD=Nus*fac_us;       //ʱ�����      
 SysTick->CTRL|=0x01;            //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));//�ȴ�ʱ�䵽�� 
 SysTick->CTRL=0X00000000;       //�رռ�����
 SysTick->VAL=0X00000000;        //��ռ�����   
  
  }   

  void Delay_ms(uint16_t nms)
  {
  
   SysTick->LOAD=(u32)nms*fac_ms; //ʱ�����  
 SysTick->CTRL|=0x01;               //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));   //�ȴ�ʱ�䵽�� 
 SysTick->CTRL&=0XFFFFFFFE;         //�رռ�����
 SysTick->VAL=0X00000000;           //��ռ����� 
  
  }

void Init_18b20()   
{   
    Init_DS18B20_IO();
    Delay_init(72);//72M
    SET_DIR_1WIRE;     // ��������Ϊ���  
    SET_OP_1WIRE;    
    CLR_OP_1WIRE;   
    Delay_us(250);      
    Delay_us(250);     // �ȴ�500us 
     
    SET_OP_1WIRE;   
    CLR_DIR_1WIRE;   
    Delay_us(20);      // 15~60us   
    while(CHECK_IP_1WIRE);   
    SET_DIR_1WIRE;   
    SET_OP_1WIRE;   
    Delay_us(140);     // 60~240us   
}  

void Write_18b20(unsigned char x)   
{      
    unsigned char m;   
    for(m=0;m<8;m++)   
    {   
       CLR_OP_1WIRE;   
       if(x&(1<<m))    // ��д��λ����  
       SET_OP_1WIRE;   
       else   
       {CLR_OP_1WIRE;}   
       Delay_us(40);   // 15~60us   
       SET_OP_1WIRE;   
    }   
}   
unsigned char Read_18b20()   
{       
    unsigned char temp,k,n;   
    temp=0;   
    for(n=0;n<8;n++)   
    {   
        CLR_OP_1WIRE;   
        SET_OP_1WIRE;   
        CLR_DIR_1WIRE;   
        k=(CHECK_IP_1WIRE);    // �ӵ�λ��ʼ������   
        if(k)   
            temp|=(1<<n);   
        else   
            temp&=~(1<<n);   
        Delay_us(50);          // 60~120us       
        SET_DIR_1WIRE;   
   }   
   return (temp);   
}    
void Gettemp(void)                  
{   
     unsigned char tem_h,tem_l;//
     unsigned char Familycode;//family code 	 
     unsigned char Crc;	//crc code
    Init_18b20();        // ��λ18b20 
	
Write_18b20(0x33);   // ��ȡROM����   
 	Familycode=Read_18b20();//read family code always 28H

	for(Crc=0;Crc<6;Crc++)
	{
	   Uid[Crc]=Read_18b20();//��ȡ48bit id
	}
	 Crc=Read_18b20();//��ȡCRC	
  //Write_18b20(0xcc);   // ����ROM����  plese read datasheet   
    Write_18b20(0x44);   // �����¶�ת�� 
    Init_18b20();
    Write_18b20(0xcc);   // ��������ROM���� 
    Write_18b20(0xbe);   // ���Ͷ��������� 
    tem_l=Read_18b20();   // ���¶ȵ��ֽ�   
    tem_h=Read_18b20();   // ���¶ȸ��ֽ� 



	Integer = ((tem_h<<4)&0xf0)+((tem_l>>4)&0x0f);
	  k=  Integer+ (tem_l&0x0f)*0.0625;

	 Integer=k;
    /*WenDu=tem_h; 
    WenDu<<=8; 
    WenDu+=tem_l; 
    WenDu=6.25*WenDu; */
} 







