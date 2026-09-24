#ifndef __MPU6050_H
#define __MPU6050_H

void MPU6050_WriteReg(uint8_t RegAddr, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddr);
void MPU6050_Init();
void MPU6050_GetData(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z,
	int16_t *GYRO_X, int16_t *GYRO_Y, int16_t *GYRO_Z);
uint8_t MPU6050_GetID();
#endif
