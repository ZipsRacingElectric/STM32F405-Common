#ifndef BMS_H
#define BMS_H

// Battery Management System CAN Node -----------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.16
//
// Description: Object representing the BMS CAN node.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
} bmsConfig_t;

typedef enum {

    /// @brief No state present that would cause the battery to put the car in limp mode.
    LIMP_MODE_OFF = 0b00,

    /// @brief Number of cells have dropped within voltage limp mode margin.
    LIMP_MODE_VOLTAGE = 0b01,

    /// @brief Number of temp sensors have risen within limp mode margin. 
    LIMP_MODE_TEMP = 0b10,

    /// @brief Both voltage condition and temp condition have been entered for limp mode.
    LIMP_MODE_BOTH = 0b11
    
} limpModeState_t;

typedef struct
{
	CAN_NODE_FIELDS;

	/// @brief Indicates a BMS fault is present.
	bool bmsFault;

	/// @brief Indicates an IMD fault is present.
	bool imdFault;

	/// @brief Indicates the BMS is charging.
	bool charging;

	/// @brief Indicates the BMS is balancing.
	bool balancing;

	/// @brief Indicates the negative isolation relay is closed. This does not indicate precharge is complete, for that,
	/// see @c positiveIrClosed .
	bool negativeIrClosed;

	/// @brief Indicates the positive isolation relay is closed. This indicates precharge is complete.
	bool positiveIrClosed;

	/// @brief The voltage of the pack.
	float packVoltage;

	/// @brief The current being delivered by the pack.
	float packCurrent;

	/// @brief The power being delivered by the pack.
	float powerDelivery;

	/// @brief The max temperature of the sense lines.
	float temperatureMax;
	
	/// @brief The average temperature of the sense lines.
	float temperatureAvg;

	/// @brief Indicates the BMS put the car in limp mode.
	limpModeState_t limpModeState;
} bms_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, const bmsConfig_t* config);

/**
 * @brief Gets the power delivery of the BMS.
 * @note Locks and unlocks the CAN node's mutex.
 * @param bms The BMS to use.
 * @return The power delivery, in Watts.
 */
float bmsGetPowerLock (bms_t* bms);

#endif // BMS_H