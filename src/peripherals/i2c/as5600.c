// Header
#include "as5600.h"

// Functions ------------------------------------------------------------------------------------------------------------------
msg_t write8bit(as5600_t* as5600, uint8_t reg, uint8_t value){
	msg_t status;
	uint8_t txbuf[2] = {reg, value};

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif

status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, txbuf, sizeof txbuf, NULL, 0, as5600->config->timeout);

#if I2C_USE_MUTUAL_EXCLUSION
i2cReleaseBus(as5600->config->i2c);
#endif

return status;
}

msg_t read8bit(as5600_t* as5600, uint8_t reg, uint8_t* out) {
	msg_t status;
	uint8_t txbuf = reg;
	uint8_t rxbuf;

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, &txbuf, sizeof txbuf, &rxbuf, sizeof rxbuf, as5600->config->timeout);

	if(status == MSG_OK) {
		*out = rxbuf;
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif

	return status;
}

msg_t write16bit(as5600_t* as5600, uint8_t reg, uint16_t value) {
	msg_t status;
	uint8_t txbuf[3];

	txbuf[0] = reg;
	txbuf[1] = (value >> 8) & 0xFF;
	txbuf[2] = value & 0xFF;
	
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, txbuf, sizeof txbuf, NULL, 0, as5600->config->timeout);

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif

	return status;
}

msg_t read16bit(as5600_t* as5600, uint8_t reg, uint16_t* out) {
	msg_t status;
	uint8_t txbuf = reg;
	uint8_t rxbuf[2];

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, &txbuf, sizeof txbuf, rxbuf, sizeof rxbuf, as5600->config->timeout);

	if (status == MSG_OK) {
		*out = ((uint16_t)rxbuf[0] << 8) | rxbuf[1];
		*out &= 0x0FFF;
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif

	return status;
}

// 12 bit representation of Angle, affected by ZPOS and MPOS
msg_t getBinAngle(as5600_t* as5600, uint8_t reg, uint16_t* binAngle) {
	return read16bit(as5600, reg, binAngle);
}

void setAngleOffset(as5600_t* as5600) {
	msg_t status;
	uint16_t binAngle;
	uint16_t minAngle;
	uint16_t maxAngle;

	// Read current ZPOS and MPOS register values
	status = read16bit(as5600, as5600->config->ZPOS_REG, &minAngle);
	if (status != MSG_OK) return;

	status = read16bit(as5600, as5600->config->MPOS_REG, &maxAngle);
	if (status != MSG_OK) return;

	// Read current angle
	status = getBinAngle(as5600, as5600->config->ANGLE_REG, &binAngle);
	if (status != MSG_OK) return;

	// Apply offset
	minAngle = (minAngle + binAngle) & 0x0FFF;
	maxAngle = (maxAngle + binAngle) & 0x0FFF;

	setMinAngle(as5600, as5600->config->ZPOS_REG, minAngle);
	setMaxAngle(as5600, as5600->config->MPOS_REG, maxAngle);
}

// Take binAngle, apply scaling to convert to float
float convertAngle( uint16_t binAngle) {
return ((float)(binAngle - 2048)) * (360.0f / 4096.0f);
}

msg_t setMinAngle(as5600_t* as5600, uint8_t ZPOS_REG, uint16_t minAngle) {
	return write16bit(as5600, ZPOS_REG, minAngle);
}

msg_t setMaxAngle(as5600_t* as5600, uint8_t MPOS_REG, uint16_t maxAngle) {
	return write16bit(as5600, MPOS_REG, maxAngle);

}

bool as5600Init (as5600_t* as5600, const as5600Config_t* config)
{
	// Store the configuration.
	as5600->config = config;

	// TODO: More stuff here later...
	return true;
}
