#pragma once

#include "stdint.h"
#include "stdbool.h"

#include "obd2_pids.h"

int16_t obd2_parse_packet(uint8_t packet[], uint8_t len);
void obd2_request_pid(uint8_t pid);
