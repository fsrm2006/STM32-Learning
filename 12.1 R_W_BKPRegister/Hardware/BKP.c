#include<stm32f10x.h>

void BKP_Init(){
	//开启BKP与PWR时钟，注意都在APB1上
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);	//开启备份域访问控制，允许CPU读写BKP、RTC相关寄存器
}