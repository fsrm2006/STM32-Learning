#ifndef __MYRTC_H
#define __MYRTC_H

#include<time.h>

void MyRTC_Init();
void MyRTC_SetTime(struct tm* mytime);

struct tm* MyRTC_GetTime();

#endif