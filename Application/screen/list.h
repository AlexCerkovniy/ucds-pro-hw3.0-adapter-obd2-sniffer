#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct{
	uint8_t x, y, item_height;
	uint8_t item;
	uint8_t item_selected;
	uint8_t item_top;
	char **item_name;
	uint8_t item_count;
	uint8_t drawable_item_count;
	char string_tmp[64];
	void (*value_str_request)(uint8_t item, char *str, uint8_t *length);
	bool (*select)(uint8_t item);
	void (*deselect)(uint8_t item);
	void (*change)(uint8_t item, bool sign);
} gui_list_t;

void gui_list_init(gui_list_t *list);
void gui_list_draw(gui_list_t *list);
void gui_list_inc(gui_list_t *list);
void gui_list_dec(gui_list_t *list);
void gui_list_select(gui_list_t *list);
void gui_list_deselect_forced(gui_list_t *list);
uint8_t gui_list_get(void);
