#ifndef __MOTORS_H__
#define __MOTORS_H__

#include "config.h"

void Motors_init();


// 前进
void Motors_forward(int speed); // [0, 100]

// 后退
void Motors_backward(int speed); // [0, 100]

// 左平移
void Motors_left(int speed, int dir);

// 右平移
void Motors_right(int speed, int dir);

// 原地打转 direction
void Motors_around(int speed, int dir);

void Motors_move(int x, int y);

void Motors_stop();
#endif