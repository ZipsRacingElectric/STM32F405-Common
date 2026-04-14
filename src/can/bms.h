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
} bms_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, const bmsConfig_t* config);

#endif // BMS_H