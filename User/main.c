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
#include <stdio.h>

static bool SETUP_FINISHED = false;

static u16 const BG_COLOR = 0x877F;
static u16 const WOOD_COLOR = 0xB160;
static u16 const MYSELF_COLOR = 0x0E46;

// Game stuff.
static int const LAND_WIDTH = 55;
static int const DROP_Y = 5;
static bool lose = 0;
static int score = 0;
static char score_str[5];

// Wood stuff.
static int const WOOD_GAP = 10;
static int const WOOD_WIDTH = 20, WOOD_LENGTH = 60;
static int const MAX_NUM_WOOD = 10;
static int x[MAX_NUM_WOOD], y[MAX_NUM_WOOD];
static int woodIndex = 1;

static void createNewWood(int i) {
	int prev = (i + MAX_NUM_WOOD - 1)% MAX_NUM_WOOD;
	if (x[prev] == 55)
			x[i] = 81;
	else if (x[prev] == 159)
			x[i] = 133;
	else 
	{
			if (rand() % 2 == 1)
				x[i] = x[prev] + 26;
			else 
				x[i] = x[prev] - 26;
	}
	y[i] = V - WOOD_LENGTH;
}

static void drawWood(int i) {
	for (int j = 0; j < WOOD_WIDTH; j++){
		if (x[i] + j < 0)
			continue;
		for (int k = 0; k < WOOD_LENGTH; k++){
			if (y[i] + k < 0)
				continue;
			IERG3810_LCD_draw_dot(x[i] + j, y[i] + k, WOOD_COLOR);
		}
	}
}

static void clearWood(int i) {
	IERG3810_LCD_draw_rect(x[i], y[i] + WOOD_LENGTH - DROP_Y, WOOD_WIDTH, DROP_Y, BG_COLOR);
}


// Myself stuff.
static int const MYSELF_X = 12, MYSELF_Y = 20;
static int currentWood = 0;

static void drawMyself() {
	for (int i = 0; i < MYSELF_X; i++){
			for (int j = 0; j < MYSELF_Y; j++){
				IERG3810_LCD_draw_dot(x[currentWood] + 4 + i, y[currentWood] + 40 + j, MYSELF_COLOR);
			}
	}
}
static void clearMyself() {
	for (int i = 0; i < MYSELF_X; i++){
			for (int j = 0; j < MYSELF_Y; j++){
				IERG3810_LCD_draw_dot(x[currentWood] + 4 + i, y[currentWood] + 40 + j, BG_COLOR);
			}
	}
}

static void gameInit() {
	score = 0;
	char score_str[5] = "";
	currentWood = 0;
	IERG3810_LCD_draw_rect(0, 0, H, V, BG_COLOR);
	IERG3810_LCD_draw_str(H-40,V-16,"score",0x0000,BG_COLOR);
	sprintf(score_str, "%d",score);
	IERG3810_LCD_draw_str(H-26,V-32,score_str,0x0000,BG_COLOR);
	for( int i = 0; i < MAX_NUM_WOOD; i++)
	{
			x[i] = -1000;
			y[i] = -1000;
	}
	woodIndex = 1;
	x[0] = LAND_WIDTH + (rand() % 5) * 26;
	y[0] = V - WOOD_LENGTH - 30;
	SETUP_FINISHED = true;
}

void TIM3_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(3);
	if (!SETUP_FINISHED) {
		return;
	}
	if (!lose)
	{
		int maxY = 0;
		//clearMyself();
		for (int i = 0; i < MAX_NUM_WOOD; i++) {
			clearWood(i);
			y[i] -= DROP_Y;
			if (y[i] > -WOOD_LENGTH) {
				drawWood(i);
				if (y[i] > maxY) {
					maxY = y[i];
				}
			}else if (i == currentWood)
			{
				lose = true;
			}
		}
		drawMyself();
		IERG3810_LCD_draw_rect(H-26,V-32, 8, 16, BG_COLOR);
		sprintf(score_str, "%d",score);
		IERG3810_LCD_draw_str(H-26,V-32,score_str,0x0000,BG_COLOR);
		if (maxY < V - WOOD_GAP - WOOD_LENGTH * 2) {
			createNewWood(woodIndex);
			//drawWood(woodIndex);
			maxY = y[woodIndex];
			woodIndex = (woodIndex + 1) % MAX_NUM_WOOD;
		}
	}
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
	gameInit();
	while (true) {
		while (!lose)
		{
			char res = IERG3810_PS2_get();
			if (res == '4') {
				if (x[currentWood] < x[(currentWood + 1) % 10] || y[currentWood] > V - WOOD_GAP - WOOD_LENGTH * 2)
				{
					lose = true;
					
				}
				else 
				{
					score++;
					currentWood = (currentWood + 1) % MAX_NUM_WOOD;
				}
				IERG3810_LED_toggle(0);
			} else if (res == '6') {
					if (x[currentWood] > x[(currentWood + 1) % 10] || y[currentWood] > V - WOOD_GAP - WOOD_LENGTH * 2)
					{
						lose = true;
					}
					else 
					{
						score++;
						currentWood = (currentWood + 1) % MAX_NUM_WOOD;
					}
					IERG3810_LED_toggle(1);
			}
		}
		while(lose)
		{
			char res = IERG3810_PS2_get();
			if (res == '+')
			{
				SETUP_FINISHED = false;
				lose = 0;
			}
		}
		gameInit();
	}
}

