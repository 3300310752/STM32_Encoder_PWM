#include "stm32f10x.h"                  // Device header
#include "bsp.h"
#include "key.h"
#include "OLED.h"
#include "stdio.h"

RCC_ClocksTypeDef clk;//各时钟源频率结构体
uint16_t CCR;
int8_t Duty = 50;//Debug修改参数 50对应占空比50%
uint8_t ARP;
uint16_t PWM_Freq;
int8_t PWM_PSC = 1;//Debug修改参数 1对应100us
uint16_t string[20];
int8_t *P = &Duty; //目前只控制占空比
int16_t ntime;
extern enum mode MODE;

int main(void)
{
    System_Init();
    RCC_GetClocksFreq(&clk);//获取各时钟频率到clk结构体
    OLED_Init();
    while(1)
    {
      /* 修改模式控制和显示 */
      KeyScan();
      switch (MODE)
      {
      case DUTYMODE:
        P = &Duty;
        OLED_ShowString(3,6,"Duty  ");
        break;
      case PERIODMODE:
        P = &PWM_PSC;
        OLED_ShowString(3,6,"Period");
        break;
      default:
        P = &Duty;
        OLED_ShowString(3,6,"Duty");
        break;
      }
      OLED_ShowString(3,1,"Mode:");

      /* 占空比和周期范围限制 */
      if(Duty<=1) //占空比最低1%
      {
        Duty = 1;
      }
      else if (Duty>=99) //占空比最高99%
      {
        Duty = 99;
      }
      if (PWM_PSC<=1) //周期最低100us
      {
        PWM_PSC = 1;
      }
      else if (PWM_PSC>=20) //周期最高2ms
      {
        PWM_PSC = 20;
      }

      /* 占空比和周期控制 */
			CCR = 7200*Duty/100 - 1;
      TIM_SetCompare1(TIM1,CCR);//占空比重设置
      sprintf((char*)string,"Duty cycle:%d%% ",Duty);
      OLED_ShowString(1,1,(char*)string);

      TIM_PrescalerConfig(TIM1,(PWM_PSC-1),TIM_PSCReloadMode_Update);//预分频器值重设置
      sprintf((char*)string,"Peroid:%dus ",PWM_PSC*100);
      OLED_ShowString(2,1,(char*)string);
    }
}
