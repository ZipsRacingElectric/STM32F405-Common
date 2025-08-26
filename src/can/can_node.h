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
#include "ch.h"
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	/// @brief Indicates all the data in a CAN node is complete and up-to-date.
	CAN_NODE_VALID = 0,

	/// @brief Indicates some data in a CAN node is not complete (due to a timeout).
	CAN_NODE_INCOMPLETE = 1,

	/// @brief Indicates all data in a CAN node is invalid (has timed out).
	CAN_NODE_TIMEOUT = 2
} canNodeState_t;

/**
 * @brief Function for handling received CAN messages. This should check the message's @c SID / @c EID to determine if the
 * message belongs to this node.
 * @param node A pointer to the node to update (datatype is implementation specific).
 * @param frame The frame that was received.
 * @return A unique index for what message was received, or -1 to indicate the message does not belong to this node.
 */
typedef int8_t (canReceiveHandler_t) (void* node, CANRxFrame* frame);

/**
 * @brief Function for handling a generic event triggered by a CAN node.
 * @param node The CAN node that triggered the event.
 */
typedef void (canEventHandler_t) (void* node);

typedef struct
{
	/// @brief The CAN driver of the bus the node belongs to. Used for sending messages to the node.
	CANDriver* driver;

	/// @brief Function for handling received messages that may or may not belong to the node.
	canReceiveHandler_t* receiveHandler;

	/// @brief Optional function to invoke when the node's data times out, use @c NULL to not handle timeout events.
	canEventHandler_t* timeoutHandler;

	/// @brief The interval to timeout the node's data after.
	sysinterval_t timeoutPeriod;

	/// @brief The total number of messages belonging to the node. Used to determine if the dataset is complete or not.
	uint8_t messageCount;
} canNodeConfig_t;

#define CAN_NODE_FIELDS							\
	canNodeState_t			state;				\
	CANDriver*				driver;				\
	canReceiveHandler_t*	receiveHandler;		\
	canEventHandler_t*		timeoutHandler;		\
	sysinterval_t			timeoutPeriod;		\
	systime_t				timeoutDeadline;	\
	uint64_t				messageFlags;		\
	uint64_t				validFlags;			\
	mutex_t					mutex

/**
 * @brief Polymorphic base object representing a node in a CAN bus.
 * @note When deriving this struct, use the @c CAN_NODE_FIELDS macro to define the first set of fields.
 */
typedef struct
{
	CAN_NODE_FIELDS;
} canNode_t;

// CAN Node Functions ---------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the CAN node using the specified configuration. Note that this should only be used internally by CAN
 * nodes. To initialize an implementation of a node, use the implementation provided initializer.
 * @param node The node to initialize.
 * @param config The configuration to use.
 */
void canNodeInit (canNode_t* node, const canNodeConfig_t* config);

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
void canNodeCheckTimeout (canNode_t* node, systime_t timePrevious, systime_t timeCurrent);

/**
 * @brief Locks a CAN node for exclusive access.
 * @param node The node to lock.
 */
void canNodeLock (canNode_t* node);

/**
 * @brief Unlocks a previously locked CAN node.
 * @param node The node to unlock.
 */
void canNodeUnlock (canNode_t* node);

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
void canNodesCheckTimeout (canNode_t** nodes, uint8_t nodeCount, systime_t timePrevious, systime_t timeCurrent);

#endif // CAN_NODE_H