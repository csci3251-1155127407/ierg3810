#include <IERG3810_interrupt.h>
#include <stm32f10x.h>
#include <stdbool.h>
#include <IERG3810_util.h>

static void nvicInit(void) {
	u32 aircr = SCB->AIRCR;
	aircr &= 0x0000F8FF;
	aircr |= 0x05FA0000;
	SCB->AIRCR = aircr;
}

static void ps2Init(void) {
	RCC->APB2ENR |= 0x00000011;
	GPIOA->CRH &= 0xFFFF00FF;
	GPIOA->CRH |= 0x00008800;
	GPIOA->BRR = 0x00000C00;
	AFIO->EXTICR[2] &= 0xFFFF0FFF;
	AFIO->EXTICR[2] |= 0x00002000;
	EXTI->IMR |= 0x00000800;
	EXTI->FTSR |= 0x00000800;
	NVIC->IP[40] = 0x80;
	NVIC->ISER[1] |= 0x00000100;
}

void IERG3810_interrupt_init(void) {
	nvicInit();
	ps2Init();
}

void IERG3810_PS2_clear_interrupt(void) {
	EXTI->PR = 0x00000800;
}

int IERG3810_PS2_bit_get(void) {
	return GPIOC->IDR >> 10 & 0x00000001;
}
