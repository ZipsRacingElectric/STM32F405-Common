// Header
#include "dhab_s124.h"

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Updates the value a channel.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The sensor to update (must be a @c dhabS124Channel_t* ).
 * @param sample The read sample.
 * @param sampleVdd The sample of the analog supply voltage.
 */
static void callback (void* object, uint16_t sample, uint16_t sampleVdd);

/**
 * @brief Updates the value of a sensor based on the values of its 2 channels.
 * @param sensor The sensor to update.
 */
static void update (dhabS124_t* sensor);

// Functions ------------------------------------------------------------------------------------------------------------------

bool dhabS124Init (dhabS124_t* sensor, const dhabS124Config_t* config)
{
	// Store the configurations
	sensor->config = config;
	sensor->channel1.config = &config->channel1Config;
	sensor->channel2.config = &config->channel2Config;
	sensor->channel1.callback = callback;
	sensor->channel2.callback = callback;
	sensor->channel1.parent = sensor;
	sensor->channel2.parent = sensor;

	// Validate the configurations
	if (config->channel1Config.sampleMin >= config->channel1Config.sampleMax)
		sensor->channel1.state = ANALOG_SENSOR_CONFIG_INVALID;
	else
		sensor->channel1.state = ANALOG_SENSOR_SAMPLE_INVALID;

	if (config->channel2Config.sampleMin >= config->channel2Config.sampleMax)
		sensor->channel2.state = ANALOG_SENSOR_CONFIG_INVALID;
	else
		sensor->channel2.state = ANALOG_SENSOR_SAMPLE_INVALID;

	// Set values to their defaults
	sensor->value = 0.0f;
	sensor->channel1.value = 0.0f;
	sensor->channel1.sample = 0;
	sensor->channel2.value = 0.0f;
	sensor->channel2.sample = 0;

	return sensor->channel1.state != ANALOG_SENSOR_CONFIG_INVALID ||
		sensor->channel2.state != ANALOG_SENSOR_CONFIG_INVALID;
}

void callback (void* object, uint16_t sample, uint16_t sampleVdd)
{
	dhabS124Channel_t* channel = (dhabS124Channel_t*) object;
	(void) sampleVdd;

	// Store the sample
	channel->sample = sample;

	// If the peripheral has failed or the config is invalid, don't check anything else.
	if (channel->state == ANALOG_SENSOR_CONFIG_INVALID || channel->state == ANALOG_SENSOR_FAILED)
		return;

	// Check the sample is in the valid range
	if (sample < channel->config->sampleMin || sample > channel->config->sampleMax)
	{
		channel->state = ANALOG_SENSOR_SAMPLE_INVALID;
		channel->value = 0;
		return;
	}

	channel->state = ANALOG_SENSOR_VALID;

	// Map input based on sensitivity and offset.
	channel->value = ((float) channel->sample - channel->config->sampleOffset) * channel->config->sensitivity;

	// Update the parent sensor (choose which channel to use)
	update (channel->parent);
}

static void update (dhabS124_t* sensor)
{
	// Prioritize channel 1, if saturated, switch to channel 2.
	bool channel1Saturated = sensor->channel1.value > sensor->config->channel1SaturationCurrent
		|| sensor->channel1.value < -sensor->config->channel1SaturationCurrent;

	if (channel1Saturated)
		sensor->value = sensor->channel2.value;
	else
		sensor->value = sensor->channel1.value;

	// Clamp any readings within the deadzone.
	if (sensor->value < sensor->config->deadzoneCurrent && sensor->value > -sensor->config->deadzoneCurrent)
		sensor->value = 0.0f;
}