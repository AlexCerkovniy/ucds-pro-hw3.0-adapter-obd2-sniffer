#pragma once

#include "stdint.h"
#include "stdbool.h"

#include "obd2_pids.h"

typedef struct {
	uint8_t *pids_list;
	uint8_t size;
} obd2_pids_list_t;

typedef struct {
	/* 0x00 */ uint32_t supported_pids_0_20;
	/* 0x01 */ uint8_t mon_status_since_dtc_cleared[8];
	/* 0x02 */ uint8_t dummy0;
	/* 0x03 */ uint8_t fuel_system_status;
	/* 0x04 */ uint8_t engine_load; /* 0 - 100% */
	//TODO
} obd2_data_t;

void obd2_init(void);
void obd2_set_refresh_rate(uint32_t ticks);
void obd2_main(void);
void obd2_tick(uint32_t period);
int16_t obd2_parse_packet(uint8_t packet[], uint8_t len);
void obd2_request_pid(uint8_t pid);
