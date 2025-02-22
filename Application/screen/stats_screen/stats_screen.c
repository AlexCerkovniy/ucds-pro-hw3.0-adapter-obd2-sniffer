#include <stats_screen/stats_screen.h>
#include "battery/battery.h"
#include "bvna_application.h"
#include "screen.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);
static void _Data(uint8_t type, void *data);

static uint8_t evaluate_rssi_level(int8_t rssi);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t stats_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = _Data
};

/* Private variables ---------------------------------------------------------------------------------------------- */
static bool battery_icon_visible = true;
static uint16_t battery_icon_timer = 0;
static uint16_t update_screen_timer = STATS_SCREEN_UPDATE_PERIOD_MS;

#define SLOTS_COUNT 			(8)
#define REPEATERS_COUNT 		(1)

struct {
	uint16_t x, y, w, h;
} slot_pos[SLOTS_COUNT] = {
		{0, 17, 63, 11},
		{0, 29, 63, 11},
		{0, 41, 63, 11},
		{0, 53, 63, 11},
		{65, 17, 63, 11},
		{65, 29, 63, 11},
		{65, 41, 63, 11},
		{65, 53, 63, 11}
};

struct {
	uint16_t x, y, w, h;
} repeater_pos[REPEATERS_COUNT] = {
		{0, 0, 96, 11},
};

static uint8_t signal_strength_glyph[6] = {0x70, 0x78, 0x7C, 0x7E, 0x7F};

static void _Init(void){

}

static void _Draw(void){
    uint8_t battery_bar_level;
    uint8_t signal_strength;
    bvna_device_t *device;

    G8Lib_GetDisplayDrv()->clear();

    /* Draw battery icon */
	if(battery_icon_visible || !battery_is_low()){
		/* value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min */
		uint8_t battery_bar_level = (battery_get_voltage() - DEAD_BATTERY_MILLIVOLTS) * (8 - 1) / (HIGH_BATTERY_MILLIVOLTS - DEAD_BATTERY_MILLIVOLTS) + 1;
		G8Lib_DrawBatteryStatus(117, 1, 8, battery_bar_level, GFX8_ADAPTIVE);
	}

    /* Draw backlight state icon */
    if(G8Lib_GetDisplayDrv()->get_backlight()){
        G8Lib_DrawBitmap(109, 2, backlight_icon, GFX8_ADAPTIVE);
    }

    /* Draw repeater status */
    device = bvna_get_repeater_status(0);
    G8Lib_RectNoFill(repeater_pos[0].x, repeater_pos[0].y, repeater_pos[0].w, repeater_pos[0].h, GFX8_SET);
    G8Lib_SetFont(font_6x5);
    G8Lib_SetCursor(repeater_pos[0].x + 3, repeater_pos[0].y + 1);
    if(device->available == false){
    	G8Lib_String("REP:- - - - - -", GFX8_ADAPTIVE);
	}
    else if(device->offline){
    	G8Lib_String("REP:Offline", GFX8_ADAPTIVE);
    }
    else{
    	G8Lib_String("REP:Ready", GFX8_ADAPTIVE);

    	G8Lib_DrawBitmap(repeater_pos[0].x + 67, repeater_pos[0].y + 2, antenna_icon, GFX8_ADAPTIVE);

    	signal_strength = evaluate_rssi_level(device->rssi);
    	for(uint8_t i = 0; i < signal_strength; i++){
    		G8Lib_PutByte(repeater_pos[0].x + 72 + i * 2, repeater_pos[0].y + 2, signal_strength_glyph[i], GFX8_ADAPTIVE);
    	}

		/* value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min */
		battery_bar_level = device->battery_soc * (8 - 1) / 100 + 1;
		G8Lib_DrawBatteryStatus(repeater_pos[0].x + 83, repeater_pos[0].y + 2, 8, battery_bar_level, GFX8_ADAPTIVE);
    }

    /* Draw slots */
	G8Lib_SetFont(font_6x5);
    for(uint8_t slot = 0; slot < SLOTS_COUNT; slot++){
    	device = bvna_get_device_status(slot);

    	G8Lib_RectNoFill(slot_pos[slot].x, slot_pos[slot].y, slot_pos[slot].w, slot_pos[slot].h, GFX8_SET);
    	if(device->available == false){
    		G8Lib_SetCursor(slot_pos[slot].x + 3, slot_pos[slot].y + 1);
    		G8Lib_Print(GFX8_ADAPTIVE, "%u:- - - -", slot + 1);
    		continue;
    	}

    	if(device->selected){
    		G8Lib_Rect(slot_pos[slot].x, slot_pos[slot].y, slot_pos[slot].w, slot_pos[slot].h, GFX8_SET);
		}

    	if(device->offline){
    		G8Lib_SetCursor(slot_pos[slot].x + 3, slot_pos[slot].y + 1);
    		G8Lib_Print(GFX8_ADAPTIVE, "%u:Offline", slot + 1);
    	}
    	else{
    		G8Lib_SetCursor(slot_pos[slot].x + 3, slot_pos[slot].y + 1);
    		G8Lib_Print(GFX8_ADAPTIVE, "%u:Ready", slot + 1);

    		/* value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min */
			battery_bar_level = device->battery_soc * (8 - 1) / 100 + 1;
			G8Lib_DrawBatteryStatus(slot_pos[slot].x + 50, slot_pos[slot].y + 2, 8, battery_bar_level, GFX8_ADAPTIVE);
    	}
    }

    G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){
	if(battery_is_low()){
		if(battery_icon_timer > milliseconds){
			battery_icon_timer -= milliseconds;
		}
		else{
			battery_icon_timer = LOW_BATTERY_ICON_BLINK_PERIOD_MS;
			battery_icon_visible ^= true;
			SCREEN_Invalidate();
		}
	}

	if(update_screen_timer > milliseconds){
		update_screen_timer -= milliseconds;
	}
	else{
		update_screen_timer = STATS_SCREEN_UPDATE_PERIOD_MS;
		SCREEN_Invalidate();
	}
}

static void _Data(uint8_t type, void *data){
	if(type != BUTTON_TYPE){
		return;
	}

	uint8_t button = *((uint8_t *)data);

	switch(button){
//		case ENC_BTN_TYPE:
//		case SELECT_BTN_TYPE:
//			break;

		case ENC_LONG_BTN_TYPE:
		case EXIT_BTN_TYPE:
			SCREEN_Set(&menu_screen);
			break;

		default:
			return;
	}
}

static uint8_t evaluate_rssi_level(int8_t rssi){
	if(rssi < -110){
		return 1;
	}
	else if(rssi < -100){
		return 2;
	}
	else if(rssi < -90){
		return 3;
	}
	else if(rssi < -75){
		return 4;
	}

	return 5;
}
