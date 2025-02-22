#include "radio_test_screen/radio_test_screen.h"
#include "bvna_application.h"
#include "battery/battery.h"

#include "screen.h"
#include "version.h"
#include "sx127x_lora.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t radio_test_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

/* Private variables ---------------------------------------------------------------------------------------------- */
uint16_t update_timer_ms = 1000;
static bvna_device_t *repeater = NULL;
static char *header = "REPEATER INFO";
static char *repeater_not_added = "REPEATER NOT ADDED";

static void _Init(void){

}

static void _Draw(void){
	repeater = bvna_get_repeater_status(0);

	G8Lib_GetDisplayDrv()->clear();
	G8Lib_SetFont(hunter_font_8x8);
	G8Lib_Rect(0, 0, 128, 11, GFX8_SET);
	G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(header) * G8Lib_GetFont()->width)/2, 2);
	G8Lib_String(header, GFX8_ADAPTIVE);

	G8Lib_SetFont(font_6x5);
	if(repeater->available){
		G8Lib_SetCursor(10, 16);
		if(repeater->offline){
			G8Lib_String("STATUS:OFFLINE", GFX8_ADAPTIVE);
		} else {
			G8Lib_String("STATUS:ONLINE", GFX8_ADAPTIVE);
		}
		G8Lib_SetCursor(10, 24);
		G8Lib_Print(GFX8_ADAPTIVE, "BATTERY:%u%%", repeater->battery_soc);
		G8Lib_SetCursor(10, 32);
		G8Lib_Print(GFX8_ADAPTIVE, "FREQ ERR:%dHz", repeater->freq_error_hz);
		G8Lib_SetCursor(10, 40);
		G8Lib_Print(GFX8_ADAPTIVE, "RSSI:%d", repeater->rssi);
		G8Lib_SetCursor(10, 48);
		G8Lib_Print(GFX8_ADAPTIVE, "LOSTS:%lu", repeater->losts);
	}
	else{
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(repeater_not_added) * G8Lib_GetFont()->width)/2, 32);
		G8Lib_String(repeater_not_added, GFX8_ADAPTIVE);
	}

	G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){
	if(update_timer_ms >= milliseconds){
		update_timer_ms -= milliseconds;
		if(update_timer_ms < milliseconds){
			update_timer_ms = 1000;
			SCREEN_Invalidate();
		}
	}
}

static void _Data(uint8_t type, void *data){
	if(type != BUTTON_TYPE){
		return;
	}

	uint8_t button = *((uint8_t *)data);

	switch(button){
		case ENC_BTN_TYPE:
		case SELECT_BTN_TYPE:
			break;

		case ENC_LONG_BTN_TYPE:
		case EXIT_BTN_TYPE:
			SCREEN_Set(&menu_screen);
			break;

		default:
			return;
	}
}
