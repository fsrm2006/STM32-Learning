#include<stm32f10x.h>

void MyIWDG_Init(){	//初始化看门狗，并启动
	//启用看门狗步骤
	//开启LSI（启动IWDG自动强制启动LSI作为IWDG时钟源），无需手动开启
	//解除写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//设置预分频：在重装值RL足够大的情况下尽可能小，这样可以尽可能利用RL减少误差。
	IWDG_SetPrescaler(IWDG_Prescaler_16);
	//设置重装值：设置1s期限 RL = 1/(16/40000）- 1 = 2499
	IWDG_SetReload(2499);
	//先喂一次狗，防止第一个周期时间错误
	IWDG_ReloadCounter();
	//启动看门狗，此函数附带给IWDG寄存器写保护
	IWDG_Enable();
}

void MyIWDG_FeedDog(){
	IWDG_ReloadCounter();	//喂狗
}