#ifndef DHAB_S124_H
#define DHAB_S124_H

// DHAB S/124 Current Sensor --------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.04.17
//
// Description: Analog sensor representing the DHAB S/124 current sensor. This sensor is dual channel, having a
//   low-range / high-precision channel and a high-range / low-precision channel. When sampled, the sensor object will choose
//   the more precise of the two channels.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The sensitivity of the channel, in amps per count.
	float sensitivity;

	/// @brief The offset of the channel sample, in counts.
	uint16_t sampleOffset;

	/// @brief The minimum plausible sample, before offset.
	uint16_t sampleMin;

	/// @brief The maximum plausible sample, before offset.
	uint16_t sampleMax;
} dhabS124ChannelConfig_t;

struct dhabS124;

typedef struct
{
	ANALOG_SENSOR_FIELDS;

	/// @brief The configuration of the channel.
	const dhabS124ChannelConfig_t* config;

	/// @brief The sensor this channel belongs to.
	struct dhabS124* parent;

	/// @brief The raw sample of this channel.
	uint16_t sample;

	/// @brief The sampled current value, in amps.
	float value;
} dhabS124Channel_t;

typedef struct
{
	/// @brief The configuration the first (low-range) channel of the device.
	dhabS124ChannelConfig_t channel1Config;

	/// @brief The configuration of the second (high-range) channel of the device.
	dhabS124ChannelConfig_t channel2Config;

	/// @brief The current at which channel 1 saturates (positive and negative). This determines whether channel 1 or channel
	/// 2 should be used.
	float channel1SaturationCurrent;

	/// @brief The magnitude of current below which to clamp to the output 0A.
	float deadzoneCurrent;
} dhabS124Config_t;

struct dhabS124
{
	/// @brief The configuration of the sensor.
	const dhabS124Config_t* config;

	/// @brief The first (low-range) channel of the device.
	dhabS124Channel_t channel1;

	/// @brief The second (high-range) channel of the device.
	dhabS124Channel_t channel2;

	/// @brief The sampled current value, in amps.
	float value;
};
typedef struct dhabS124 dhabS124_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool dhabS124Init (dhabS124_t* sensor, const dhabS124Config_t* config);

#endif // DHAB_S124_H