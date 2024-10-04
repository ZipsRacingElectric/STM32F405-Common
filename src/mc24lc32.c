// Header
#include "mc24lc32.h"

// ChibiOS
#include "hal_i2c.h"

// C Standard Library
#include <string.h>

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Maximum number of bytes to be written in a single operation.
#define PAGE_SIZE 32

// Function Prototypes --------------------------------------------------------------------------------------------------------

bool mc24lc32SequentialRead (mc24lc32_t* mc24lc32, uint16_t address, uint16_t count);

bool mc24lc32PageWrite (mc24lc32_t* mc24lc32, uint16_t address, uint8_t count);

// Function Definitions -------------------------------------------------------------------------------------------------------

/// @brief Read a sequential section of memory (see datasheet Section 8.3).
bool mc24lc32SequentialRead (mc24lc32_t* mc24lc32, uint16_t address, uint16_t count)
{
	// Transactions starts with address (big-endian)
	uint8_t tx [2] = { (uint8_t) (address) >> 8, (uint8_t) (address) };

	// Receive into cache
	uint8_t* rx = mc24lc32->cache + address;
	
	msg_t result =  i2cMasterTransmit (mc24lc32->config->i2c, mc24lc32->config->addr, tx, 2, rx, count);
	return result == MSG_OK;
}

/// @brief Write into a page of memory (see datasheet Section 6.2).
bool mc24lc32PageWrite (mc24lc32_t* mc24lc32, uint16_t address, uint8_t count)
{
	// Transactions starts with address (big-endian)
	uint8_t tx [PAGE_SIZE + 2] = { (uint8_t) (address) >> 8, (uint8_t) (address) };

	// Max of 32 bytes of data follow
	memcpy (tx + 2, mc24lc32->cache + address, count);
	
	msg_t result = i2cMasterTransmit (mc24lc32->config->i2c, mc24lc32->config->addr, tx, count + 2, NULL, 0);
	return result == MSG_OK;
}

bool mc24lc32Init(mc24lc32_t* mc24lc32, mc24lc32Config_t *config)
{
	// Store the driver configuration
	mc24lc32->config = config;

	// Read the EEPROM contents into memory
	return mc24lc32Read (mc24lc32);
}

bool mc24lc32Read (mc24lc32_t* mc24lc32)
{
	// Acquire the bus
	i2cAcquireBus(mc24lc32->config->i2c);

	// Perform a sequential read starting at address 0
	bool result = mc24lc32SequentialRead(mc24lc32, 0x00, MC24LC32_SIZE);
	
	// Release the bus
	i2cReleaseBus (mc24lc32->config->i2c);
	
	// If the transaction failed, exit early
	if (!result)
		return false;

	// Check the validity of the memory
	return mc24lc32IsValid (mc24lc32);
}

bool mc24lc32Write (mc24lc32_t* mc24lc32)
{
	// Acquire the bus
	i2cAcquireBus (mc24lc32->config->i2c);

	bool result = true;
	for (uint16_t address = 0; address < MC24LC32_SIZE; address += PAGE_SIZE)
	{
		// If the transaction failed, exit early
		result = mc24lc32PageWrite (mc24lc32, address, PAGE_SIZE);
		if (!result)
			break;
	}

	// Release the bus
	i2cReleaseBus (mc24lc32->config->i2c);
	return result;
}

bool mc24lc32IsValid (mc24lc32_t* mc24lc32)
{
	// Check the magic string is correct (including terminator)
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	return strncmp ((const char*) mc24lc32->cache, mc24lc32->config->magicString, stringSize) == 0;
}

void mc24lc32Validate (mc24lc32_t* mc24lc32)
{
	// Copy the magic string into the beginning of memory (including terminator)
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	memcpy (mc24lc32->cache, mc24lc32->config->magicString, stringSize);
}