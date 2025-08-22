#ifndef THERMISTOR_PULLDOWN_H
#define THERMISTOR_PULLDOWN_H

// Thermistor Pulldown Network ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.04.04
//
// Description: Analog sensor representing a thermistor connected as the pulldown resistor of a resistor divider.
//
// Circuit Schematic:
//    VDD
//    ---
//     |
//   -----
//   |   |
//   | R |
//   |   |
//   -----
//     |
//     |-----o ADC
//     |
//   -----
//   |   |
//   | T |
//   |   |
//   -----
//     |
//    ---
//    GND

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The A coefficient of the Steinhart-Hart equation.
	float steinhartHartA;

	/// @brief The B coefficient of the Steinhart-Hart equation.
	float steinhartHartB;

	/// @brief The C coefficient of the Steinhart-Hart equation.
	float steinhartHartC;

	/// @brief The D coefficient of the Steinhart-Hart equation.
	float steinhartHartD;

	/// @brief The reference resistance of the Steinhart-Hart equation.
	float resistanceReference;

	/// @brief The resistance of the circuit's pullup resistor, in Ohms.
	float resistancePullup;

	/// @brief The minimum plausible temperature, used to detect open-circuit faults.
	float temperatureMin;

	/// @brief The maximum plausible temperature, used to detect short-circuit faults.
	float temperatureMax;
} thermistorPulldownConfig_t;

/**
 * @brief Datatype representing a thermistor connected in a resistor divider.
 */
typedef struct
{
	ANALOG_SENSOR_FIELDS;
	const thermistorPulldownConfig_t* config;
	uint16_t sample;
	float resistance;
	float temperature;
	bool undertemperatureFault;
	bool overtemperatureFault;
} thermistorPulldown_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the thermistor using the specified configuration.
 * @param thermistor The thermistor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool thermistorPulldownInit (thermistorPulldown_t* thermistor, const thermistorPulldownConfig_t* config);

#endif // THERMISTOR_PULLDOWN_H