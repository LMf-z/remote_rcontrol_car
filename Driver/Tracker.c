#include "Tracker.h"


void Tracker_init(){

		// 初始化GPIO
	TRACKER_GPIO_INIT();
}

// 扫描当前所有灯状态
// 1: 黑线，光敏电阻没收到光：指示灯灭
// 0: 地面，光敏电阻收到光了：指示灯亮
void Tracker_scan(u8 states[]){
	states[0] = TRACK_1;
	states[1] = TRACK_2;
	states[2] = TRACK_3;
}


// 读取线位置
// 1高电平: 黑线，指示灯灭 (光敏电阻没收到光)
// 0低电平: 地面，指示灯亮（光敏电阻收到光了）
int Tracker_get_position(){
	
	int sum = 0, count = 0, pos = 0;
	// 记录上一次的位置, 只初始化1次
	static int last_pos = 0;
	
	if(TRACK_1 == 1){
		sum += -32;
		count++;
	}
	if(TRACK_2 == 1){
		sum += 0;
		count++;
	}
	if(TRACK_3 == 1){
		sum += 32;
		count++;
	}
	
	// 没有灯压到黑线
	if(count == 0){
		// 状态丢失。以上一次状态为准
		return last_pos;
	}
	
	pos = sum / count;
	
	// 把当前值记录下来，用于下一次没压到线时，返回用
	last_pos = pos;
	
	return pos;
}



