#include "App.h"
#include "Ultrasonic.h"
#include "Buzzer.h"

u8 is_ultrasonic_enable = FALSE; //  «∑Ò∆Ù”√≥¨…˘≤®
float global_distance_cm = -1;

void task_ultrasonic() _task_ TASK_ULTRA {	
	float distance;
	u8 rst;
	
  Ultrasonic_init();
	
	while(1){	
		if(!is_ultrasonic_enable){
			// 200ms
			// –›√ﬂ
			os_wait2(K_TMO, 40); // 200ms = 5ms * 40
			continue;
		}
		
		// ¥•∑¢≤‚æ‡
		rst = Ultrasonic_get_distance(&distance);
		if(rst == SUCCESS){ // 0
			printf("distance: %.3f cm\n", distance);
			global_distance_cm = distance;

			if(distance < 20){
				// œÏcount¥Œ, ∞¥’’÷∏∂®º‰∏Ù ±º‰gap ms
				Buzzer_play(2, 100);
			}
			
		}else { // -1, 1, 2
			printf("≤‚æ‡ ß∞‹: %.3f Rst:%d\n", distance, (int)rst);
		}
		
		// 200ms
		os_wait2(K_TMO, 40); // 200ms = 5ms * 40
	}
	
}