#ifndef EEPROM_H
#define EEPROM_H

// EEPROM ---------------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.02.25
//
// Description: Standard interface for EEPROM devices.

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

/**
 * @brief Hook to be called after an EEPROM's memory has been written to.
 * @param eeprom The EEPROM that was modified.
 */
typedef void (eepromWriteHook_t) (void* eeprom);

#define EEPROM_FIELDS						\
	eepromWrite_t*		writeHandler;		\
	eepromRead_t*		readHandler;		\
	eepromWriteHook_t*	writeHook

/**
 * @brief Polymorphic base object representing an EEPROM.
 * @note When deriving this struct, use the @c EEPROM_FIELDS macro to define the first set of fields.
 */
typedef struct
{
	EEPROM_FIELDS;
} eeprom_t;

typedef struct
{
	/// @brief The array of EEPROM's to aggregate into a single memory map.
	eeprom_t** eeproms;

	/// @brief The base address to map each EEPROM's memory to.
	uint16_t* addrs;

	/// @brief The size of each EEPROM's map.
	uint16_t* sizes;

	/// @brief The number of EEPROMs in the @c eeproms array.
	uint16_t count;
} virtualEepromConfig_t;

typedef struct
{
	EEPROM_FIELDS;
	const virtualEepromConfig_t* config;
} virtualEeprom_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void virtualEepromInit (virtualEeprom_t* eeprom, const virtualEepromConfig_t* config);

#endif // EEPROM_H