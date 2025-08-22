#ifndef ANALOG_LINEAR_H
#define ANALOG_LINEAR_H

// Analog Linear Sensor -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.29
//
// Description: Object representing a sensor with a linear transfer function.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The minimum raw ADC measurement of the sensor.
	uint16_t sampleMin;

	/// @brief The maximum raw ADC measurement of the sensor.
	uint16_t sampleMax;

	/// @brief The output value mapped to the minimum sample value.
	float valueMin;

	/// @brief The output value mapped to the maximum sample value.
	float valueMax;

} linearSensorConfig_t;

typedef struct
{
	ANALOG_SENSOR_FIELDS;
	const linearSensorConfig_t*	config;
	uint16_t sample;
	float value;
} linearSensor_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the sensor using the specified configuration.
 * @param sensor The sensor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool linearSensorInit (linearSensor_t* sensor, const linearSensorConfig_t* config);

#endif // ANALOG_LINEAR_H