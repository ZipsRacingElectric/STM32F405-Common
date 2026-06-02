// Header
#include "bms.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID		0x101
#define POWER_MESSAGE_ID		0x102

#define STATUS_MESSAGE_FLAG_POS 0x00
#define POWER_MESSAGE_FLAG_POS	0x01

// Conversions ----------------------------------------------------------------------------------------------------------------

// Voltage (V)
#define VOLTAGE_FACTOR			0.0125f
#define WORD_TO_VOLTAGE(word)	((word) * VOLTAGE_FACTOR)

// Current (A)
#define CURRENT_FACTOR			0.019073486328125f
#define WORD_TO_CURRENT(word)	((word) * CURRENT_FACTOR)

// Power (W)
#define POWER_FACTOR			4.0f
#define WORD_TO_POWER(word)		((word) * POWER_FACTOR)

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
		.messageCount	= 2
	};
	canNodeInit ((canNode_t*) bms, &nodeConfig);
}

float bmsGetPowerLock (bms_t* bms)
{
	canNodeLock ((canNode_t*) bms);
	float power = bms->state == CAN_NODE_VALID ? bms->powerDelivery : 0.0f;
	canNodeUnlock ((canNode_t*) bms);
	return power;
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void bmsHandleStatusMessage (bms_t* bms, CANRxFrame* frame)
{
	bms->bmsFault				= (frame->data8 [0] & 0b10000000) == 0b10000000;
	bms->imdFault				= (frame->data8 [1] & 0b00000001) == 0b00000001;
	bms->charging				= (frame->data8 [1] & 0b00000010) == 0b00000010;
	bms->balancing				= (frame->data8 [1] & 0b00000100) == 0b00000100;
	bms->negativeIrClosed		= (frame->data8 [2] & 0b00000001) == 0b00000001;
	bms->positiveIrClosed		= (frame->data8 [2] & 0b00000010) == 0b00000010;
}

void bmsHandlePowerMessage (bms_t* bms, CANRxFrame* frame)
{
	bms->packVoltage			= WORD_TO_VOLTAGE (frame->data16 [0]);
	// TODO(Barach): Validate this fixes regen.
	bms->packCurrent			= WORD_TO_CURRENT ((int16_t) frame->data16 [1]);
	bms->powerDelivery			= WORD_TO_POWER ((int16_t) frame->data16 [2]);
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
	else if (id == POWER_MESSAGE_ID)
	{
		// Power message.
		bmsHandlePowerMessage (bms, frame);
		return POWER_MESSAGE_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}