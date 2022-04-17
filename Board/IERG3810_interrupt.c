#include <IERG3810_interrupt.h>
#include <stm32f10x.h>
#include <stdbool.h>
#include <IERG3810_util.h>

static int ps2Cnt;
static u8 ps2Save;

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
	NVIC->IP[40] = 0xF0;
	NVIC->ISER[1] |= 0x00000100;
}

void IERG3810_interrupt_init(void) {
	nvicInit();
	ps2Init();
}

void EXTI15_10_IRQHandler(void) {
	EXTI->PR = 0x00000800;
	int bit = GPIOC->IDR >> 10 & 0x00000001;
	if (ps2Cnt >= 1 && ps2Cnt <= 8) {
		ps2Save |= bit << ps2Cnt - 1;
	}
	ps2Cnt++;
	IERG3810_delay(10); // Needed for some reason...
}

static char decode(int ps2Code) {
	if (ps2Code == 0x71) {return '.';}
	if (ps2Code == 0x5A) {return 'E';}
	if (ps2Code == 0x79) {return '+';}
	if (ps2Code == 0x7B) {return '-';}
	if (ps2Code == 0x7C) {return '*';}
	if (ps2Code == 0x4A) {return '/';}
	if (ps2Code == 0x70) {return '0';}
	if (ps2Code == 0x69) {return '1';}
	if (ps2Code == 0x72) {return '2';}
	if (ps2Code == 0x7A) {return '3';}
	if (ps2Code == 0x6B) {return '4';}
	if (ps2Code == 0x73) {return '5';}
	if (ps2Code == 0x74) {return '6';}
	if (ps2Code == 0x6C) {return '7';}
	if (ps2Code == 0x75) {return '8';}
	if (ps2Code == 0x7D) {return '9';}
	return '\0';
}

char IERG3810_PS2_get(void) {
	static u8 ps2Last;
	if (ps2Cnt >= 11) {
		int res = -1;
		if (ps2Last != 0xF0) {
			res = ps2Save;
		}
		ps2Last = ps2Save;
		ps2Cnt = 0;
		ps2Save = 0;
		return decode(res);
	}
	return '\0';
}
