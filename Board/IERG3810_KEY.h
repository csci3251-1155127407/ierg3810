#pragma once

#include <stdbool.h>

void IERG3810_KEY_init(void);
bool IERG3810_KEY_get(int id /*KEY_UP = 3*/);
bool IERG3810_KEY_clicked(int id /*KEY_UP = 3*/);
