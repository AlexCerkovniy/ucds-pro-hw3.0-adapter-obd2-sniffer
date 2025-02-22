#include "info_screen/info_screen.h"
#include "screen.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t info_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

/* Private variables ---------------------------------------------------------------------------------------------- */
static char *header = "PARAMETERS";

static void _Init(void){

}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();
	G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(header) * G8Lib_GetFont()->width)/2, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);

	G8Lib_SetFont(font_6x8);
	G8Lib_SetCursor(3, 14);
	G8Lib_Print(GFX8_ADAPTIVE, "ECT:%dC", 0);

	G8Lib_SetCursor(3, 24);
	G8Lib_Print(GFX8_ADAPTIVE, "OIL:%dC", 0);

	G8Lib_SetCursor(3, 34);
	G8Lib_Print(GFX8_ADAPTIVE, "RPM:%d", 0);

	G8Lib_SetCursor(3, 44);
	G8Lib_Print(GFX8_ADAPTIVE, "BAT:%u.%uV", 0, 0);

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
			break;

		case ENC_DEC_BTN_TYPE:
		case MINUS_BTN_TYPE:
			break;

		case ENC_LONG_BTN_TYPE:
		case EXIT_BTN_TYPE:
			SCREEN_Set(&menu_screen);
			break;

		default:
			return;
	}
}
