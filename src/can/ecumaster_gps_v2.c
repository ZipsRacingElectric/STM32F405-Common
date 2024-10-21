// Header
#include "ecumaster_gps_v2.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define MESSAGE_0_ID 0x400
#define MESSAGE_1_ID 0x401
#define MESSAGE_2_ID 0x402
#define MESSAGE_3_ID 0x403
#define MESSAGE_4_ID 0x404

// Message Flags --------------------------------------------------------------------------------------------------------------

#define MESSAGE_0_FLAG_POS 0x00
#define MESSAGE_1_FLAG_POS 0x01
#define MESSAGE_2_FLAG_POS 0x02
#define MESSAGE_3_FLAG_POS 0x03
#define MESSAGE_4_FLAG_POS 0x04

// Function Prototypes --------------------------------------------------------------------------------------------------------

int8_t ecumasterReceiveHandler (void* node, CANRxFrame* frame);

// Functions -------------------------------------------------------------------------------------------------------------------

void ecumasterInit (ecumasterGps_t* gps, ecumasterGpsConfig_t* config)
{	
	// Initialize the CAN node
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= ecumasterReceiveHandler,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 5
	};
	canNodeInit ((canNode_t*) gps, &nodeConfig);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void ecumasterGpsHandler0 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler1 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler2 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler3 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler4 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

int8_t ecumasterReceiveHandler (void* node, CANRxFrame* frame)
{
	ecumasterGps_t* gps = (ecumasterGps_t*) node;
	uint16_t id = frame->SID;

	// TODO(Barach): Magic constants and bad names
	
	// Identify and handle the message.
	if (id == MESSAGE_0_ID)
	{
		// Message 0
		ecumasterGpsHandler0 (gps, frame);
		return MESSAGE_0_FLAG_POS;
	}
	else if (id == MESSAGE_1_ID)
	{
		// Message 1
		ecumasterGpsHandler1 (gps, frame);
		return MESSAGE_1_FLAG_POS;
	}
	else if (id == MESSAGE_2_ID)
	{
		// Message 2
		ecumasterGpsHandler2 (gps, frame);
		return MESSAGE_2_FLAG_POS;
	}
	else if (id == MESSAGE_3_ID)
	{
		// Message 3
		ecumasterGpsHandler3 (gps, frame);
		return MESSAGE_3_FLAG_POS;
	}
	else if (id == MESSAGE_4_ID)
	{
		// Message 4
		ecumasterGpsHandler4 (gps, frame);
		return MESSAGE_4_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}