#ifndef DRIVER_INPUT_BOARD_H
#define DRIVER_INPUT_BOARD_H

// Driver Input Board CAN Node ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.16
//
// Description: Object representing a driver input board CAN node. Note that multiple driver input boards can be used on the
//   same CAN bus, in which case CAN ID is used to differentiate them.

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
} dibConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;

	uint16_t canId;

	/// @brief Indicates whether each button of the driver input board is pressed.
	bool buttonsPressed [8];

	/// @brief The analog reading of each analog input of the driver input board. In range [0, 1].
	float analogValues [2];
} dib_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void dibInit (dib_t* dib, dibConfig_t* config);

#endif // DRIVER_INPUT_BOARD_H