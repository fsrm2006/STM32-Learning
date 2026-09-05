#ifndef __SERIAL_H
#define __SERIAL_H
#include<stdio.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];
extern uint8_t Serial_RxFlag;
void Serial_Init();
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char* str);
void Serial_SendArray(uint8_t arr[], uint16_t n);
void Serial_SendNumber(uint32_t Number);
void Serial_printf(char *format, ...);
void Serial_GetRXData();
void Serial_SendPacket();
uint8_t Serial_GetRxFlag();


#endif
