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
	ECUMASTER_GPS_STATUS_2D			= 3,
	ECUMASTER_GPS_STATUS_3D			= 4
} ecumasterGpsStatus_t;

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
} ecumasterGpsConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;

	// GPS data

	/// @brief The GPS latitude, in degrees. Only valid if @c ecumasterGpsValid returns @c true .
	float latitude;
	/// @brief The GPS longitude, in degrees. Only valid if @c ecumasterGpsValid returns @c true .
	float longitude;
	/// @brief The GPS-based speed, in km/h. Only valid if @c ecumasterGpsValid returns @c true .
	float speed;
	/// @brief The GPS-based height, in m. Only valid if @c ecumasterGpsValid returns @c true and the GPS has a 3D lock.
	float height;

	// IMU data

	/// @brief The angle rate along the x-axis, in deg/s.
	float xAngleRate;
	/// @brief The angle rate along the y-axis, in deg/s.
	float yAngleRate;
	/// @brief The angle rate along the z-axis, in deg/s.
	float zAngleRate;

	/// @brief The acceleration along the x-axis, in g's.
	float xAcceleration;
	/// @brief The acceleration along the y-axis, in g's.
	float yAcceleration;
	/// @brief The acceleration along the z-axis, in g's.
	float zAcceleration;

	// Misc
	uint8_t satellitesNumber;
	uint8_t gpsFrameIndex;
	uint8_t emptyFrameIndex;
	ecumasterGpsStatus_t gpsStatus;
	float headingMotion;
	float headingVehicle;
} ecumasterGps_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void ecumasterInit (ecumasterGps_t* gps, const ecumasterGpsConfig_t* config);

static inline ecumasterGpsStatus_t ecumasterGetGpsStatus (ecumasterGps_t* gps)
{
	if (gps->state != CAN_NODE_VALID)
		return ECUMASTER_GPS_STATUS_INVALID;

	return gps->gpsStatus;
}

static inline bool ecumasterGpsValid (ecumasterGps_t* gps)
{
	ecumasterGpsStatus_t status = ecumasterGetGpsStatus (gps);
	return status == ECUMASTER_GPS_STATUS_2D || status == ECUMASTER_GPS_STATUS_3D;
}

#endif // ECUMASTER_GPS_V2_H