#include "config.h"
#include "UART.h"
#include "NVIC.h"
#include "Switch.h"
#include "../App/App.h"

#include "Light.h"
#include "Key.h"
#include "Buzzer.h"
#include "Motors.h"

/*******************************
App_Bluetooth.c��������ͨ��UART2����������
	- ����С���ƶ�������ҡ�ˣ�
	- ��������
	- �����ر�Ѳ��
	- ����������
	- ��ת/��ת
	
App_Tracker.c ����ѭ��ҵ��

���ճ������

App_Ultrasonic.c ѭ�����
	- �������������������������
	- �������ǰ���������20cm��ͣ��

App_Battery.c ѭ�����е�ص������
	- �����ͣ�������������������
	- ��������12V������Ӧ�˶�ָ��
	
********************************/

void GPIO_config(void) {

  // UART1
  GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//ָ��Ҫ��ʼ����IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);//��ʼ��
}

void UART_config(void) {
  // >>> �ǵ���� NVIC.c, UART.c, UART_Isr.c <<<
  COMx_InitDefine		COMx_InitStructure;					//�ṹ����
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
  UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������1 UART1,UART2,UART3,UART4

  NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
  UART1_SW(UART1_SW_P30_P31);		// ����ѡ��, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}



void Key_on_keyup() {

  Light_off(ALL); // ȫ��Ϩ��

//	Buzzer_close();
//	Motors_stop();
}


int counter = 0;

void Key_on_keydown() {
//	char rst;
  // ȫ������
  Light_on(ALL);

  // ������			-----------------------------------
//	Buzzer_beep();
//	Buzzer_open();
//	Buzzer_play(3, 100); // 3�Σ�ÿ�μ��100ms


  // 0ǰ����1���ˣ�2��ƽ�� 3��ƽ�ƣ�4���ͷ��5�ҵ�ͷ
//	switch(counter){
//		case 0:Motors_forward(30);break;
//		case 1:Motors_backward(30);break;
//		case 2:Motors_left(40, 0);break;
//		case 3:Motors_right(40, 0);break;
//		case 4:Motors_around(40, 0);break;
//		case 5:Motors_around(40, 1);break;
//		default: Motors_stop();break;
//	}
//	counter++;
//	counter %= 6; // 0,1,2,3,4,5
//
//	os_wait2(K_TMO, 200);
//	Motors_stop();


	// ����ʹ�ܣ���������˯���л���
	bt_enable();
	
}

void sys_init() {
  EAXSFR(); // ��չ�Ĵ���ʹ��
  GPIO_config();
  UART_config();
  EA = 1; // �ж��ܿ���

  // ��ʼ��Ӳ������
  Light_init();
  Buzzer_init();
  Motors_init();
}

void start_main() _task_ 0 {
  // ��ʼ������
  sys_init();

  // ������������
  os_create_task(TASK_KEY);
  os_create_task(TASK_UART1);
  os_create_task(TASK_BLUETOOTH);
  os_create_task(TASK_ULTRA);
//	os_create_task(TASK_BATTERY);
	
  // ���ٻ����Լ�
  os_delete_task(0);
}

void task1() _task_ TASK_KEY {
  Key_init();
  while(1) {
    Key_scan();
    os_wait2(K_TMO, 2);// 10ms = 5ms * 2
  }
}
// -100 -> 100   (int8)
//        ֡ͷ1 ֡ͷ2   0		 1		 2
// 				0xDD  0x77  ָ��	����1	����2
// ����		0xDD  0x77  0x01  0x03
// ������	0xDD  0x77  0x02  0x04  0x0A
// ǰ��		0xDD  0x77  0x03  0x64
// ����		0xDD  0x77  0x04 	0x32
// ��Ư		0xDD  0x77  0x05  0x32
// ��Ư		0xDD  0x77  0x06  0x32
// ֹͣ		0xDD  0x77  0x07 
// ǰ��  	0xDD  0x77  0x13  0x1F 				����ָ���ٶ� [1]:���µ��ٶ�ֵ
int forward_speed = 30;
void on_uart1_recv(void){
	// ���ղ�������λ���������ź�
	static u8 light_state = 0; // 0��1��
	
	
	if (RX1_Buffer[0] == 0x01){
		if (light_state == 0){
			Light_on(ALL);
		}else {
			Light_off(ALL);
		}
		// 0 -> 1
		light_state = !light_state;
	}else if (RX1_Buffer[0] == 0x02){
		Buzzer_play(3, 200);
	}
	
	switch (RX1_Buffer[0]) // �ж�ָ��
  {
  	case 0x03:  // ǰ��
			Motors_forward(forward_speed);
  		break;
  	case 0x07:	// ֹͣ
			Motors_stop();
  		break;
  	case 0x13:	// ����ָ���ٶ�ǰ��
			forward_speed = (int)RX1_Buffer[1];
			Motors_forward(forward_speed);
  		break;
  	default:
  		break;
  }
}

// 1. UART1�������񣺰�UART1��PC�յ�������ת����UART2����ģ��
void task_uart1() _task_ TASK_UART1 {
  u8 i;

  while(1) {
    if(COM1.RX_TimeOut > 0) {
      //��ʱ����
      if(--COM1.RX_TimeOut == 0) {
        if(COM1.RX_Cnt > 0) {
					
					on_uart1_recv();
          // ���ﴦ���յ������ݣ���������߼������Ե����Լ��� on_uart1_recv
          for(i=0; i<COM1.RX_Cnt; i++)	{
            // RX1_Buffer[i]����ǽ��յ�ÿ���ֽڣ�д���� TX1_write2buff
//            TX1_write2buff(RX1_Buffer[i]); // ����
						TX2_write2buff(RX1_Buffer[i]); // ����UART2
          }
        }
        COM1.RX_Cnt = 0;
      }
    }
    // ��Ҫ�����̫��
    os_wait2(K_TMO, 2); // 10ms
  }
}

