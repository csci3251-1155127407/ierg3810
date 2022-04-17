#pragma once

#include <stdbool.h>

void IERG3810_LED_init(void);
void IERG3810_LED_set(int id, bool on);
void IERG3810_LED_toggle(int id);
