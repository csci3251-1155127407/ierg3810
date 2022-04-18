#include <IERG3810_LCD.h>
#include <IERG3810_FONTS.h>
#include <stdbool.h>
#include <IERG3810_util.h>

static u16 volatile
		* const LCD_CMD = (u16 *)0x6C000000,
		* const LCD_PRM = (u16 *)0x6C000800; // Parameter.

void IERG3810_LCD_init(void) {
	RCC->AHBENR |= 0x00000100;
	RCC->APB2ENR |= 0x00000168;
	GPIOB->CRL &= 0xFFFFFFF0;
	GPIOB->CRL |= 0x00000003;
	GPIOD->CRH &= 0x00FFF000;
	GPIOD->CRH |= 0xBB000BBB;
	GPIOD->CRL &= 0xFF00FF00;
	GPIOD->CRL |= 0x00BB00BB;
	GPIOE->CRH &= 0x00000000;
	GPIOE->CRH |= 0xBBBBBBBB;
	GPIOE->CRL &= 0x0FFFFFFF;
	GPIOE->CRL |= 0xB0000000;
	GPIOG->CRH &= 0xFFF0FFFF;
	GPIOG->CRH |= 0x000B0000;
	GPIOG->CRL &= 0xFFFFFFF0;
	GPIOG->CRL |= 0x0000000B;
	FSMC_Bank1->BTCR[6] = 0x00005011; // BCR
	FSMC_Bank1->BTCR[7] = 0x00000F01; // BTR
	FSMC_Bank1E->BWTR[6] = 0x00000300;
	
	*LCD_CMD = 0x01;
	*LCD_CMD = 0x11;
	*LCD_CMD = 0x3A;
	*LCD_PRM = 0x55;
	
	*LCD_CMD = 0x29;
	*LCD_CMD = 0x36;
	*LCD_PRM = 0xCA;
	
	GPIOB->BSRR = 0x00000001;
}

void IERG3810_LCD_draw_rect_init(u16 x, u16 y, u16 x_len, u16 y_len) {
	*LCD_CMD = 0x2A;
	*LCD_PRM = x >> 8 & 0xFF;
	*LCD_PRM = x & 0xFF;
	*LCD_PRM = x + x_len - 1 >> 8 & 0xFF;
	*LCD_PRM = x + x_len - 1 & 0xFF;
	
	*LCD_CMD = 0x2B;
	*LCD_PRM = y >> 8 & 0xFF;
	*LCD_PRM = y & 0xFF;
	*LCD_PRM = y + y_len - 1 >> 8 & 0xFF;
	*LCD_PRM = y + y_len - 1 & 0xFF;
	
	*LCD_CMD = 0x2C;
}

void IERG3810_LCD_draw_dot(u16 x, u16 y, u16 color, bool init) {
	if (init) {
		IERG3810_LCD_draw_rect_init(x, y, X - x, Y - y);
	}
	*LCD_PRM = color;
}

void IERG3810_LCD_draw_rect(int x, int y, u16 x_len, u16 y_len, u16 color) {
	if (x < 0) {
		x_len -= -x;
		x = 0;
	}
	if (y < 0) {
		y_len -= -y;
		y = 0;
	}
	x_len = IERG3810_min(x_len, X - x);
	y_len = IERG3810_min(y_len, Y - y);
	IERG3810_LCD_draw_rect_init(x, y, x_len, y_len);
	for (int i = 0; i < x_len; i++) {
		for (int j = 0; j < y_len; j++) {
			*LCD_PRM = color;
		}
	}
}

static inline bool withinScreen(u16 x, u16 y) {
	return x < X && y < Y;
}

u16 IERG3810_LCD_draw_char(u16 x, u16 y, char c, u16 color, u32 bgColor) {
	int const X_LEN = 8, Y_LEN = 16;
	if (!withinScreen(x + X_LEN - 1, y + Y_LEN - 1)) {
		return x;
	}
	bool const transparent = bgColor > 0xFFFF;
	if (!transparent) {
		IERG3810_LCD_draw_rect_init(x, y, X_LEN, Y_LEN);
	}
	
	u16 x_now = x, y_now = y;
	for (int k = Y_LEN / 8 - 1; k >= 0; k--) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < X_LEN; j++) {
				if (FONTS[c - ' '][j * 2 + k] >> i & 1) {
					if (!transparent) {
						*LCD_PRM = color;
					} else {
						IERG3810_LCD_draw_dot(x_now, y_now, color, false);
					}
				} else {
					if (!transparent) {
						*LCD_PRM = bgColor;
					}
				}
				x_now++;
				if (x_now == x + X_LEN) {
					x_now = x;
					y_now++;
				}
			}
		}
	}
	return x + X_LEN;
}

u16 IERG3810_LCD_draw_str(u16 x, u16 y, char * const str, u16 color, u32 bgColor) {
	int x_now = x;
	for (int i = 0; str[i]; i++) {
		x_now = IERG3810_LCD_draw_char(x_now, y, str[i], color, bgColor);
	}
	return x_now;
}
