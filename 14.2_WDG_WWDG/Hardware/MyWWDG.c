#include<stm32f10x.h>

// 窗口看门狗初始化
// 配置合法喂狗窗口约30ms~50ms，启动时自动完成首次喂狗
void MyWWDG_Init(){
    // 1. 开启WWDG外设时钟，WWDG挂载在APB1总线上
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    
    // 2. 设置预分频与窗口值
    // 配置8分频，计数时钟 = PCLK1(36MHz) / 4096固定分频 / 8 ≈ 1099Hz，单步约0.91ms
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    // 设置窗口上限为0x55（十进制85）
    // 计数器 > 窗口值时喂狗会触发复位（喂得太早）
    // 只有计数器落在 0x40 ~ 窗口值 区间内，喂狗才合法
    WWDG_SetWindowValue(0x40 | 21);
    
    // 3. 启动WWDG并写入初始计数值，完成首次喂狗
    //  | 0x40保证T6位置1，计数器进入正常计数区间
    // 初始值0x76（十进制118），对应约30ms后进入合法窗口、约50ms超时复位
    // 启动后下一次喂狗必须落在30ms~50ms的时间窗口内
    WWDG_Enable(0x40 | 54);
}

// 窗口看门狗喂狗函数
// 必须在合法时间窗口内调用，早喂、晚喂都会触发复位
void MyWWDG_FeedDog(){
    // 重装载计数器值为0x76，保持T6位为1，避免计数器溢出复位
    WWDG_SetCounter(0x40 | 54);
}
