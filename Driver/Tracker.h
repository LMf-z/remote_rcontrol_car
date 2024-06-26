#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "config.h"
#include "GPIO.h"

#define TRACK_1 	P01
#define TRACK_2 	P02
#define TRACK_3 	P03

#define TRACKER_GPIO_INIT() P0_MODE_IO_PU(GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);

void Tracker_init();

// 扫描当前所有灯状态
// 1: 黑线，光敏电阻没收到光：指示灯灭
// 0: 地面，光敏电阻收到光了：指示灯亮
void Tracker_scan(u8 states[]);

// 读取线位置
int Tracker_get_position();


#endif