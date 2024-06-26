#include "Key.h"

static void GPIO_config(u8 gpio, u8 pin) {
	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
	GPIO_InitStructure.Pin  = pin;		//指定要初始化的IO,
	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_InitStructure.Mode = GPIO_PullUp;	
	GPIO_Inilize(gpio, &GPIO_InitStructure);//初始化
}


void Key_init(){
	GPIO_config(GPIO_KEY);
	
	KEY = 1;
}

u8 last_key_state = 1;
void Key_scan(){
	
	// 状态发生变化的时候
	if(KEY != last_key_state){
		// 记录最新的状态
		last_key_state = KEY;
		// 根据按钮最新的电平，判定抬起还是按下
		if(KEY){ // 1 是上升沿（低->高）抬起
			Key_on_keyup();			
		}else { // 0 是下降沿（高->低）按下
			Key_on_keydown();
		}
	}
}
