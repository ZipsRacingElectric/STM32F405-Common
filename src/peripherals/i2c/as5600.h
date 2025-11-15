#ifndef AS5600_H
#define AS5600_H

// AS5600 Device Driver -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach, ...
// Date Created: 2025.08.22
//
// Description: I2C driver class for the AS5600 angular encoder.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	i2caddr_t addr;
	I2CDriver* i2c;
	sysinterval_t timeout;

	const uint8_t ZPOS_REG;
	const uint8_t MPOS_REG;
	const uint8_t ANGLE_REG;

	uint16_t minAngle;
	uint16_t maxAngle;
	uint16_t binAngle;
} as5600Config_t;

typedef struct
{
	const as5600Config_t* config;
} as5600_t;

// AS5600 Member Functions
msg_t write8bit(as5600_t* as5600, uint8_t reg, uint8_t value);
msg_t read8bit(as5600_t* as5600, uint8_t reg, uint8_t* out);
msg_t write16bit(as5600_t* as5600, uint8_t reg, uint16_t value);
msg_t read16bit(as5600_t* as5600, uint8_t reg, uint16_t* out);
msg_t getBinAngle(as5600_t* as5600, uint8_t reg, uint16_t* binAngle);
msg_t setMinAngle(as5600_t* as5600, uint8_t reg, uint16_t minAngle);
msg_t setMaxAngle(as5600_t* as5600, uint8_t reg, uint16_t maxAngle);
void setAngleOffset(as5600_t* as5600);

float convertAngle(uint16_t binAngle);

bool as5600Init (as5600_t* as5600, const as5600Config_t* config);

#endif // AS5600_H