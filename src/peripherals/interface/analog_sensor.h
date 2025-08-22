#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

// Analog Sensor --------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2022.03.01
//
// Description: Base object for an analog sensor. This object provides a standard interface for any peripheral that applies a
//   transfer function to an ADC measurement. Note the ADC can be external to this device (doesn't need to be the STM ADC).

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Represents state of an analog sensor.
 */
typedef enum
{
	/// @brief State indicating a sensor has experienced a hardware failure.
	ANALOG_SENSOR_FAILED = 0,

	/// @brief State indicating a sensor's configuration is invalid.
	ANALOG_SENSOR_CONFIG_INVALID = 1,

	/// @brief State indicating a sensor has read an invalid sample.
	ANALOG_SENSOR_SAMPLE_INVALID = 2,

	/// @brief State indicating a sensor is valid.
	ANALOG_SENSOR_VALID = 3
} analogSensorState_t;

/**
 * @brief Callback for when an analog sensor is sampled.
 * @param object The handler of the callback.
 * @param sample The sample that was taken. Range depends on the resolution of the caller ADC.
 * @param sampleVdd The sample of the analog supply voltage. May be constant depending on the ADC implementation.
 */
typedef void (analogSensorHandler_t) (void* sensor, uint16_t sample, uint16_t sampleVdd);

#define ANALOG_SENSOR_FIELDS			\
	analogSensorHandler_t* callback;	\
	analogSensorState_t state

/**
 * @brief Polymorphic base object representing an analog sensor.
 */
typedef struct
{
	ANALOG_SENSOR_FIELDS;
} analogSensor_t;

#endif // ANALOG_SENSOR_H