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
App_Bluetooth.c接收蓝牙通过UART2传来的数据
	- 控制小车移动（根据摇杆）
	- 点亮车灯
	- 开启关闭巡线
	- 蜂鸣器响起
	- 左转/右转
	
App_Tracker.c 进行循线业务

解决粘包问题

App_Ultrasonic.c 循环测距
	- 距离近：蜂鸣器响起，两声短响
	- 距离近：前方距离低于20cm，停车

App_Battery.c 循环进行电池电量检测
	- 电量低：蜂鸣器响起，三声短响
	- 电量低于12V：不响应运动指令
	
********************************/

void GPIO_config(void) {

  // UART1
  GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P3, &GPIO_InitStructure);//初始化
}

void UART_config(void) {
  // >>> 记得添加 NVIC.c, UART.c, UART_Isr.c <<<
  COMx_InitDefine		COMx_InitStructure;					//结构定义
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
  UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4

  NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
  UART1_SW(UART1_SW_P30_P31);		// 引脚选择, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}



void Key_on_keyup() {

  Light_off(ALL); // 全部熄灭

//	Buzzer_close();
//	Motors_stop();
}


int counter = 0;

void Key_on_keydown() {
//	char rst;
  // 全部点亮
  Light_on(ALL);

  // 蜂鸣器			-----------------------------------
//	Buzzer_beep();
//	Buzzer_open();
//	Buzzer_play(3, 100); // 3次，每次间隔100ms


  // 0前进，1后退，2左平移 3右平移，4左掉头，5右掉头
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


	// 蓝牙使能，让蓝牙从睡眠中唤醒
	bt_enable();
	
}

void sys_init() {
  EAXSFR(); // 扩展寄存器使能
  GPIO_config();
  UART_config();
  EA = 1; // 中断总开关

  // 初始化硬件驱动
  Light_init();
  Buzzer_init();
  Motors_init();
}

void start_main() _task_ 0 {
  // 初始化外设
  sys_init();

  // 创建其他任务
  os_create_task(TASK_KEY);
  os_create_task(TASK_UART1);
  os_create_task(TASK_BLUETOOTH);
  os_create_task(TASK_ULTRA);
//	os_create_task(TASK_BATTERY);
	
  // 销毁回收自己
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
//        帧头1 帧头2   0		 1		 2
// 				0xDD  0x77  指令	参数1	参数2
// 车灯		0xDD  0x77  0x01  0x03
// 蜂鸣器	0xDD  0x77  0x02  0x04  0x0A
// 前进		0xDD  0x77  0x03  0x64
// 后退		0xDD  0x77  0x04 	0x32
// 左漂		0xDD  0x77  0x05  0x32
// 右漂		0xDD  0x77  0x06  0x32
// 停止		0xDD  0x77  0x07 
// 前进  	0xDD  0x77  0x13  0x1F 				按照指定速度 [1]:最新的速度值
int forward_speed = 30;
void on_uart1_recv(void){
	// 接收并处理上位机发来的信号
	static u8 light_state = 0; // 0灭，1亮
	
	
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
	
	switch (RX1_Buffer[0]) // 判断指令
  {
  	case 0x03:  // 前进
			Motors_forward(forward_speed);
  		break;
  	case 0x07:	// 停止
			Motors_stop();
  		break;
  	case 0x13:	// 按照指定速度前进
			forward_speed = (int)RX1_Buffer[1];
			Motors_forward(forward_speed);
  		break;
  	default:
  		break;
  }
}

// 1. UART1接收任务：把UART1从PC收到的数据转发给UART2蓝牙模块
void task_uart1() _task_ TASK_UART1 {
  u8 i;

  while(1) {
    if(COM1.RX_TimeOut > 0) {
      //超时计数
      if(--COM1.RX_TimeOut == 0) {
        if(COM1.RX_Cnt > 0) {
					
					on_uart1_recv();
          // 这里处理收到的数据，做具体的逻辑，可以调用自己的 on_uart1_recv
          for(i=0; i<COM1.RX_Cnt; i++)	{
            // RX1_Buffer[i]存的是接收的每个字节，写出用 TX1_write2buff
//            TX1_write2buff(RX1_Buffer[i]); // 回显
						TX2_write2buff(RX1_Buffer[i]); // 发给UART2
          }
        }
        COM1.RX_Cnt = 0;
      }
    }
    // 不要处理的太快
    os_wait2(K_TMO, 2); // 10ms
  }
}

