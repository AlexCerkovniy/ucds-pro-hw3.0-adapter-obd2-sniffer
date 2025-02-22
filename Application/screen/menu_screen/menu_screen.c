#include "menu_screen/menu_screen.h"
#include "screen.h"
#include "list.h"
#include "main.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _PrepareDraw(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t menu_screen = {
    .init = _Init,
	.prepare_draw = _PrepareDraw,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

/* Private variables ---------------------------------------------------------------------------------------------- */
const char *menu_item_name[] = {
  "ADD DEVICE",
  "REPEATER INFO",
  "DEVICE INFO",
  "SETTINGS",
  "STATISTIC",
  "GAMES",
  "ABOUT"
};

static void menu_item_val_str_request(uint8_t item, char *str, uint8_t *length);
static bool menu_item_select(uint8_t item);
static void menu_item_deselect(uint8_t item);
static void menu_item_change(uint8_t item, bool sign);

gui_list_t menu_list = {
		.x = 0, .y = 12, .item_height = 10,
		.item_name = (char **)menu_item_name,
		.item_count = sizeof(menu_item_name)/sizeof(menu_item_name[0]),
		.drawable_item_count = 5,
		.value_str_request = menu_item_val_str_request,
		.select = menu_item_select,
		.deselect = menu_item_deselect,
		.change = menu_item_change
};

static char *header = "MENU";

static void _Init(void){
	gui_list_init(&menu_list);
}

static void _PrepareDraw(void){
	gui_list_init(&menu_list);
}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();
    G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(header) * G8Lib_GetFont()->width)/2, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);
	G8Lib_SetFont(font_6x5);
	G8Lib_SetCursor(3, 1);
	G8Lib_Print(GFX8_ADAPTIVE, "%u/%u", menu_list.item + 1, menu_list.item_count);
	gui_list_draw(&menu_list);
	G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){

}

static void _Data(uint8_t type, void *data){
	if(type != BUTTON_TYPE){
		return;
	}

	uint8_t button = *((uint8_t *)data);

	switch(button){
		case ENC_INC_BTN_TYPE:
		case PLUS_BTN_TYPE:
			gui_list_inc(&menu_list);
			break;

		case ENC_DEC_BTN_TYPE:
		case MINUS_BTN_TYPE:
			gui_list_dec(&menu_list);
			break;

		case ENC_BTN_TYPE:
		case SELECT_BTN_TYPE:
			gui_list_select(&menu_list);
			break;

		case ENC_LONG_BTN_TYPE:
		case EXIT_BTN_TYPE:
			SCREEN_Set(&stats_screen);
			break;

		default:
			return;
	}
}

static void menu_item_val_str_request(uint8_t item, char *str, uint8_t *length){

}

static bool menu_item_select(uint8_t item){
	switch(item){
		case 0: SCREEN_Set(&add_screen); break;
		case 1: SCREEN_Set(&radio_test_screen); break;
		case 2: SCREEN_Set(&device_info_screen); break;

		default:
			break;
	}

	return false;
}

static void menu_item_deselect(uint8_t item){

}

static void menu_item_change(uint8_t item, bool sign){

}


