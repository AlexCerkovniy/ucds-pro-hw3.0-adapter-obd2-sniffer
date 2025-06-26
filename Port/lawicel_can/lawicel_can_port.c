#include "lawicel_can.h"
#include "main.h"

void lawicell_port_can_send_frame(lawicel_can_phy_frame_t *frame){
	CAN_TxHeaderTypeDef TxHeader = {0};
	uint32_t TxMailbox;

	if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0){
		return;
	}

	if(frame->is29BitId){
		TxHeader.IDE = CAN_ID_EXT;
		TxHeader.ExtId = frame->id;
	}
	else{
		TxHeader.IDE = CAN_ID_STD;
		TxHeader.StdId = frame->id;
	}

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = frame->dlc;

	if(HAL_CAN_AddTxMessage(&hcan2, &TxHeader, frame->data, &TxMailbox) == HAL_OK){
		console_print("%.8lu TX: ID=0x%X DLC=%lu %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
					HAL_GetTick(), TxHeader.StdId, TxHeader.DLC,
					frame->data[0], frame->data[1], frame->data[2], frame->data[3],
					frame->data[4], frame->data[5], frame->data[6], frame->data[7]);
	}
	else{
		console_print("%.8lu TX ERROR! CODE=0x%.8X\r\n", HAL_GetTick(), HAL_CAN_GetError(&hcan2));
		HAL_CAN_ResetError(&hcan2);
	}
}


