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
#include <IERG3810_assets.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern u32 IERG3810_SysTick_cntr;

static bool SETUP_FINISHED = false;
static bool HOME_SCREEN = true;

static u16 const BG_COLOR = 0x877F;
//static u16 const WOOD_COLOR = 0xB160;

// Game stuff.
static int const LAND_WIDTH = 45;
static int const DROP_Y = 5;
static bool lose = 0, started = false;
static int score = 0;
static char score_str[5];

void drawEndGameMessage (void) {
	char const * strs[2] = {
		"You Lose!",
		"Press ENTER to restart"
	};
	for (int i = 0; i < 2; i++) {
		IERG3810_LCD_draw_str((H-strlen(strs[i])*8)/2, V-40-i*16,strs[i],0x0000,-1);
	}
}

// Wood stuff.
static int const WOOD_GAP_X = 5;
static int const WOOD_GAP_Y = 10;
static int const WOOD_WIDTH = 25, WOOD_LENGTH = 60;
static int const MAX_NUM_WOOD = 10;
static int x[MAX_NUM_WOOD], y[MAX_NUM_WOOD];
static int woodIndex = 1;

static void createNewWood(int i) {
	int prev = (i + MAX_NUM_WOOD - 1)% MAX_NUM_WOOD;
	if (x[prev] == LAND_WIDTH)
			x[i] = LAND_WIDTH + WOOD_WIDTH + WOOD_GAP_X;
	else if (x[prev] == LAND_WIDTH + (WOOD_WIDTH + WOOD_GAP_X)* 4)
			x[i] = x[prev] - WOOD_WIDTH - WOOD_GAP_X;
	else 
	{
			if (rand() % 2 == 1)
				x[i] = x[prev] + WOOD_WIDTH + WOOD_GAP_X;
			else 
				x[i] = x[prev] - WOOD_WIDTH - WOOD_GAP_X;
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
			if (wood[59 - k][j] == 0xffff)
				IERG3810_LCD_draw_dot(x[i] + j, y[i] + k, BG_COLOR);
			else 
				IERG3810_LCD_draw_dot(x[i] + j, y[i] + k, wood[59 - k][j]);
		}
	}
}

static void clearWood(int i) {
	if (y[i] + WOOD_LENGTH >= 0)
		IERG3810_LCD_draw_rect(x[i], y[i] + WOOD_LENGTH - DROP_Y, WOOD_WIDTH, DROP_Y, BG_COLOR);
}


// Myself stuff.
static int const MYSELF_WIDTH = 21, MYSELF_LENGTH = 21;
static int const MYSELF_X = 2 , MYSELF_Y = 35; // pos on the wood
static int myself_x, myself_y; // pos of the frog

static int currentWood = 0;

static void drawMyself() {
	for (int i = 0; i < MYSELF_WIDTH; i++){
			for (int j = 0; j < MYSELF_LENGTH; j++){
				if (myself_y + j < 0 || frog[20 - j][i] == 0xffff)
					continue;
				IERG3810_LCD_draw_dot(myself_x + i, myself_y + j, frog[20 - j][i]);
			}
	}
}

static void clearMyself() {
	for (int i = 0; i < MYSELF_WIDTH; i++){
			for (int j = 0; j < MYSELF_LENGTH; j++){
				if (myself_y + j >= 0)
					IERG3810_LCD_draw_dot(myself_x + i, myself_y + j, BG_COLOR);
			}
	}
}

static void gameInit() {
	score = 0;
	lose = false;
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
	x[0] = LAND_WIDTH + (rand() % 5) * (WOOD_WIDTH + WOOD_GAP_X);
	y[0] = V - WOOD_LENGTH - 30;
	SETUP_FINISHED = true;
	IERG3810_SysTick_cntr=0;
}

void TIM3_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(3);
	if (!SETUP_FINISHED) {
		return;
	}
	if (!started) {
		return;
	}
	
	if (started)
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
		if (!lose)
		{
			myself_x = x[currentWood] + MYSELF_X;
			myself_y = y[currentWood] + MYSELF_Y;
		}
		else 
		{
			drawEndGameMessage();
			clearMyself();
			myself_y -= DROP_Y;
		}
		drawMyself();
		IERG3810_LCD_draw_rect(H-26,V-32, 8, 16, BG_COLOR);
		sprintf(score_str, "%d",score);
		IERG3810_LCD_draw_str(H-26,V-32,score_str,0x0000,BG_COLOR);
		if (maxY < V - WOOD_GAP_Y - WOOD_LENGTH * 2 && !lose) {
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

	if (HOME_SCREEN) {
		if (ps2GetChar() == 'E') {
			srand(IERG3810_SysTick_cntr);
			HOME_SCREEN = false;
			return;
		}
		return;
	}
	
	if (!started)
	{
		return;
	}
	
	if (!lose)
	{
		char res = ps2GetChar();
		if (res == '4') {
			if (y[currentWood] > V - WOOD_GAP_Y - WOOD_LENGTH * 2 || x[currentWood] == LAND_WIDTH)
			{
				// do nothing
			}
			else if (x[currentWood] < x[(currentWood + 1) % 10])
			{
				lose = true;
				myself_x = x[currentWood] - WOOD_GAP_X - WOOD_WIDTH;
				myself_y = y[currentWood] + WOOD_GAP_Y + WOOD_LENGTH + MYSELF_Y;
			}
			else 
			{
				score++;
				currentWood = (currentWood + 1) % MAX_NUM_WOOD;
			}
			IERG3810_LED_toggle(0);
		} else if (res == '6') {
				if (x[currentWood] == LAND_WIDTH + 4 * (WOOD_WIDTH + WOOD_GAP_X)|| y[currentWood] > V - WOOD_GAP_Y - WOOD_LENGTH * 2 )
				{
					// do nothing
				}
				else if (x[currentWood] > x[(currentWood + 1) % 10] )
				{
					lose = true;
					myself_x = x[currentWood] + WOOD_GAP_X + WOOD_WIDTH;
					myself_y = y[currentWood] + WOOD_GAP_Y + WOOD_LENGTH + MYSELF_Y;
				}
				else 
				{
					score++;
					currentWood = (currentWood + 1) % MAX_NUM_WOOD;
				}
				IERG3810_LED_toggle(1);
		}
	}
	if(lose)
	{
		char res = ps2GetChar();
		if (res == 'E')
		{
			SETUP_FINISHED = false;
			lose = 0;
		}
	}
}

void drawStartScreen(void) {
	IERG3810_LCD_draw_rect(0, 0, H, V, 0xaff4);
	char const * strs[6] = {
		"I E   F r o g",
		"Gp15 1155127407 | 1155137344",
		"Instructions",
		"LEFT to jump left",
		"RIGHT to jump right",
		"ENTER to start"
	};
	for (int i = 0; i < 2; i++) {
		IERG3810_LCD_draw_str((H-strlen(strs[i])*8)/2, V-40-i*16,strs[i],0x0000,-1);
	}
	int const SCALE = 8;
	for (int ii = 0; ii < 21*SCALE; ii++) {
		for (int jj = 0; jj < 21*SCALE; jj++) {
			int i = ii / SCALE, j = jj / SCALE;
			if (frog[20 - j][i] == 0xffff) {
				IERG3810_LCD_draw_dot(ii+(H-21*8)/2, 83+jj, 0xaff4);
			} else {
				IERG3810_LCD_draw_dot(ii+(H-21*8)/2, 83+jj, frog[20 - j][i]);
			}
		}
	}
	for (int i = 3; i < 6; i++) {
		IERG3810_LCD_draw_str((H-strlen(strs[i])*8)/2, 97-i*16,strs[i],0x0000,-1);
	}
}

void drawCountDownScreen (void) {
	// Does not work for some reason
	IERG3810_LCD_draw_rect(0, 0, H, V, 0xaff4);
	IERG3810_LCD_draw_str((H-8)/2,(V-16)/2,"3",0x0000,-1);
	//for (int i = 0; i < 5000000; i++);
	IERG3810_LCD_draw_rect(0, 0, H, V, 0xaff4);
	IERG3810_LCD_draw_str((H-8)/2,(V-16)/2,"2",0x0000,-1);
	//for (int i = 0; i < 5000000; i++);
	IERG3810_LCD_draw_rect(0, 0, H, V, 0xaff4);
	IERG3810_LCD_draw_str((H-8)/2,(V-16)/2,"1",0x0000,-1);
	//for (int i = 0; i < 5000000; i++); 
	IERG3810_LCD_draw_rect(0, 0, H, V, 0xaff4);
	IERG3810_LCD_draw_str((H-8*8)/2,(V-16)/2,"Have Fun!",0x0000,-1);
	//for (int i = 0; i < 500000; i++);
	// Does not work for some reason END
}

int main() {
	IERG3810_KEY_init();
	IERG3810_LED_init();
	IERG3810_buzzer_init();
	IERG3810_clock_init();
	IERG3810_SysTick_init(1);
	IERG3810_USART_init();
	IERG3810_LCD_init();
	IERG3810_interrupt_init();
	IERG3810_TIM3_init(7199, 4199);
	IERG3810_TIM4_init(7199, 4199);
	IERG3810_delay(100000);
	IERG3810_LCD_draw_rect(0, 0, H, V, 0x0000);
	
	gameInit();
	u16 psc = 4200;

	drawStartScreen();
	
	while (true) {
		while (HOME_SCREEN) {}
		//drawCountDownScreen();
		gameInit();
		started = true;
		while (!lose) {
			IERG3810_TIM3_init(7199, psc / (8 + IERG3810_SysTick_cntr / 2000) - 1);
		}
		while (lose) {}
	}
}

