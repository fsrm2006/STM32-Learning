#ifndef __I2C_H
#define __I2C_H

void MyI2C_W_SCL(uint8_t BitValue);
void MyI2C_W_SDA(uint8_t BitValue);
uint8_t MyI2C_R_SDA();

void MyI2C_Init();
void MyI2C_Start();
void MyI2C_Stop();
void MyI2C_SendByte(uint8_t Byte);

uint8_t MyI2C_ReceiveByte();
void MyI2C_SendACK(uint8_t ACKBit);
uint8_t MyI2C_ReceiveACK();

#endif
