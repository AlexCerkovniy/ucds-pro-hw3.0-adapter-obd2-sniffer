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

extern int32_t battery_voltage_mv;
extern int16_t coolant_temp;
extern int16_t intake_temp;
extern int16_t intake_map;
extern int16_t rpm;

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
	G8Lib_Print(GFX8_ADAPTIVE, "ECT:%dC", coolant_temp);

	G8Lib_SetCursor(3, 24);
	G8Lib_Print(GFX8_ADAPTIVE, "INTAKE:%dC", intake_temp);

	G8Lib_SetCursor(3, 34);
	G8Lib_Print(GFX8_ADAPTIVE, "MAP:%dkPa", intake_map);

	G8Lib_SetCursor(3, 44);
	G8Lib_Print(GFX8_ADAPTIVE, "RPM:%d", rpm);

	G8Lib_SetCursor(3, 54);
	G8Lib_Print(GFX8_ADAPTIVE, "BAT:%u.%.2uV", battery_voltage_mv/1000, (battery_voltage_mv%1000)/10);

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
