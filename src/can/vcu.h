#ifndef VCU_H
#define VCU_H

// VCU CAN Node ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach, ...
// Date Created: 2025.09.18
//
// Description: TODO

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	CANDriver* driver;
	sysinterval_t timeoutPeriod;
} vcuConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;
	// TODO: Fields here
} vcu_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void vcuInit (vcu_t* vcu, const vcuConfig_t* config);

#endif // VCU_H