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

bool mc24lc32AcknowledgePoll (mc24lc32_t* mc24lc32);

// Function Definitions -------------------------------------------------------------------------------------------------------

/// @brief Read a sequential section of memory (see datasheet Section 8.3).
bool mc24lc32SequentialRead (mc24lc32_t* mc24lc32, uint16_t address, uint16_t count)
{
	// Check the device is available for transfer
	if (!mc24lc32AcknowledgePoll (mc24lc32))
		return false;

	// Transactions starts with address (big-endian)
	uint8_t tx [2] = { (uint8_t) (address) >> 8, (uint8_t) (address) };

	// Receive into cache
	uint8_t* rx = mc24lc32->cache + address;

	msg_t result =  i2cMasterTransmit (mc24lc32->i2c, mc24lc32->addr, tx, 2, rx, count);

	if (result != MSG_OK)
	{
		mc24lc32->state = MC24LC32_STATE_FAILED;
		return false;
	}

	return true;
}

/// @brief Write into a page of memory (see datasheet Section 6.2).
bool mc24lc32PageWrite (mc24lc32_t* mc24lc32, uint16_t address, uint8_t count)
{
	// Check the device is available for transfer
	if (!mc24lc32AcknowledgePoll (mc24lc32))
		return false;

	// Transactions starts with address (big-endian)
	uint8_t tx [PAGE_SIZE + 2] = { (uint8_t) ((address) >> 8), (uint8_t) (address) };

	// Max of 32 bytes of data follow
	memcpy (tx + 2, mc24lc32->cache + address, count);

	msg_t result = i2cMasterTransmit (mc24lc32->i2c, mc24lc32->addr, tx, count + 2, NULL, 0);

	if (result != MSG_OK)
	{
		mc24lc32->state = MC24LC32_STATE_FAILED;
		return false;
	}

	return true;
}

bool mc24lc32AcknowledgePoll (mc24lc32_t* mc24lc32)
{
	systime_t timeStart = chVTGetSystemTime ();

	uint8_t tx [2] = { 0x00, 0x00 };

	while (chTimeDiffX (timeStart, chVTGetSystemTime ()) < mc24lc32->timeoutPeriod)
	{
		msg_t result = i2cMasterTransmit (mc24lc32->i2c, mc24lc32->addr, tx, 2, NULL, 0);
		if (result == MSG_OK)
			return true;
	}

	mc24lc32->state = MC24LC32_STATE_FAILED;
	return false;
}

bool mc24lc32Init (mc24lc32_t* mc24lc32, mc24lc32Config_t *config)
{
	// Store the driver configuration
	mc24lc32->addr			= config->addr;
	mc24lc32->i2c			= config->i2c;
	mc24lc32->magicString	= config->magicString;
	mc24lc32->timeoutPeriod	= config->timeoutPeriod;

	// Start the device in the ready state
	mc24lc32->state = MC24LC32_STATE_READY;

	// Read the EEPROM contents into memory
	return mc24lc32Read (mc24lc32);
}

bool mc24lc32Read (mc24lc32_t* mc24lc32)
{
	// Acquire the bus
	i2cAcquireBus (mc24lc32->i2c);

	// Perform a sequential read starting at address 0
	bool result = mc24lc32SequentialRead (mc24lc32, 0x00, MC24LC32_SIZE);

	// Release the bus
	i2cReleaseBus (mc24lc32->i2c);

	// If the transaction failed, exit early
	if (!result)
		return false;

	// Check the validity of the memory
	return mc24lc32IsValid (mc24lc32);
}

bool mc24lc32Write (mc24lc32_t* mc24lc32)
{
	// Acquire the bus
	i2cAcquireBus (mc24lc32->i2c);

	bool result = true;
	for (uint16_t address = 0; address < MC24LC32_SIZE; address += PAGE_SIZE)
	{
		// If the transaction failed, exit early
		result = mc24lc32PageWrite (mc24lc32, address, PAGE_SIZE);
		if (!result)
			break;
	}

	// Release the bus
	i2cReleaseBus (mc24lc32->i2c);
	return result;
}

bool mc24lc32WriteThrough (mc24lc32_t* mc24lc32, uint16_t address, uint8_t* data, uint8_t dataCount)
{
	// Copy the data into cache
	memcpy (mc24lc32->cache + address, data, dataCount);

	// Acquire the bus
	i2cAcquireBus (mc24lc32->i2c);

	// Write the cached data to the device.
	bool result = mc24lc32PageWrite (mc24lc32, address, dataCount);

	// Release the bus
	i2cReleaseBus (mc24lc32->i2c);
	return result;
}

bool mc24lc32IsValid (mc24lc32_t* mc24lc32)
{
	// Check the magic string is correct (including terminator)
	uint8_t stringSize = strlen (mc24lc32->magicString) + 1;
	int result = strncmp ((const char*) mc24lc32->cache, mc24lc32->magicString, stringSize);

	if (result != 0)
	{
		mc24lc32->state = MC24LC32_STATE_INVALID;
		return false;
	}

	return true;
}

void mc24lc32Validate (mc24lc32_t* mc24lc32)
{
	// Copy the magic string into the beginning of memory (including terminator)
	uint8_t stringSize = strlen (mc24lc32->magicString) + 1;
	memcpy (mc24lc32->cache, mc24lc32->magicString, stringSize);
}

void mc34lc32Invalidate (mc24lc32_t* mc24lc32)
{
	// Write all 1's over the magic string
	uint8_t stringSize = strlen (mc24lc32->magicString) + 1;
	for (uint8_t index = 0; index < stringSize; ++index)
		mc24lc32->cache [index] = 0xFF;
}