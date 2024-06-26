#include "Buzzer.h"
#include "GPIO.h"
#include "STC8H_PWM.h"
#include "Switch.h"
#include "NVIC.h"
#include "delay.h"


#define BUZZER								P34
#define BUZZER_GPIO_INIT()		P3_MODE_OUT_PP(GPIO_Pin_4)

#define BUZZER_HZ			1000

#define PERIOD 				((MAIN_Fosc / BUZZER_HZ) - 1)
PWMx_Duty dutyB;
void	PWM_config(u8 enable)
{
	PWMx_InitDefine		PWMx_InitStructure;

	// 配置PWM8
	PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
	PWMx_InitStructure.PWM_Duty    		= PERIOD * 0.5f;	//PWM占空比时间, 0~Period
	PWMx_InitStructure.PWM_EnoSelect  = enable ? ENO8P : 0;			//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
	PWM_Configuration(PWM8, &PWMx_InitStructure);			//初始化PWM,  PWMA,PWMB

	// 配置PWMB
	PWMx_InitStructure.PWM_Period   = PERIOD;			//周期时间,   0~65535
	PWMx_InitStructure.PWM_DeadTime = 0;					//死区发生器设置, 0~255
	PWMx_InitStructure.PWM_MainOutEnable= ENABLE;			//主输出使能, ENABLE,DISABLE
	PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;			//使能计数器, ENABLE,DISABLE
	PWM_Configuration(PWMB, &PWMx_InitStructure);			//初始化PWM通用寄存器,  PWMA,PWMB

	// 切换PWM通道
	PWM8_SW(PWM8_SW_P34);					//PWM8_SW_P23,PWM8_SW_P34,PWM8_SW_P03,PWM8_SW_P77

	// 初始化PWMB的中断
	NVIC_PWM_Init(PWMB,DISABLE,Priority_0);
}

void Buzzer_init(){
	BUZZER_GPIO_INIT();
	
	PWM_config(DISABLE);
	
	BUZZER = 0;
}

// 按照指定频率震荡
void Buzzer_open(){
//	BUZZER = 1;
	PWM_config(ENABLE);
}

// 关闭震荡
void Buzzer_close(){
//	BUZZER = 0;
	PWM_config(DISABLE);
}

// 响count次, 按照指定间隔时间gap ms . gap < 1275
void Buzzer_play(u8 count, u16 gap){ 
	u8 i;
	for (i = 0; i < count; i++){
		Buzzer_open();
//		delay_ms(100);
		os_wait2(K_TMO, 20);// 20 * 5ms = 100ms
		
		Buzzer_close();
//		delay_ms(gap);
		os_wait2(K_TMO, gap / 5);// 100ms / 5ms = N
	}
}

void Delay100us(void)	//@24.000MHz
{
	unsigned char data i, j;

	i = 4;
	j = 27;
	do
	{
		while (--j);
	} while (--i);
}

// 响一小段时间 100ms
void Buzzer_beep(){
	int i;
	for(i = 0; i < 1000; i++){
		BUZZER = !BUZZER;
		//  1ms -> 500Hz
		//0.1ms -> 5000Hz
		Delay100us();
	}
	// 1000 * 0.1ms = 100ms
}