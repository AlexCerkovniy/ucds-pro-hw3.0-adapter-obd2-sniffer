#include "screen.h"

screen_object_t *last_screen = NULL;
screen_object_t *current_screen = NULL;
screen_object_t *new_screen = NULL;
bool invalidate = false;

void SCREEN_Init(void){

}

void SCREEN_Set(screen_object_t *screen){
	new_screen = screen;
}

screen_object_t *SCREEN_Get(void){
	return current_screen;
}

void SCREEN_Invalidate(void){
    invalidate = true;
}

void SCREEN_Main(void){
	if(new_screen != NULL){
		last_screen = current_screen;
		current_screen = new_screen;
		new_screen = NULL;
		invalidate = true;

		if(current_screen->prepare_draw){
			current_screen->prepare_draw();
		}
	}

    if(current_screen){
        if(invalidate){
            if(current_screen->draw){
                current_screen->draw();
                invalidate = false;
            }
        }
    }
}

void SCREEN_SendData(uint8_t type, void *data){
    if(current_screen){
        if(current_screen->data){
            switch(type){
               /* Insert data type handler */

                default:
                    break;
            }

            current_screen->data(type, data);
            invalidate = true;
        }
    }
}

void SCREEN_Tick(uint16_t milliseconds){
    if(current_screen){
        if(current_screen->tick){
            current_screen->tick(milliseconds);
        }
    }
}
