#include <IERG3810_util.h>
#include <stdlib.h>

void IERG3810_delay(long long count) {
	for (volatile long long i = 0; i < count; i++) {}
}

void IERG3810_delay_sec(int sec) {
	IERG3810_delay(6020000LL * sec);
}

u16 IERG3810_to_color(u8 r, u8 g, u8 b) {
	u16 color = 0x0000;
	color |= (r & 0xF8 >> 3) << 11;
	color |= (g & 0xFC >> 2) << 5;
	color |= (b & 0xF8 >> 3);
	return color;
}

int IERG3810_rand(int l, int r) {
	return rand() % (r - l) + l;
}


