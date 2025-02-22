#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"

#include "screen_defines.h"

#include "gfx8lib.h"

typedef struct{
    void (*init)(void);
    void (*prepare_draw)(void);
    void (*draw)(void);
    void (*tick)(uint16_t milliseconds);
    void (*data)(uint8_t type, void *data);
} screen_object_t;

/* Screens */
extern screen_object_t logo_screen;
extern screen_object_t malfunction_screen;
extern screen_object_t menu_screen;
extern screen_object_t info_screen;

void SCREEN_Init(void);
void SCREEN_Main(void);
void SCREEN_Tick(uint16_t milliseconds);
void SCREEN_Set(screen_object_t *screen);
screen_object_t *SCREEN_Get(void);
void SCREEN_Invalidate(void);
void SCREEN_SendData(uint8_t type, void *data);
