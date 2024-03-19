#include "stm32f10x.h"
#include "pwm.h"

void PWM_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    //GPIO 使用TIM1的CH1(PA8)和CH1N(PB13)组成互补通道，使用TIM1的BKIN(PB12)作为刹车
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//TIM1_CH1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//TIM1_CH1N
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//TIM1_BKIN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    //TIM1时基单元      F=72000000/1/7200=10KHz  T=100us
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 1-1;//PSC
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
    TIM_TimeBaseStructure.TIM_Period = 7200-1;//ARP
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数器的值，没用到不用管
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
    //TIM1输出比较单元      n=3600/7200=50% T=0.5*100us=50us
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM1模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//PWM输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//PWM互补输出使能
    TIM_OCInitStructure.TIM_Pulse = 3600-1;//占空比CCR值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出通道电平极性
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//互补输出通道电平极性
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//输出通道空闲电平极性
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;//互补输出通道空闲电平极性
    TIM_OC1Init(TIM1,&TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//使能CCR1上的TIM外围预加载寄存器
    //TIM1刹车和死区单元
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel =TIM_LOCKLevel_1;
    //输出死区时间配置计算  10us
    //0xED=1110 1101  BIN 01101->DEC 13  T=(13+32)*16/72000000=10us
    //新占空比n=(50-10)/100=40%
    TIM_BDTRInitStructure.TIM_DeadTime = 0;//xED;
    //刹车配置
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;//使能刹车功能
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;//低电平刹车
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;//使能自动输出功能
    TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);

    TIM_ARRPreloadConfig(TIM1,ENABLE);

    TIM_Cmd(TIM1,ENABLE);

    //主输出使能，当使用的定时器为通用定时器时不需要该命令
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
}
