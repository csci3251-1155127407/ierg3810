#include <IERG3810_clock.h>
#include <stm32f10x.h>

void IERG3810_clock_init(void) {
	RCC->CFGR &= 0xF8FF0000;
	RCC->CR &= 0xFEF6FFFF;
	RCC->CR |= 0x00010000;
	while (!(RCC->CR >> 17 & 0x00000001)) {}
	RCC->CFGR = 0x001D0400;
	FLASH->ACR |= 0x00000030;
	RCC->CR |= 0x01000000;
	while (!(RCC->CR >> 25 & 0x00000001)) {}
	RCC->CFGR |= 0x00000002;
	for (u32 i = 0; i != 2;) {
		i = RCC->CFGR >> 2 & 0x00000003;
	}
}
