#ifndef AS5600_H
#define AS5600_H

// AS5600 Device Driver -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach, ...
// Date Created: 2025.08.22
//
// Description: I2C driver class for the AS5600 angular encoder.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief the 7-bit I2C address of the device
	i2caddr_t addr;
	
	/// @brief The I2C bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval of a transaction. Note that if this is less than 20ms, transactions occurring after a write
	/// operation may fail.
	sysinterval_t timeout;

	/// @brief as5600 start position register (minimum angle).
	const uint8_t ZPOS_REG;

	/// @brief as5600 stop position register (maximum angle).
	const uint8_t MPOS_REG;

	/// @brief Use as5600 ANGLE register not RAW ANGLE register.
	const uint8_t ANGLE_REG;

	/// @brief as5600 uses 12-bit registers (0 - 4095).
	uint16_t minAngle;

	/// @brief as5600 uses 12-bit registers (0 - 4095).
	uint16_t maxAngle;

	/// @brief Binary representation of angle from ANGLE register affected by MPOS and ZPOS, (0 - 4095).
	uint16_t binAngle;

	/// @brief The sensor to call after sampling successfully.
	analogSensor_t* sensor;

} as5600Config_t;

typedef struct
{
	const as5600Config_t* config;
} as5600_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the device using the specified configuration.
 * @param as5600 The device to initialize.
 * @param config The configuration to use.
 * @return True if successful, false if a hardware failure occurred.
 */
bool as5600Init (as5600_t* as5600, const as5600Config_t* config);

/**
 * @brief Samples the position of the sensor.
 * @param as5600 The device to sample.
 * @return MSG_OK if successful.
 */
msg_t as5600Sample(as5600_t* as5600);

#endif // AS5600_H