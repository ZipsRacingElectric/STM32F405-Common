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

// Constants ------------------------------------------------------------------------------------------------------------------

#define BMS_CELL_COUNT			144
#define BMS_TEMPERATURE_COUNT	60

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
} bmsConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;
	bool tractiveSystemsActive;
	float cellVoltages [BMS_CELL_COUNT];
	float temperatures [BMS_TEMPERATURE_COUNT];
} bms_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, bmsConfig_t* config);

#endif // BMS_H