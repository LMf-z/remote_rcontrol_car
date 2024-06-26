#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "config.h"

#define TRIG	P47
#define ECHO	P33

#define GPIO_INIT()	            \
	/* TRIG 推挽*/                \
	P4_MODE_OUT_PP(GPIO_Pin_7);   \
	/* ECHO 高阻*/              \
	P3_MODE_IN_HIZ(GPIO_Pin_3);	  \
	
void Ultrasonic_init();

// 触发测距，并计算距离
// 返回值：是否成功，0成功SUCCESS， -1失败FAIL
// distance会被填充为距离，单位是cm
char Ultrasonic_get_distance(float* distance);

#endif