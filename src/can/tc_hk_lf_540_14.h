#ifndef TC_HK_LF_540_14_H
#define TC_HK_LF_540_14_H

// TC Charger Model HK-LF-540-14 CAN Interface --------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.02.21
//
// Description: CAN node representing the TC 6.6kW on-board charger.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/can_node.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum requestable voltage limit, in volts.
#define TC_CHARGER_VOLTAGE_LIMIT_MAX 680

/// @brief The maximum requestable current limit, in amps.
#define TC_CHARGER_CURRENT_LIMIT_MAX 14

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	TC_CHARGER_FAULTED	= 0,
	TC_CHARGER_IDLE		= 1,
	TC_CHARGER_CHARGING	= 2
} tcChargingState_t;

typedef enum
{
	TC_WORKING_MODE_STARTUP = 0,
	TC_WORKING_MODE_CLOSING = 1,
	TC_WORKING_MODE_SLEEP	= 2
} tcWorkingMode_t;

typedef struct
{
	CANDriver* driver;
	sysinterval_t timeoutPeriod;
} tcChargerConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;

	const tcChargerConfig_t* config;

	tcChargingState_t chargingState;
	float outputVoltage;
	float outputCurrent;
} tcCharger_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void tcChargerInit (tcCharger_t* charger, const tcChargerConfig_t* config);

/**
 * @brief Sends a command to the TC charger.
 * @param charger The charger to command.
 * @param mode The working mode to command, see @c tcWorkingMode_t for details.
 * @param voltageLimit The voltage limit to set, in volts.
 * @param currentLimit The current limit to set, in amps.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t tcChargerSendCommand (tcCharger_t* charger, tcWorkingMode_t mode, float voltageLimit, float currentLimit,
	sysinterval_t timeout);

#endif // TC_HK_LF_540_14_H