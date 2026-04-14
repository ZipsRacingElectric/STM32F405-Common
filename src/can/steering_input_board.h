#ifndef STEERING_INPUT_BOARD_H
#define STEERING_INPUT_BOARD_H

// Steering Input Board CAN Node ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.10.16
//
// Description: Object representing a driver input board CAN node. Note that multiple steering input boards can be used on the
//   same CAN bus, in which case the CAN ID is used to differentiate them.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Objects --------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The CAN driver of the bus the node belongs to.
	CANDriver* driver;

	/// @brief The amount of time the node should be timed out after.
	sysinterval_t timeoutPeriod;

	/// @brief The standard CAN ID of the device's input message.
	uint16_t canId;
} sibConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;

	uint16_t canId;

	/// @brief Indicates whether each button of the driver input board is pressed.
	bool buttonsPressed [8];

	/// @brief The analog reading of each analog input of the driver input board. In range [0, 1].
	float analogValues [2];
} sib_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void sibInit (sib_t* sib, const sibConfig_t* config);

#endif // STEERING_INPUT_BOARD_H