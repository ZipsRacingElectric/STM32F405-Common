#ifndef DEBUG_H
#define DEBUG_H

// Debug ----------------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.21
//
// Description: Group of functions related to board debugging.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Starts board's heartbeat thread.
 * @param ledLine Pointer to I/O line controlling the heartbeat LED. Note this pointer must be valid for the whole duration of
 * the program, otherwise a race condition occurs.
 * @param priority The priority to start the thread at.
 */
void debugHeartbeatStart (ioline_t* ledLine, tprio_t priority);

#endif // DEBUG_H