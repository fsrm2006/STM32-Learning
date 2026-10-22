#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"MyRTC.h"

// 配置初始时间（北京时间）
struct tm time_set={
	.tm_year =2026, 
	.tm_mon = 10, 
	.tm_mday = 16,
	.tm_hour = 19,
	.tm_min = 47,
	.tm_sec = 35
};

int main(){
//	注意事项：
//	待机模式唤醒后，程序从头开始执行，while只执行一次，可以去除while
//	待机模式唤醒后HSE会关闭，主频变为HSI 8MHz，需要重新调用SystemInit();
//	进入待机模式之前，最好把外部模块能停的都停掉，最大化省电
//	标准库V3.6.0进入待机前去要手动清楚PWR的PWR_FLAG_WU位，否则不会自动清除，会反复唤醒！！！
	
	OLED_Init();
	MyRTC_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR时钟
	
	
	//RTC定时唤醒
//	OLED_ShowString(1, 1, "CNT :");
//	OLED_ShowString(2, 1, "ALR :");
//	OLED_ShowString(3, 1, "ALRF:");
//	
//	uint32_t Alarm = RTC_GetCounter() + 5;		//设置闹钟值，每过5秒唤醒一次
//	
//	RTC_SetAlarm(Alarm);
//	
//	OLED_ShowNum(1, 6, RTC_GetCounter(), 10);
//	OLED_ShowNum(2, 6, Alarm, 10);
//	OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);
	
	
	//WKUP引脚（PA0）上升沿唤醒
	PWR_WakeUpPinCmd(ENABLE);	//使能WKUP引脚（PA0），自动配置为输入下拉，无需初始化GPIO
	
	OLED_ShowString(4, 1, "Running");
	Delay_ms(100);
	OLED_ShowString(4, 1, "       ");
	Delay_ms(100);

	OLED_ShowString(4, 9, "StandBy");
	Delay_ms(1000);
	OLED_ShowString(4, 9, "       ");
	Delay_ms(100);

	//进入待机模式前关闭所有外部模块省电
	OLED_Clear();
	
	//标准库V3.6.0进入待机前去要手动清除PWR的PWR_FLAG_WU位，不会自动清除，不清会反复唤醒！！！
	PWR_ClearFlag(PWR_FLAG_WU);
	//进入待机模式，自动配置SLEEPDEEP，PDDS寄存器位
	PWR_EnterSTANDBYMode();
	
	return 0;
}
