// Header
#include "analog.h"

bool analogInit (analog_t* analog, analogConfig_t* config)
{
	// Store the configuration.
	analog->config = config;

	// Compute the conversion group for the ADC.
	ADCConversionGroup group =
	{
		.circular		=	false,
		.num_channels	=	config->channelCount,
		.end_cb			= 	NULL,
		.error_cb		= 	NULL,
		.cr1			=	0,
		.cr2			=	ADC_CR2_SWSTART,									// ADC is started by software.
		.smpr1			=	(ADC_SAMPLE_480 << ADC_SMPR1_SMP15_Pos) |			// Channel 15 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP14_Pos) |			// Channel 14 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP13_Pos) |			// Channel 13 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP12_Pos) |			// Channel 12 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP11_Pos) |			// Channel 11 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP10_Pos),			// Channel 10 sample time.
		.smpr2			= 	(ADC_SAMPLE_480 << ADC_SMPR2_SMP9_Pos) |			// Channel 9 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP8_Pos) |			// Channel 8 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP7_Pos) |			// Channel 7 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP6_Pos) |			// Channel 6 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP5_Pos) |			// Channel 5 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP4_Pos) |			// Channel 4 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP3_Pos) |			// Channel 3 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP2_Pos) |			// Channel 2 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP1_Pos) |			// Channel 1 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP0_Pos),				// Channel 0 sample time.
		.htr			=	0,													// No watchdog threshold.
		.ltr			=	0,
		.sqr1			= 	(ADC_SQR1_SQ16_N (config->channels [15])) |			// Sample 16 channel index.
							(ADC_SQR1_SQ15_N (config->channels [14])) |			// Sample 15 channel index.
							(ADC_SQR1_SQ14_N (config->channels [13])) |			// Sample 14 channel index.
							(ADC_SQR1_SQ13_N (config->channels [12])),			// Sample 13 channel index.
		.sqr2			=	(ADC_SQR2_SQ12_N (config->channels [11])) |			// Sample 12 channel index.
							(ADC_SQR2_SQ11_N (config->channels [10])) |			// Sample 11 channel index.
							(ADC_SQR2_SQ10_N (config->channels [9])) |			// Sample 10 channel index.
							(ADC_SQR2_SQ9_N (config->channels [8])) |			// Sample 9 channel index.
							(ADC_SQR2_SQ8_N (config->channels [7])) |			// Sample 8 channel index.
							(ADC_SQR2_SQ7_N (config->channels [6])),			// Sample 7 channel index.
		.sqr3			=	(ADC_SQR3_SQ6_N (config->channels [5])) |			// Sample 6 channel index.
							(ADC_SQR3_SQ5_N (config->channels [4])) |			// Sample 5 channel index.
							(ADC_SQR3_SQ4_N (config->channels [3])) |			// Sample 4 channel index.
							(ADC_SQR3_SQ3_N (config->channels [2])) |			// Sample 3 channel index.
							(ADC_SQR3_SQ2_N (config->channels [1])) |			// Sample 2 channel index.
							(ADC_SQR3_SQ1_N (config->channels [0]))				// Sample 1 channel index.
	};
	analog->group = group;

	// Start the ADC with default configuration.
	return adcStart (analog->config->driver, NULL) == MSG_OK;
}

bool analogSample (analog_t* analog)
{
	// If the API is enabled, lock the ADC's mutex.
	#if ADC_USE_MUTUAL_EXCLUSION
	adcAcquireBus (analog->config->driver);
	#endif // ADC_USE_MUTUAL_EXCLUSION

	// Sample the ADC.
	if (adcConvert (analog->config->driver, &analog->group, analog->buffer, 1) != MSG_OK)
		return false;

	// If the API is enabled, unlock the ADC's mutex.
	#if ADC_USE_MUTUAL_EXCLUSION
	adcReleaseBus (analog->config->driver);
	#endif // ADC_USE_MUTUAL_EXCLUSION

	// Call the conversion event handlers.
	for (adc_channels_num_t index = 0; index < analog->config->channelCount; ++index)
		if (analog->config->handlers [index] != NULL)
			analog->config->handlers [index] (analog->config->objects [index], analog->buffer [index]);

	return true;
}