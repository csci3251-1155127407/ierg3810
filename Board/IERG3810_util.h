#pragma once

#include <stm32f10x.h>

static int const H = 240, V = 320;

void IERG3810_delay(long long count);
void IERG3810_delay_sec(int sec);
u16 IERG3810_to_color(u8 r, u8 g, u8 b);
int IERG3810_rand(int l, int r);
