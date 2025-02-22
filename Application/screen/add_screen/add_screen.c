#include "add_screen/add_screen.h"
#include "battery/battery.h"
#include "bvna_application.h"
#include "screen.h"
#include "version.h"
#include "list.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _PrepareDraw(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t add_screen = {
    .init = _Init,
	.prepare_draw = _PrepareDraw,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

/* Private variables ---------------------------------------------------------------------------------------------- */
const char *add_device_list_item_name[] = {
  "ADD REPEATER",
  "ADD DETONATOR #1",
  "ADD DETONATOR #2",
  "ADD DETONATOR #3",
  "ADD DETONATOR #4",
  "ADD DETONATOR #5",
  "ADD DETONATOR #6",
  "ADD DETONATOR #7",
  "ADD DETONATOR #8"
};

static bool add_device_list_item_select(uint8_t item);

gui_list_t add_device_list = {
		.x = 0, .y = 12, .item_height = 10,
		.item_name = (char **)add_device_list_item_name,
		.item_count = 9,
		.drawable_item_count = 5,
		.value_str_request = NULL,
		.select = add_device_list_item_select,
		.deselect = NULL,
		.change = NULL
};

static char *header = "ADD DEVICE";

static char *add_string_line_1 = "TURN ON DEVICE AND";
static char *add_string_line_2 = "HOLD BUTTON ON IT";
static char *add_string_line_3 = "FOR REGISTRATION";

static char *add_timeout_string = "ADDING TIMEOUT";
static char *add_done_string = "DEVICE ADDED!";
static char *add_cancel_string = "CANCELLED! EXIT...";

/* Add screen state machine (all states in add_screen.h) */
static uint8_t state = ADD_SCREEN_SELECT_DEVICE;
static uint16_t timer;

static void _Init(void){

}

static void _PrepareDraw(void){
	state = ADD_SCREEN_SELECT_DEVICE;
	gui_list_init(&add_device_list);
}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();
	G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(header) * G8Lib_GetFont()->width)/2, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);

	/* Show appropriate data */
	switch(state){
		case ADD_SCREEN_SELECT_DEVICE:
			G8Lib_SetFont(font_6x5);
			gui_list_draw(&add_device_list);
			break;

		case ADD_SCREEN_ADD_DEVICE:
			G8Lib_SetFont(font_6x5);
			G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_string_line_1) * G8Lib_GetFont()->width)/2, 24);
			G8Lib_String(add_string_line_1, GFX8_ADAPTIVE);
			G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_string_line_2) * G8Lib_GetFont()->width)/2, 32);
			G8Lib_String(add_string_line_2, GFX8_ADAPTIVE);
			G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_string_line_3) * G8Lib_GetFont()->width)/2, 40);
			G8Lib_String(add_string_line_3, GFX8_ADAPTIVE);
			break;

		case ADD_SCREEN_ADD_DEVICE_DONE:
			if(timer){
				G8Lib_SetFont(font_6x5);
				G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_done_string) * G8Lib_GetFont()->width)/2, 32);
				G8Lib_String(add_done_string, GFX8_ADAPTIVE);
			}
			else{
				SCREEN_Set(&stats_screen);
			}
			break;

		case ADD_SCREEN_ADD_DEVICE_TIMEOUT:
			if(timer){
				G8Lib_SetFont(font_6x5);
				G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_timeout_string) * G8Lib_GetFont()->width)/2, 32);
				G8Lib_String(add_timeout_string, GFX8_ADAPTIVE);
			}
			else{
				SCREEN_Set(&menu_screen);
			}
			break;

		case ADD_SCREEN_ADD_DEVICE_CANCEL:
			if(timer){
				G8Lib_SetFont(font_6x5);
				G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(add_cancel_string) * G8Lib_GetFont()->width)/2, 32);
				G8Lib_String(add_cancel_string, GFX8_ADAPTIVE);
			}
			else{
				SCREEN_Set(&menu_screen);
			}
			break;

		default:
			break;
	}

	G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){
	if(timer >= milliseconds){
		timer -= milliseconds;

		if(timer < milliseconds){
			timer = 0;
			SCREEN_Invalidate();
		}
	}
}

static void _Data(uint8_t type, void *data){
	if(type == BUTTON_TYPE){
		uint8_t button = *((uint8_t *)data);

		if(button == ENC_LONG_BTN_TYPE || button == EXIT_BTN_TYPE){
			if(state != ADD_SCREEN_SELECT_DEVICE){
				bvna_stop_registration();
				state = ADD_SCREEN_ADD_DEVICE_CANCEL;
				timer = 1000;
				SCREEN_Invalidate();
			}
			else{
				SCREEN_Set(&menu_screen);
			}
			return;
		}

		if(state != ADD_SCREEN_SELECT_DEVICE){
			return;
		}

		switch(button){
			case ENC_INC_BTN_TYPE:
			case PLUS_BTN_TYPE:
				gui_list_inc(&add_device_list);
				break;

			case ENC_DEC_BTN_TYPE:
			case MINUS_BTN_TYPE:
				gui_list_dec(&add_device_list);
				break;

			case ENC_BTN_TYPE:
			case SELECT_BTN_TYPE:
				gui_list_select(&add_device_list);
				break;

			default:
				return;
		}
	}
	else if(type == ADD_SCREEN_ADD_DEVICE_DONE || type == ADD_SCREEN_ADD_DEVICE_TIMEOUT){
		state = type;
		timer = 1000;
		SCREEN_Invalidate();
	}
}

static bool add_device_list_item_select(uint8_t item){
	if(item){
		bvna_start_registration(item, BVNA_DEV_TYPE_DETONATOR); //"ADD DETONATOR #1-8"
	}
	else{
		bvna_start_registration(item + 1, BVNA_DEV_TYPE_REPEATER); //"ADD REPEATER"
	}

	state = ADD_SCREEN_ADD_DEVICE;
	return false;
}
