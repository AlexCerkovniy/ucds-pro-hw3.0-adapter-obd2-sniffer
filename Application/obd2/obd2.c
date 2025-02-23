/* Private includes ----------------------------------------------------------*/
#include "obd2.h"
#include "console.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
extern CAN_HandleTypeDef hcan2; // CAN Data Transmit Setup

uint8_t pids_list[] = {
		PID_COOLANT_TEMP,
		PID_INTAKE_TEMP,
		PID_INTAKE_MAP,
		PID_RPM,
		PID_SPEED,
		PID_FUEL_LEVEL,
		PID_ENGINE_LOAD
};

obd2_pids_list_t obd2_list = {
		.pids_list = pids_list,
		.size = sizeof(pids_list)/sizeof(pids_list[0])
};

uint8_t pid_request_index = 0;
uint32_t pid_request_timer = 0;
uint32_t pid_refresh_ticks = 250;

int16_t coolant_temp = 0;
int16_t intake_temp = 0;
int16_t intake_map = 0;
int16_t rpm = 0;
int16_t speed = 0;
int16_t fuel_level = 0;
int16_t engine_load = 0;

void obd2_init(void){

}

void obd2_set_refresh_rate(uint32_t ticks){
	if(ticks == 0){
		ticks = 250;
	}

	pid_refresh_ticks = ticks;
}

void obd2_main(void){
	if(obd2_list.size == 0){
		return;
	}

	if(pid_request_timer == 0){
		if(obd2_request_pid(obd2_list.pids_list[pid_request_index]) == OBD_OK){
			pid_request_timer = pid_refresh_ticks;
		}
	}
}

void obd2_tick(uint32_t period){
	if(pid_request_timer){
		pid_request_timer--;
	}
}

int16_t obd2_parse_packet(uint8_t packet[], uint8_t len)
{
	//uint8_t length = RxData[0];
	//uint8_t status = RxData[1];
	uint16_t pid  = packet[2];
	uint8_t data1 = packet[3];
	uint8_t data2 = packet[4];

	int16_t value = 0;

	switch (pid) {
		case PID_RPM:
		case PID_EVAP_SYS_VAPOR_PRESSURE: // kPa
			value = (data2 | ((int16_t)data1 << 8)) / 4;
			break;
		case PID_FUEL_PRESSURE: // kPa
			value = data1 * 3;
			break;
		case PID_COOLANT_TEMP:
		case PID_INTAKE_TEMP:
		case PID_AMBIENT_TEMP:
		case PID_ENGINE_OIL_TEMP:
			value = (int16_t)data1 - 40;
			break;
		case PID_THROTTLE:
		case PID_COMMANDED_EGR:
		case PID_COMMANDED_EVAPORATIVE_PURGE:
		case PID_FUEL_LEVEL:
		case PID_RELATIVE_THROTTLE_POS:
		case PID_ABSOLUTE_THROTTLE_POS_B:
		case PID_ABSOLUTE_THROTTLE_POS_C:
		case PID_ACC_PEDAL_POS_D:
		case PID_ACC_PEDAL_POS_E:
		case PID_ACC_PEDAL_POS_F:
		case PID_COMMANDED_THROTTLE_ACTUATOR:
		case PID_ENGINE_LOAD:
		case PID_ABSOLUTE_ENGINE_LOAD:
		case PID_ETHANOL_FUEL:
		case PID_HYBRID_BATTERY_PERCENTAGE:
			value = (int16_t)data1 * 100 / 255;
			break;
		case PID_MAF_FLOW: // grams/sec
			value = (data2 | data1 << 8) / 100;
			break;
		case PID_TIMING_ADVANCE:
			value = (data1 / 2) - 64;
			break;
		case PID_DISTANCE: // km
		case PID_DISTANCE_WITH_MIL: // km
		case PID_TIME_WITH_MIL: // minute
		case PID_TIME_SINCE_CODES_CLEARED: // minute
		case PID_RUNTIME: // second
		case PID_FUEL_RAIL_PRESSURE: // kPa
		case PID_ENGINE_REF_TORQUE: // Nm
			value = (data2 | data1 << 8);
			break;
		case PID_CONTROL_MODULE_VOLTAGE: // V
			value = (data2 | data1 << 8) / 1000;
			break;
		case PID_ENGINE_FUEL_RATE: // L/h
			value = (data2 | data1 << 8) / 20;
			break;
		case PID_ENGINE_TORQUE_DEMANDED: // %
		case PID_ENGINE_TORQUE_PERCENTAGE: // %
			value = data1 - 125;
			break;
		case PID_SHORT_TERM_FUEL_TRIM_1:
		case PID_LONG_TERM_FUEL_TRIM_1:
		case PID_SHORT_TERM_FUEL_TRIM_2:
		case PID_LONG_TERM_FUEL_TRIM_2:
		case PID_EGR_ERROR:
			value = (data1 * 100 / 128) - 100;
			break;
		case PID_FUEL_INJECTION_TIMING:
			value = ((data2 | data1 << 8) / 128) - 210;
			break;
		case PID_CATALYST_TEMP_B1S1:
		case PID_CATALYST_TEMP_B2S1:
		case PID_CATALYST_TEMP_B1S2:
		case PID_CATALYST_TEMP_B2S2:
			value = ((data2 | data1 << 8) / 10) - 40;
			break;
		case PID_AIR_FUEL_EQUIV_RATIO: // 0~200
			value = (data2 | data1 << 8) * 2 / 65536;
			break;
		default:
			value = data1;
	}

	switch (pid) {
		case PID_RPM: rpm = value; break;
		case PID_COOLANT_TEMP: coolant_temp = value; break;
		case PID_INTAKE_TEMP: intake_temp = value; break;
		case PID_INTAKE_MAP: intake_map = value; break;
		case PID_FUEL_LEVEL: fuel_level = value; break;
		case PID_ENGINE_LOAD: engine_load = value; break;

		default:
			break;
	}

	/* Go to next index */
	if(obd2_list.pids_list[pid_request_index] == pid){
		pid_request_index++;
		if(pid_request_index >= obd2_list.size){
			pid_request_index = 0;
		}
	}

	console_print("PID=%.2X VAL=%d\r\n", pid, value);

	return value;
}

obd2_status_t obd2_request_pid(uint8_t pid){
	obd2_status_t status = OBD_OK;
	uint32_t TxMailbox;

	static CAN_TxHeaderTypeDef TxHeader = {
			.IDE = CAN_ID_STD,
			.StdId = 0x7DF,
			.RTR = CAN_RTR_DATA,
			.DLC = 8,
			.TransmitGlobalTime = DISABLE
	};

	static uint8_t TxData[8] = {
			0x02, // Payload length
			0x01, // Standart request
			0x00, // PID field
			0x55,
			0x55,
			0x55,
			0x55,
			0x55
	};

	if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0){
		return OBD_BUSY;
	}

	/* Set PID */
	TxData[2] = pid;

	if(HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) == HAL_OK){
		console_print("%.8lu TX: ID=0x%X DLC=%lu %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
					HAL_GetTick(), TxHeader.StdId, TxHeader.DLC,
					TxData[0], TxData[1], TxData[2], TxData[3], TxData[4], TxData[5], TxData[6], TxData[7]);
	}
	else{
		console_print("%.8lu TX ERROR! CODE=0x%.8X\r\n", HAL_GetTick(), HAL_CAN_GetError(&hcan2));
		HAL_CAN_ResetError(&hcan2);
	}

	return status;
}
