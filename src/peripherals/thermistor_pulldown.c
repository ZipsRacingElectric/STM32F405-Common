// Header
#include "thermistor_pulldown.h"

// Includes
#include "controls/steinhart_hart.h"

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Updates the temperature of a thermistor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The thermistor to update (must be a @c thermistorPulldown_t* ).
 * @param sample The read sample.
 * @param sampleVdd The sample of the analog supply voltage.
 */
static void callback (void* object, uint16_t sample, uint16_t sampleVdd);

// Functions ------------------------------------------------------------------------------------------------------------------

bool thermistorPulldownInit (thermistorPulldown_t* thermistor, const thermistorPulldownConfig_t* config)
{
	// Store the configuration
	thermistor->config = config;
	thermistor->callback = callback;

	// Validate the configuration
	if (config->temperatureMin >= config->temperatureMax)
		thermistor->state = ANALOG_SENSOR_CONFIG_INVALID;
	else
	 	thermistor->state = ANALOG_SENSOR_SAMPLE_INVALID;

	// Set values to their defaults
	thermistor->temperature = 0.0f;
	thermistor->sample = 0;

	return thermistor->state != ANALOG_SENSOR_CONFIG_INVALID;
}

void callback (void* object, uint16_t sample, uint16_t sampleVdd)
{
	thermistorPulldown_t* thermistor = (thermistorPulldown_t*) object;

	// Store the sample.
	thermistor->sample = sample;

	// If the peripheral has failed or the config is invalid, don't check anything else.
	if (thermistor->state == ANALOG_SENSOR_CONFIG_INVALID || thermistor->state == ANALOG_SENSOR_FAILED)
		return;

	// Check the sample is in the valid range
	if (sample >= sampleVdd)
	{
		thermistor->state = ANALOG_SENSOR_SAMPLE_INVALID;
		thermistor->undertemperatureFault = true;
		thermistor->temperature = thermistor->config->temperatureMin;
		return;
	}
	else if (sample == 0)
	{
		thermistor->sample = ANALOG_SENSOR_SAMPLE_INVALID;
		thermistor->overtemperatureFault = true;
		thermistor->temperature = thermistor->config->temperatureMax;
		return;
	}

	thermistor->state = ANALOG_SENSOR_VALID;

	// Circuit Analysis:
	//   V_ADC (R_Therm) = R_Therm * V_VDD / (R_Pullup + R_Therm)
	//   R_Therm (V_ADC) = V_ADC * R_Pullup / (V_VDD - V_ADC)
	//
	//   X_ADC (V_ADC) = X_VDD * V_ADC / V_VDD
	//   V_ADC (X_ADC) = X_ADC * V_VDD / X_VDD
	//
	//   R_Therm (X_ADC) = X_ADC * V_VDD / X_VDD * R_Pullup / (V_VDD - X_ADC * V_VDD / X_VDD)
	//                   = X_ADC * R_Pullup / (X_VDD * (1 - X_ADC/X_VDD))
	//                   = X_ADC * R_Pullup / (X_VDD - X_ADC)

	thermistor->resistance = sample * thermistor->config->resistancePullup / (sampleVdd - sample);

	thermistor->temperature = steinhartHartTemperature (thermistor->resistance, thermistor->config->resistanceReference,
		thermistor->config->steinhartHartA, thermistor->config->steinhartHartB, thermistor->config->steinhartHartC,
		thermistor->config->steinhartHartD);

	thermistor->overtemperatureFault = thermistor->temperature > thermistor->config->temperatureMax;
	thermistor->undertemperatureFault = thermistor->temperature < thermistor->config->temperatureMin;
}