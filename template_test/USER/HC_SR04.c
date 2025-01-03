#include "HC_SR04.h"


/***
初始化输出高电平的引脚

***/

void GPIO_Init_TRIG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIOC 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 配置 PC0 为普通输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}



void TIM_ICU_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    TIM_ICInitTypeDef TIM_ICStruct;

    // 使能 GPIOA 和 TIM13 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);

    // 配置 PA6 为 TIM13_CH1 输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 映射 PA6 到 TIM13_CH1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM13);

    // 配置 TIM13 基本参数
    TIM_BaseStruct.TIM_Prescaler = 84 - 1;  // 1 MHz 计数频率
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 0xFFFF; // 最大计数值（16 位定时器）
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM13, &TIM_BaseStruct);

    // 配置 TIM13 输入捕获
    TIM_ICStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Rising; // 捕获上升沿
    TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 不分频
    TIM_ICStruct.TIM_ICFilter = 0;
    TIM_ICInit(TIM13, &TIM_ICStruct);

    // 开启中断
    TIM_ITConfig(TIM13, TIM_IT_CC1, ENABLE);

    // 启动 TIM13
    TIM_Cmd(TIM13, ENABLE);

    // 配置 NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn; // TIM13 使用 TIM8_UP_TIM13 中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


void HC_SR04_Init(void){
    // 初始化 TRIG 和 ECHO
    GPIO_Init_TRIG();
    TIM_ICU_Init();	
		
}

volatile uint32_t T1 = 0, T2 = 0;
volatile uint8_t isCaptured = 0;

void TIM8_UP_TIM13_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM13, TIM_IT_CC1) != RESET)
    {
        if (isCaptured == 0) // 捕获上升沿
        {
            T1 = TIM_GetCapture1(TIM13); // 获取 T1
            TIM_OC1PolarityConfig(TIM13, TIM_ICPolarity_Falling); // 切换到下降沿
            isCaptured = 1;
        }
        else if (isCaptured == 1) // 捕获下降沿
        {
            T2 = TIM_GetCapture1(TIM13); // 获取 T2
            TIM_OC1PolarityConfig(TIM13, TIM_ICPolarity_Rising); // 切换回上升沿
            isCaptured = 2; // 捕获完成
        }

        TIM_ClearITPendingBit(TIM13, TIM_IT_CC1); // 清除中断标志
    }
}

float Calculate_Distance(void)
{
    uint32_t pulse_width;

    // 发送 TRIG 信号
    GPIO_SetBits(GPIOC, GPIO_Pin_0); // 设置 TRIG 为高电平
    delay_us(10);                    // 延时 10 微秒
    GPIO_ResetBits(GPIOC, GPIO_Pin_0); // 设置 TRIG 为低电平

    // 等待捕获完成
    while (isCaptured != 2);

    // 计算脉冲宽度
    if (T2 >= T1)
        pulse_width = T2 - T1;
    else
        pulse_width = (0xFFFFFFFF - T1 + T2);

    // 重置捕获标志
    isCaptured = 0;

    // 计算距离 (单位：cm)
    float distance = (pulse_width * 0.034) / 2.0;
    return distance;
}

