// Header
#include "am4096.h"

// Constants / Macros ---------------------------------------------------------------------------------------------------------

#define FACTORY_SETTINGS_0_ADDR			4
#define FACTORY_SETTINGS_0_SIZE			4

#define RELATIVE_POS_ADDR				32
#define RELATIVE_POS_GET_SRCH(register)	(((register) >> 15) & 0b1)
#define RELATIVE_POS_GET_RPOS(register)	((register) & 0xFFF)

#define ABSOLUTE_POS_ADDR				33
#define ABSOLUTE_POS_GET_SRCH(register)	(((register) >> 15) & 0b1)
#define ABSOLUTE_POS_GET_RPOS(register)	((register) & 0xFFF)

#define STATUS_ADDR						34
#define STATUS_GET_WEL(register)		(((register) >> 13) & 0b1)
#define STATUS_GET_WEH(register)		(((register) >> 14) & 0b1)

#define TACHO_ADDR						35

#define TESTING_SETTINGS_ADDR			36
#define TESTING_SETTINGS_SIZE			12

#define FACTORY_SETTINGS_1_ADDR			52
#define FACTORY_SETTINGS_1_SIZE			4

// Function Prototypes --------------------------------------------------------------------------------------------------------

static bool pollAck (am4096_t* am4096);

static bool writeRegister (am4096_t* am4096, uint8_t addr, uint16_t data);

static bool readRegister (am4096_t* am4096, uint8_t addr, uint16_t* data);

static bool writeBlock (void* object, uint16_t addr, const void* data, uint16_t dataCount);

static bool readBlock (void* object, uint16_t addr, void* data, uint16_t dataCount);

// Functions ------------------------------------------------------------------------------------------------------------------

bool am4096Init (am4096_t* am4096, const am4096Config_t* config)
{
	am4096->writeHandler	= writeBlock;
	am4096->readHandler		= readBlock;
	am4096->config			= config;

	am4096->state = AM4096_STATE_READY;

	return am4096Sample (am4096);
}

bool am4096Sample (am4096_t* am4096)
{
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	uint16_t relativePos;
	bool result = readRegister (am4096, RELATIVE_POS_ADDR, &relativePos);

	if (result)
	{
		// If successful, update the sensor.
		am4096->sample = RELATIVE_POS_GET_RPOS (relativePos);
		am4096->config->sensor->callback (am4096->config->sensor, am4096->sample, 4095);
	}
	else
	{
		// Otherwise, put the sensor in the fail state.
		am4096->sample = 0;
		am4096->config->sensor->state = ANALOG_SENSOR_FAILED;
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return result;
}

bool pollAck (am4096_t* am4096)
{
	// The AM4096 will not ACK an I2C transaction unless it is available for read/write. This function uses this feature to
	// poll the device periodically until it responds with an ACK.

	// Track the start time so we can time out.
	systime_t timeStart = chVTGetSystemTime ();

	// Dummy data, doesn't actually matter what we send so long as it isn't a full read/write command.
	uint8_t tx = 0x00;
	uint16_t rx;

	// Loop until the device responds or we time out.
	while (chTimeDiffX (timeStart, chVTGetSystemTime ()) < am4096->config->timeout)
		if (i2cMasterTransmitTimeout (am4096->config->i2c, am4096->config->addr, &tx, sizeof (tx),
			(uint8_t*) &rx, sizeof (rx), am4096->config->timeout) == MSG_OK)
			return true;

	// Timeout occurred, enter fail state.
	am4096->state = AM4096_STATE_FAILED;
	return false;
}

bool writeRegister (am4096_t* am4096, uint8_t addr, uint16_t data)
{
	// Writes a word to the device's memory. See AM4096 datasheet, figure 6 for more details.

	// Check the device is ready for data transfer.
	if (!pollAck (am4096))
		return false;

	// Check the address is writable (can't write to factory settings, testing settings, or read-only registers).
	if ((addr >= FACTORY_SETTINGS_0_ADDR && addr < FACTORY_SETTINGS_0_ADDR + FACTORY_SETTINGS_0_SIZE) ||
		(addr >= TESTING_SETTINGS_ADDR && addr < TESTING_SETTINGS_ADDR + TESTING_SETTINGS_SIZE) ||
		(addr > FACTORY_SETTINGS_1_ADDR) || addr == RELATIVE_POS_ADDR || addr == ABSOLUTE_POS_ADDR ||
		addr == STATUS_ADDR || addr == TACHO_ADDR)
		return false;

	// Write the address and data.
	uint8_t tx [sizeof (addr) + sizeof (data)] = { addr, data >> 8, data };
	if (i2cMasterTransmitTimeout (am4096->config->i2c, am4096->config->addr, tx, sizeof (tx),
		NULL, 0, am4096->config->timeout) != MSG_OK)
	{
		am4096->state = AM4096_STATE_FAILED;
		return false;
	}
	am4096->state = AM4096_STATE_READY;

	return true;
}

bool readRegister (am4096_t* am4096, uint8_t addr, uint16_t* data)
{
	// Reads a word from the device's memory. See AM4096 datasheet, figure 7 for more details.

	// TODO(Barach): Could lead with the transaction then poll on failure.
	// Check the device is ready for data transfer.
	if (!pollAck (am4096))
		return false;

	// Write the address to read from, then read the data.
	uint16_t rx;
	if (i2cMasterTransmitTimeout (am4096->config->i2c, am4096->config->addr, &addr, sizeof (addr),
		(uint8_t*) &rx, sizeof (rx), am4096->config->timeout) != MSG_OK)
	{
		am4096->state = AM4096_STATE_FAILED;
		return false;
	}
	am4096->state = AM4096_STATE_READY;

	// Convert the data from big-endian to little-endian.
	*data = __REV16 (rx);
	return true;
}

bool writeBlock (void* object, uint16_t addr, const void* data, uint16_t dataCount)
{
	am4096_t* am4096 = (am4096_t*) object;

	// Force operations to use 16-bit alignment and data size.
	if (addr % sizeof (uint16_t) != 0 || dataCount % sizeof (uint16_t) != 0)
		return false;

	// Convert the address, data, and count into register values.
	uint8_t registerAddr = addr / sizeof (uint16_t);
	const uint16_t* registerData = data;
	uint16_t registerCount = dataCount / sizeof (uint16_t);

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// Write each register, one by one.
	bool result = true;
	for (uint16_t registerOffset = 0; registerOffset < registerCount; ++registerOffset)
	{
		if (!writeRegister (am4096, registerAddr + registerOffset, registerData [registerOffset]))
		{
			result = false;
			break;
		}
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return result;
}

bool readBlock (void* object, uint16_t addr, void* data, uint16_t dataCount)
{
	am4096_t* am4096 = (am4096_t*) object;

	// Force operations to use 16-bit alignment and data size.
	if (addr % sizeof (uint16_t) != 0 || dataCount % sizeof (uint16_t) != 0)
		return false;

	// Convert the address, data, and count into register values.
	uint8_t registerAddr = addr / sizeof (uint16_t);
	uint16_t* registerData = data;
	uint16_t registerCount = dataCount / sizeof (uint16_t);

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// Read each register, one by one.
	bool result = true;
	for (uint16_t registerOffset = 0; registerOffset < registerCount; ++registerOffset)
	{
		if (!readRegister (object, registerAddr + registerOffset, registerData + registerOffset))
		{
			result = false;
			break;
		}
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus (am4096->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return result;
}