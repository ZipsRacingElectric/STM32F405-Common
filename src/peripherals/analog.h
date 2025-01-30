#ifndef ANALOG_H
#define ANALOG_H

// Analog to Digital Converter Wrapper ----------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.29
//
// Description: Wrapper for the ChibiOS ADC driver. This object is intended to wrap access to the ADC peripheral such that
//   multiple unrelated objects may share access.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum number of channels in an ADC conversion group.
#define ANALOG_CHANNEL_COUNT 16

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef void (analogHandler_t) (void* object, adcsample_t sample);

typedef struct
{
	/// @brief The ADC peripheral to use.
	ADCDriver* driver;

	/// @brief The ADC channels to sample, in order. @note Un-used channels must be initialized to @c ADC_CHANNEL_NULL .
	adc_channels_num_t channels [ANALOG_CHANNEL_COUNT];

	/// @brief The number of ADC channels to sample.
	uint16_t channelCount;

	/// @brief Event handler for each channel's sample being completed.
	analogHandler_t* handlers [ANALOG_CHANNEL_COUNT];

	/// @brief Subscriber to each channel's event handler, passed as @c object of the handler.
	void* objects [ANALOG_CHANNEL_COUNT];
} analogConfig_t;

typedef struct
{
	analogConfig_t*		config;
	ADCConversionGroup	group;
	adcsample_t			buffer [ANALOG_CHANNEL_COUNT];
} analog_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the peripheral using the specified configuration.
 * @param analog The ADC to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool analogInit (analog_t* analog, analogConfig_t* config);

/**
 * @brief Samples all of the ADC's channels, blocking until the operation is complete.
 * @param analog The ADC to sample from.
 * @return True is successful, false otherwise.
 */
bool analogSample (analog_t* analog);

#endif // ANALOG_H