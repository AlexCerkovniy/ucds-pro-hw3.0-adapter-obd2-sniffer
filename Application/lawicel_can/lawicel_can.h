#pragma once

#include "stdint.h"
#include "stdbool.h"

#include "lawicel_can_version.h"

typedef struct {
	uint16_t timestamp;
	bool is29BitId;
	uint16_t id;
	uint8_t dlc;
	uint8_t data[8];
} lawicel_can_phy_frame_t;

void lawicell_can_parce(char* command);
void lawicell_can_handle_received_phy_frame(lawicel_can_phy_frame_t *frame);

/* Weak functions */
void lawicell_port_can_send_frame(lawicel_can_phy_frame_t *frame);
