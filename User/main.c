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

static bool SETUP_FINISHED = false;

static u16 const BG_COLOR = 0x877F;
static u16 const WOOD_COLOR = 0xB160;
static u16 const MYSELF_COLOR = 0x0E46;

// Wood stuff.
static int const WOOD_GAP = 110;
static int const WOOD_X = 80, WOOD_Y = 15;
static int const MAX_NUM_WOOD = 10;
static int const BORDER_X = 3, DROP_Y = 2;
static int x[MAX_NUM_WOOD], y[MAX_NUM_WOOD];
static int woodIndex;
static void createNewWood(int i) {
	x[i] = IERG3810_rand(BORDER_X, H - BORDER_X - WOOD_X);
	y[i] = V - WOOD_Y;
}
static void drawWood(int i) {
	IERG3810_LCD_draw_rect(x[i], y[i], WOOD_X, WOOD_Y, WOOD_COLOR);
}
static void clearWood(int i) {
	IERG3810_LCD_draw_rect(x[i], y[i], WOOD_X, WOOD_Y, BG_COLOR);
}

// Myself stuff.
static int const MYSELF_X = 40, MYSELF_Y = 40;
static int const MOVE_X = 20, MOVE_Y = 20;
static int xMyself = (240 - MYSELF_X) / 2, yMyself = DROP_Y;
static void drawMyself() {
	IERG3810_LCD_draw_rect(xMyself, yMyself, MYSELF_X, MYSELF_Y, MYSELF_COLOR);
}
static void clearMyself() {
	IERG3810_LCD_draw_rect(xMyself, yMyself, MYSELF_X, MYSELF_Y, BG_COLOR);
}

void TIM3_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(3);
	if (!SETUP_FINISHED) {
		return;
	}
	int maxY = 0;
	for (int i = 0; i < MAX_NUM_WOOD; i++) {
		clearWood(i);
		y[i] -= DROP_Y;
		if (y[i] > 0) {
			drawWood(i);
			if (y[i] > maxY) {
				maxY = y[i];
			}
		}
	}
	if (maxY < V - WOOD_GAP) {
		createNewWood(woodIndex);
		drawWood(woodIndex);
		maxY = y[woodIndex];
		woodIndex = (woodIndex + 1) % MAX_NUM_WOOD;
	}
	
	clearMyself();
	yMyself -= DROP_Y;
	drawMyself();
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
	IERG3810_LCD_draw_rect(0, 0, H, V, 0x0000);
	srand(3810);
	
	IERG3810_LCD_draw_rect(0, 0, H, V, BG_COLOR);
	drawMyself();
	SETUP_FINISHED = true;
	
	while (true) {
		char res = IERG3810_PS2_get();
		if (res == '4') {
			clearMyself();
			xMyself -= MOVE_X;
			yMyself += MOVE_Y;
			drawMyself();
			IERG3810_LED_toggle(0);
		} else if (res == '6') {
			clearMyself();
			xMyself += MOVE_X;
			yMyself += MOVE_Y;
			drawMyself();
			IERG3810_LED_toggle(1);
		}
	}
}

