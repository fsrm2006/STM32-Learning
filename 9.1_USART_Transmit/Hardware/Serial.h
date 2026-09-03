#ifndef __SERIAL_H
#define __SERIAL_H
#include<stdio.h>
void Serial_Init();
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char* str);
void Serial_SendArray(uint8_t arr[], uint16_t n);
void Serial_SendNumber(uint32_t Number);
#endif
