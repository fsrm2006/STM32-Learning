#include<stm32f10x.h>

// 读取Flash指定地址的一个字(32bit)
uint32_t MyFlash_ReadWord(uint32_t Address){
	return *((__IO uint32_t*)(Address));	//__IO 是voilate的宏定义，防止编译器优化导致读取失败
}

// 读取Flash指定地址的一个半字(16bit)
uint16_t MyFlash_ReadHalfWord(uint32_t Address){
	return *((__IO uint16_t*)(Address));
}

// 读取Flash指定地址的一个字节(8bit)
uint8_t MyFlash_ReadByte(uint32_t Address){
	return *((__IO uint8_t*)(Address));
}

// 擦除芯片全部Flash页面
void MyFlash_EraseAllPages(){
	FLASH_Unlock();			// 解锁FPEC，允许Flash操作
	FLASH_EraseAllPages();
	FLASH_Lock();			// 操作完成上锁，防止误写
}

// 擦除指定Flash页，参数为页起始地址
void MyFlash_ErasePage(uint32_t PageAddress){
	FLASH_Unlock();
	FLASH_ErasePage(PageAddress);
	FLASH_Lock();
}

// 向指定地址写入一个字(32bit)，地址需要4字节对齐，写入前必须擦除对应页
void MyFlash_ProgramWord(uint32_t Address, uint32_t Data){
	FLASH_Unlock();
	FLASH_ProgramWord(Address, Data);
	FLASH_Lock();
}

// 向指定地址写入一个半字(16bit)，地址需要2字节对齐，写入前必须擦除对应页
void MyFlash_ProgramHalfWord(uint32_t Address, uint16_t Data){
	FLASH_Unlock();
	FLASH_ProgramHalfWord(Address, Data);
	FLASH_Lock();
}
