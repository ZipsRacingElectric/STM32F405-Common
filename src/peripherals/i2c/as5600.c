// Header
#include "as5600.h"

// Functions ------------------------------------------------------------------------------------------------------------------

bool as5600Init (as5600_t* as5600, const as5600Config_t* config)
{
	// Store the configuration.
	as5600->config = config;

	// TODO: More stuff here later...
	return true;
}

bool as5600ReadRegister (as5600_t* as5600, uint8_t addr, uint8_t* data)
{
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus (as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// TODO: Perform a read operation.
	// Should use: i2cMasterTransmit (i2cp, addr, txbuf, txbytes, rxbuf, rxbytes);
	// See common/doc/datasheets/AS5600 Datasheet.pdf for how this should be done (pg.10 to pg.17).

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus (as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// TODO: Return false if it fails
	return true;
}

bool as5600WriteRegister (as5600_t* as5600, uint8_t addr, uint8_t data)
{
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus (as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// TODO: Perform a write operation.
	// Should use: i2cMasterTransmit (i2cp, addr, txbuf, txbytes, rxbuf, rxbytes);
	// See common/doc/datasheets/AS5600 Datasheet.pdf for how this should be done (pg.10 to pg.17).

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus (as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	// TODO: Return false if it fails
	return false;
}