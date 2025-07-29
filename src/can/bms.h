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
	bool undervoltageFault;
	bool overvoltageFault;
	bool undertemperatureFault;
	bool overtemperatureFault;
	bool senseLineFault;
	bool isoSpiFault;
	bool selfTestFault;
	bool charging;
	bool balancing;
	bool shutdownClosed;
	bool prechargeComplete;
	bool bmsRelayStatus;
	bool imdRelayStatus;
} bms_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, const bmsConfig_t* config);

#endif // BMS_H