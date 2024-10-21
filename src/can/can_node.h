#ifndef CAN_NODE_H
#define CAN_NODE_H

// CAN Node -------------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
//
// Description: Base object representing a node in a CAN bus. This object provides a standard interface for an object that
//   broadcasts periodic CAN messages.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

enum canNodeState
{
	CAN_NODE_VALID		= 0,
	CAN_NODE_INCOMPLETE	= 1,
	CAN_NODE_TIMEOUT	= 2
};

typedef enum canNodeState canNodeState_t;

/**
 * @brief Function for handling received CAN messages. This function should return a unique index for what message was
 * received, or -1 to indicate the message does not belong to this node.
 */
typedef int8_t (canReceiveHandler_t) (void* node, CANRxFrame* frame);

typedef void (canEventHandler_t) (void* node);

struct canNodeConfig
{
	CANDriver*				driver;
	canReceiveHandler_t*	receiveHandler;
	canEventHandler_t*		timeoutHandler;
	sysinterval_t			timeoutPeriod;
	uint8_t					messageCount;
};

typedef struct canNodeConfig canNodeConfig_t;

#define CAN_NODE_FIELDS							\
	canNodeState_t			state;				\
	CANDriver*				driver;				\
	canReceiveHandler_t*	receiveHandler;		\
	canEventHandler_t*		timeoutHandler;		\
	sysinterval_t			timeoutPeriod;		\
	systime_t				timeoutDeadline;	\
	uint64_t				messageFlags;		\
	uint64_t				validFlags

/**
 * @brief Polymorphic base object representing a node in a CAN bus.
 * @note When deriving this struct, use the @c CAN_NODE_FIELDS macro to define
 * the first set of fields.
 */
struct canNode
{
	CAN_NODE_FIELDS;
};

typedef struct canNode canNode_t;

// CAN Node Functions ---------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the CAN node using the specified configuration.
 * @param node The node to initialize.
 * @param config The configuration to use.
 */
void canNodeInit (canNode_t* node, canNodeConfig_t* config);

/**
 * @brief Checks whether a received CAN message originated from a node. Resets the node's timeout deadline and message validity
 * flags if so.
 * @param node The node to check.
 * @param frame The received CAN frame.
 * @return True if the message was identified and handled, false otherwise.
 */
bool canNodeReceive (canNode_t* node, CANRxFrame* frame);

/**
 * @brief Checks whether the CAN node's timeout deadline has expired. If so, the node is put into the @c CAN_NODE_TIMEOUT state
 * the timeout event handler is called.
 * @param node The node to check.
 * @param timeCurrent The current system time.
 */
void canNodeCheckTimeout (canNode_t* node, systime_t timeCurrent);

// CAN Node Array Functions ---------------------------------------------------------------------------------------------------

/**
 * @brief Checks whether a received CAN message originated from a node within an array.
 * @param nodes The array of nodes to check.
 * @param nodeCount The number of elements in @c nodes .
 * @param frame The received CAN frame.
 * @return True if the message was identified and handled, false otherwise.
 */
bool canNodesReceive (canNode_t** nodes, uint8_t nodeCount, CANRxFrame* frame);

/**
 * @brief Checks whether the timeout deadline of each CAN node in an array has expired.
 * @param nodes The array of nodes to check.
 * @param nodeCount The number of elements in @c nodes .
 */
void canNodesCheckTimeout (canNode_t** nodes, uint8_t nodeCount);

#endif // CAN_NODE_H