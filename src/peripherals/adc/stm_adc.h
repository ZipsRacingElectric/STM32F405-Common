#ifndef STM_ADC_H
#define STM_ADC_H

// STM32 On-Board Analog to Digital Converter ---------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.29
//
// Description: Wrapper for the ChibiOS ADC driver. This object is intended to wrap access to the ADC peripheral such that
//   multiple unrelated objects may share access.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum number of channels in an ADC conversion group.
#define STM_ADC_CHANNEL_COUNT 16

// Datatypes ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Configuration for the @c stmAdc_t object.
 */
typedef struct
{
	/// @brief The ADC peripheral to use.
	ADCDriver* driver;

	/// @brief The ADC channels to sample, in order. @note Un-used channels must be initialized to 0.
	adc_channels_num_t channels [STM_ADC_CHANNEL_COUNT];

	/// @brief The sensor to update with for each channel. The @c callback field of each sensor is called when a value is
	/// sampled.
	analogSensor_t* sensors [STM_ADC_CHANNEL_COUNT];

	/// @brief The number of ADC channels to sample, number of initialized elements in @c sensors .
	uint16_t channelCount;
} stmAdcConfig_t;

/**
 * @brief Peripheral representing the STM's onboard ADC. This peripheral exposes a set of callbacks that, upon sampling,
 * update each sensor's reading.
 */
typedef struct
{
	const stmAdcConfig_t* config;
	ADCConversionGroup group;
	adcsample_t buffer [STM_ADC_CHANNEL_COUNT];
} stmAdc_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the peripheral using the specified configuration.
 * @param adc The ADC to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool stmAdcInit (stmAdc_t* adc, const stmAdcConfig_t* config);

/**
 * @brief Samples all of the ADC's channels, blocking until the operation is complete.
 * @param adc The ADC to sample from.
 * @return True is successful, false otherwise.
 */
bool stmAdcSample (stmAdc_t* adc);

#endif // STM_ADC_H