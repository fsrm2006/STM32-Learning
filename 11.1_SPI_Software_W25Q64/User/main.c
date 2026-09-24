#include<stm32f10x.h>
#include"OLED.h"
#include"Delay.h"
#include"W25Q64.h"

uint8_t MID;
uint16_t DID;

uint8_t Array_Read[256];
uint8_t Array_Write[256] = {0x12, 0x34, 0x56, 0x78};
int main(){
	W25Q64_Init();
	OLED_Init();
	
	W25Q64_ReadID(&MID, &DID);
	
	OLED_ShowString(1, 1, "MID:   DID:");
	OLED_ShowString(2, 1, "W:");
	OLED_ShowString(3, 1, "R:");

	OLED_ShowHexNum(1, 5, MID, 2);
	OLED_ShowHexNum(1, 12, DID, 4);
	
	//写入先擦除再写入
	W25Q64_SectorErase(0x000000);	//前两字节页地址，后一字节页内偏移，第三字节更改擦除不影响页
	W25Q64_PageProgram(0x000000, Array_Write, 4);
	
	W25Q64_WaitBusy();		//重要！！写入是事前等待，这里刚写完要等待写入完成再读取，不然会读取到FF
	W25Q64_ReadData(0x000000, Array_Read, 4);
	for (uint8_t i = 0; i < 4; i++){
		OLED_ShowHexNum(2, 3+i*3, Array_Write[i], 2);
		OLED_ShowHexNum(3, 3+i*3, Array_Read[i], 2);
	}
	while(1)
	{
	
	}
	return 0;
}
