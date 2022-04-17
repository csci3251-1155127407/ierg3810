#include <IERG3810_buzzer.h>
#include <stm32f10x.h>

void IERG3810_buzzer_init(void) {
	RCC->APB2ENR |= 0x00000008;
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000003;
	GPIOB->BRR = 0x00000100;
}

void IERG3810_buzzer_set(bool on) {
	if (on) {
		GPIOB->BSRR = 0x00000100;
	} else {
		GPIOB->BRR = 0x00000100;
	}
}

void IERG3810_buzzer_toggle(void) {
	GPIOB->ODR ^= 0x00000100;
}
