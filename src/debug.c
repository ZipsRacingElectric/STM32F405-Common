// Header
#include "debug.h"

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

void debugHeartbeatStart (ioline_t* ledLine, tprio_t priority)
{
	// Start the thread
	chThdCreateStatic (heartbeatWa, sizeof (heartbeatWa), priority, heartbeatThread, ledLine);
}