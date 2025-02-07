// Header
#include "linear_sensor.h"

// Includes
#include "controls/lerp.h"

bool linearSensorInit (linearSensor_t* sensor, linearSensorConfig_t* config)
{
	// Store the configuration
	sensor->config = config;

	// Validate the configuration
	if (config->sampleMin >= config->sampleMax)
		sensor->state = LINEAR_SENSOR_CONFIG_INVALID;
	else
	 	sensor->state = LINEAR_SENSOR_VALUE_INVALID;

	// Set values to their defaults
	sensor->value = 0.0f;
	sensor->sample = 0;

	return sensor->state != LINEAR_SENSOR_CONFIG_INVALID;
}

void linearSensorUpdate (void* object, adcsample_t sample)
{
	linearSensor_t* sensor = (linearSensor_t*) object;

	// Store the sample.
	sensor->sample = sample;

	// If the config is invalid, don't check anything else.
	if (sensor->state == LINEAR_SENSOR_CONFIG_INVALID)
		return;

	// Check the sample is in the valid range
	if (sample < sensor->config->sampleMin || sample > sensor->config->sampleMax)
	{
		sensor->state = LINEAR_SENSOR_VALUE_INVALID;
		sensor->value = 0;
		return;
	}

	sensor->state = LINEAR_SENSOR_VALID;

	// Map input min to output min, input max to output max.
	sensor->value = lerp2d (sample, sensor->config->sampleMin, sensor->config->valueMin,
		sensor->config->sampleMax, sensor->config->valueMax);
}