#ifndef AS5600_H
#define AS5600_H

// AS5600 Device Driver -------------------------------------------------------------------------------------------------------
//
// Author:
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
	/// @brief The 7-bit I2C address of the device.
	i2caddr_t addr;

	/// @brief The I2C bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval of a transaction.
	sysinterval_t timeout;
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
 * @brief Reads the value of a register from the device.
 * @param as5600 The device to read from.
 * @param addr The address to read from.
 * @param data Buffer to write the read value into.
 * @return True if successful, false otherwise.
 */
bool as5600ReadRegister (as5600_t* as5600, uint8_t addr, uint8_t* data);

/**
 * @brief Writes a value to a register of the device.
 * @param as5600 The device to write to.
 * @param addr The address to write to.
 * @param data The value to write.
 * @return True if successful, false otherwise.
 */
bool as5600WriteRegister (as5600_t* as5600, uint8_t addr, uint8_t data);

#endif // AS5600_H