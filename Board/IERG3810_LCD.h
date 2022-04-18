#pragma once

#include <stm32f10x.h>
#include <stdbool.h>

void IERG3810_LCD_init(void);
void IERG3810_LCD_draw_rect_init(u16 x, u16 y, u16 x_len, u16 y_len);
void IERG3810_LCD_draw_dot(u16 x, u16 y, u16 color, bool init);
void IERG3810_LCD_draw_rect(int x, int y, u16 x_len, u16 y_len, u16 color);
// Transparent background if `bg_color` > 0xFFFF.
// Returns the `x` after this character.
u16 IERG3810_LCD_draw_char(u16 x, u16 y, char c, u16 color, u32 bg_color);
u16 IERG3810_LCD_draw_str(u16 x, u16 y, char * const str, u16 color, u32 bg_color);
