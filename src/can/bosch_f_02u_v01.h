#ifndef BOSCH_F_02U_V01_H
#define BOSCH_F_02U_V01_H

// Bosch F 02U V01 511-02 IMU -------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.05.29
//
// Description: CAN node for the Bosch F 02U V01 511-02 IMU.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

/// @brief Configuration structure for the @c boschF02uV01_t struct.
typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
} boschF02uV01Config_t;

/// @brief Struct representing the Bosch F 02U V01 511-02 IMU.
typedef struct
{
	CAN_NODE_FIELDS;

	/// @brief The acceleration on the x-axis, in g's.
	float xAcceleration;
	/// @brief The acceleration on the y-axis, in g's.
	float yAcceleration;
	/// @brief The acceleration on the z-axis, in g's.
	float zAcceleration;

	/// @brief The angle rate on the x-axis, in deg/s.
	float xAngleRate;
	/// @brief The angle rate on the z-axis, in deg/s.
	float zAngleRate;
} boschF02uV01_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void boschF02uV01Init (boschF02uV01_t* imu, const boschF02uV01Config_t* config);

#endif // BOSCH_F_02U_V01_H