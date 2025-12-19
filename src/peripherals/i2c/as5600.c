// Includes
#include "as5600.h"

#include "debug.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define ZPOS_ADDR	0x01
#define MPOS_ADDR	0x03
#define ANGLE_ADDR	0x0E

// Functions Prototypes -------------------------------------------------------------------------------------------------------

msg_t write8bit(as5600_t* as5600, uint8_t reg, uint8_t value);
msg_t read8bit(as5600_t* as5600, uint8_t reg, uint8_t* out);
msg_t write16bit(as5600_t* as5600, uint8_t reg, uint16_t value);
msg_t read16bit(as5600_t* as5600, uint8_t reg, uint16_t* out);
msg_t getBinAngle(as5600_t* as5600, uint8_t reg, uint16_t* binAngle);
msg_t setMinAngle(as5600_t* as5600, uint8_t reg, uint16_t minAngle);
msg_t setMaxAngle(as5600_t* as5600, uint8_t reg, uint16_t maxAngle);
void setAngleOffset(as5600_t* as5600);

float convertAngle(uint16_t binAngle);

// Functions -------------------------------------------------------------------------------------------------------------------

msg_t write8bit(as5600_t* as5600, uint8_t reg, uint8_t value)
{
	msg_t status;
	uint8_t txbuf[2] = {reg, value};

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, txbuf, sizeof txbuf, NULL, 0, as5600->config->timeout);
	
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION
	
	return status;
}

msg_t read8bit(as5600_t* as5600, uint8_t reg, uint8_t* out) 
{
	msg_t status;
	uint8_t txbuf = reg;
	uint8_t rxbuf;

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, &txbuf, sizeof txbuf, &rxbuf, sizeof rxbuf, as5600->config->timeout);

	if(status == MSG_OK) {
		*out = rxbuf;
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return status;
}

msg_t write16bit(as5600_t* as5600, uint8_t reg, uint16_t value) 
{
	// AS5600 uses 12 bit registers, see AS5600 datasheet page 18.
	msg_t status;
	uint8_t txbuf[3];

	txbuf[0] = reg;
	txbuf[1] = (value >> 8) & 0xFF;
	txbuf[2] = value & 0xFF;
	
	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, txbuf, sizeof txbuf, NULL, 0, as5600->config->timeout);

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return status;
}

msg_t read16bit(as5600_t* as5600, uint8_t reg, uint16_t* out) 
{
	// AS5600 uses 12 bit registers, see AS5600 datasheet page 18.

	msg_t status;
	uint8_t txbuf = reg;
	uint8_t rxbuf[2];

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cAcquireBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	status = i2cMasterTransmitTimeout(as5600->config->i2c, as5600->config->addr, &txbuf, sizeof txbuf, rxbuf, sizeof rxbuf, as5600->config->timeout);

	if (status == MSG_OK) {
		*out = ((uint16_t)rxbuf[0] << 8) | rxbuf[1];
		*out &= 0x0FFF;
	}

	#if I2C_USE_MUTUAL_EXCLUSION
	i2cReleaseBus(as5600->config->i2c);
	#endif // I2C_USE_MUTUAL_EXCLUSION

	return status;
}

// 12 bit representation of Angle, affected by ZPOS and MPOS
msg_t getBinAngle(as5600_t* as5600, uint8_t reg, uint16_t* binAngle) 
{
	return read16bit(as5600, reg, binAngle);
}

void setAngleOffset(as5600_t* as5600) 
{
	msg_t status;
	uint16_t binAngle;
	uint16_t minAngle;
	uint16_t maxAngle;

	// Read current ZPOS and MPOS register values
	status = read16bit(as5600, ZPOS_ADDR, &minAngle);
	if (status != MSG_OK) return;

	status = read16bit(as5600, MPOS_ADDR, &maxAngle);
	if (status != MSG_OK) return;

	// Read current angle
	status = getBinAngle(as5600, ANGLE_ADDR, &binAngle);
	if (status != MSG_OK) return;

	// Apply offset
	minAngle = (minAngle + binAngle) & 0x0FFF;
	maxAngle = (maxAngle + binAngle) & 0x0FFF;

	setMinAngle(as5600, ZPOS_ADDR, minAngle);
	setMaxAngle(as5600, MPOS_ADDR, maxAngle);
}

// Take binAngle, apply scaling to convert to float
float convertAngle( uint16_t binAngle) {
return ((float)(binAngle - 2048)) * (360.0f / 4096.0f);
}

msg_t setMinAngle(as5600_t* as5600, uint8_t ZPOS_REG, uint16_t minAngle) 
{
	return write16bit(as5600, ZPOS_REG, minAngle);
}

msg_t setMaxAngle(as5600_t* as5600, uint8_t MPOS_REG, uint16_t maxAngle) 
{
	return write16bit(as5600, MPOS_REG, maxAngle);
}

bool as5600Init (as5600_t* as5600, const as5600Config_t* config)
{
	// Store the configuration.
	as5600->config = config;

	// TODO: More stuff here later...
	return true;
}

msg_t as5600Sample(as5600_t* as5600) 
{
	uint16_t ADC_SAMPLE;
	msg_t status = getBinAngle(as5600, ANGLE_ADDR, &ADC_SAMPLE);
	if (status == MSG_OK) 
	{
		// If succeed, update the sensor
		analogSensorUpdate(as5600->config->sensor, ADC_SAMPLE, 4095);
	}
	else 
	{
		// If fail, put sensor in fail state
		analogSensorFail(as5600->config->sensor);
	}

	return status;
}

