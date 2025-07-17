#ifndef CAN_THREAD_H
#define CAN_THREAD_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.04.01
//
// Description Thread object for receiving and handling messages from a CAN driver.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

#define CAN_THREAD_WORKING_AREA(name) THD_WORKING_AREA (name, 512)

typedef struct
{
	const char* name;
	CANDriver* driver;
	sysinterval_t period;
	canNode_t** nodes;
	uint16_t nodeCount;
	canReceiveHandler_t* rxHandler;
} canThreadConfig_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void canThreadStart (void* workingArea, size_t workingAreaSize, tprio_t priority, const canThreadConfig_t* config);

#endif // CAN_THREAD_H