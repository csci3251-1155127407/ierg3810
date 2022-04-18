#pragma once

#include <stm32f10x.h>

static int const X = 240, Y = 320;

void IERG3810_delay(long long cnt);
void IERG3810_delay_sec(int sec);
u16 IERG3810_to_color(u8 r, u8 g, u8 b);
int IERG3810_rand(int l, int r); // [l, r)
int IERG3810_min(int x, int y);
int IERG3810_max(int x, int y);
void IERG3810_to_str(char * dest, int x);
