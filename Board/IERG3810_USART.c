#include <IERG3810_USART.h>
#include <stm32f10x.h>
#include <IERG3810_util.h>

void IERG3810_USART_init(void) {
	RCC->APB2ENR |= 0x00004004;
	RCC->APB1ENR |= 0x00020000;
	GPIOA->CRH &= 0xFFFFF00F;
	GPIOA->CRH |= 0x000008B0;
	GPIOA->CRL &= 0xFFFF00FF;
	GPIOA->CRL |= 0x00008B00;
	RCC->APB2RSTR |= 0x00004000;
	RCC->APB2RSTR &= 0xFFFFBFFF;
	RCC->APB1RSTR |= 0x00020000;
	RCC->APB1RSTR &= 0xFFFDFFFF;
	float tmp;
	u32 mantissa, frac;
	
	tmp = 72e6 / (9600 * 16);
	mantissa = tmp;
	frac = (tmp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += frac;
	USART1->BRR = mantissa;
	
	tmp = 36e6 / (9600 * 16);
	mantissa = tmp;
	frac = (tmp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += frac;
	USART2->BRR = mantissa;
	
	USART1->CR1 |= 0x00002008;
	USART2->CR1 |= 0x00002008;
}

void IERG3810_USART_print_str(int id, char * const str) {
	for (int i = 0; str[i]; i++) {
		USART_TypeDef * USART = id == 1? USART1 : USART2;
		USART->DR = str[i];
		while (!(USART->SR >> 7 & 0x00000001)) {}
	}
}

void IERG3810_USART_print_int(int id, int x) {
	char tmp[99];
	IERG3810_to_str(tmp, x);
	IERG3810_USART_print_str(id, tmp);
}
