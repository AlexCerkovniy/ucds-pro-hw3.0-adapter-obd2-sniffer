#pragma once

#include <stdint.h>

#define MALFUNCTION_CODE_NONE						(0)
#define MALFUNCTION_CODE_BATTERY_INIT				(1)
#define MALFUNCTION_CODE_RADIO_INIT					(100)

void malfunction_set(uint32_t code);
uint32_t malfunction_get(void);
