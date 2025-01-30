#ifndef SENSOR_LINEAR_H
#define SENSOR_LINEAR_H

// Linear Analog Sensor -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.29
//
// Description: Object representing a sensor with a linear transfer function. While this is mainly designed to be used with the
//   analog_t object, this may be used to represent analog input of any form (ex. CAN).

// Includes -------------------------------------------------------------------------------------------------------------------

#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The minimum raw ADC measurement of the sensor.
	adcsample_t sampleMin;

	/// @brief The maximum raw ADC measurement of the sensor.
	adcsample_t sampleMax;

	/// @brief The output value mapped to the minimum sample value.
	float valueMin;

	/// @brief The output value mapped to the maximum sample value.
	float valueMax;

} linearSensorConfig_t;

typedef enum
{
	/// @brief Indicates the sensor's configuration is invalid.
	LINEAR_SENSOR_CONFIG_INVALID = 0,

	/// @brief Indicates the sensor's last read value was invalid.
	LINEAR_SENSOR_VALUE_INVALID = 2,

	/// @brief Indicates the sensor's reading is valid.
	LINEAR_SENSOR_VALID = 3
} linearSensorState_t;

typedef struct
{
	linearSensorState_t		state;
	linearSensorConfig_t*	config;
	adcsample_t				sample;
	float					value;
} linearSensor_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the sensor using the specified configuration.
 * @param sensor The sensor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool linearSensorInit (linearSensor_t* sensor, linearSensorConfig_t* config);

/**
 * @brief Updates the value of the sensor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The sensor to update (must be a @c linearSensor_t* ).
 * @param sample The read sample.
 */
void linearSensorUpdate (void* object, adcsample_t sample);

#endif // SENSOR_LINEAR_H