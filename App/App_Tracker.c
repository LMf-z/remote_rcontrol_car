#include "App.h"
#include "Tracker.h"
#include "Motors.h"

void task_tracker() _task_ TASK_TRACK {
//  u8 states[3];
//  u8 i;
  int pos, last_pos;
  // 不断检查地面 0 0 0 \r\n
  Tracker_init();
	
  while(1) {
//		Tracker_scan(states);
//		for(i = 0; i < 3; i++){
//			printf("%d ", (int)states[i]);
//		}
//		printf("\r\n");
    // 障碍物判断
		if(is_ultrasonic_enable && global_distance_cm < 20){
			Motors_stop();
			os_wait2(K_TMO, 2);// 10ms = 5ms * 2
			continue;
		}
		
    // 低电量判断
		if(isLowPower()){
			Motors_stop();
			os_wait2(K_TMO, 2);// 10ms = 5ms * 2
			continue;
		}
		
    pos = Tracker_get_position();
    if(pos == last_pos) {
      os_wait2(K_TMO, 2);// 10ms = 5ms * 2
      // 黑线位置没发生变化，直接返回
      continue;
    }
    // 记录最后一次pos
    last_pos = pos;

    // -32, -16, 0, 16, 32
    if (pos < 0) {
      Motors_around(40, 0);
//      printf("pos: %d -> %s\r\n", (int)pos, "左转");
    } else if(pos > 0) {
      Motors_around(40, 1);
//      printf("pos: %d -> %s\r\n", (int)pos, "右转");
    } else {
      Motors_forward(40);
//      printf("pos: %d -> %s\r\n", (int)pos, "直行");
    }
//    os_wait2(K_TMO, 40);// 200ms = 5ms * 40
//    os_wait2(K_TMO, 4);// 20ms = 5ms * 4
    os_wait2(K_TMO, 2);// 10ms = 5ms * 2
  }
}