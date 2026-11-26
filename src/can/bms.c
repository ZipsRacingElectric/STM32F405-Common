// Header
#include "bms.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID				0x101
#define POWER_MESSAGE_ID				0x102
#define TEMPERATURE_MESSAGE_ID  		0x104

#define STATUS_MESSAGE_FLAG_POS 		0x00
#define POWER_MESSAGE_FLAG_POS			0x01
#define TEMPERATURE_MESSAGE_FLAG_POS	0x02

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

// Temperature (C)
#define TEMPERATURE_FACTOR 		0.0625f
#define TEMPERATURE_OFFSET 		(-106.0f)
#define WORD_TO_TEMP(word)		((word) * TEMPERATURE_FACTOR + TEMPERATURE_OFFSET)

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
		.messageCount	= 3
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
	bms->limpModeState			= (frame->data8 [5] >> 2) & 0b00000011;
}

void bmsHandlePowerMessage (bms_t* bms, CANRxFrame* frame)
{	
	bms->packVoltage			= WORD_TO_VOLTAGE (frame->data16 [0]);
	// TODO(Barach): Validate this fixes regen.
	bms->packCurrent			= WORD_TO_CURRENT ((int16_t) frame->data16 [1]);
	bms->powerDelivery			= WORD_TO_POWER ((int16_t) frame->data16 [2]);
}

void bmsHandleTemperatureMessage (bms_t* bms, CANRxFrame* frame)
{
	uint32_t payload =
		(uint32_t)frame->data8[0] |
		((uint32_t)frame->data8[1] << 8 ) |
		((uint32_t)frame->data8[2] << 16);
		
	uint16_t tempMaxRaw = (payload >> 0) & 0xFFF; 
	uint16_t tempAvgRaw = (payload >> 12) & 0xFFF;

	bms->temperatureMax 		= WORD_TO_TEMP(tempMaxRaw);
	bms->temperatureAvg 		= WORD_TO_TEMP(tempAvgRaw);
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
	else if (id == TEMPERATURE_MESSAGE_ID)
	{
		// Temperature message.
		bmsHandleTemperatureMessage(bms, frame);
		return TEMPERATURE_MESSAGE_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}