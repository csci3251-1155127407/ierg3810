#include <stm32f10x.h>
#include <stdbool.h>
#include <IERG3810_LED.h>
#include <IERG3810_KEY.h>
#include <IERG3810_buzzer.h>
#include <IERG3810_clock.h>
#include <IERG3810_USART.h>
#include <IERG3810_LCD.h>
#include <IERG3810_util.h>
#include <IERG3810_interrupt.h>
#include <IERG3810_timer.h>
#include <stdlib.h>
#include <string.h>

static bool SETUP_FINISHED = false;

static u16 const BG_COLOR = 0x877F;
/*
static u16 const WOOD_COLOR = 0xB160;
static u16 const WOOD_X = 80, WOOD_Y = 15;
static u16 const MYSELF_COLOR = 0x0E46;
static u16 const MYSELF_X = 40, MYSELF_Y = 40;
static int const DROP_Y = 2;

static int const WOOD_GAP_Y = 110;
static int const WOOD_BORDER_X = 3;
static int const MYSELF_MOVE_X = 20, MYSELF_MOVE_Y = 20;
*/

static int ps2Cnt;
static u8 ps2Save;

static char ps2Decode(int ps2Code) {
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

static char ps2GetChar(void) {
	static u8 ps2Last;
	if (ps2Cnt >= 11) {
		int res = -1;
		if (ps2Last != 0xF0) {
			res = ps2Save;
		}
		ps2Last = ps2Save;
		ps2Cnt = 0;
		ps2Save = 0;
		return ps2Decode(res);
	}
	return '\0';
}

void EXTI15_10_IRQHandler(void) {
	IERG3810_PS2_clear_interrupt();
	int bit = IERG3810_PS2_bit_get();
	if (ps2Cnt >= 1 && ps2Cnt <= 8) {
		ps2Save |= bit << ps2Cnt - 1;
	}
	ps2Cnt++;
	IERG3810_delay(10); // Needed for some reason...
	
	char res = ps2GetChar();
	if (res == '4') {
		// TODO
		IERG3810_LED_toggle(0);
	} else if (res == '6') {
		// TODO
		IERG3810_LED_toggle(1);
	}
}

void TIM3_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(3);
	if (!SETUP_FINISHED) {
		return;
	}
	
	// TODO
}

void TIM4_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(4);
}

int main() {
	IERG3810_KEY_init();
	IERG3810_LED_init();
	IERG3810_buzzer_init();
	IERG3810_clock_init();
	IERG3810_USART_init();
	IERG3810_LCD_init();
	IERG3810_interrupt_init();
	IERG3810_TIM3_init(7199, 524);
	IERG3810_TIM4_init(7199, 4199);
	IERG3810_SysTick_init(10);
	IERG3810_delay(100000);
	IERG3810_LCD_draw_rect(0, 0, X, Y, 0x0000);
	srand(3810);
	
	IERG3810_LCD_draw_rect(0, 0, X, Y, BG_COLOR);
	SETUP_FINISHED = true;
	
	while (true) {}
}

