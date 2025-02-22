#include "malfunction_screen/malfunction_screen.h"
#include "malfunction/malfunction.h"
#include "screen.h"

/* Private Functions Prototypes ----------------------------------------------------------------------------------- */
static void _Init(void);
static void _Draw(void);
static void _Tick(uint16_t milliseconds);

/* Exported variables ----------------------------------------------------------------------------------------------- */

/* Public variables ----------------------------------------------------------------------------------------------- */
screen_object_t malfunction_screen = {
    .init = _Init,
	.prepare_draw = NULL,
    .draw = _Draw,
    .tick = _Tick,
    .data = NULL
};

/* Private variables ---------------------------------------------------------------------------------------------- */
uint16_t reboot_counter = 1000;

static void _Init(void){

}

static void _Draw(void){
	G8Lib_GetDisplayDrv()->clear();
	G8Lib_DrawBitmap(8, 8, malfunction_icon, GFX8_SET);
	G8Lib_SetFont(pixel_operator_bold_8x16);
	G8Lib_SetCursor(64, 16);
	G8Lib_String("ERROR", GFX8_ADAPTIVE);
	G8Lib_SetCursor(64, 32);
	G8Lib_Print(GFX8_ADAPTIVE, "%lu", malfunction_get());
	G8Lib_GetDisplayDrv()->draw();
}

static void _Tick(uint16_t milliseconds){
	if(reboot_counter > milliseconds){
		reboot_counter -= milliseconds;
	}
	else{
		/* Reboot */
		while(1);
	}
}
