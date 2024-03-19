#include "key.h"
#include "bsp.h"
#include "stm32f10x.h"

enum mode MODE;

/// @brief 键盘扫描 按键按下LED翻转电平
/// @param  
void KeyScan(void)
{
    if (KeyState == 0)
    {
        delay_ms(100);//消抖
        if(KeyState == 0) 
        {
            MODE++;
            if(MODE>PERIODMODE)
            {
              MODE = DUTYMODE;
            }
        }
    }
    
}
