#ifndef KEY_H
#define KEY_H

#define KeyState    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)

enum mode
{
  DUTYMODE,   //占空比修改模式
  PERIODMODE  //周期修改模式
};

void KeyScan(void);

#endif
