#include "bsp.h"
#include "pwm.h"
#include "encoder.h"

/// @brief 系统时钟初始化72M
/// @param  
static void Clock_Init(void)
{
    //将外设RCC寄存器重设为缺省值
    RCC_DeInit();

    //使能HSE，开启外部晶振
    RCC_HSEConfig(RCC_HSE_ON);

    //等待HSE启动稳定
    if(RCC_WaitForHSEStartUp() == SUCCESS)
    {
        //使能FLASH预存取缓冲区(操作FLASH闪存时用到，不操作可将这两行注释)
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        //闪存访问时间 0:0~24M  1:24M~48M  2:48M~72M
        FLASH_SetLatency(FLASH_Latency_2);

        //AHB预分频因子设置为1分频，HCLK=SYSCLK
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        //APB1预分频因子设置为2，APB1最大36M
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //APB2预分频因子设置为1,APB2最大72M
        RCC_PCLK2Config(RCC_HCLK_Div1);

        //设置PLL时钟来源为分频为1的HSE,设置PLL倍频因子为9  8M/1*9=72M
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);

        //开启PLL
        RCC_PLLCmd(ENABLE);

        //等待PLL稳定
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {};

        //选择系统时钟源为PLL时钟
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //读系统时钟切换状态位，确保PLL被切换为系统时钟源
        while(RCC_GetSYSCLKSource() != 0x08) {};
    }
    
}

/// @brief LED相关GPIO初始化
/// @param  
static void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/// @brief 独立按键GPIO初始化
///        -按下为低电平 -抬起为高电平
/// @param  
static void KEY_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    //输入模式不需要配置速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;

    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/// @brief 系统初始化，包含时钟,GPIO等各部分外设的初始化
/// @param  
void System_Init(void)
{
    Clock_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // LED_GPIO_Init();
    KEY_GPIO_Init();
    PWM_Init();
    Encoder_Init();
    TIM_Config();
}

/// @brief 微妙延时
/// @param time 
void delay_us(__IO u32 time)
{    
   u32 i=0;  
   SysTick_Config(SystemCoreClock/1000000);

   for(i=0;i<time;i++)
   {
      //当计数器的值减小到0时,systick->CTRL的bit16会置1
      while(!(SysTick->CTRL&(1<<16)));
   }
   //关闭Systick定时器
   SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/// @brief 毫秒延时
/// @param time 
void delay_ms(__IO u32 time)
{    
   u32 i=0;  
   SysTick_Config(SystemCoreClock/1000);

    //每个循环为延时1ms
   for(i=0;i<time;i++)
   {
      //当计数器的值减小到0时,systick->CTRL的bit16会置1
      while(!(SysTick->CTRL&(1<<16)));
   }
   //关闭Systick定时器
   SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
