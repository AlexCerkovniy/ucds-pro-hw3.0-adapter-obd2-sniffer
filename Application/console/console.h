#pragma once

#include <stdint.h>

void console_init(void);
void console_main(void);
void console_input(uint8_t *buffer, uint32_t length);
void console_print(char *fmt, ...);
