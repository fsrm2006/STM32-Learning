#include<stm32f10x.h>
#include"OLED.h"
#include"Key.h"
#include"MyFlash.h"
#include"Store.h"


int main(){
	
	Store_Init();
	Key_Init();
	OLED_Init();
	
	OLED_ShowString(1, 1, "Flag:");
	OLED_ShowString(2, 1, "Data:");
	
	OLED_ShowHexNum(1, 6, MyFlash_ReadHalfWord(0x0800FC00), 4);
	OLED_ShowHexNum(3, 1, MyFlash_ReadHalfWord(0x0800FC02), 4);
	OLED_ShowHexNum(3, 8, MyFlash_ReadHalfWord(0x0800FC04), 4);
	OLED_ShowHexNum(4, 1, MyFlash_ReadHalfWord(0x0800FC06), 4);
	OLED_ShowHexNum(4, 8, MyFlash_ReadHalfWord(0x0800FC08), 4);
	
	uint8_t KeyNum = 0;
	Store_Data[1] = 0x1210;
	Store_Data[2] = 0x2320;
	Store_Save();		//保存修改
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1){		//PB1
			//按下按键改变数据
			Store_Data[1] += 2;
			Store_Data[2] += 4;
			Store_Data[3] += 6;
			Store_Data[4] += 8;
			Store_Save();		//保存修改
		}
		else if (KeyNum == 2){	//PB11
			Store_Clear();		//清除数据
		}
		//实时显示Flash数据看是否修改成功
		OLED_ShowHexNum(1, 6, MyFlash_ReadHalfWord(0x0800FC00), 4);
		OLED_ShowHexNum(3, 1, MyFlash_ReadHalfWord(0x0800FC02), 4);
		OLED_ShowHexNum(3, 8, MyFlash_ReadHalfWord(0x0800FC04), 4);
		OLED_ShowHexNum(4, 1, MyFlash_ReadHalfWord(0x0800FC06), 4);
		OLED_ShowHexNum(4, 8, MyFlash_ReadHalfWord(0x0800FC08), 4);
	}
	return 0;
}
