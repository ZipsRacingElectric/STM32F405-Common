#ifndef ECUMASTER_GPS_V2_H
#define ECUMASTER_GPS_V2_H

// ECUMaster GPS to CAN V2 ----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
//
// Description: Object representing the ECUMaster GPS CAN module.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	ECUMASTER_GPS_STATUS_INVALID	= 0,
	ECUMASTER_GPS_STATUS_NO_FIX		= 1,
	ECUMASTER_GPS_STATUS_VALID		= 3,
} ecumasterGpsStatus_t;

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
} ecumasterGpsConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;

	float latitude;
	float longitude;
	float speed;
	float height;
	uint8_t satellitesNumber;
	uint8_t gpsFrameIndex;
	uint8_t emptyFrameIndex;
	ecumasterGpsStatus_t gpsStatus;
	float headingMotion;
	float headingVehicle;
	float xAngleRate;
	float yAngleRate;
	float zAngleRate;
	float xAcceleration;
	float yAcceleration;
	float zAcceleration;
} ecumasterGps_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void ecumasterInit (ecumasterGps_t* gps, const ecumasterGpsConfig_t* config);

ecumasterGpsStatus_t ecumasterGpsStatus (ecumasterGps_t* gps);

#endif // ECUMASTER_GPS_V2_H