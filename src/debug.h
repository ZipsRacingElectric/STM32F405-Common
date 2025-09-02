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
#include "chprintf.h"

// Globals --------------------------------------------------------------------------------------------------------------------

extern BaseSequentialStream* debugStream;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Printf-like function for printing to the debug serial port. Note this can only be used after @c debugSerialInit has
 * been called.
 */
#define debugPrintf(...) chprintf (debugStream, __VA_ARGS__)

/**
 * @brief Initializes the specified serial driver as the debugging serial port.
 * @param serial The driver to initialize.
 * @param config The configuration to use. Use @c NULL for the default config.
 */
void debugSerialInit (SerialDriver* serial, SerialConfig* config);

/**
 * @brief Starts board's heartbeat thread.
 * @param ledLine Pointer to I/O line controlling the heartbeat LED. Note this pointer must be valid for the whole duration of
 * the program, otherwise a race condition occurs.
 * @param priority The priority to start the thread at.
 */
void debugHeartbeatStart (ioline_t* ledLine, tprio_t priority);

#endif // DEBUG_H