#ifndef THERMISTOR_PULLDOWN_H
#define THERMISTOR_PULLDOWN_H

// Thermistor Pulldown Network ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.04.04
//
// Description: Analog sensors representing a thermistor connected as the pulldown resistor of a resistor divider.
//
// Circuit Schematic:
//    VDD
//    ---
//     |
//   -----
//   |   |
//   | R | Pullup resistance (constant)
//   |   |
//   -----
//     |
//     |-----o ADC
//     |
//   -----
//   |   |
//   | T | Thermistor resistance (variable)
//   |   |
//   -----
//     |
//    ---
//    GND
//
// Note that there are multiple ways of converting from a thermistor's resistance to a temperature. These different models of
//   conversion use different parameters to characterize the thermistor, and therefore use different configurations. Depending
//   on what information is provided in the datasheet, different models can be used here.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

/// @brief Config for the @c thermistorSteinhartHartPulldown_t analog sensor.
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

	/// @brief The reference resistance of the Steinhart-Hart equation, in Ohms.
	float resistanceReference;

	/// @brief The resistance of the circuit's pullup resistor, in Ohms.
	float resistancePullup;

	/// @brief The minimum plausible temperature, used to detect open-circuit faults.
	float temperatureMin;

	/// @brief The maximum plausible temperature, used to detect short-circuit faults.
	float temperatureMax;
} thermistorSteinhartHartPulldownConfig_t;

/**
 * @brief Datatype representing a thermistor connected in a resistor divider.
 * @note This thermistor uses the Steinhart-Hart model for temperature conversion. See below for more info:
 * https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
 */
typedef struct
{
	ANALOG_SENSOR_FIELDS;
	const thermistorSteinhartHartPulldownConfig_t* config;
	uint16_t sample;
	float resistance;
	float temperature;
	bool undertemperatureFault;
	bool overtemperatureFault;
} thermistorSteinhartHartPulldown_t;

/// @brief Config for the @c thermistorBetaPulldown_t analog sensor.
typedef struct
{
	/// @brief The beta coefficient of the thermistor's Beta Parameter Equation (in degrees Kelvin).
	float beta;

	/// @brief The resistance of the thermistor at a known temperature (the reference temperature), in Ohms.
	float referenceResistance;

	/// @brief The temperature of the thermistor at a known resistance (the reference resistance), in degrees Celsius.
	float referenceTemperature;

	/// @brief The resistance of the circuit's pullup resistor, in Ohms.
	float resistancePullup;

	/// @brief The minimum plausible temperature, used to detect open-circuit faults.
	float temperatureMin;

	/// @brief The maximum plausible temperature, used to detect short-circuit faults.
	float temperatureMax;
} thermistorBetaPulldownConfig_t;

/**
 * @brief Datatype representing a thermistor connected in a resistor divider.
 * @note This thermistor uses the Beta Parameter Equation for temperature conversion. See below for more info:
 * https://www.ametherm.com/thermistor/ntc-thermistor-beta
 */
typedef struct
{
	ANALOG_SENSOR_FIELDS;
	const thermistorBetaPulldownConfig_t* config;
	uint16_t sample;
	float resistance;
	float temperature;
	bool undertemperatureFault;
	bool overtemperatureFault;
} thermistorBetaPulldown_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the thermistor using the specified configuration.
 * @param thermistor The thermistor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool thermistorSteinhartHartPulldownInit (thermistorSteinhartHartPulldown_t* thermistor,
	const thermistorSteinhartHartPulldownConfig_t* config);

/**
 * @brief Initializes the thermistor using the specified configuration.
 * @param thermistor The thermistor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool thermistorBetaPulldownInit (thermistorBetaPulldown_t* thermistor, const thermistorBetaPulldownConfig_t* config);

#endif // THERMISTOR_PULLDOWN_H