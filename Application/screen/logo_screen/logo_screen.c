#include <logo_screen/logo_screen.h>
#include "battery/battery.h"
#include "screen.h"
#include "version.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t logo_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = NULL
};

/* Private variables ---------------------------------------------------------------------------------------------- */
uint16_t logo_show_timer_ms = 2000;
bool dead_battery_show = true;

char name_string[] = "-- BAVOVNA --";
char description_string[] = "8-ch RC detonator";
char manufacturer_string[] = "Made in Ukraine";
char version_string[] = VERSION_STRING;

static void _Init(void){

}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();

	if(battery_is_dead()){
		if(dead_battery_show){
			G8Lib_RectNoFill(32, 16, 64, 32, GFX8_SET);
			G8Lib_Rect(96, 24, 3, 16, GFX8_SET);
		}
	}
	else{
		G8Lib_SetFont(atari_font_8x16);
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(name_string) * G8Lib_GetFont()->width)/2, 10);
		G8Lib_String(name_string, GFX8_ADAPTIVE);
		G8Lib_SetFont(font_6x5);
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(description_string) * G8Lib_GetFont()->width)/2, 29);
		G8Lib_String(description_string, GFX8_ADAPTIVE);
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(manufacturer_string) * G8Lib_GetFont()->width)/2, 39);
		G8Lib_String(manufacturer_string, GFX8_ADAPTIVE);
		G8Lib_SetCursor((G8Lib_GetDisplayDrv()->width - strlen(version_string) * G8Lib_GetFont()->width)/2, 49);
		G8Lib_String(version_string, GFX8_ADAPTIVE);
	}

	G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){
	logo_show_timer_ms--;
	if(logo_show_timer_ms == 0){
		if(battery_is_dead()){
			logo_show_timer_ms = 1000;
			dead_battery_show ^= true;
			SCREEN_Invalidate();
		}
		else{
			SCREEN_Set(&stats_screen);
		}
	}
}

