#include "App.h"
#include "GPIO.h"
#include "UART.h"
#include "NVIC.h"
#include "Switch.h"
#include "Buzzer.h"
#include "Light.h"
#include "Motors.h"

#define BT_EN	P40
void bt_enable(){
	// 蓝牙使能 （产生下降沿即可唤醒蓝牙模块）
	BT_EN = 1;
	os_wait2(K_TMO, 1);
	BT_EN = 0;
}

static void GPIO_config(){
  GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义

  // UART2
  GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P1, &GPIO_InitStructure);//初始化

  // BT_EN
  GPIO_InitStructure.Pin  = GPIO_Pin_0;		//指定要初始化的IO,
  GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
  GPIO_Inilize(GPIO_P4, &GPIO_InitStructure);//初始化
}

void UART2_config(void) {
  COMx_InitDefine		COMx_InitStructure;					//结构定义
  COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
  COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
  COMx_InitStructure.UART_BaudRate  = 9600ul;			//波特率, 一般 110 ~ 115200
  COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
  COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
  UART_Configuration(UART2, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4

  NVIC_UART2_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
  UART2_SW(UART2_SW_P10_P11);		// 引脚选择, UART2_SW_P10_P11,UART2_SW_P46_P47
}
#define BUTTON_A 	4
#define BUTTON_B 	5
#define BUTTON_C 	6
#define BUTTON_D 	7

#define IS_PRESSED(btn) (RX2_Buffer[btn] == 0x01)
/************************
 0	1	 2  3  4  5  6  7  数组索引
 帧头  x  y  A  B  C  D
DD 77 EF 50 01 00 01 00  数据示例

帧头 01：DD 77
方向 23：EF 50
按钮 4567: ABCD  按下01 抬起00

A: 蜂鸣器/车灯
B: 左旋转: 按下开始转,抬起停止转
C: 右旋转: 按下开始转,抬起停止转
D: 开启/关闭巡线
**************************/
u8 is_turning 	= FALSE; 	// 是否正在掉头旋转
u8 is_tracking 	= FALSE; 	// 是否正在巡线
void do_control_action(u8* array, u8 len){
	static char x, y;
	u8 i;
	if(len < 8){
		return;
	}
	// 根据收到的蓝牙数据（字节数组），对小车进行控制
//	printf("data: %02x\n", (int)array[4]);
	printf("-> ");
	for(i = 0; i < 8; i++){
		printf("%02X ", (int)array[i]);
	}
	printf("\n");
	
	if(array[0] != 0xDD){
		return;
	}
	if(array[1] != 0x77){
		return;
	}
	
	// ----------------------------------------- 按钮A判定
	if(array[BUTTON_A] == 0x01){
		Light_on(ALL);
		Buzzer_play(2, 200);
		Light_off(ALL);
		
		// 开启关闭障碍物判定逻辑（超声波模块）
		is_ultrasonic_enable = !is_ultrasonic_enable;
		return;
	}
	
	// 低电量禁止以下轮子运动
	if(isLowPower()){
		Motors_stop();
		return;
	}
	
	// ----------------------------------------- 按钮BC旋转判定
	if(array[BUTTON_B] == 0x01){// 左旋转
		Motors_around(25, 0);
		is_turning = TRUE;
		return;
	}else if(array[BUTTON_C] == 0x01){ // 右旋转
		Motors_around(25, 1);
		is_turning = TRUE;
		return;
	}else if (is_turning){	// 停止旋转
		is_turning = FALSE;
		Motors_stop();
		return;
	}
	
	// -------------------------------------------按钮D 开启关闭巡线
	if(array[BUTTON_D] == 0x01){
		if(is_tracking) {
			// 第二次按下：停止巡线
			Motors_stop();
			os_delete_task(TASK_TRACK);
			printf("结束巡线\n");
		} else {
			// 第一次按下：巡线
			os_create_task(TASK_TRACK);
			printf("开始巡线\n");
		}
		is_tracking = !is_tracking;
		return;
	}
	// -------------------------------------------- 摇杆控制移动
	x = array[2];
	y = array[3];
	
	printf("x->%d,\ty->%d\r\n", (int)x, (int)y);
	
	Motors_move((int)x, (int)y);
}

void do_control(){
	u8 i;
	u8 total = COM2.RX_Cnt;
	// 如果COM2.RX_Cnt == 8
	// 如果COM2.RX_Cnt == 16
	
	// DD 77 EF 50 01 00 01 00 DD 77 00 00 01 00 01 00
	// DD 77 EF 50 01 00 01 00 DD 77 00 00  ==> 8 + 4 = 12
	// DD 77 EF 50 01 00 01 00 DD 77 00 00 01 00 01 00 DD 77 3A 4B
	
	for(i = 0; i < COM2.RX_Cnt - 1; i++){
		// 找连续出现的 DD 77， 截取后边的6个字节，一共8个字节
		if(RX2_Buffer[i] == 0xDD && RX2_Buffer[i+1] == 0x77){
			
			// i==8, i+1==9 处理末尾数据不足情况（可以不考虑）
			if(COM2.RX_Cnt - i < 8){ // 12 - 8 == 4
				// 剩余数量 < 8直接结束循环
				break;
			}
			
			// 从DD开始，取出8个字节
			do_control_action(&RX2_Buffer[i], 8);
			i += 7;
		}
	}
}

// 2. UART2接收任务：把UART2从蓝牙收到的数据转发给UART1电脑PC
void task_bluetooth() _task_ TASK_BLUETOOTH {
	u8 i;
	GPIO_config();
	UART2_config();
	
  while(1) {
    if(COM2.RX_TimeOut > 0) {
      //超时计数
      if(--COM2.RX_TimeOut == 0) {
        if(COM2.RX_Cnt > 0) {
					
					// 在此函数内部，对收到的数据进行分包处理
					do_control();
					
          for(i=0; i<COM2.RX_Cnt; i++)	{
            // RX2_Buffer[i]存的是接收的数据，写出用 TX2_write2buff
            // TODO: 做具体的逻辑 on_uart2_recv
						TX1_write2buff(RX2_Buffer[i]); // 转发给PC
          }
        }
        COM2.RX_Cnt = 0;
      }
    }
		
    // 不要处理的太快
    os_wait2(K_TMO, 2); // 10ms
  }
}