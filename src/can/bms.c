// Header
#include "bms.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID 0x101

#define STATUS_MESSAGE_FLAG_POS 0x00

// Function Prototypes --------------------------------------------------------------------------------------------------------

int8_t bmsReceiveHandler (void *node, CANRxFrame *frame);

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, const bmsConfig_t* config)
{
	// Initialize the CAN node
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= bmsReceiveHandler,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 1
	};
	canNodeInit ((canNode_t*) bms, &nodeConfig);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void bmsHandleStatusMessage (bms_t* bms, CANRxFrame* frame)
{
	bms->undervoltageFault		= (frame->data8 [0] & 0b00000001) == 0b00000001;
	bms->overvoltageFault		= (frame->data8 [0] & 0b00000010) == 0b00000010;
	bms->undertemperatureFault	= (frame->data8 [0] & 0b00000100) == 0b00000100;
	bms->overtemperatureFault	= (frame->data8 [0] & 0b00001000) == 0b00001000;
	bms->senseLineFault			= (frame->data8 [0] & 0b00010000) == 0b00010000;
	bms->isoSpiFault			= (frame->data8 [0] & 0b00100000) == 0b00100000;
	bms->selfTestFault			= (frame->data8 [0] & 0b01000000) == 0b01000000;
	bms->charging				= (frame->data8 [0] & 0b10000000) == 0b10000000;
	bms->balancing				= (frame->data8 [1] & 0b00000001) == 0b00000001;
	bms->shutdownClosed			= (frame->data8 [1] & 0b00000010) == 0b00000010;
	bms->prechargeComplete		= (frame->data8 [1] & 0b00000100) == 0b00000100;
	bms->bmsRelayStatus			= (frame->data8 [1] & 0b00010000) == 0b00010000;
	bms->imdRelayStatus			= (frame->data8 [1] & 0b00100000) == 0b00100000;
}

int8_t bmsReceiveHandler (void *node, CANRxFrame *frame)
{
	bms_t* bms = (bms_t*) node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id == STATUS_MESSAGE_ID)
	{
		// Status message.
		bmsHandleStatusMessage (bms, frame);
		return STATUS_MESSAGE_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}