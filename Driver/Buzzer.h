#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "config.h"

void Buzzer_init();

// 按照指定频率震荡
void Buzzer_open();

// 关闭震荡
void Buzzer_close();

// 响一小段时间
void Buzzer_beep();

// 响count次, 按照指定间隔时间gap ms
void Buzzer_play(u8 count, u16 gap);

#endif