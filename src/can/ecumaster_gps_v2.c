// Header
#include "ecumaster_gps_v2.h"

// Includes
#include "debug.h"

// Conversions ----------------------------------------------------------------------------------------------------------------

// Coordinates
#define COORDINATE_FACTOR 1E-7f
#define WORD_TO_COORDINATE(word) (((int32_t) __REV (word)) * COORDINATE_FACTOR)

// Speed (Unit km/h)
#define SPEED_FACTOR 0.036f
#define WORD_TO_SPEED(word) (((int16_t) __REV16 (word)) * SPEED_FACTOR)

// Length (Unit m)
#define LENGTH_FACTOR 1
#define WORD_TO_LENGTH(word) (((int16_t) __REV16 (word)) * LENGTH_FACTOR)

// Angle (Unit deg)
#define ANGLE_FACTOR 1
#define WORD_TO_ANGLE(word) (__REV16 (word) * ANGLE_FACTOR)

// Angle Rate (Unit deg/s)
#define ANGLE_RATE_FACTOR 0.01f
#define WORD_TO_ANGLE_RATE(word) (((int16_t) __REV16 (word)) * ANGLE_RATE_FACTOR)

// Acceleration (Unit g)
#define ACCELERATION_FACTOR 0.01f
#define WORD_TO_ACCELERATION(word) (((int16_t) __REV16 (word)) * ACCELERATION_FACTOR)

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define POSITION_MESSAGE_ID		0x400
#define VELOCITY_MESSAGE_ID		0x401
#define HEADING_IMU0_MESSAGE_ID	0x402
#define IMU1_MESSAGE_ID			0x403
#define UTC_MESSAGE_ID			0x404

// Message Flags --------------------------------------------------------------------------------------------------------------

#define POSITION_MESSAGE_FLAG_POS		0x00
#define VELOCITY_MESSAGE_FLAG_POS		0x01
#define HEADING_IMU0_MESSAGE_FLAG_POS	0x02
#define IMU1_MESSAGE_FLAG_POS			0x03
#define UTC_MESSAGE_FLAG_POS			0x04

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

void ecumasterHandlePosition (ecumasterGps_t* gps, CANRxFrame* frame)
{
	gps->latitude	= WORD_TO_COORDINATE (frame->data32 [0]);
	gps->longitude	= WORD_TO_COORDINATE (frame->data32 [1]);
}

void ecumasterHandleVelocity (ecumasterGps_t* gps, CANRxFrame* frame)
{
	gps->speed				= WORD_TO_SPEED (frame->data16 [0]);
	gps->height				= WORD_TO_LENGTH (frame->data16 [1]);
	gps->satellitesNumber	= frame->data8 [5];
	gps->gpsFrameIndex		= frame->data8 [6] & 0xF;
	gps->emptyFrameIndex	= (frame->data8 [6] >> 4) & 0xF;
	gps->gpsStatus			= frame->data8 [7] & 0x7;
}

void ecumasterHandleHeadingImu0 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	gps->headingMotion	= WORD_TO_ANGLE (frame->data16 [0]);
	gps->headingVehicle	= WORD_TO_ANGLE (frame->data16 [1]);
	gps->xAngleRate		= WORD_TO_ANGLE_RATE (frame->data16 [2]);
	gps->yAngleRate		= WORD_TO_ANGLE_RATE (frame->data16 [3]);
}

void ecumasterHandleImu1 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	gps->zAngleRate		= WORD_TO_ANGLE_RATE (frame->data16 [0]);
	gps->xAcceleration	= WORD_TO_ACCELERATION (frame->data16 [1]);
	gps->yAcceleration	= WORD_TO_ACCELERATION (frame->data16 [2]);
	gps->zAcceleration	= WORD_TO_ACCELERATION (frame->data16 [3]);
}

void ecumasterUtc (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

int8_t ecumasterReceiveHandler (void* node, CANRxFrame* frame)
{
	ecumasterGps_t* gps = (ecumasterGps_t*) node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id == POSITION_MESSAGE_ID)
	{
		ecumasterHandlePosition (gps, frame);
		return POSITION_MESSAGE_FLAG_POS;
	}
	else if (id == VELOCITY_MESSAGE_ID)
	{
		ecumasterHandleVelocity (gps, frame);
		return VELOCITY_MESSAGE_FLAG_POS;
	}
	else if (id == HEADING_IMU0_MESSAGE_ID)
	{
		ecumasterHandleHeadingImu0 (gps, frame);
		return HEADING_IMU0_MESSAGE_FLAG_POS;
	}
	else if (id == IMU1_MESSAGE_ID)
	{
		ecumasterHandleImu1 (gps, frame);
		return IMU1_MESSAGE_FLAG_POS;
	}
	else if (id == UTC_MESSAGE_ID)
	{
		ecumasterUtc (gps, frame);
		return UTC_MESSAGE_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}