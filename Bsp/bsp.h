#ifndef BSP_H
#define BSP_H

#include "stm32f10x.h"

#define LED_OFF  GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define LED_ON  GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define LED_TOGGLE  GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_7)?LED_ON:LED_OFF


void System_Init(void);
void delay_us(__IO u32 time);
void delay_ms(__IO u32 time);

#endif
