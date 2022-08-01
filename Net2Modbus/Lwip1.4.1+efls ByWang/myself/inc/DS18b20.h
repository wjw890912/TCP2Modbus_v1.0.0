#ifndef DS18B20_H_  
#define DS18B20_H_  
  
//DS18B20ָ��  
typedef enum   
{  
    SEARCH_ROM          =   0xf0,   //����ROMָ��  
    READ_ROM            =   0x33,   //��ȡROMָ��  
    MATH_ROM            =   0x55,   //ƥ��ROMָ��  
    SKIP_ROM            =   0xcc,   //����ROMָ��  
    ALARM_SEARCH        =   0xec,   //��������ָ��  
    CONVERT_T           =   0x44,   //�¶�ת��ָ��  
    WRITE_SCRATCHPAD    =   0x4e,   //д�ݴ���ָ��  
    READ_SCRATCHPAD     =   0xbe,   //��ȡ�ݴ���ָ��  
    COPY_SCRATCHPAD     =   0x48,   //�����ݴ���ָ��  
    RECALL_E2           =   0xb8,   //�ٻ�EEPROMָ��  
    READ_POWER_SUPPLY   =   0xb4,   //��ȡ��Դģʽָ��  
} DS18B20_CMD;  
  
  
  
//DS18B20 ROM����  
typedef struct  
{  
    u8  DS18B20_CODE;   //DS18B20�����߱���:0x19  
    u8  SN_1;           //���кŵ�1�ֽ�  
    u8  SN_2;           //���кŵ�2�ֽ�  
    u8  SN_3;           //���кŵ�3�ֽ�  
    u8  SN_4;           //���кŵ�4�ֽ�  
    u8  SN_5;           //���кŵ�5�ֽ�  
    u8  SN_6;           //���кŵ�6�ֽ�  
    u8  crc8;           //CRC8У����     
} DS18B20_ROM_CODE;   
  
  
  
#define DS18B20_Init()      (DeviceClockEnable(DEV_GPIOB,ENABLE))       //ʹ��GPIOBʱ��  
  
s16 DS18B20_ReadTemper(void);   //��ȡDS18B20�¶�  
void DS18B20_WriteData(u8 data);  
  
u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num);  //����ROM;  
s16 DS18B20_ReadDesignateTemper(u8 pID[8]); //��ȡָ��ID��DS18B20�¶�  
void Init_DS18B20_IO(void);
void Delay_init(uint8_t SYSCLK);
void Delay_us(uint32_t Nus);
void Delay_ms(uint16_t nms);
void TemperatureThread(uint32_t Time);
  
  
  
#endif /*DS18B20_H_*/  
