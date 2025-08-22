#ifndef CAN_THREAD_H
#define CAN_THREAD_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.04.01
//
// Description: Thread object for receiving and handling messages from a CAN driver. The thread polls the CAN driver until a
//   message is received. If the message belongs a known CAN node, its handler will be invoked. If the CAN message does not
//   belong to a known node, the generic handler will be invoked.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

#define CAN_THREAD_WORKING_AREA(name) THD_WORKING_AREA (name, 512)

typedef struct
{
	/// @brief Name to give the thread, used for debugging.
	const char* name;

	/// @brief The CAN driver the receive from.
	CANDriver* driver;

	/// @brief The minimum period to check the CAN node timeouts at.
	sysinterval_t period;

	/// @brief The array of CAN nodes to receive for (array of pointers).
	canNode_t** nodes;

	/// @brief The number of elements in the @c nodes array.
	uint16_t nodeCount;

	/// @brief Handler to invoke upon receiving an unknown CAN message.
	canReceiveHandler_t* rxHandler;

	/// @brief CAN driver to re-transmit received messages on. This is used to virtually bridge to CAN busses, while
	/// maintaining electrical separation. Note this re-transmission is uni-directional, to create a bi-directional bridge, a
	/// separate CAN thread should be instanced for the other driver.
	CANDriver* bridgeDriver;
} canThreadConfig_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates a thread bound to the specified CAN interface.
 * @param workingArea The working area to provide the thread. One instance must be created per thread. Should be instanced
 * using the @c CAN_THREAD_WORKING_AREA macro.
 * @param workingAreaSize The size of the thread's @c workingArea . Should be obtained using @c sizeof(workingArea) .
 * @param priority The priority to assign the thread.
 * @param config The configuration to provide the thread.
 */
void canThreadStart (void* workingArea, size_t workingAreaSize, tprio_t priority, const canThreadConfig_t* config);

#endif // CAN_THREAD_H