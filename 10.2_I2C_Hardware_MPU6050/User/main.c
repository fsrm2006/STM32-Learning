#include<stm32f10x.h>
#include"MPU6050.h"
#include"OLED.h"
#include"Delay.h"

int8_t ID;
int16_t AX,AY,AZ,GX,GY,GZ;
int main(){
	OLED_Init();
	MPU6050_Init();
	
	//显示MPU6050ID
	ID = MPU6050_GetID();
	OLED_ShowHexNum(1,1,ID,2);
	while(1)
	{
		//读取加速度和陀螺仪传感器值并显示
		MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
		OLED_ShowSignedNum(2,1,AX,5);
		OLED_ShowSignedNum(3,1,AY,5);
		OLED_ShowSignedNum(4,1,AZ,5);
		OLED_ShowSignedNum(2,8,GX,5);
		OLED_ShowSignedNum(3,8,GY,5);
		OLED_ShowSignedNum(4,8,GZ,5);
	}
	return 0;
}
