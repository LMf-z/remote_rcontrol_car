#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "config.h"
#include "GPIO.h"

#define LED_L	P07
#define LED_R	P52

#define LIGHT_GPIO_INIT()        \
	P0_MODE_OUT_PP(GPIO_Pin_7);    \
	P5_MODE_OUT_PP(GPIO_Pin_2);    \
	
typedef enum {
	ALL,
	LEFT,
	RIGHT,
}Light;

void Light_init();

// 点亮指定的灯
void Light_on(Light light);

// 熄灭指定的灯
void Light_off(Light light);

#endif