// Header
#include "can_thread.h"

// C Standard Library
#include <string.h>

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

THD_FUNCTION (canRxThread, arg)
{
	// Only argument is config
	const canThreadConfig_t* config = (canThreadConfig_t*) arg;

	// Set the name
	chRegSetThreadName (config->name);

	CANRxFrame rxFrame;

	systime_t timeCurrent = chVTGetSystemTimeX ();
	systime_t timePrevious;

	while (true)
	{
		// Block until the next message arrives, or timeout check, whichever is first
		msg_t result = canReceiveTimeout (config->driver, CAN_ANY_MAILBOX, &rxFrame, config->period);
		timePrevious = timeCurrent;
		timeCurrent = chVTGetSystemTimeX ();

		if (result == MSG_OK)
		{
			// Find the handler of the message
			if (!canNodesReceive (config->nodes, config->nodeCount, &rxFrame))
			{
				// If no node handled the message, pass it to the handler.
				if (config->rxHandler != NULL)
					config->rxHandler ((void*) config, &rxFrame);
			}

			// If a bridge driver is specified, relay the message
			if (config->bridgeDriver != NULL)
			{
				// Copy the received frame into a transmit frame.
				CANTxFrame txFrame =
				{
					.SID	= rxFrame.SID,
					.EID	= rxFrame.EID,
					.DLC	= rxFrame.DLC
				};
				memcpy (txFrame.data8, rxFrame.data8, rxFrame.DLC);

				// Re-transmit the frame on the bridge interface.
				canTransmitTimeout (config->bridgeDriver, CAN_ANY_MAILBOX, &txFrame, config->period);
			}
		}

		// Check node timeouts
		canNodesCheckTimeout (config->nodes, config->nodeCount, timePrevious, timeCurrent);
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void canThreadStart (void* workingArea, size_t workingAreaSize, tprio_t priority, const canThreadConfig_t* config)
{
	// Create and start the thread
	// - We are trusting the thread and RX handler to not modify the config, hence the cast.
	chThdCreateStatic (workingArea, workingAreaSize, priority, canRxThread, (void*) config);
}