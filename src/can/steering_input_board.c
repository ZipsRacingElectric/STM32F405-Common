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
		{
			sib->buttonsPressed [index] = ((frame->data8 [0] >> index) & 0b1) == 0b1;

			if (!sib->buttonsPressed [index])
				sib->buttonsHeld [index] = false;
		}

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

bool sibGetButtonDownLock (sib_t* sib, uint8_t index)
{
	canNodeLock ((canNode_t*) sib);

	// Button was just pressed down if it is currently pressed and is not being held.
	bool value = sib->buttonsPressed [index] && !sib->buttonsHeld [index];

	// Button is held if it is currently pressed.
	sib->buttonsHeld [index] = sib->buttonsPressed [index];

	canNodeUnlock ((canNode_t*) sib);

	return value;
}

bool sibGetButtonHeldLock (sib_t* sib, uint8_t index)
{
	canNodeLock ((canNode_t*) sib);

	// Button is held if it is currently pressed.
	sib->buttonsHeld [index] = sib->buttonsPressed [index];
	bool value = sib->buttonsHeld [index];

	canNodeUnlock ((canNode_t*) sib);

	return value;
}

float sibGetAnalogValueLock (sib_t* sib, uint8_t index)
{
	canNodeLock ((canNode_t*) sib);
	float value = sib->analogValues [index];
	canNodeUnlock ((canNode_t*) sib);

	return value;
}