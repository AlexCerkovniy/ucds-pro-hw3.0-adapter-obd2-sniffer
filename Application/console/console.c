#include "console.h"
#include <stdarg.h>
#include <stdio.h>

/* Platform includes */
#include "main.h"
#include "usbd_cdc_if.h"
#include "fast_fifo.h"

fast_fifo_t my_fifo;
uint8_t my_fifo_buffer[2048];

void console_init(void){
	fast_fifo_init(&my_fifo, my_fifo_buffer, sizeof(my_fifo_buffer)/sizeof(my_fifo_buffer[0]));
}

void console_print(char *fmt, ...){
  char buffer[256];
  size_t length;

  va_list args;
  va_start(args, fmt);
  length = vsprintf(buffer, fmt, args);
  va_end(args);

  if(length){
	fast_fifo_write(&my_fifo, (uint8_t *)buffer, length);
  }
}

void console_main(void){
	static uint8_t send_buffer[128];
	static size_t send_length;

	send_length = fast_fifo_get_available(&my_fifo);

	if(send_length && (CDC_Transmit_IsBusy() == USBD_OK)){
		if(send_length > sizeof(send_buffer)){
			send_length = sizeof(send_buffer);
		}

		/* Extract & send data from fifo */
		fast_fifo_read(&my_fifo, send_buffer, &send_length);
		CDC_Transmit_FS(send_buffer, (uint16_t)send_length);
	}
}

