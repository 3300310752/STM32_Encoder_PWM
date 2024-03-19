#ifndef ENCODER_H
#define ENCODER_H

#include "stdint.h"

extern int16_t ntime;

void Encoder_Init(void);
void TIM_Config(void);
int16_t GetEncoderCnt(void);

#endif
