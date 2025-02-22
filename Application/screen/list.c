#include "list.h"
#include "gfx8lib.h"

void gui_list_init(gui_list_t *list){
	list->item = 0;
	list->item_top = 0;
	list->item_selected = false;
}

void gui_list_draw(gui_list_t *list){
	uint8_t text_length;
	uint8_t item_window_size;
	uint8_t display_width = G8Lib_GetDisplayDrv()->width;

	/* Calculate actual items window size (count) */
	if(list->item_count < list->drawable_item_count){
		item_window_size = list->item_count;
	}
	else{
		item_window_size = list->drawable_item_count;
	}

	/* Draw highlighted line */
	G8Lib_Rect(list->x, list->y + ((list->item - list->item_top)  * list->item_height), display_width, list->item_height, GFX8_SET);

	for(uint8_t i = 0; i < item_window_size; i++){
		G8Lib_SetCursor(list->x + 4, list->y + (i * list->item_height) + 1);
		G8Lib_String(list->item_name[list->item_top + i], GFX8_ADAPTIVE);

		/* Print value */
		text_length = 0;
		if(list->value_str_request){
			list->value_str_request(list->item_top + i, list->string_tmp, &text_length);
		}

		/* Skip printing item value if its text length is zero */
		if(text_length == 0){
		  continue;
		}

		if(list->item_selected && (list->item == list->item_top + i)){
			G8Lib_SetCursor(display_width - ((text_length + 1) * G8Lib_GetFont()->width) - 1, list->y + (i * list->item_height) + 2);
			G8Lib_String(">", GFX8_ADAPTIVE);
		}

		G8Lib_SetCursor(display_width - (text_length * G8Lib_GetFont()->width) - 1, list->y + (i * list->item_height) + 2);
		G8Lib_String(list->string_tmp, GFX8_ADAPTIVE);
	}
}

void gui_list_inc(gui_list_t *list){
	if(list->item_selected){
		list->change(list->item, true);
	}
	else if(list->item){
		if(list->item_top == list->item){
			list->item_top--;
		}
		list->item--;
	}
}

void gui_list_dec(gui_list_t *list){
	if(list->item_selected){
		if(list->change){
			list->change(list->item, false);
		}
	}
	else if(list->item < (list->item_count - 1)){
		list->item++;
		if(list->item > list->item_top + (list->drawable_item_count - 1)){
			if(list->item_top + list->drawable_item_count < list->item_count){
				list->item_top++;
			}
		}
	}
}

void gui_list_select(gui_list_t *list){
	if(list->item_selected){
		list->item_selected = false;
		if(list->deselect){
			list->deselect(list->item);
		}
	}
	else{
		list->item_selected = true;
		if(list->select){
			if(list->select(list->item) == false){
				list->item_selected = false;
			}
		}
	}
}

void gui_list_deselect_forced(gui_list_t *list){
	if(list->item_selected){
		list->item_selected = false;
		if(list->deselect){
			list->deselect(list->item);
		}
	}
}
