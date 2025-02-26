#ifndef MC24LC32_H
#define MC24LC32_H

// MC24LC32 Device Driver -----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
//
// Description: Driver for the Microchip 24LC32 I2C EEPROM.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "eeprom.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief Memory size of the MC24LC32 EEPROM in bytes.
#define MC24LC32_SIZE 4096

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	MC24LC32_STATE_FAILED	= 0,
	MC24LC32_STATE_INVALID	= 1,
	MC24LC32_STATE_READY	= 3
} mc24lc32State_t;

typedef struct
{
	/// @brief The 7-bit I2C address of the device.
	i2caddr_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval for the device's acknowledgement polling. If the device does not send an acknowledgement
	/// within this timeframe, it will be considered invalid.
	sysinterval_t timeout;

	/// @brief The magic string used to validate the EEPROM's contents.
	const char* magicString;

	/// @brief Callback for when the device's data is modified.
	eepromWriteHook_t* writeHook;
} mc24lc32Config_t;

/**
 * @brief Driver for the Microchip 24LC32 I2C EEPROM.
 */
typedef struct
{
	EEPROM_FIELDS;

	/// @brief Cached copy of the EEPROM's contents. Use for read / write operations.
	/// @note This field should always be first in its struct to ensure 4-byte alignment.
	uint8_t cache [MC24LC32_SIZE];

	/// @brief The device's configuration.
	const mc24lc32Config_t* config;

	/// @brief State of the device.
	mc24lc32State_t state;
} mc24lc32_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the device using the specified configuration.
 * @param mc24lc32 The device to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool mc24lc32Init (mc24lc32_t* mc24lc32, const mc24lc32Config_t* config);

/**
 * @brief Writes data through cache to the device.
 * @param mc24lc32 The device to write to.
 * @param address The byte address to write to.
 * @param data The data to write.
 * @param dataCount The number of bytes to write. Note that this cannot cross a page boundary (multiples of 32).
 * @return True if successful, false otherwise.
 */
bool mc24lc32Write (void* mc24lc32, uint16_t address, const void* data, uint16_t dataCount);

/**
 * @brief Reads data from device cache.
 * @param mc24lc32 The device to read from.
 * @param address The byte address to read from.
 * @param data The buffer to write the data into.
 * @param dataCount The number of bytes to read.
 * @return True if successful, false otherwise.
 */
bool mc24lc32Read (void* mc24lc32, uint16_t address, void* data, uint16_t dataCount);

/**
 * @brief Checks whether the cached memory of the device is valid.
 * @param mc24lc32 The device to check.
 * @return True if the cached memory is valid, false otherwise.
 */
bool mc24lc32IsValid (mc24lc32_t* mc24lc32);

/**
 * @brief Validates the cached memory of the device.
 * @param mc24lc32 The device to validate.
 */
bool mc24lc32Validate (mc24lc32_t* mc24lc32);

/**
 * @brief Invalidates the cached memory of the device.
 * @param mc24lc32 The device to invalidate.
 */
bool mc24lc32Invalidate (mc24lc32_t* mc24lc32);

#endif // MC24LC32_H