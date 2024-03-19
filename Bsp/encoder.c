#include "encoder.h"
#include "stm32f10x.h"

extern int16_t ntime;
extern int8_t *P;

/// @brief 编码器初始化-TIM3
/// @param  
void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //TIM时基单元初始化
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 1-1;//PSC预分频器值：0
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//在这里不起作用，因为计数方向受编码器接口托管
    TIM_TimeBaseStructure.TIM_Period = 65536-1;//计数器自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频：不分频
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    //TIM输入捕获单元初始化
    TIM_ICInitTypeDef TIM_ICInitStructure;
	  TIM_ICStructInit(&TIM_ICInitStructure);//该结构体最后两个参数在此无用处 因为这里只需配置滤波器和极性选择 但删除其他两个参数后可能显示结构体不完整 所以调用此函数给结构体赋一个初始值
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1|TIM_Channel_2;//配置通道
    TIM_ICInitStructure.TIM_ICFilter = 0xF;//配置滤波器
    // TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//此处配置为反相 在编码器模式下不代表检测边缘极性，而是代表高低电平极性是否反转，因为编码器模式下上升下降边缘都有效
    // TIM_ICInitStructure.TIM_ICSelection = 
    // TIM_ICInitStructure.TIM_ICPrescaler = 
    TIM_ICInit(TIM3,&TIM_ICInitStructure);

    //编码器接口配置    TIM3/IC1计数/IC1极性反转/IC2极性不反转
    //此函数第3和第4个传参配置的寄存器与输入捕获单元TIM_ICInitStructure.TIM_ICPolarity配置的寄存器相同，属于覆盖配置，可不配置捕获单元寄存器，而在此处配置极性寄存器
    TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI1,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);
    //NVIC
    // TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);//使能TIM1溢出中断
    // NVIC_InitTypeDef NVIC_InitStructure;
    // NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3,ENABLE);
}

/// @brief TIM2初始化-为编码器测速提供定时中断
void TIM_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    //TIM时基单元   (PSC+1)*(CNT+1)/PCLK = 10ms
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//PSC预分频器值：0
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
    TIM_TimeBaseStructure.TIM_Period = 10000-1;//计数器自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频：不分频
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
    //NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearFlag(TIM2,TIM_FLAG_Update);//清除计数器中断标志
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能计数器中断
    TIM_Cmd(TIM2,ENABLE);
}

/// @brief 获取编码器计数值并返回
/// @param  无
/// @return 编码器计数值
int16_t GetEncoderCnt(void)
{
    int16_t CNT;
    CNT = TIM_GetCounter(TIM3);
    TIM_SetCounter(TIM3,0); //清零cnt值
    return CNT;
}

/// @brief TIM2中断服务函数 10ms中断
/// @param  无
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
    {
        //此处用来测试TIM2中断周期
        (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_8)==RESET)?GPIO_SetBits(GPIOA,GPIO_Pin_8):GPIO_ResetBits(GPIOA,GPIO_Pin_8);
        //获取中断周期内计数器计数值，在主函数内除以中断周期得到速度
        ntime = GetEncoderCnt();
        *P += ntime;
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }
}
