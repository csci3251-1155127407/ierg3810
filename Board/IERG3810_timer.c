#include <IERG3810_timer.h>

void IERG3810_TIM3_init(u16 arr, u16 psc) {
	RCC->APB1ENR |= 0x00000002;
	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM3->DIER |= 0x00000001;
	TIM3->CR1 |= 0x00000001;
	NVIC->IP[29] = 0xF0;
	NVIC->ISER[0] |= 0x20000000;
}

void IERG3810_TIM4_init(u16 arr, u16 psc) {
	RCC->APB1ENR |= 0x00000004;
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM4->DIER |= 0x00000001;
	TIM4->CR1 |= 0x00000001;
	NVIC->IP[30] = 0xF0;
	NVIC->ISER[0] |= 0x40000000;
}

void IERG3810_TIM_clear_interrupt(int id) {
	if (id == 3) {
		TIM3->SR &= 0xFFFFFFFE;
	} else {
		TIM4->SR &= 0xFFFFFFFE;
	}
}

void IERG3810_SysTick_init(int ms) {
	SysTick->CTRL = 0x00000003;
	SysTick->LOAD = (u32)(9e6 / 1000 * ms) - 1; // Cortex System timer = 72 / 8 MHz
}
