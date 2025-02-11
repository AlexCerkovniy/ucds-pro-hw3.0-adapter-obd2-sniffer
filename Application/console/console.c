#include "console.h"
#include <stdarg.h>
#include <stdio.h>

/* Platform includes */
#include "main.h"
#include "usbd_cdc_if.h"

void console_print(char *fmt, ...){
  char buffer[256];
  size_t length;

  va_list args;
  va_start (args, fmt);
  length = vsprintf (buffer, fmt, args);
  va_end (args);

  if(length > 0){
	  CDC_Transmit_FS((uint8_t *)buffer, (uint16_t)length);
  }
}

