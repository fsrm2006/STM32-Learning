#include<stm32f1xx_hal.h>
#include"rtc.h"
#ifdef __RTC_H__

// 全局缓存：日期、时间、硬件时间戳
RTC_TimeTypeDef MyRTC_Time;
RTC_DateTypeDef MyRTC_Date;
uint32_t MyRTC_TimeStamp;

// 初始化RTC日期（二进制格式），星期由HAL自动计算，无需手动赋值
void MyRTC_InitTime(){
	RTC_DateTypeDef Date;
	Date.Year = 26;		// 年份后两位，对应2026年
	Date.Month = 11;	// 十进制月份，BIN格式不可使用BCD常量
	Date.Date = 12;
	HAL_RTC_SetDate(&hrtc, &Date, RTC_FORMAT_BIN);
}

// 直接读取硬件32位秒计数器
// 先读高16位触发硬件锁存低16位，保证两次读数为同一时刻
// 注意：硬件计数器不可与HAL软件日历混用，否则会被改写只剩当日秒数
uint32_t MyRTC_GetTimeStamp(){
	return ((uint32_t)(RTC->CNTH) << 16 ) | RTC->CNTL;
}

// 直接写入硬件32位秒计数器
// 注意：写入会破坏HAL软件日历基准，二者只能二选一使用
void MyRTC_SetTimeStamp(uint32_t TimeStamp){
    RTC->CRL |= RTC_CRL_CNF;        // 进入配置模式，开放寄存器写权限
	RTC->CNTH = (uint16_t)(TimeStamp >> 16);
	RTC->CNTL = (uint16_t)TimeStamp;
	RTC->CRL &= ~RTC_CRL_CNF;       // 退出配置模式，写入生效
}

#endif
