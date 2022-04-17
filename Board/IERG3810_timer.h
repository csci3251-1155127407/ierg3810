#pragma once

#include <stm32f10x.h>

void IERG3810_TIM3_init(u16 arr, u16 psc);
void IERG3810_TIM4_init(u16 arr, u16 psc);
void IERG3810_TIM_clear_interrupt(int id);
void IERG3810_SysTick_init(int ms);
