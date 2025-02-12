/* Private includes ----------------------------------------------------------*/
#include "obd2.h"
#include "console.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
extern CAN_HandleTypeDef hcan2; // CAN Data Transmit Setup
static uint32_t last_request_time = 0;

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
			value = (data2 | data1 << 8) / 4;
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
			value = data1 * 100 / 255;
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

	console_print("PID=%.2X VAL=%d\r\n", pid, value);

	return value;
}

void obd2_request_pid(uint8_t pid){
	HAL_StatusTypeDef	TxStatus = HAL_OK;
	CAN_TxHeaderTypeDef	TxHeader;
	uint32_t			TxMailbox;
	uint8_t				TxData[8];

	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = 0x7DF;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxData[0] = 0x02;
	TxData[1] = 0x01;
	TxData[2] = pid;
	TxData[3] = 0x55;
	TxData[4] = 0x55;
	TxData[5] = 0x55;
	TxData[6] = 0x55;
	TxData[7] = 0x55;

	TxStatus = HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox);
	if(TxStatus == HAL_OK){
		LED_RED_ON();

		console_print("%.8lu TX: ID=0x%X DLC=%lu %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
					HAL_GetTick(), TxHeader.StdId, TxHeader.DLC,
					TxData[0], TxData[1], TxData[2], TxData[3], TxData[4], TxData[5], TxData[6], TxData[7]);
	}
	else{
		console_print("%.8lu TX ERROR! CODE=0x%.8X\r\n", HAL_GetTick(), HAL_CAN_GetError(&hcan2));
		HAL_CAN_ResetError(&hcan2);
	}

	last_request_time = HAL_GetTick();
}

uint32_t obd2_getLastRequestTime(){
	return last_request_time;
}
