#include <IERG3810_KEY.h>
#include <stm32f10x.h>

static int const DEBOUNCE = 25000;

void IERG3810_KEY_init(void) {
	RCC->APB2ENR |= 0x00000044;
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	GPIOA->BRR = 0x00000001;
	GPIOE->CRL &= 0xFFF000FF;
	GPIOE->CRL |= 0x00088800;
	GPIOE->BSRR = 0x0000001C;
}

bool IERG3810_KEY_get(int id) {
	if (id == 0) {
		return GPIOE->IDR >> 4 & 0x00000001 ^ 0x00000001;
	} else if (id == 1) {
		return GPIOE->IDR >> 3 & 0x00000001 ^ 0x00000001;
	} else if (id == 2) {
		return GPIOE->IDR >> 2 & 0x00000001 ^ 0x00000001;
	} else {
		return GPIOA->IDR >> 0 & 0x00000001;
	}
}

bool IERG3810_KEY_clicked(int id) {
	static int cnt[4];
	static bool lastRes[4];
	bool res = IERG3810_KEY_get(id);
	if (res == lastRes[id]) {
		if (cnt[id] <= DEBOUNCE) {
			cnt[id]++;
		}
		if (cnt[id] == DEBOUNCE) {
			return res;
		}
	} else {
		cnt[id] = 0;
		lastRes[id] = res;
	}
	return false;
}
