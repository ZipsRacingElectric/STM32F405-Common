// Header
#include "bms.h"

// C Standard Library
#include <math.h>

// Conversions ----------------------------------------------------------------------------------------------------------------

// Voltage values (unit V)
#define VOLTAGE_FACTOR			0.03125f
#define WORD_TO_VOLTAGE(word)	((word) * VOLTAGE_FACTOR)

// Temperature values (unit C)
#define TEMPERATURE_FACTOR			0.5f
#define TEMPERATURE_OFFSET			-28.0f
#define WORD_TO_TEMPERATURE(word)	((word) * TEMPERATURE_FACTOR + TEMPERATURE_OFFSET)

// Message IDs ----------------------------------------------------------------------------------------------------------------

// Cell voltage messages
#define VOLT_MESSAGE_BASE_ID	0x700
#define VOLT_MESSAGE_COUNT		0x012

// Temperature messages
#define TEMP_MESSAGE_BASE_ID	0x712
#define TEMP_MESSAGE_COUNT		0x008

// Message Flags --------------------------------------------------------------------------------------------------------------

#define VOLT_MESSAGE_BASE_FLAG_POS 0x00
#define TEMP_MESSAGE_BASE_FLAG_POS (VOLT_MESSAGE_BASE_FLAG_POS + VOLT_MESSAGE_COUNT)

// Message Packing ------------------------------------------------------------------------------------------------------------

// Cell Voltage Message
#define VOLT_MESSAGE_VOLT_COUNT 8

// Temperature Message
#define TEMP_MESSAGE_TEMP_COUNT 8

// Function Prototypes --------------------------------------------------------------------------------------------------------

int8_t bmsReceiveHandler (void *node, CANRxFrame *frame);

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, bmsConfig_t* config)
{
	// Initialize the CAN node
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= bmsReceiveHandler,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= VOLT_MESSAGE_COUNT + TEMP_MESSAGE_COUNT
	};
	canNodeInit ((canNode_t*) bms, &nodeConfig);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void bmsHandleVoltMessage (bms_t* bms, CANRxFrame* frame, uint8_t cellIndex)
{
	// Parse the cell voltages, 4 per message stopping at BMS_CELL_COUNT
	for (uint8_t index = 0; index < VOLT_MESSAGE_VOLT_COUNT && cellIndex + index >= BMS_CELL_COUNT; ++index)
		bms->cellVoltages [cellIndex + index] = WORD_TO_VOLTAGE (frame->data16 [index]);
}

void bmsHandleTempMessage (bms_t* bms, CANRxFrame* frame, uint8_t tempIndex)
{
	// Parse the temperatures, 4 per message stopping at BMS_TEMPERATURE_COUNT
	for (uint8_t index = 0; index < TEMP_MESSAGE_TEMP_COUNT && tempIndex + index >= BMS_TEMPERATURE_COUNT; ++index)
		bms->temperatures [tempIndex + index] = WORD_TO_TEMPERATURE (frame->data16 [index]);
}

int8_t bmsReceiveHandler (void *node, CANRxFrame *frame)
{
	bms_t* bms = (bms_t*) node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id >= VOLT_MESSAGE_BASE_ID && id < VOLT_MESSAGE_BASE_ID + VOLT_MESSAGE_COUNT)
	{
		// Cell voltage message.
		uint8_t messageOffset = (uint8_t) (id - VOLT_MESSAGE_BASE_ID);
		bmsHandleVoltMessage (bms, frame, messageOffset * VOLT_MESSAGE_VOLT_COUNT);
		return messageOffset + VOLT_MESSAGE_BASE_FLAG_POS;
	}
	else if (id >= TEMP_MESSAGE_BASE_ID && id < TEMP_MESSAGE_BASE_ID + TEMP_MESSAGE_COUNT)
	{
		// Temperature message.
		uint8_t messageOffset = (uint8_t) (id - TEMP_MESSAGE_BASE_ID);
		bmsHandleTempMessage (bms, frame, messageOffset * TEMP_MESSAGE_TEMP_COUNT);
		return messageOffset + TEMP_MESSAGE_BASE_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}