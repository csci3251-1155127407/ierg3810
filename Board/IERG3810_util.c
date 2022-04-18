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

int IERG3810_min(int x, int y) {
	return x < y? x : y;
}

int IERG3810_max(int x, int y) {
	return x > y? x : y;
}

void IERG3810_to_str(char * dest, int x) {
	int len = 0;
	for (; x > 0; x /= 10) {
		dest[len++] = x % 10 + '0';
	}
	for (int i = 0; i < len / 2; i++) {
		char tmp = dest[i];
		dest[i] = dest[len - 1 - i];
		dest[len - 1 - i] = tmp;
	}
	dest[len++] = '\n';
	dest[len++] = '\0';
}
