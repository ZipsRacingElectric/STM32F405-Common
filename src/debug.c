// Header
#include "debug.h"

// Globals --------------------------------------------------------------------------------------------------------------------

BaseSequentialStream* debugStream = NULL;

// Threads --------------------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (heartbeatWa, 128);

static THD_FUNCTION (heartbeatThread, arg)
{
	ioline_t ledLine = *((ioline_t*) arg);
	chRegSetThreadName ("heartbeat");

	while (true)
	{
		palSetLine (ledLine);
		chThdSleepMilliseconds (500);
		palClearLine (ledLine);
		chThdSleepMilliseconds (500);
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void debugSerialInit (SerialDriver* serial, SerialConfig* config)
{
	// Start the driver
	sdStart (serial, config);

	// Set the debug stream
	debugStream = (BaseSequentialStream*) serial;

	debugPrintf ("Initializing board '%s'...\r\n", BOARD_NAME);
}

void debugHeartbeatStart (ioline_t* ledLine, tprio_t priority)
{
	// Start the thread
	chThdCreateStatic (heartbeatWa, sizeof (heartbeatWa), priority, heartbeatThread, ledLine);
}