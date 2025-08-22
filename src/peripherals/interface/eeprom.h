#ifndef EEPROM_H
#define EEPROM_H

// EEPROM ---------------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.02.25
//
// Description: Standard interface for EEPROM devices. This module defines a polymorphic interface for EEPROM-implementing
//   objects to inherit. This also defines a 'virtual' EEPROM that allows multiple EEPROM objects to be aggregated into a
//   single memory map.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Writes a block of memory to an EEPROM.
 * @param eeprom The EEPROM to write to.
 * @param addr The address to write to.
 * @param data The data to be written.
 * @param dataCount The amount of data to write.
 * @return True if successful, false otherwise.
 */
typedef bool (eepromWrite_t) (void* eeprom, uint16_t addr, const void* data, uint16_t dataCount);

/**
 * @brief Reads a block of memory from an EEPROM.
 * @param eeprom The EEPROM to read from.
 * @param addr The address to read from.
 * @param data Buffer to read the data into.
 * @param dataCount The amount of data to read.
 * @return True if successful, false otherwise.
 */
typedef bool (eepromRead_t) (void* eeprom, uint16_t addr, void* data, uint16_t dataCount);

#define EEPROM_FIELDS						\
	eepromWrite_t*		writeHandler;		\
	eepromRead_t*		readHandler

/**
 * @brief Polymorphic base object representing an EEPROM.
 * @note When deriving this struct, use the @c EEPROM_FIELDS macro to define the first set of fields.
 */
typedef struct
{
	EEPROM_FIELDS;
} eeprom_t;

/**
 * @brief Entry in a @c virtualEeprom_t device.
 */
typedef struct
{
	eeprom_t* eeprom;
	uint16_t addr;
	uint16_t size;
} virtualEepromEntry_t;

/**
 * @brief Configuration for the @c virtualEeprom_t object.
 */
typedef struct
{
	/// @brief The number of EEPROMs in the @c entries array.
	uint16_t count;

	/// @brief The array of EEPROMs to map to this device's memory.
	virtualEepromEntry_t entries [];
} virtualEepromConfig_t;

/**
 * @brief Object mapping multiple EEPROM's memory to a single object. See the @c eeprom_can module for a case where this would
 * be useful.
 */
typedef struct
{
	EEPROM_FIELDS;
	const virtualEepromConfig_t* config;
} virtualEeprom_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes a base EEPROM object using the specified handlers.
 * @note This is only to be used on objects that are direct instances of @c eeprom_t . Derived objects will provide their own
 * initializers that must be used instead.
 * @param eeprom The base EEPROM to initialize.
 * @param writeHandler The handler to use for write operations.
 * @param readHandler The handler to use for read operations.
 */
void eepromInit (eeprom_t* eeprom, eepromWrite_t* writeHandler, eepromRead_t* readHandler);

/**
 * @brief Initializes a virtual EEPROM using the specified configuration.
 * @param eeprom The virtual EEPROM to initialize.
 * @param config The configuration to use.
 */
void virtualEepromInit (virtualEeprom_t* eeprom, const virtualEepromConfig_t* config);

#endif // EEPROM_H