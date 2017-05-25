#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_
#include "includes.h"

void SysTickConfig(void);
void Delay_us(u32 time);
void Delay_ms(u32 time);

void SysTick_Handler(void);

#endif
