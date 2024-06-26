#include "App.h"
#include "Battery.h"
#include "Buzzer.h"

#define LOW_POWER	11.0

float voltage;

// 判断是否是低电量
u8 isLowPower(){
	return voltage < LOW_POWER;
}
	
void task_battery() _task_ TASK_BATTERY {
  Battery_init();
	while(1){
		// 读取VCC电压 -----------------------------------
		voltage = Battery_get_voltage();
		printf("voltage: %.3f V\n", voltage);
		
		// 4.2V * 3 = 12.6V
		// 3.7V * 3 = 11.1V
		// 3.5V * 3 = 10.5V
		if(isLowPower()){
			printf("Battery voltage is too low, please charge it: %.2f\n", voltage);
		
			// 滴滴滴，连续三声
			Buzzer_play(3, 50);
		}

		// 2000ms
		os_wait2(K_TMO, 200);// 1000ms = 5ms * 200
		os_wait2(K_TMO, 200);// 1000ms = 5ms * 200
	}
}
