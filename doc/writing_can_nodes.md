# Writing CAN Nodes - Zips Racing
The STM32F405 common library further abstracts ChibiOS's CAN interface with the CAN node interface. The main purpose of a CAN node is to decode the CAN messages of a specific devices a the CAN-bus (sensors and control units, for example). The CAN node loosely uses an inheritance pattern, meaning there is an interface (the `canNode_t` struct) and then there are implementors of said interface (ex. `bms_t` or `amk_t`). The interface is never meant to be directly instanced, rather it simply describes how specific CAN nodes should look.

## Declaration
In order to make different implementations of the CAN node appear the same, they must begin with the same fields. This can be done using the `CAN_NODE_FIELDS` macro. For example, consider the below implementation:
```
typedef struct
{
	// The structure must start with these fields.
	CAN_NODE_FIELDS;

	// Now we can add whatever we want here...
	uint8_t testField0;
	uint8_t testField1;
} testNode_t;
```

By defining the structure this way, it is interchangeable with the `canNode_t` base structure.
```
// Here we have some instance of the structure we defined.
testNode_t nodeInstance = ...;

// To use it with a function expecting a base CAN node, we can use a cast.
canNodeResetTimeout ((canNode_t*) &nodeInstance);
```

## Initialization
Before it can be used, a CAN node must first be initialized and configured. To do this the `canNodeConfig_t` structure and `canNodeInit` function are provided.

The configuration structure allows the implementor to modify the behavior of the node. Note the `receiveHandler` function, which will be described later in the 'Receive Handler' section of this document.
```
struct canNodeConfig_t
{
	// The CAN driver of the bus the node belongs to. Used for sending messages to the node.
	CANDriver* driver;

	// Function for handling received messages that may or may not belong to the node.
	canReceiveHandler_t* receiveHandler;

	// Optional function to invoke when the node's data times out, use NULL to not handle timeout events.
	canEventHandler_t* timeoutHandler;

	// The interval to timeout the node's data after.
	sysinterval_t timeoutPeriod;

	// The total number of messages belonging to the node. Used to determine if the dataset is complete or not.
	uint8_t messageCount;
};
```

Inside of its implementation of an initialization function, a CAN node should call the `canNodeInit` function to initialize the fields of the `canNode_t` portion of structure.
```
void canNodeInit (canNode_t* node, const canNodeConfig_t* config);
```
Parameters:
- `node` - A pointer to the node to initialize.
- `config` - A pointer to the configuration to use.

## Implementing a Receive Handler
Each CAN node implementation should have a unique `canReceiveHandler_t` function. This purpose of this function is to determine whether the received message belongs to a node (by checking the frame's SID/EID). If the message does belong to the node, the message should be decoded to update the internal field(s) of the structure.
```
typedef int8_t (canReceiveHandler_t) (void* node, CANRxFrame* frame);
```
Parameters:
- `node`	- A pointer to the CAN node (the node that should be checked for / updated).
- `frame`	- The CAN frame that was received.

Return Value:
- A unique index for what message was received, between 0 (inclusive) and `messageCount` (exclusive).
- -1 to indicate the message does not belong to this node.

## A Complete Example
Header file `test_node.h`:
```
#ifndef TEST_NODE_H
#define TEST_NODE_H

// Includes
#include "can/can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	// The CAN driver of the bus the node belongs to. Used for sending messages to the node.
	CANDriver* driver;

	// The interval to timeout the node's data after.
	sysinterval_t timeoutPeriod;

	// If we have other application-specific things, add them here...
} testNodeConfig_t;

typedef struct
{
	// All CAN nodes must start with this macro.
	CAN_NODE_FIELDS;

	float signal0;
	float signal1;
	uint32_t signal2;
	uint32_t signal3;
	uint8_t signal4;
} testNode_t;

// Functions ------------------------------------------------------------------------------------------------------------------

// User function, initializes the node.
void testNodeInit (testNode_t* testNode, const testNodeConfig_t* config);

#endif // TEST_NODE_H
```

Source file `test_node.c`:
```
// Note that all signals in this example are little-endian, for convenience.

// Header
#include "test_node.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define MESSAGE_0_ID 0x10A					// The standard ID of the 1st message.
#define MESSAGE_1_ID 0x10B					// The standard ID of the 2nd message.
#define MESSAGE_2_ID 0x10C					// The standard ID of the 3rd message.

#define MESSAGE_0_FLAG_POS 0x00				// The flag index of the 1st message.
#define MESSAGE_1_FLAG_POS 0x01				// The flag index of the 2nd message.
#define MESSAGE_2_FLAG_POS 0x02				// The flag index of the 3rd message.

// Function Prototypes --------------------------------------------------------------------------------------------------------

// Implements the canReceiveHandler_t signature. See 'Implementing a Receive Handler' for what this does.
int8_t testNodeReceiveHandler (void* node, CANRxFrame* frame);

// Functions ------------------------------------------------------------------------------------------------------------------

void testNodeInit (testNode_t* testNode, const testNodeConfig_t* config)
{
	// Here we have to initialize the canNode_t portion of the structure.
	canNodeConfig_t nodeConfig =
	{
		// Use the user-specified CANDriver.
		.driver = config->driver,

		// Use the internal receive handler we've defined.
		.receiveHandler = testNodeReceiveHandler,

		// We don't care about timeouts, so no handler needed.
		.timeoutHandler = NULL,

		// Use the user-specified timeout.
		.timeoutPeriod = config->timeoutPeriod,

		// We have 3 total messages (flags 0, 1, and 2).
		.messageCount = 3
	};
	canNodeInit ((canNode_t*) testNode, &nodeConfig);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

// Decodes the 1st message
void testNodeHandleMessage0 (testNode_t* testNode, CANRxFrame* frame)
{
	// Update the relevant fields. In this case, the message contains 2 float signals.
	testNode->signal0 = frame->data16 [0] * 0.1f;
	testNode->signal1 = frame->data16 [1] * 0.1f;
}

// Decodes the 2nd message
void testNodeHandleMessage1 (testNode_t* testNode, CANRxFrame* frame)
{
	// Update the relevant fields. In this case the message contains 2 32-bit unsigned ints.
	testNode->signal2 = frame->data32 [0];
	testNode->signal3 = frame->data32 [1];
}

// Decodes the 3rd message
void testNodeHandleMessage2 (testNode_t* testNode, CANRxFrame* frame)
{
	// Update the relevant fields. In this case, the message only contains 1 byte.
	testNode->signal4 = frame->data8 [0];
}

int8_t testNodeReceiveHandler (void* node, CANRxFrame* frame)
{
	// First cast the pointer into the correct datatype. The function signature has to be generic, so it uses void*.
	testNode_t* testNode = (testNode_t*) node;

	// Get the ID of the message. If it is an extended ID, we use the EID field.
	uint16_t id = frame->SID;

	// Identify whether the message belongs to this node, and, if so, which message it is.
	if (id == MESSAGE_0_ID)
	{
		// Handle message 0.
		testNodeHandleMessage0 (testNode, frame);
		return MESSAGE_0_FLAG_POS;
	}
	else if (id == MESSAGE_1_ID)
	{
		// Handle message 1.
		testNodeHandleMessage1 (testNode, frame);
		return MESSAGE_1_FLAG_POS;
	}
	else if (id == MESSAGE_2_ID)
	{
		// Handle message 2.
		testNodeHandleMessage2 (testNode, frame);
		return MESSAGE_2_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node, we return -1 to indicate this.
		return -1;
	}
}
```