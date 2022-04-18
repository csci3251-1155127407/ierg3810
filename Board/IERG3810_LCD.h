#pragma once

#include <stm32f10x.h>

void IERG3810_LCD_init(void);
void IERG3810_LCD_draw_dot(u16 x, u16 y, u16 color);
void IERG3810_LCD_draw_rect(u16 x, u16 y, u16 x_len, u16 y_len, u16 color);
// Transparent background if `bgColor` > 0xFFFF.
// Returns the `x` after this character.
u16 IERG3810_LCD_draw_char(u16 x, u16 y, char c, u16 color, u32 bgColor);
u16 IERG3810_LCD_draw_str(u16 x, u16 y, char const * str, u16 color, u32 bgColor);
