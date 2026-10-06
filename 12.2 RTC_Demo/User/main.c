#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"MyRTC.h"

// 预设初始时间，首次上电初始化RTC时使用
struct tm time_set;

int main(){
	// 配置初始时间（北京时间）
	time_set.tm_year = 2026;	// 年份：实际值，设置函数内部会自动减1900适配mktime格式
	time_set.tm_mon = 10;		// 月份：1~12，设置函数内部会自动减1适配mktime格式（0~11）
	time_set.tm_mday = 6;		// 日期
	time_set.tm_hour = 16;		// 时
	time_set.tm_min = 35;		// 分
	time_set.tm_sec = 25;		// 秒
	
	// 外设初始化
	OLED_Init();
	MyRTC_Init();
	
	// 首次上电时会自动执行时间设置，已初始化过则跳过
//	MyRTC_SetTime(&time_set);
	
	struct tm * time_now;		// 指向当前时间结构体的指针
	uint16_t div = 0;			// 存储RTC分频器当前计数值
	
	// OLED显示固定提示符
	OLED_ShowString(1,1,"Date:    .  .");
	OLED_ShowString(2,1,"Time:  :  :  .");
	OLED_ShowString(3,1,"CNT :");
	OLED_ShowString(4,1,"DIV :");
	
	while(1)
	{
		// 获取当前北京时间对应的时间结构体
		time_now = MyRTC_GetTime();
		
		// 显示日期：年.月.日
		OLED_ShowNum(1,6,time_now->tm_year,4);	
		OLED_ShowNum(1,11,time_now->tm_mon,2);
		OLED_ShowNum(1,14,time_now->tm_mday,2);
		
		// 显示时间：时:分:秒
		OLED_ShowNum(2,6,time_now->tm_hour,2);	
		OLED_ShowNum(2,9,time_now->tm_min,2);
		OLED_ShowNum(2,12,time_now->tm_sec,2);
		
		// 显示RTC 32位计数器原始值（UTC时间戳，单位秒）
		OLED_ShowNum(3,6,RTC_GetCounter(),10);
		
		// 读取分频器剩余计数值，计算并显示百分秒（0~99）
		div = RTC_GetDivider();
		OLED_ShowNum(4,6,div,5);
		// 计算公式：(重装值-当前剩余值)/总分频值 = 已过去的秒数小数部分，乘以100得到百分秒
		OLED_ShowNum(2,15,((32767 - div) / 32768.0) * 100,2);
	}
	return 0;
}
