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

	/// @brief Indicates whether each button of the steering input board is pressed.
	bool buttonsPressed [8];

	/// @brief Indicates whether each button of the steering input board is held.
	bool buttonsHeld [8];

	/// @brief The analog reading of each analog input of the steering input board. In range [0, 1].
	float analogValues [2];
} sib_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void sibInit (sib_t* sib, const sibConfig_t* config);

/**
 * @brief Checks whether a button on the steering input board has just been pressed. Next call to this will return false until
 * the button is released.
 * @note This automatically locks and unlocks the CAN node. Do not call between locks.
 * @param sib The steering input board to check.
 * @param index The index of the button to check.
 * @return True if the button has just been pressed, false otherwise.
 */
bool sibGetButtonDownLock (sib_t* sib, uint8_t index);

/**
 * @brief Checks whether a button on the steering input board is being held. This will return true until the button is
 * released.
 * @note This automatically locks and unlocks the CAN node. Do not call between locks.
 * @param sib The steering input board to check.
 * @param index The index of the button to check.
 * @return True if the button is being held, false otherwise.
 */
bool sibGetButtonHeldLock (sib_t* sib, uint8_t index);

/**
 * @brief Gets the value of an analog input (paddle).
 * @note This automatically locks and unlocks the CAN node. Do not call between locks.
 * @param sib The steering input board to check.
 * @param index The index of the analog value to get.
 * @return The analog value, or 0 if invalid.
 */
float sibGetAnalogValueLock (sib_t* sib, uint8_t index);

#endif // STEERING_INPUT_BOARD_H