// Header
#include "driver_input_board.h"

static int8_t receive (void* node, CANRxFrame* frame)
{
	dib_t* dib = node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id == dib->canId)
	{
		// Handle input message

		// Button inputs
		for (uint8_t index = 0; index < 8; ++index)
			dib->buttonsPressed [index] = ((frame->data8 [0] >> index) & 0b1) == 0b1;

		// Analog inputs
		for (uint8_t index = 0; index < 2; ++index)
			dib->analogValues [index] = frame->data8 [index + 1] / 255.0f;

		return 0;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}

void dibInit (dib_t* dib, dibConfig_t* config)
{
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= receive,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 1,
	};
	canNodeInit ((canNode_t*) dib, &nodeConfig);
	dib->canId = config->canId;
}