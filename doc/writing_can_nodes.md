# Introduction to CAN Nodes - Zips Racing
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

	// 16-bit field transmitted in little-endian.
	uint16_t littleEndianField;

	// 16-bit field transmitted in big-endian.
	uint16_t bigEndianField;

	// 32-bit field transmitted in big-endian.
	uint32_t bigEndianField2;

	// 32-bit field.
	uint32_t longField;

	// 16-bit signed field.
	int16_t signedField;

	// Floating-point number field.
	float floatingField;

	// 1-bit field.
	bool boolField;

	// 3-bit field.
	uint8_t smallField;

	// 12-bit field.
	uint8_t mediumField;

	// 1-bit field.
	bool boolField2;
} testNode_t;

// Functions ------------------------------------------------------------------------------------------------------------------

// User function, initializes the node.
void testNodeInit (testNode_t* testNode, const testNodeConfig_t* config);

#endif // TEST_NODE_H
```

Source file `test_node.c`:
```
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
	// This signal is a 16-bit unsigned integer, transmitted in little-endian.
	// The STM's native format is little-endian, so we can read the 2 bytes directly into our field.
	// Note: data16 [0] refers to bytes 0 & 1.
	testNode->littleEndianField = frame->data16 [0];

	// This signal is also 16-bit unsigned integer, however it is transmitted in big-endian.
	// Here we have to reverse the data, the __REV16 instruction reverses the endianness
	// of a 16-bit number, which is exactly what we need.
	// Note: data16 [1] refers to bytes 2 & 3.
	testNode->bigEndianField = __REV16 (frame->data16 [1]);

	// This signal is a 32-bit unsigned integer, transmitted in big-endian.
	// We have to reverse the endianess here too, however this is a 32-bit number so we have to use the __REV instruction,
	// which operates on 32-bit numbers.
	// Note: data32 [1] refers to bytes 4, 5, 6, & 7.
	testNode->bigEndianField2 = __REV (frame->data32 [1]);

	// Note that all other signals in this example are little-endian, for convenience.
}

// Decodes the 2nd message
void testNodeHandleMessage1 (testNode_t* testNode, CANRxFrame* frame)
{
	// Since this is little-endian, we can just copy directly from the buffer as we did with the first message.
	// Note: data32 [0] refers to bytes 0, 1, 2, & 3.
	testNode->longField = frame->data32 [0];

	// This signal is a 16-bit signed integer.
	// For a signed integer, we take the data (which is interpreted as a uint16_t) and cast it into a int16_t.
	// Note: data16 [2] refers to bytes 4 & 5.
	testNode->signedField = (int16_t) frame->data16 [2];
}

// Decodes the 3rd message
void testNodeHandleMessage2 (testNode_t* testNode, CANRxFrame* frame)
{
	// If the node has a scale factor and offset, we can convert it into a float.
	// This is using a scale factor of 0.1 / LSB and offset of 100.
	// Here we are using a 16-bit signed integer, so we first cast the raw data into the correct type.
	// After casting, we apply our scale factor and offset.
	testNode->floatingField = ((int16_t) frame->data16 [0]) * 0.1f + 100.0f;

	// For signals that aren't multiples of 8-bit, we'll need to do bitwise operators. This one is a 3-bit unsigned integer, so
	// we use the bitwise AND operator to mask out the 3 bits we are interested in.
	testNode->smallField = frame->data16 [1] & 0b111;

	// For signals that don't start at bit 0, we'll need to use a shift operator. If we also need to mask, we can do so after
	// shifting.
	testNode->mediumField = (frame->data16 [1] >> 3) & 0b111111111111;

	// For 1-bit signals, we can treat them as booleans. By masking out the single bit we are interested in and checking its
	// value, it is converted into a bool.
	testNode->boolField = (frame->data8 [4] & 0b1) == 0b1;

	// We can also shift before converting into a bool.
	testNode->boolField2 = ((frame->data8 [4] >> 1) & 0b1) == 0b1;
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