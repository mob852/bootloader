#include "HC_SR04.h"


/***
��ʼ������ߵ�ƽ������

***/

void GPIO_Init_TRIG(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ�� GPIOC ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // ���� PC0 Ϊ��ͨ���ģʽ
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

    // ʹ�� GPIOA �� TIM13 ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);

    // ���� PA6 Ϊ TIM13_CH1 ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ӳ�� PA6 �� TIM13_CH1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM13);

    // ���� TIM13 ��������
    TIM_BaseStruct.TIM_Prescaler = 84 - 1;  // 1 MHz ����Ƶ��
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseStruct.TIM_Period = 0xFFFF; // ������ֵ��16 λ��ʱ����
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM13, &TIM_BaseStruct);

    // ���� TIM13 ���벶��
    TIM_ICStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Rising; // ����������
    TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1; // ����Ƶ
    TIM_ICStruct.TIM_ICFilter = 0;
    TIM_ICInit(TIM13, &TIM_ICStruct);

    // �����ж�
    TIM_ITConfig(TIM13, TIM_IT_CC1, ENABLE);

    // ���� TIM13
    TIM_Cmd(TIM13, ENABLE);

    // ���� NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn; // TIM13 ʹ�� TIM8_UP_TIM13 �ж�
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


void HC_SR04_Init(void){
    // ��ʼ�� TRIG �� ECHO
    GPIO_Init_TRIG();
    TIM_ICU_Init();	
		
}

volatile uint32_t T1 = 0, T2 = 0;
volatile uint8_t isCaptured = 0;

void TIM8_UP_TIM13_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM13, TIM_IT_CC1) != RESET)
    {
        if (isCaptured == 0) // ����������
        {
            T1 = TIM_GetCapture1(TIM13); // ��ȡ T1
            TIM_OC1PolarityConfig(TIM13, TIM_ICPolarity_Falling); // �л����½���
            isCaptured = 1;
        }
        else if (isCaptured == 1) // �����½���
        {
            T2 = TIM_GetCapture1(TIM13); // ��ȡ T2
            TIM_OC1PolarityConfig(TIM13, TIM_ICPolarity_Rising); // �л���������
            isCaptured = 2; // �������
        }

        TIM_ClearITPendingBit(TIM13, TIM_IT_CC1); // ����жϱ�־
    }
}

float Calculate_Distance(void)
{
    uint32_t pulse_width;

    // ���� TRIG �ź�
    GPIO_SetBits(GPIOC, GPIO_Pin_0); // ���� TRIG Ϊ�ߵ�ƽ
    delay_us(10);                    // ��ʱ 10 ΢��
    GPIO_ResetBits(GPIOC, GPIO_Pin_0); // ���� TRIG Ϊ�͵�ƽ

    // �ȴ��������
    while (isCaptured != 2);

    // ����������
    if (T2 >= T1)
        pulse_width = T2 - T1;
    else
        pulse_width = (0xFFFFFFFF - T1 + T2);

    // ���ò����־
    isCaptured = 0;

    // ������� (��λ��cm)
    float distance = (pulse_width * 0.034) / 2.0;
    return distance;
}

