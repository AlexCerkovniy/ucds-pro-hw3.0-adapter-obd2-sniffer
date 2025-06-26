#include "lawicel_can.h"
#include "stdio.h"
#include "string.h"

static size_t char_hex2bin(char *str, uint32_t *output, size_t max_count);
static bool parce_t_T_send_packet_request(char *command);

void lawicell_can_parce(char* command){
	char answer[128];
	bool ack = false;
	bool nack = false;

	if(command[0] == 'C'){
		ack = true;
	}
	else if(command[0] == 'L'){
		//TODO: This command will switch the CAN controller in Listen Only mode.
		ack = true;
	}
	else if(command[0] == 'O'){
		//TODO: This command switches the CAN controller from reset in operational mode. The controller is then involved in bus activities. It works only if the initiated with S command before, or controller was set to reset mode with command C.
		ack = true;
	}
	else if(command[0] == 'N'){
		snprintf(answer, sizeof(answer), "N%s\r", DEV_SERIAL_NUM_STRING);
	}
	else if(command[0] == 'S'){
		//TODO: This command will set the CAN controller to a predefined standard bit rate.
		ack = true;
	}
	else if(command[0] == 'v'){
		snprintf(answer, sizeof(answer), "v%s\r", DEV_MA_MI_VER_STRING);
	}
	else if(command[0] == 'V'){
		snprintf(answer, sizeof(answer), "V%s\r", DEV_HW_SW_VER_STRING);
	}
	else if(command[0] == 't' || command[0] == 'T') { //Format: tiiiLDDDDDDDDDDDDDDDD[CR] or TiiiLDDDDDDDDDDDDDDDD[CR] Return: [CR] or [BEL]
		parce_t_T_send_packet_request(command) ? (ack = true) : (nack = true);
	}
	else {
		return;
	}

	if(nack){
		answer[0] = '\a';
		answer[1] = 0;
	}
	else if(ack){
		answer[0] = '\r';
		answer[1] = 0;
	}

	console_string(answer);
}

void lawicell_can_handle_received_phy_frame(lawicel_can_phy_frame_t *frame){
	char string[64];
	char IdLengthIdentifierChar[] = "t"; //'t' for 11 bit id

	if(frame->is29BitId){
		IdLengthIdentifierChar[0] = 'T';
	}

	snprintf(string, sizeof(string), "%s%.3X%.1X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X[%.4X]\r",
			IdLengthIdentifierChar,
			frame->id,
			frame->dlc,
			frame->data[0], frame->data[1], frame->data[2], frame->data[3],
			frame->data[4], frame->data[5], frame->data[6], frame->data[7],
			frame->timestamp);

	console_string(string);
}

__attribute__((weak)) void lawicell_port_can_send_frame(lawicel_can_phy_frame_t *frame){
	(void)frame;
}

static bool parce_t_T_send_packet_request(char *command){
	lawicel_can_phy_frame_t frame = {0};
	uint32_t bin_decoded_string[32];

	if(strlen(command) != 22){
		return false;
	}

	if(char_hex2bin(command + 1, bin_decoded_string, sizeof(bin_decoded_string)/sizeof(bin_decoded_string[0])) != 20){
		return false;
	}

	if(command[0] == 'T'){
		frame.is29BitId = true;
	}

	/* Parce ID */
	frame.id += bin_decoded_string[0] * 0x100;
	frame.id += bin_decoded_string[1] * 0x10;
	frame.id += bin_decoded_string[2];

	/* Parce DLC */
	frame.dlc = bin_decoded_string[3];

	/* Pacre data */
	uint32_t *bin = &bin_decoded_string[4];
	for(uint32_t i = 0; i < frame.dlc; i++){
		frame.data[i] = (*bin) * 0x10 + (*(bin + 1));
		bin += 2;
	}

	/* Send frame */
	lawicell_port_can_send_frame(&frame);
	return true;
}

static size_t char_hex2bin(char *str, uint32_t *output, size_t max_count){
	static uint32_t hex2bin[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Placeholders for converting ASCII to dec, i.e. gap between digits and letters
			0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	};

	char c = *str;
	uint32_t count = 0;

	while(c >= '0' && c <= 'F') {
		*output = hex2bin[c - '0'];
		output++;
		count++;
		str++;
		c = *str;

		if(count == max_count){
			break;
		}
	}

	return count;
}
