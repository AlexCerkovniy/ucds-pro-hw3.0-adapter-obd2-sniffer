#include "st7565_port.h"

#include "main.h"
#include "spi.h"

void ST7565_PL_Init(void){

}

void ST7565_PL_RST_Pin(bool state){
	if(state){
		LL_GPIO_SetOutputPin(DISP_RST_GPIO_Port, DISP_RST_Pin);
	}
	else{
		LL_GPIO_ResetOutputPin(DISP_RST_GPIO_Port, DISP_RST_Pin);
	}
}

void ST7565_PL_CS_Pin(bool state){
	if(state){
		LL_GPIO_SetOutputPin(DISP_CS_GPIO_Port, DISP_CS_Pin);
	}
	else{
		LL_GPIO_ResetOutputPin(DISP_CS_GPIO_Port, DISP_CS_Pin);
	}
}

void ST7565_PL_DC_Pin(bool state){
	if(state){
		LL_GPIO_SetOutputPin(DISP_DC_GPIO_Port, DISP_DC_Pin);
	}
	else{
		LL_GPIO_ResetOutputPin(DISP_DC_GPIO_Port, DISP_DC_Pin);
	}
}

void ST7565_PL_Backlight(bool state){
	if(state){
		LL_GPIO_SetOutputPin(DISP_BCL_GPIO_Port, DISP_BCL_Pin);
	}
	else{
		LL_GPIO_ResetOutputPin(DISP_BCL_GPIO_Port, DISP_BCL_Pin);
	}
}

uint8_t ST7565_PL_SPI_Transfer(uint8_t byte){
	LL_SPI_TransmitData8(SPI1, byte);
	while ((LL_SPI_IsActiveFlag_TXE(SPI1) == 0) || (LL_SPI_IsActiveFlag_RXNE(SPI1) == 0)){};
	LL_SPI_ReceiveData8(SPI1);
    return 0;
}

void ST7565_PL_Delay(uint32_t milliseconds){
	delay(milliseconds);
}
