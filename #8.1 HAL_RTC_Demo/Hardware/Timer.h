#ifndef __TIMER_H
#define __TIMER_H

extern int16_t Encoder_Count;
extern int16_t Encoder_Last;
extern int16_t Encoder_Speed;

int16_t Encoder_GetCNT();
void Encoder_Start();
void TIM1_ITStart();

#endif