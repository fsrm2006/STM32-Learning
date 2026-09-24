#include<stm32f10x.h>
#include"MySPI.h"
#include"W25Q64_Ins.h"

//=============================
// W25Q64 SPI Flash 软件模拟驱动
// 基于软件SPI模式0实现，遵循高位先行规则
// 核心操作规则：写/擦除前必须写使能、操作前后必须等待BUSY空闲
//=============================

/**
 * @brief  W25Q64初始化
 * @note   仅需初始化底层软件SPI总线，Flash上电后自动进入待机状态
 */
void W25Q64_Init(){
	MySPI_Init();
}

/**
 * @brief  读取JEDEC设备ID
 * @param  MID: 输出厂商ID（Winbond固定为0xEF）
 * @param  DID: 输出设备ID（内存类型+容量，共2字节）
 * @note   指令0x9F后依次返回：厂商ID(1B) + 内存类型(1B) + 容量(1B)
 *         读取时发送0xFF为哑字节，SPI全双工读取必须发送数据占位
 */
void W25Q64_ReadID(uint8_t* MID, uint16_t* DID){
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);		// 发送JEDEC ID读取指令 0x9F
	*MID = MySPI_SwapByte(0xFF);			// 读取第1字节：厂商ID
	*DID = MySPI_SwapByte(0xFF) << 8;		// 读取第2字节：设备ID高8位（内存类型）
	*DID |= MySPI_SwapByte(0xFF);			// 读取第3字节：设备ID低8位（容量）
	MySPI_Stop();
}

/**
 * @brief  发送写使能指令
 * @note   所有修改类操作（编程、擦除、写状态寄存器）前必须调用
 *         一次写使能仅对后续一条指令有效，操作完成后自动失效
 *         必须使用独立的CS周期，不能和其他指令合并
 */
void W25Q64_WriteEnable(){
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

/**
 * @brief  轮询等待BUSY位清零，带超时保护
 * @note   BUSY位为状态寄存器第0位，为1表示芯片正在执行内部编程/擦除操作
 *         忙状态下仅响应读状态寄存器指令，其他指令会被忽略
 *         超时机制防止硬件故障导致程序卡死
 */
void W25Q64_WaitBusy(){
	uint32_t Timeout = 10000;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);	// 发送读状态寄存器1指令 0x05
	// 循环读取状态，直到BUSY位(bit0)为0或超时
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 1){
		Timeout--;
		if (Timeout == 0)
			break;	// 超时退出，可扩展错误处理逻辑
	}
	MySPI_Stop();
}

/**
 * @brief  页编程：向指定地址写入数据
 * @param  Address: 24位起始地址
 * @param  Data: 待写入数据缓冲区指针
 * @param  cnt: 写入字节数，最大不超过256字节（1页）
 * @note   1. 写入前自动等待芯片空闲
 *         2. 写入前必须执行写使能
 *         3. 指令格式：0x02 + 24位地址 + 写入数据，地址高位先行
 *         4. 单次写入不可跨页，超过256字节会地址回卷，覆盖页首数据
 *         5. 拉高CS后芯片才开始内部编程，后续操作前需再次调用WaitBusy
 */
void W25Q64_PageProgram(uint32_t Address, uint8_t *Data, uint16_t cnt){
	W25Q64_WaitBusy();			// 等待芯片空闲，确保上一次操作完成
	
	W25Q64_WriteEnable();		// 页编程属于修改操作，必须先写使能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);	// 发送页编程指令 0x02
	// 发送24位目标地址，高字节先发（MSB First）
	MySPI_SwapByte(Address >> 16);			// 发送最高8位地址（A23-A16）
	MySPI_SwapByte(Address >> 8);			// 发送中间8位地址（A15-A8）
	MySPI_SwapByte(Address);				// 发送最低8位地址（A7-A0）
	// 连续写入数据
	for (uint16_t i = 0; i < cnt; i++){
		MySPI_SwapByte(Data[i]);
	}
	MySPI_Stop();
}

/**
 * @brief  扇区擦除：擦除地址所在的整个4KB扇区
 * @param  Address: 24位目标地址（无需扇区对齐，硬件自动定位）
 * @note   1. 擦除前自动等待芯片空闲 + 写使能
 *         2. 指令格式：0x20 + 24位地址
 *         3. 擦除最小单位为4KB，无论地址在扇区哪个位置，都会擦除整个扇区
 *         4. 拉高CS后开始内部擦除，耗时较长，后续操作前必须调用WaitBusy
 */
void W25Q64_SectorErase(uint32_t Address){
	W25Q64_WaitBusy();			// 等待芯片空闲
	W25Q64_WriteEnable();		// 擦除属于修改操作，必须先写使能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);	// 发送扇区擦除指令 0x20
	// 发送24位擦除目标地址，高字节先发
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	MySPI_Stop();
}

/**
 * @brief  连续读取数据
 * @param  Address: 24位起始地址
 * @param  Data: 接收数据缓冲区指针
 * @param  cnt: 读取字节数，芯片内地址自动递增，无页边界限制
 * @note   1. 读操作不需要写使能，也不会置BUSY位
 *         2. 指令格式：0x03 + 24位地址
 *         3. 发送0xFF哑字节驱动时钟，同时接收从机返回的数据
 *         4. 只要保持CS低电平，可连续读取任意长度数据
 */
void W25Q64_ReadData(uint32_t Address, uint8_t *Data, uint32_t cnt){
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);	// 发送读数据指令 0x03
	// 发送24位起始地址，高字节先发
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	// 连续读取数据
	for (uint32_t i = 0; i < cnt; i++){
		Data[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();
}
