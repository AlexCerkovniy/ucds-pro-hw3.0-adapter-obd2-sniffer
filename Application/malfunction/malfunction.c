#include "malfunction/malfunction.h"

uint32_t malfunction = 0;

void malfunction_set(uint32_t code){
	malfunction = code;
}

uint32_t malfunction_get(void){
	return malfunction;
}

