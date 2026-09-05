#include<stm32f10x.h>
#include<math.h>
#include<stdio.h>
#include<stdarg.h>
#include"OLED.h"
void Serial_SendByte(uint8_t Byte);
void Serial_SendString(char* str);
void Serial_SendArray(uint8_t arr[], uint16_t n);
void Serial_SendNumber(uint32_t Number);
void Serial_GetRXData();

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];
uint8_t Serial_RxFlag;

/**
 * @brief  USART1串口初始化，PA9(TX)发送，PA10(RX)接收，开启RXNE接收中断
 * @note   波特率9600，8N1：8数据位、无校验、1停止位
 *         TX引脚：复用推挽输出，引脚交给USART外设控制
 *         RX引脚：上拉输入，串口外设读取引脚电平
 *         开启USART_IT_RXNE中断：收到1字节，硬件置RXNE，自动进入USART1中断服务函数
 */
void Serial_Init(){
	//开启USART1外设时钟、GPIOA引脚时钟，外设使用前必须开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//配置PA9 TX发送引脚：复用推挽输出，引脚电平控制权交给串口外设，不是软件控制GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//配置PA10 RX接收引脚：上拉输入模式，USART外设读取引脚电平
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//USART参数配置  帧格式：8N1，波特率9600，同时开启发送、接收
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;					//波特率，决定串口通信速率，收发双方必须完全一致
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控，本工程不使用RTS/CTS硬件握手
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;			//同时开启发送、接收功能
	USART_InitStruct.USART_Parity = USART_Parity_No;		//校验位：无校验；可选奇校验/偶校验，用于简单检错
	USART_InitStruct.USART_StopBits = USART_StopBits_1;		//停止位：1位，标记一帧数据结束
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//数据位长度8bit；如果开启校验，实际有效数据7bit
	USART_Init(USART1, &USART_InitStruct);
	
	//使能USART RXNE接收中断：RDR寄存器收到数据，就触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//配置NVIC中断优先级分组2：2位抢占优先级，2位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;			//选择USART1中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能该中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;			//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	USART_Cmd(USART1, ENABLE);	//使能串口外设，所有参数配置完成后打开串口
}

/**
 * @brief USART1中断服务函数
 * @note  当RXNE置1（RDR收到新字节），硬件自动进入此函数
 *        读取USART_ReceiveData()读RDR，硬件自动清除RXNE标志位
 *        重点：USART_IT_RXNE，读RDR就自动清零中断挂起位，不需要手动调用ClearITPendingBit；此处调用属于多余操作
 */
void USART1_IRQHandler(){
	//全局静态变量，只能在本函数使用，只会初始化一次，后续数据会保留
	static uint8_t RxState = 0;	
	static uint8_t RxNum = 0;	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){		//判断是不是RXNE接收中断触发
		uint8_t RxData = USART_ReceiveData(USART1);
		//状态机
		switch (RxState){
			case 0:		//等待接收包头
				if (RxData == 0xFF && Serial_RxFlag == 0){	//收到包头，并且之前数据已处理，转移状态为接收数据
					RxState = 1;
				}
				break;
			case 1:		//接收数据
				Serial_RxPacket[RxNum] = USART_ReceiveData(USART1);
				RxNum++;
				if (RxNum >= 4){
					RxState = 2;
					RxNum = 0;
				}
				break;
			case 2:		//等待接收包尾
				if (RxData == 0xFE){	//收到包尾，转移状态
					RxState = 0;
					Serial_RxFlag = 1;
				}
				break;
		}
		
		//读RDR硬件自动清除中断挂起位，但是这里养成习惯，手动清零
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		
	}
}
uint8_t Serial_GetRxFlag(){
	return Serial_RxFlag;
}
//发送数据包
void Serial_SendPacket(){
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	Serial_SendByte(0xFE);
}

/**
 * @brief 串口发送1字节
 * @param Byte：待发送字节数据
 * @note  写DR即写入TDR发送寄存器；等待TXE标志，代表TDR数据已经搬运到移位寄存器，可以写下一字节
 *        TXE置1：TDR空，可写入新数据；不等于TC，TC代表整帧全部bit（含停止位）发送完成
 */
void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART1, Byte);
	//等待TDR数据转移到发送移位寄存器，TDR为空，TXE置1
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
 * @brief 发送字符串，遇到'\0'结束
 * @param str：字符串指针，增加判空防止空指针崩溃
 */
void Serial_SendString(char* str){
	if (str){
		uint16_t i = 0;
		while(str[i] != '\0'){	//C字符串以'\0'作为结束标志
			Serial_SendByte(str[i]);
			i++;
		}
	}
}

/**
 * @brief 发送原始字节数组
 * @param arr：数组首地址
 * @param n：要发送的字节个数
 */
void Serial_SendArray(uint8_t arr[], uint16_t n){
	if (arr){
		for (uint16_t i = 0; i < n; i++){
			Serial_SendByte(arr[i]);
		}
	}
}

/**
 * @brief 发送十进制数字，转为ASCII字符输出
 * @param Number：无符号32位数字
 * @note  使用pow函数做数学取位；数字+48转换为对应ASCII字符('0'的ASCII码是48)
 *        注意：pow返回浮点数，存在极小概率浮点误差，适合教学演示，产品不推荐，建议用取模%10拆分数字
 */
void Serial_SendNumber(uint32_t Number){
	uint32_t temp = Number;
	uint8_t len = 0;
	if (temp == 0) len = 1;	//数字为0时单独处理，否则长度算出来是0
	while (temp){
		temp /= 10;
		len++;
	}
	for (int i = len; i > 0; i--){
		Serial_SendByte((int)(Number/pow(10,i-1))%10 + 48);
	}
}

///**
// * @brief printf重定向接口
// * @note 魔术棒Target必须勾选Use MicroLIB；ARM Compiler V5编译器有效
// *       原理：标准库printf底层会调用fputc输出单个字符，重定向后输出转到串口
// *       不开启MicroLIB时，需要补充_sys_exit等其他底层接口，否则链接报错
// */
//int fputc(int ch, FILE *f){
//	Serial_SendByte(ch);
//	return ch;
//}

/**
 * @brief 自定义串口printf，不依赖标准库FILE，不需要MicroLIB也可以用
 * @param format：格式化字符串，和printf用法一致
 * @note  可变参数va_list解析参数，vsprintf格式化到数组，再调用串口发送
 *        缓冲区大小100字节，输出总长度不能超过99，否则数组溢出内存越界
 */
void Serial_printf(char *format, ...){
	char String[100];
	va_list ap;			//可变参数列表结构体
	va_start(ap, format);	//初始化可变参数，从format后面取参数
	vsprintf(String, format, ap);	//把格式化结果输出到字符数组
	va_end(ap);			//结束可变参数解析
	
	Serial_SendString(String);
}

/**
 * @brief 查询方式读取串口接收数据（RXNE轮询，和中断接收二选一）
 * @note  不使用中断时，在主循环反复调用该函数；判断RXNE标志，读到RDR寄存器硬件自动清除RXNE
 *        当前工程开启了RXNE中断，此函数和中断不要同时混用，会出现数据读取冲突
 */
void Serial_GetRXData(){
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET){	//RXNE置1，代表RDR寄存器存在收到的数据
		uint8_t RXData = USART_ReceiveData(USART1);				//读取RDR寄存器，硬件自动清零RXNE标志位
		OLED_ShowHexNum(1, 1, RXData, 2);
	}
}