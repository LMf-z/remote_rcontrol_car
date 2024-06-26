#include "Motors.h"
#include	"STC8H_PWM.h"
#include	"NVIC.h"
#include	"Switch.h"
#include "GPIO.h"

typedef struct {

	// -100 --------------- 0 -------------- 100
	int LQ_speed;
	int RQ_speed;
	int LH_speed;
	int RH_speed;
	
}MotorsCfg;

/************
[-100 --------------- 0 -------------- 100]
将范围在[-100,100]速度值转成对应占空比，比值
 100  ---------------50 --------------- 0  duty

*************/
int speed2duty(int speed){
	// speed < 0 后退 eg. -60
	// speed > 0 前进 eg. 50
	// speed ==0 停止
	
	// B_Max               stop             F_Max
	// -100 --------------- 0 -------------- 100 speed
	
	// [-100, 100] / 2 ==> [-50, 50]
	// [-50, 50] * -1  ==> [50, -50]
	// [50, -50] + 50  ==> [100, 0]
	
	// 100  ---------------50 --------------- 0  duty
	
	return -(speed / 2) + 50;
}

#define PERIOD ((MAIN_Fosc / 1000) - 1)
PWMx_Duty dutyA;
void	PWM_config(MotorsCfg cfg)
{
  PWMx_InitDefine		PWMx_InitStructure;
	
//	cfg.LQ_speed = CLIP(cfg.LQ_speed, -100, 100); 
//	cfg.RQ_speed = CLIP(cfg.RQ_speed, -100, 100); 
//	cfg.LH_speed = CLIP(cfg.LH_speed, -100, 100); 
//	cfg.RH_speed = CLIP(cfg.RH_speed, -100, 100); 
	
	// 将传过来的四个轮子的速度转成duty占空比
	int LQ_duty = speed2duty(cfg.LQ_speed);
	int RQ_duty = speed2duty(cfg.RQ_speed);
	int LH_duty = speed2duty(cfg.LH_speed);
	int RH_duty = speed2duty(cfg.RH_speed);
	
	u8 is_LQ_enable = (cfg.LQ_speed != 0); // 速度不是0，才需要选择对应的引脚
	u8 is_RQ_enable = (cfg.RQ_speed != 0); // 速度不是0，才需要选择对应的引脚
	u8 is_LH_enable = (cfg.LH_speed != 0); // 速度不是0，才需要选择对应的引脚
	u8 is_RH_enable = (cfg.RH_speed != 0); // 速度不是0，才需要选择对应的引脚
	// 任意一个PWM是enable，PWMA启用
	u8 is_main_enable = is_LQ_enable || is_RQ_enable || is_LH_enable || is_RH_enable;

  // 配置PWM1 M3 左后轮 LH
  PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
  PWMx_InitStructure.PWM_Duty    		= PERIOD * LH_duty / 100;	//PWM占空比时间, 0~Period
  PWMx_InitStructure.PWM_EnoSelect	= is_LH_enable ? (ENO1P | ENO1N) : 0;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
  PWM_Configuration(PWM1, &PWMx_InitStructure);			//初始化PWM
  // 配置PWM2 M4 右后轮 RH
  PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
  PWMx_InitStructure.PWM_Duty    		= PERIOD * RH_duty / 100;	//PWM占空比时间, 0~Period
  PWMx_InitStructure.PWM_EnoSelect  = is_RH_enable ? (ENO2P | ENO2N) : 0;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
  PWM_Configuration(PWM2, &PWMx_InitStructure);			//初始化PWM
  // 配置PWM3 M1 左前轮 LQ
  PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
  PWMx_InitStructure.PWM_Duty    		= PERIOD * LQ_duty / 100;	//PWM占空比时间, 0~Period
  PWMx_InitStructure.PWM_EnoSelect  = is_LQ_enable ? (ENO3P | ENO3N) : 0;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
  PWM_Configuration(PWM3, &PWMx_InitStructure);
  // 配置PWM4 M2 右前轮 RQ
  PWMx_InitStructure.PWM_Mode    		= CCMRn_PWM_MODE1;	//模式,		CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
  PWMx_InitStructure.PWM_Duty    		= PERIOD * RQ_duty / 100;	//PWM占空比时间, 0~Period
  PWMx_InitStructure.PWM_EnoSelect  = is_RQ_enable ? (ENO4P | ENO4N) : 0;	//输出通道选择,	ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
  PWM_Configuration(PWM4, &PWMx_InitStructure);

  // 配置PWMA
  PWMx_InitStructure.PWM_Period   = PERIOD;			//周期时间,   0~65535
  PWMx_InitStructure.PWM_DeadTime = 0;					//死区发生器设置, 0~255
  PWMx_InitStructure.PWM_MainOutEnable= is_main_enable;			//主输出使能, ENABLE,DISABLE
  PWMx_InitStructure.PWM_CEN_Enable   = is_main_enable;			//使能计数器, ENABLE,DISABLE
  PWM_Configuration(PWMA, &PWMx_InitStructure);			//初始化PWM通用寄存器,  PWMA,PWMB

  // 切换PWM通道
  PWM1_SW(PWM1_SW_P20_P21);			//PWM1_SW_P10_P11,PWM1_SW_P20_P21,PWM1_SW_P60_P61
  PWM2_SW(PWM2_SW_P22_P23);			//PWM2_SW_P12_P13,PWM2_SW_P22_P23,PWM2_SW_P62_P63
  PWM3_SW(PWM3_SW_P14_P15);			//PWM3_SW_P14_P15,PWM3_SW_P24_P25,PWM3_SW_P64_P65
  PWM4_SW(PWM4_SW_P16_P17);			//PWM4_SW_P16_P17,PWM4_SW_P26_P27,PWM4_SW_P66_P67,PWM4_SW_P34_P33

  // 初始化PWMA的中断
  NVIC_PWM_Init(PWMA,DISABLE,Priority_0);
}
void Motors_init() {
	
	// 推挽
	P1_MODE_OUT_PP(GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	P2_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	
}

// 前进
void Motors_forward(int speed) {
	// 0 |--------------| 50 |--------------| 100
	MotorsCfg cfg;
	
//	printf("forward\r\n");
  cfg.LQ_speed = speed;
  cfg.RQ_speed = speed;
  cfg.LH_speed = speed;
  cfg.RH_speed = speed;
	PWM_config(cfg);
}

void Motors_backward(int speed){ 
	// 0 |--------------| 50 |--------------| 100
	MotorsCfg cfg = {0, 0, 0, 0};
	
//	printf("backward\r\n");
  cfg.LQ_speed = -speed;
  cfg.RQ_speed = -speed;
  cfg.LH_speed = -speed;
  cfg.RH_speed = -speed;
	PWM_config(cfg);
	
}
// 左平移
// dir: 0左，1左前，-1左后
void Motors_left(int speed, int dir){
	MotorsCfg cfg = {0, 0, 0, 0};
	
//	printf("left\r\n");
	if(dir == 1){
		cfg.RQ_speed = speed;
		cfg.LH_speed = speed;		
	} else if(dir == -1){
		cfg.LQ_speed = -speed;
		cfg.RH_speed = -speed;		
	} else { // 0
		cfg.RQ_speed = speed;
		cfg.LH_speed = speed;	
		cfg.LQ_speed = -speed;
		cfg.RH_speed = -speed;			
	}
  
	PWM_config(cfg);
}

// 右平移
// dir: 0右，1右前，-1右后
void Motors_right(int speed, int dir){
	MotorsCfg cfg = {0, 0, 0, 0};
	
//	printf("right\r\n");
	if(dir == 1 || dir == 0){
		cfg.LQ_speed = speed;
		cfg.RH_speed = speed;		
	}
  
	if(dir == -1 || dir == 0){
		cfg.RQ_speed = -speed;
		cfg.LH_speed = -speed;	
	}

 	PWM_config(cfg);
}

// 原地打转 direction
// dir: 0左掉头（逆时针） 1右掉头（顺时针）
void Motors_around(int speed, int dir){
	MotorsCfg cfg = {0, 0, 0, 0};
	if(dir == 1){
		cfg.LQ_speed = speed;
		cfg.LH_speed = speed;	
		cfg.RQ_speed = -speed;
		cfg.RH_speed = -speed;	
	}else {
		cfg.LQ_speed = -speed;
		cfg.LH_speed = -speed;	
		cfg.RQ_speed = speed;
		cfg.RH_speed = speed;	
	}
	
 	PWM_config(cfg);
}
int clip(int value, int min, int max){
	if(value < min){
		return min;
	}else if(value > max){
		return max;
	}
	return value;
}
/**************************
根据x, y运动
┏		↑Y+  ┓
   
    
←	 	 0 		→
X-					X+
		
┗		↓Y-	┛

X    -100 ------- 0 ------- 100
Y		 -100 ------- 0 ------- 100
****************************/
float max_ratio = 0.6;
void Motors_move(int x, int y){
	MotorsCfg cfg = {0, 0, 0, 0};
	// speed: [-100, 100]
	// 左平移: y == 0, x == -100
	// 右平移: y == 0, x == 100
	
	cfg.LQ_speed = y + x;
	cfg.LH_speed = y - x;	
	cfg.RQ_speed = y - x;
	cfg.RH_speed = y + x;	
	
	// [-60, 60]
	cfg.LQ_speed = max_ratio * clip(cfg.LQ_speed, -100, 100);
	cfg.LH_speed = max_ratio * clip(cfg.LH_speed, -100, 100);
	cfg.RQ_speed = max_ratio * clip(cfg.RQ_speed, -100, 100);
	cfg.RH_speed = max_ratio * clip(cfg.RH_speed, -100, 100);
 	PWM_config(cfg);
}

void Motors_stop(){

	MotorsCfg cfg;
//	printf("stop\r\n");
  cfg.LQ_speed = 0;
  cfg.RQ_speed = 0;
  cfg.LH_speed = 0;
  cfg.RH_speed = 0;
	PWM_config(cfg);
}