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
static char *header = "INFO";

extern int32_t battery_voltage_mv;
extern int16_t coolant_temp;
extern int16_t intake_temp;
extern int16_t intake_map;
extern int16_t rpm;
extern int16_t speed;
extern int16_t fuel_level;
extern int16_t engine_load;

static void _Init(void){

}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();
	G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor(36, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);

	G8Lib_SetCursor(91, 2);
	if(battery_voltage_mv < 10000) G8Lib_SetCursor(99, 2);
	G8Lib_Print(GFX8_ADAPTIVE, "%u.", battery_voltage_mv/1000);
	G8Lib_SetCursor(111, 2);
	G8Lib_Print(GFX8_ADAPTIVE, "%dV", (battery_voltage_mv%1000)/100);

	G8Lib_SetFont(font_6x8);
	G8Lib_SetCursor(2, 14);
	G8Lib_Print(GFX8_ADAPTIVE, "ECT:%dC", coolant_temp);

	G8Lib_SetCursor(2, 24);
	G8Lib_Print(GFX8_ADAPTIVE, "INT:%dC", intake_temp);

	G8Lib_SetCursor(2, 34);
	G8Lib_Print(GFX8_ADAPTIVE, "MAP:%dkPa", intake_map);

	G8Lib_SetCursor(2, 44);
	G8Lib_Print(GFX8_ADAPTIVE, "FUEL:%d%%", fuel_level);

	G8Lib_SetCursor(2, 54);
	G8Lib_Print(GFX8_ADAPTIVE, "LOAD:%d%%", engine_load);

	speed = 199;

	G8Lib_SetFont(font_6x8);
	G8Lib_SetCursor(84, 15);
	G8Lib_String("SPEED", GFX8_ADAPTIVE);
	G8Lib_SetFont(pixel_operator_bold_8x16);
	if(speed < 10) G8Lib_SetCursor(95, 21);
	else if(speed < 100) G8Lib_SetCursor(91, 21);
	else G8Lib_SetCursor(87, 21);
	G8Lib_Print(GFX8_ADAPTIVE, "%d", speed);

	rpm = 1222;

	G8Lib_SetFont(font_6x8);
	G8Lib_SetCursor(90, 40);
	G8Lib_String("RPM", GFX8_ADAPTIVE);
	G8Lib_SetFont(pixel_operator_bold_8x16);
	if(speed < 10) G8Lib_SetCursor(95, 46);
	else if(speed < 100) G8Lib_SetCursor(91, 46);
	else if(rpm < 1000) G8Lib_SetCursor(87, 46);
	else G8Lib_SetCursor(83, 46);
	G8Lib_Print(GFX8_ADAPTIVE, "%d", rpm);


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
