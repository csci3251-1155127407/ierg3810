#include <IERG3810_LED.h>
#include <stm32f10x.h>

void IERG3810_LED_init(void) {
	RCC->APB2ENR |= 0x00000048;
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;
	GPIOB->BSRR = 0x00000020;
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000;
	GPIOE->BSRR = 0x00000020;
}

void IERG3810_LED_set(int id, bool on) {
	if (id == 0) {
		if (on) {
			GPIOB->BRR = 0x00000020;
		} else {
			GPIOB->BSRR = 0x00000020;
		}
	} else {
		if (on) {
			GPIOE->BRR = 0x00000020;
		} else {
			GPIOE->BSRR = 0x00000020;
		}
	}
}

void IERG3810_LED_toggle(int id) {
	if (id == 0) {
		GPIOB->ODR ^= 0x00000020;
	} else {
		GPIOE->ODR ^= 0x00000020;
	}
}
