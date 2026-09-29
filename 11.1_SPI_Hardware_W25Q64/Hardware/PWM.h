#ifndef __PWM_H
#define __PWM_H

void PWM_Init();
void PWM_SetCompare1(uint16_t Compare);		//设置TIM2 CH1通道的CCR寄存器值
void PWM_SetPrescaler(uint16_t Prescaler);

#endif