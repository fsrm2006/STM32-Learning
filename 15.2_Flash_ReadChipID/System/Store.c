#include<stm32f10x.h>
#include"MyFlash.h"

// Flash存储页起始地址，放在用户程序最后一页，避免覆盖代码
#define STORE_START_ADDRESS 0x0800FC00
// 有效标志，用于判断该存储页是否已经初始化
#define USE_FLAG 0xA5A5
// 存储半字数量，512个半字 = 1KB，正好是STM32F1一页大小
#define STORE_CNT 512

// SRAM缓存数组，缓存从Flash读出的一页数据，修改后调用Save写入Flash，相当于闪存在内存的映射
uint16_t Store_Data[STORE_CNT];

void Store_Save();

// 存储模块初始化
// 检测标志位，未初始化则自动初始化；最后把Flash数据读到SRAM缓存数组
void Store_Init(){
	if (MyFlash_ReadHalfWord(STORE_START_ADDRESS) != USE_FLAG){
		// 标志不匹配：页面未初始化，填充标志位+清零缓存
		Store_Data[0] = USE_FLAG;
		for (uint16_t i = 1; i < STORE_CNT; i++){
			Store_Data[i] = 0x0000;
		}
		Store_Save();
	}
	// 将Flash里的全部数据读取到SRAM缓存数组
	for (uint16_t i = 0; i < STORE_CNT; i++){
		Store_Data[i] = MyFlash_ReadHalfWord(STORE_START_ADDRESS + i * 2);
	}
}

// 将SRAM缓存数组整页写入Flash
// 内部自动执行擦页，调用后缓存的数据才会掉电保存
void Store_Save(){
	MyFlash_ErasePage(STORE_START_ADDRESS);	// Flash写入前必须先擦除整页
	for (uint16_t i = 0; i < STORE_CNT; i++){
		MyFlash_ProgramHalfWord(STORE_START_ADDRESS + i * 2, Store_Data[i]);
	}
}

// 清空业务数据，保留有效标志位，不破坏存储页初始化状态
void Store_Clear(){
	for (uint16_t i = 1; i < STORE_CNT; i++){
		Store_Data[i] = 0x0000;
	}
	Store_Save();
}
