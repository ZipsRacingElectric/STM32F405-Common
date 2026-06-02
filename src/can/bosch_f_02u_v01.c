// Header
#include "bosch_f_02u_v01.h"

// Conversions ----------------------------------------------------------------------------------------------------------------

// Acceleration (Unit g)
#define ACCELERATION_FACTOR 0.0001274f
#define ACCELERATION_OFFSET	-4.1746432f
#define WORD_TO_ACCELERATION(word) (((word) * ACCELERATION_FACTOR) + ACCELERATION_OFFSET)

// Angle Rate (Unit deg/s)
#define ANGLE_RATE_FACTOR 0.005f
#define ANGLE_RATE_OFFSET -163.84f
#define WORD_TO_ANGLE_RATE(word) (((word) * ANGLE_RATE_FACTOR) + ANGLE_RATE_OFFSET)

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define MESSAGE_1_ID		0x174
#define MESSAGE_2_ID		0x178
#define MESSAGE_3_ID		0x17C

// Message Flags --------------------------------------------------------------------------------------------------------------

#define MESSAGE_1_FLAG_POS	0x00
#define MESSAGE_2_FLAG_POS	0x01
#define MESSAGE_3_FLAG_POS	0x02

// Receive Functions ----------------------------------------------------------------------------------------------------------

static void handleMessage1 (boschF02uV01_t* imu, CANRxFrame* frame)
{
	imu->zAngleRate		= WORD_TO_ANGLE_RATE (frame->data16 [0]);
	imu->yAcceleration	= WORD_TO_ACCELERATION (frame->data16 [2]);
}

static void handleMessage2 (boschF02uV01_t* imu, CANRxFrame* frame)
{
	imu->xAngleRate		= WORD_TO_ANGLE_RATE (frame->data16 [0]);
	imu->xAcceleration	= WORD_TO_ACCELERATION (frame->data16 [2]);
}

static void handleMessage3 (boschF02uV01_t* imu, CANRxFrame* frame)
{
	imu->zAcceleration	= WORD_TO_ACCELERATION (frame->data16 [2]);
}

static int8_t receiveHandler (void* node, CANRxFrame* frame)
{
	boschF02uV01_t* imu = node;

	// Identify and handle the message.
	switch (frame->SID)
	{
		case MESSAGE_1_ID:
			handleMessage1 (imu, frame);
			return MESSAGE_1_FLAG_POS;

		case MESSAGE_2_ID:
			handleMessage2 (imu, frame);
			return MESSAGE_2_FLAG_POS;

		case MESSAGE_3_ID:
			handleMessage3 (imu, frame);
			return MESSAGE_3_FLAG_POS;
	}

	// Message doesn't belong to this node.
	return -1;
}

// Public Functions -----------------------------------------------------------------------------------------------------------

void boschF02uV01Init (boschF02uV01_t* imu, const boschF02uV01Config_t* config)
{
	// Initialize the CAN node
	canNodeConfig_t nodeConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= receiveHandler,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 3
	};
	canNodeInit ((canNode_t*) imu, &nodeConfig);
}