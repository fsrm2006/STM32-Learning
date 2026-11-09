#ifndef __ADC_H
#define __ADC_H

extern uint16_t MyADC_ConvertValue[];
void MyADC_DMA_Start();
float MyADC_GetVoltage(uint8_t Channel);

#endif