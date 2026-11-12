#ifndef __MYRTC_H
#define __MYRTC_H

extern RTC_TimeTypeDef MyRTC_Time;
extern RTC_DateTypeDef MyRTC_Date;
extern uint32_t MyRTC_TimeStamp;
uint32_t MyRTC_GetTimeStamp();
void MyRTC_SetTimeStamp(uint32_t TimeStamp);
void MyRTC_InitTime();

#endif