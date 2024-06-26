#include "Ultrasonic.h"
#include "GPIO.h"


void Ultrasonic_init(){
	GPIO_INIT();
	
	// 默认拉低
	TRIG = 0;
	// 拉低
	ECHO = 0;
	
}
void Delay10us(void)	//@24.000MHz
{
	unsigned char data i;

	i = 78;
	while (--i);
}

// 触发测距，并计算距离
// 返回值：是否成功，0成功SUCCESS， -1失败FAIL
// distance会被填充为距离，单位是cm
char Ultrasonic_get_distance(float* distance){
	u16 cnt_10us = 0;
	float calc_dis = 0;
	
	// 1. 把TRIG拉高10us以上
	TRIG = 1;
	Delay10us();
	Delay10us();
	TRIG = 0;
	
	// Delay一段时间，确保ECHO被拉低了
	Delay10us();
	
	// 2. 等待ECHO被拉高,在一定时间内 (194us) < 300us
	while(ECHO == 0 && cnt_10us <= 30){
		Delay10us();
		cnt_10us++; // 30 -> 300us
	}
	
	if(cnt_10us * 10 >= 300){
		printf("ECHO拉高信号超时: %d us, 退出\n", (int)(cnt_10us * 10));
		return FAIL; // -1
	}
	
//	printf("ECHO: %d us\n", (int)(cnt_10us * 10));
	
	// 3. 记录ECHO从上升沿到下降沿的间隔时间
	cnt_10us = 0;
	while(ECHO == 1){
		Delay10us();
		cnt_10us++;
	}
	// 3000us -> 3ms
	// us -> ms
	printf("echo -> %.2f ms\n", (cnt_10us * 0.01));
	
	// dis = (11ms * 340m/s) / 2
	// dis = (11ms * 34000cm/1000ms) / 2
	// dis = (11ms * 34cm/ms) / 2
	// dis = 187cm
	calc_dis = ((cnt_10us * 0.01) * 34) * 0.5;
	
	*distance = calc_dis;
	
	if(calc_dis < 2.0){ // 距离太短，数值不保证
		return 1;
	}else if(calc_dis > 400.0){ // 距离太远，数值也不保证
		return 2;
	}
	
	return SUCCESS; // 0
}
