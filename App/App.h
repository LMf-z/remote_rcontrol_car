#ifndef __APP_H__
#define __APP_H__

#include "config.h"

#define TASK_KEY 				1
#define TASK_TRACK 			2
#define TASK_UART1 			3
#define TASK_BLUETOOTH 	4
#define TASK_ULTRA			5
#define TASK_BATTERY		6

// 全局距离信息
extern float global_distance_cm;

// 超声测距功能是否开启，1开启，0关闭
extern u8 is_ultrasonic_enable;

// ---------------------------- 公共函数

// 是否低电量
extern u8 isLowPower();

// 将蓝牙模块从睡眠中唤醒
extern void bt_enable();

#endif