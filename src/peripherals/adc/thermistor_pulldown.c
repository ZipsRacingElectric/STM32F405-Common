// Header
#include "peripherals/adc/thermistor_pulldown.h"

// Includes
#include "controls/steinhart_hart.h"

// C Standard Library
#include <math.h>

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Updates the temperature of a Steinhart-Hart thermistor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The thermistor to update (must be a @c thermistorSteinhartHartPulldown_t* ).
 * @param sample The read sample.
 * @param sampleVdd The sample of the analog supply voltage.
 */
static void steinhartHartCallback (void* object, uint16_t sample, uint16_t sampleVdd);

/**
 * @brief Updates the temperature of a Beta Model thermistor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The thermistor to update (must be a @c thermistorBetaPulldown_t* ).
 * @param sample The read sample.
 * @param sampleVdd The sample of the analog supply voltage.
 */
static void betaCallback (void* object, uint16_t sample, uint16_t sampleVdd);

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Gets the resistance of a thermistor from the output sample and the VDD sample.
 * @param sample The output sample.
 * @param sampleVdd The sample of the VDD voltage supply.
 * @param resistancePullup The resistance of the pullup resistor.
 * @return The calculated resistance, in Ohms.
 */
static float sampleToResistance (uint16_t sample, uint16_t sampleVdd, float resistancePullup)
{
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

	return sample * resistancePullup / (sampleVdd - sample);
}

/**
 * @brief Converts a thermistor's resistance to a temperature based on the Beta Parameter Equation.
 * @param resistance The measured resistance.
 * @param beta The beta parameter, in degrees Kelvin.
 * @param referenceResistance The reference resistance (resistance at reference temperature), in Ohms.
 * @param referenceTemperature The reference temperature (ex. 25C), in degrees Celsius.
 * @return The calculated temperature, in degrees Celsius.
 */
static float betaTemperature (float resistance, float beta, float referenceResistance, float referenceTemperature)
{
	// Refactored version of the equation provided from: https://www.ametherm.com/thermistor/ntc-thermistor-beta.
	return 1 / (1 / beta * logf (resistance / referenceResistance) + 1 / (referenceTemperature + 273.15f)) - 273.15f;
}

bool thermistorSteinhartHartPulldownInit (thermistorSteinhartHartPulldown_t* thermistor, const thermistorSteinhartHartPulldownConfig_t* config)
{
	// Store the configuration
	thermistor->config = config;
	thermistor->callback = steinhartHartCallback;

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

void steinhartHartCallback (void* object, uint16_t sample, uint16_t sampleVdd)
{
	thermistorSteinhartHartPulldown_t* thermistor = (thermistorSteinhartHartPulldown_t*) object;

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
		thermistor->state = ANALOG_SENSOR_SAMPLE_INVALID;
		thermistor->overtemperatureFault = true;
		thermistor->temperature = thermistor->config->temperatureMax;
		return;
	}

	thermistor->state = ANALOG_SENSOR_VALID;

	// Calculate the resistance and temperature from the given information

	thermistor->resistance = sampleToResistance (sample, sampleVdd, thermistor->config->resistancePullup);

	thermistor->temperature = steinhartHartTemperature (thermistor->resistance, thermistor->config->resistanceReference,
		thermistor->config->steinhartHartA, thermistor->config->steinhartHartB, thermistor->config->steinhartHartC,
		thermistor->config->steinhartHartD);

	thermistor->overtemperatureFault = thermistor->temperature > thermistor->config->temperatureMax;
	thermistor->undertemperatureFault = thermistor->temperature < thermistor->config->temperatureMin;
}

bool thermistorBetaPulldownInit (thermistorBetaPulldown_t* thermistor, const thermistorBetaPulldownConfig_t* config)
{
	// Store the configuration
	thermistor->config = config;
	thermistor->callback = betaCallback;

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

void betaCallback (void* object, uint16_t sample, uint16_t sampleVdd)
{
	thermistorBetaPulldown_t* thermistor = (thermistorBetaPulldown_t*) object;

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
		thermistor->state = ANALOG_SENSOR_SAMPLE_INVALID;
		thermistor->overtemperatureFault = true;
		thermistor->temperature = thermistor->config->temperatureMax;
		return;
	}

	thermistor->state = ANALOG_SENSOR_VALID;

	// Calculate the resistance and temperature from the given information

	thermistor->resistance = sampleToResistance (sample, sampleVdd, thermistor->config->resistancePullup);

	thermistor->temperature = betaTemperature (thermistor->resistance, thermistor->config->beta,
		thermistor->config->referenceResistance, thermistor->config->referenceTemperature);

	thermistor->overtemperatureFault = thermistor->temperature > thermistor->config->temperatureMax;
	thermistor->undertemperatureFault = thermistor->temperature < thermistor->config->temperatureMin;
}