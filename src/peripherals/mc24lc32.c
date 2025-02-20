// Header
#include "mc24lc32.h"

// ChibiOS
#include "hal_i2c.h"

// C Standard Library
#include <string.h>

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum number of bytes to be written in a single operation.
#define PAGE_SIZE 32

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Polls the device to determine whether or not it is available for read/write (see 23LC32 datasheet, section 7.0). If
 * the device is unavailable, this function will block until it becomes available or the operation times out.
 * @param mc24lc32 The device to poll.
 * @return True if the device is available, false if a timeout or other failure occured.
 */
bool mc24lc32AcknowledgePoll (mc24lc32_t* mc24lc32);

/**
 * @brief Read a sequential section of memory (see 24LC32 datasheet, section 8.3).
 * @note There is no upper limit on this transaction size, so long as no errors occur.
 * @param mc24lc32 The device to read from.
 * @param address The byte address to begin the read at.
 * @param count The number of bytes to read.
 * @return True if successful, false otherwise.
 */
bool mc24lc32SequentialRead (mc24lc32_t* mc24lc32, uint16_t address, uint16_t count);

/**
 * @brief Write into a page of memory (see 24LC32 datasheet, Section 6.2).
 * @note This transaction cannot cross a page boundary (see @c PAGE_SIZE ), otherwise the beginning of the addressed page will be
 * overwritten.
 * @param mc24lc32 The device to write to.
 * @param address The byte address to begin the write at.
 * @param count The number of bytes to read, cannot exceed @c PAGE_SIZE .
 * @return True if successful, false otherwise.
 */
bool mc24lc32PageWrite (mc24lc32_t* mc24lc32, uint16_t address, uint8_t count);

// Functions ------------------------------------------------------------------------------------------------------------------

bool mc24lc32AcknowledgePoll (mc24lc32_t* mc24lc32)
{
	// The 24LC32 will not ACK an I2C transaction unless it is available for read/write. This function uses this feature to
	// poll the device periodically until it responds with an ACK.

	// Track the start time so we can time out.
	systime_t timeStart = chVTGetSystemTime ();

	// Dummy data, doesn't actually matter what we send so long as it isn't a full read/write command.
	uint8_t tx [2] = { 0x00, 0x00 };

	// Loop until the device responds or we time out.
	while (chTimeDiffX (timeStart, chVTGetSystemTime ()) < mc24lc32->config->timeoutPeriod)
		if (i2cMasterTransmit (mc24lc32->config->i2c, mc24lc32->config->addr, tx, 2, NULL, 0) == MSG_OK)
			return true;

	// Timeout occurred, enter fail state.
	mc24lc32->state = MC24LC32_STATE_FAILED;
	return false;
}

bool mc24lc32SequentialRead (mc24lc32_t* mc24lc32, uint16_t address, uint16_t count)
{
	// Check the device is available for transfer
	if (!mc24lc32AcknowledgePoll (mc24lc32))
		return false;

	// Transactions starts with address (big-endian)
	uint8_t tx [2] = { (uint8_t) (address) >> 8, (uint8_t) (address) };

	// Receive into cache
	uint8_t* rx = mc24lc32->cache + address;

	if (i2cMasterTransmit (mc24lc32->config->i2c, mc24lc32->config->addr, tx, 2, rx, count) != MSG_OK)
	{
		mc24lc32->state = MC24LC32_STATE_FAILED;
		return false;
	}

	return true;
}

bool mc24lc32PageWrite (mc24lc32_t* mc24lc32, uint16_t address, uint8_t count)
{
	// Check the device is available for transfer
	if (!mc24lc32AcknowledgePoll (mc24lc32))
		return false;

	// Transactions starts with address (big-endian)
	uint8_t tx [PAGE_SIZE + 2] = { (uint8_t) ((address) >> 8), (uint8_t) (address) };

	// Max of 32 bytes of data follow
	memcpy (tx + 2, mc24lc32->cache + address, count);

	if (i2cMasterTransmit (mc24lc32->config->i2c, mc24lc32->config->addr, tx, count + 2, NULL, 0) != MSG_OK)
	{
		mc24lc32->state = MC24LC32_STATE_FAILED;
		return false;
	}

	return true;
}

bool mc24lc32Init (mc24lc32_t* mc24lc32, const mc24lc32Config_t *config)
{
	// Store the driver configuration
	mc24lc32->config = config;

	// Start the device in the ready state
	mc24lc32->state = MC24LC32_STATE_READY;

	// Read the EEPROM contents into memory
	return mc24lc32Read (mc24lc32);
}

bool mc24lc32Read (mc24lc32_t* mc24lc32)
{
	// Acquire the bus
	i2cAcquireBus (mc24lc32->config->i2c);

	// Perform a sequential read starting at address 0
	bool result = mc24lc32SequentialRead (mc24lc32, 0x00, MC24LC32_SIZE);

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

bool mc24lc32WriteThrough (mc24lc32_t* mc24lc32, uint16_t address, uint8_t* data, uint8_t dataCount)
{
	// Copy the data into cache
	memcpy (mc24lc32->cache + address, data, dataCount);

	// Acquire the bus
	i2cAcquireBus (mc24lc32->config->i2c);

	// Write the cached data to the device.
	bool result = mc24lc32PageWrite (mc24lc32, address, dataCount);

	// Release the bus
	i2cReleaseBus (mc24lc32->config->i2c);
	return result;
}

bool mc24lc32IsValid (mc24lc32_t* mc24lc32)
{
	// Check the magic string is correct (including terminator). Use strncmp as the cache is not guaranteed to have a
	// terminator.
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	if (strncmp ((const char*) mc24lc32->cache, mc24lc32->config->magicString, stringSize) != 0)
	{
		mc24lc32->state = MC24LC32_STATE_INVALID;
		return false;
	}

	return true;
}

void mc24lc32Validate (mc24lc32_t* mc24lc32)
{
	// Copy the magic string into the beginning of memory (including terminator)
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	memcpy (mc24lc32->cache, mc24lc32->config->magicString, stringSize);
}

void mc34lc32Invalidate (mc24lc32_t* mc24lc32)
{
	// Write all 1's over the magic string
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	for (uint8_t index = 0; index < stringSize; ++index)
		mc24lc32->cache [index] = 0xFF;
}