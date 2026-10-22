#include<stm32f10x.h>
#include<time.h>

// 全局时间结构体缓存，存放转换后的时间结果
// 每次调用MyRTC_GetTime都会更新该变量，返回其指针
struct tm Mytime;		

// 引用主文件中定义的初始时间配置
extern struct tm time_set;

// 设置RTC计数器时间，参数为待设置的时间结构体（北京时间格式）
void MyRTC_SetTime(struct tm* mytime);

// RTC初始化，带掉电记忆判断
// 利用BKP掉电不丢失的特性，通过标记位判断是否首次上电
// 首次上电：初始化时钟源、分频、设置时间、写入初始化标记
// 非首次上电：仅做同步，不重置时间，保持RTC持续走时
void MyRTC_Init(){
	// 开启PWR和BKP外设时钟，均挂载在APB1总线
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	// 解除后备域写保护，允许CPU读写BKP和RTC寄存器
	PWR_BackupAccessCmd(ENABLE);
	
	// 检测BKP标记，判断是否为首次上电初始化
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x1234){
		// 开启LSE外部32.768kHz低速晶振
		RCC_LSEConfig(RCC_LSE_ON);
		// 等待晶振起振稳定
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
		
		// 选择LSE作为RTC时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		// 使能RTC时钟
		RCC_RTCCLKCmd(ENABLE);
		
		// 备选方案：LSE故障时使用内部LSI时钟
		//	RCC_LSICmd(ENABLE);
		//	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
		//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		//	RCC_RTCCLKCmd(ENABLE);
		
		// 等待RTC寄存器与APB总线同步
		RTC_WaitForSynchro();
		// 等待上一次RTC操作完成
		RTC_WaitForLastTask();
		
		// 设置预分频重装值：32768Hz输入 / (32767+1) = 1Hz秒脉冲
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();
		
		// 设置RTC计数器初始时间
		MyRTC_SetTime(&time_set);
		
		// 写入初始化标记，下次上电检测到则跳过时间设置
		BKP_WriteBackupRegister(BKP_DR1, 0x1234);
	}
	else{
		// 已初始化过，仅执行同步，确保读取数据正确
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}

// 设置RTC时间，输入北京时间格式的tm结构体
// 内部自动转换为UTC时间戳写入RTC计数器
void MyRTC_SetTime(struct tm* mytime){
	// 转换为mktime要求的标准格式
	mytime->tm_year -= 1900;		// 年份偏移：tm_year为相对于1900的年数
	mytime->tm_mon -= 1;			// 月份偏移：tm_mon范围0~11
	
	// 转换为UTC时间戳，写入RTC 32位计数器
	// 手动将北京时间转换为UTC时间（减8小时）
	RTC_SetCounter(mktime(mytime) - 28800);
	RTC_WaitForLastTask();		// 等待写入操作硬件执行完毕
}

// 获取当前北京时间结构体，返回指向内部全局缓存的指针
// 每次调用都会更新缓存内容，注意不要长期保存指针
struct tm* MyRTC_GetTime(){
	// 读取UTC时间戳，加8小时偏移为北京时间
	time_t time_cnt = RTC_GetCounter() + 28800;
	
	// 转换为本地时间结构体，拷贝到全局缓存，避免localtime静态缓冲区被覆盖
	Mytime = *localtime(&time_cnt);
	
	// 转换为日常使用的年月日格式
	Mytime.tm_year += 1900;
	Mytime.tm_mon += 1;
	
	return &Mytime;
}
