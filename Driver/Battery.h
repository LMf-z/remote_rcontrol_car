#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "config.h"

#define BATTERY_ADC_CH	ADC_CH3
#define GPIO_PORT				GPIO_P1
#define GPIO_PIN				GPIO_Pin_3

void Battery_init();

float Battery_get_voltage();

#endif