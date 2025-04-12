// Header
#include "analog_linear.h"

// Includes
#include "controls/lerp.h"

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Updates the value of the sensor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The sensor to update (must be a @c linearSensor_t* ).
 * @param sample The read sample.
 * @param sampleVdd The sample of the analog supply voltage.
 */
static void callback (void* object, uint16_t sample, uint16_t sampleVdd);

// Functions ------------------------------------------------------------------------------------------------------------------

bool linearSensorInit (linearSensor_t* sensor, linearSensorConfig_t* config)
{
	// Store the configuration
	sensor->config = config;
	sensor->callback = callback;

	// Validate the configuration
	if (config->sampleMin >= config->sampleMax)
		sensor->state = ANALOG_SENSOR_CONFIG_INVALID;
	else
	 	sensor->state = ANALOG_SENSOR_SAMPLE_INVALID;

	// Set values to their defaults
	sensor->value = 0.0f;
	sensor->sample = 0;

	return sensor->state != ANALOG_SENSOR_CONFIG_INVALID;
}

void callback (void* object, uint16_t sample, uint16_t sampleVdd)
{
	// TODO(Barach): How to manage this?
	(void) sampleVdd;

	linearSensor_t* sensor = (linearSensor_t*) object;

	// Store the sample.
	sensor->sample = sample;

	// If the peripheral has failed or the config is invalid, don't check anything else.
	if (sensor->state == ANALOG_SENSOR_CONFIG_INVALID || sensor->state == ANALOG_SENSOR_FAILED)
		return;

	// Check the sample is in the valid range
	if (sample < sensor->config->sampleMin || sample > sensor->config->sampleMax)
	{
		sensor->state = ANALOG_SENSOR_SAMPLE_INVALID;
		sensor->value = 0;
		return;
	}

	sensor->state = ANALOG_SENSOR_VALID;

	// Map input min to output min, input max to output max.
	sensor->value = lerp2d (sample, sensor->config->sampleMin, sensor->config->valueMin,
		sensor->config->sampleMax, sensor->config->valueMax);
}