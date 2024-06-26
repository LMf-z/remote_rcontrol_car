#include "Light.h"


void Light_init(){
	LIGHT_GPIO_INIT();
	
	// 默认全部熄灭
	Light_off(ALL);
}


// 点亮指定的灯
void Light_on(Light light){
	switch(light){
		case ALL:			
			LED_L = 1;
			LED_R	= 1;
			break;
		case LEFT:			
			LED_L = 1;
			break;
		case RIGHT:			
			LED_R	= 1;
			break;
		default:
			break;
	}
}

// 熄灭指定的灯
void Light_off(Light light){
	switch(light){
		case ALL:			
			LED_L = 0;
			LED_R	= 0;
			break;
		case LEFT:			
			LED_L = 0;
			break;
		case RIGHT:			
			LED_R	= 0;
			break;
		default:
			break;
	}

}