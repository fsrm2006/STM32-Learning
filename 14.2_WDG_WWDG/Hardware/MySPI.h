#ifndef __MYSPI_H
#define __MYSPI_H

void MySPI_Init();
void MySPI_Start();
void MySPI_Stop();
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif