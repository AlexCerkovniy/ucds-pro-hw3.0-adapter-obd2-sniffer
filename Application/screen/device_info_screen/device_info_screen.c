#include "device_info_screen/device_info_screen.h"
#include "bvna_application.h"
#include "screen.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t device_info_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

uint8_t device_id = 0;
static bvna_device_t *device = NULL;
static char *header = "DEVICE INFO";
static char *device_not_added = "DEVICE NOT ADDED";

/* Private variables ---------------------------------------------------------------------------------------------- */
static void _Init(void){
	device = bvna_get_device_status(device_id);
}

static void _Draw(void){
	device = bvna_get_device_status(device_id);
	G8Lib_GetDisplayDrv()->clear();
	G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(header) * G8Lib_GetFont()->width)/2 + 10, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);
	G8Lib_SetFont(font_6x5);
	G8Lib_SetCursor(3, 1);
	G8Lib_Print(GFX8_ADAPTIVE, "%u/%u", device_id + 1, BVNA_DEVICE_COUNT);
	if(device->available){
		G8Lib_SetCursor(10, 16);
		if(device->offline){
			G8Lib_String("STATUS:OFFLINE", GFX8_ADAPTIVE);
		} else {
			G8Lib_String("STATUS:ONLINE", GFX8_ADAPTIVE);
		}
		G8Lib_SetCursor(10, 24);
		G8Lib_Print(GFX8_ADAPTIVE, "BATTERY:%u%%", device->battery_soc);
		G8Lib_SetCursor(10, 32);
		G8Lib_Print(GFX8_ADAPTIVE, "FREQ ERR:%dHz", device->freq_error_hz);
		G8Lib_SetCursor(10, 40);
		G8Lib_Print(GFX8_ADAPTIVE, "RSSI:%d", device->rssi);
		G8Lib_SetCursor(10, 48);
		G8Lib_Print(GFX8_ADAPTIVE, "LOSTS:%lu", device->losts);
	}
	else{
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(device_not_added) * G8Lib_GetFont()->width)/2, 32);
		G8Lib_String(device_not_added, GFX8_ADAPTIVE);
	}

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
			if(device_id < (BVNA_DEVICE_COUNT - 1)) device_id++;
			break;

		case ENC_DEC_BTN_TYPE:
		case MINUS_BTN_TYPE:
			if(device_id) device_id--;
			break;

		case ENC_LONG_BTN_TYPE:
		case EXIT_BTN_TYPE:
			SCREEN_Set(&menu_screen);
			break;

		default:
			return;
	}
}
