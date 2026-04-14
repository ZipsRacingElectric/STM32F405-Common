// Header
#include "steering_input_board.h"

#define SIB_MESSAGE_FLAG 0x00

static int8_t receive (void* node, CANRxFrame* frame)
{
	sib_t* sib = node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id == sib->canId)
	{
		// Handle input message

		// Button inputs
		for (uint8_t index = 0; index < 8; ++index)
			sib->buttonsPressed [index] = ((frame->data8 [0] >> index) & 0b1) == 0b1;

		// Analog inputs
		for (uint8_t index = 0; index < 2; ++index)
			sib->analogValues [index] = frame->data8 [index + 1] / 255.0f;

		return SIB_MESSAGE_FLAG;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}

void sibInit (sib_t* sib, const sibConfig_t* config)
{
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= receive,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 1,
	};
	canNodeInit ((canNode_t*) sib, &nodeConfig);
	sib->canId = config->canId;
}