#ifndef __SI4432PROTO_H__
#define __SI4432PROTO_H__




/*���߿�����ص��豸����*/
typedef enum
{
	SWITCH_86 = 0x01,         /*86�п���*/
	SWITCH_LAMP,              /*��ͷ����*/
	WINDOW_CURTAINS,          /*����*/
	TEMPHUMI_SENSOR,          /*��ʪ�ȴ�����*/
	SWITCH,             /*STM32������*/
	CTRLSIG_315,              /*315�����ź�*/
	INFRAREDTV_CTRL,          /*�������ң���ն�*/
	
	DEVTYPE_MAX               /*���ֵ*/
	
}eDevType_SI4432Ctrl;


/*ͨ����*/
typedef enum
{
	CHAN_1 = 0x01,            /*ͨ��1*/
	CHAN_2,                   /*ͨ��2*/
	CHAN_3,                   /*ͨ��3*/
	CHAN_ALL,                 /*����ͨ��*/
	CHAN_INVALID              /*ͨ����Ч*/
	
}eChan_Num;


/*��������*/

/*�ƾ���*/
#define LIGHTCTRL_OPEN          0x01          /*����*/
#define LIGHTCTRL_CLOSE         0x00          /*�ص�*/
#define LIGHTCTRL_MODULATE      0x03          /*����*/
#define LIGHTCTRL_REPORT        0x04          /*��״̬����*/
#define LIGHTCTRL_LEARN_REQ     0x05          /*���豸IDѧϰ����*/
#define LIGHTCTRL_LEARN_OK      0x06          /*���豸IDѧϰ�ظ�*///���豸����

/*������*/
#define WINDOWCTRL_T_OPEN       0x11          /*ˮƽ������ȫ����*/
#define WINDOWCTRL_T_CLOSE      0x12          /*ˮƽ�ش���ȫ�أ�*/
#define WINDOWCTRL_T_MODULATE   0x13          /*����ˮƽ�����������������ݴ��ص�������*/
#define WINDOWCTRL_T_REPORT     0x14          /*����ˮƽ���ӵ�״̬+���ȣ�����������������磺ȫ���ǰٷ�֮�٣�ȫ���ǰٷ�֮0���Դ����ƣ���*/
#define WINDOWCTRL_L_OPEN       0x15          /*������������*/
#define WINDOWCTRL_L_CLOSE      0x16          /*�½������ף�*/
#define WINDOWCTRL_L_MODULATE   0x17          /*�������¿����������������ݴ��ص�������*/
#define WINDOWCTRL_L_REPORT     0x18          /*���ش�ֱ���ӵ�״̬+���ȣ�����������������磺ȫ���ǰٷ�֮�٣�ȫ���ǰٷ�֮0���Դ����ƣ���*/

/*����ң����*/
#define INFRARED_BUF_READ       0x21          /*��ȡ�����豸�Ļ����������+�������ڣ����ݹ�10���ֽڣ�פ����������*/
#define INFRARED_BUF_REPORT     0x22          /*�ظ������豸�Ļ����������+�������ڣ����ݹ�10���ֽ�פ����������*/
#define INFRARED_CODE_SEND      0x23          /*���ͺ���ң�ر��루������������������أ�*/
#define INFRARED_CODE_REPORT    0x24          /*���غ���ң�ؿ����״̬���ɹ�����ʧ�ܣ����ص�������*/

/*��ʪ�Ȳɼ���*/
#define TEMPERATURE_REQUEST     0x31          /*Ҫ�������¶����ݣ����ݴ��ص�������*/
#define HUMIDITY_REQUEST        0x32          /*Ҫ������ʪ�����ݣ����ݴ��ص�������*/
#define TEMPERATURE_REPORT      0x33          /*�����豸�ɼ������¶����ݣ����ݴ��ص�������*/
#define HUMIDITY_REPORT         0x34          /*�����豸�ɼ�����ʪ�����ݣ����ݴ��ص�������*/

/*315������*/


/*�������� END*/



  struct switch_struct
 {
 
   unsigned char Channel_number; //ͨ����
   unsigned char state;  //��״̬
    unsigned char  Spare;  //����
 
 };

struct Protocol_Structure
{

   unsigned int dst;  //Ŀ���ַ
   unsigned int  src;  //Դ��ַ
   unsigned char   dev_type; //�豸����
  struct switch_struct s;

};

/*��������Э��֡����*/
unsigned char RecvCmdFromSI4432(unsigned char *pBuf,unsigned char leg);
void Light_State_Retrn (struct Protocol_Structure *pFram,eChan_Num Channel, unsigned char  state);
unsigned char *Package_Data_Stream(struct Protocol_Structure*pbuf,unsigned char *buf);

#endif



