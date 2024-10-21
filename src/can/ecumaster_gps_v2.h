#ifndef ECUMASTER_GPS_V2_H
#define ECUMASTER_GPS_V2_H

// ECUMaster GPS to CAN V2 ----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
// 
// Description: Object representing the ECUMaster GPS CAN module.
//
// To do:
// - Handler implementations.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct ecumasterGpsConfig
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
};

typedef struct ecumasterGpsConfig ecumasterGpsConfig_t;

struct ecumasterGps
{
	CAN_NODE_FIELDS;

	float speed;
	float headingMotion;
	float headingVehicle;
	float xAngleRate;
	float yAngleRate;
	float zAngleRate;
	float xAcceleration;
	float yAcceleration;
	float zAcceleration;
};

typedef struct ecumasterGps ecumasterGps_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void ecumasterInit (ecumasterGps_t* gps, ecumasterGpsConfig_t* config);

#endif // ECUMASTER_GPS_V2_H