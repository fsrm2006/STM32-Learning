#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"BKP.h"
#include"Key.h"

uint16_t ArrayWrite[2] = {0x1234, 0x2345};
uint16_t ArrayRead[2];

int main(){
	OLED_Init();
	BKP_Init();
	Key_Init();
	
	
	//BKP_WriteBackupRegister(BKP_DR1, 0x2345);
	
	OLED_ShowString(1, 1, "W:");
	OLED_ShowString(2, 1, "R:");

	uint8_t KeyNum = 0;
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1){
			BKP_WriteBackupRegister(BKP_DR1, ++ArrayWrite[0]);	//自增ArrayWrite值后写入
			BKP_WriteBackupRegister(BKP_DR2, ++ArrayWrite[1]);	//自增ArrayWrite值后写入
			OLED_ShowHexNum(1, 4, ArrayWrite[0], 4);		//显示写入值
			OLED_ShowHexNum(1, 9, ArrayWrite[1], 4);
		}
		
		ArrayRead[0] = BKP_ReadBackupRegister(BKP_DR1);
		ArrayRead[1] = BKP_ReadBackupRegister(BKP_DR2);
		//显示读取值
		OLED_ShowHexNum(2, 4, ArrayRead[0], 4);
		OLED_ShowHexNum(2, 9, ArrayRead[1], 4);
	}
	return 0;
}
