// Header
#include "can_node.h"

// Function Prototypes --------------------------------------------------------------------------------------------------------

void canNodeResetTimeout (canNode_t* node);

// Functions ------------------------------------------------------------------------------------------------------------------

void canNodeInit (canNode_t* node, canNodeConfig_t* config)
{
	// Store the configuration
	node->driver			= config->driver;
	node->receiveHandler	= config->receiveHandler;
	node->timeoutPeriod		= config->timeoutPeriod;

	// Reset the message flags
	node->messageFlags = 0;
	
	// Calculate the messsage flags that indicate validity.
	node->validFlags = ((1 << config->messageCount) - 1);

	canNodeResetTimeout (node);
}

bool canNodeReceive (canNode_t* node, CANRxFrame* frame)
{
	// Call the receive handler, exit early if the message is not from this node.
	int8_t result = node->receiveHandler (node, frame);
	if (result < 0)
		return false;

	// Reset the timeout.
	canNodeResetTimeout (node);

	// Mark this message as received.
	uint8_t index = (uint8_t) result;
	node->messageFlags |= (1 << index);

	// If all messages have been received, mark the node as valid.
	if (node->messageFlags == node->validFlags)
		node->state = CAN_NODE_VALID;

	return true;
}

void canNodeCheckTimeout (canNode_t* node, systime_t timeCurrent)
{
	// Skip this check if the node is already timed-out.
	if (node->state == CAN_NODE_TIMEOUT)
		return;

	// Check the timeout deadline, exit early if not expired.
	if (node->timeoutDeadline > timeCurrent)
		return;

	// Enter the timeout state
	node->state = CAN_NODE_TIMEOUT;
	
	// Reset the validity flags
	node->messageFlags = 0;

	// Call the node's timeout handler (if set)
	if (node->timeoutHandler != NULL)
		node->timeoutHandler (node);
}

void canNodeResetTimeout (canNode_t* node)
{
	// Postpone the timeout deadline.
	systime_t timeCurrent = chVTGetSystemTime ();
	node->timeoutDeadline = chTimeAddX (timeCurrent, node->timeoutPeriod);

	// Assume the node is incomplete until validity is checked.
	node->state = CAN_NODE_INCOMPLETE;
}

bool canNodesReceive (canNode_t** nodes, uint8_t nodeCount, CANRxFrame* frame)
{
	for (uint8_t index = 0; index < nodeCount; ++index)
		if (canNodeReceive (nodes [index], frame))
			return true;

	return false;
}

void canNodesCheckTimeout (canNode_t** nodes, uint8_t nodeCount)
{
	systime_t timeCurrent = chVTGetSystemTime ();
	for (uint8_t index = 0; index < nodeCount; ++index)
		canNodeCheckTimeout (nodes [index], timeCurrent);
}