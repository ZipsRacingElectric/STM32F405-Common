// Header
#include "ltc681x_internal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief Lookup table for calculating a frame's PEC. (See @c calculatePec )
static const uint16_t PEC_LUT [] =
{
	0x0000, 0xC599, 0xCEAB, 0x0B32, 0xD8CF, 0x1D56, 0x1664, 0xD3FD,
	0xF407, 0x319E, 0x3AAC, 0xFF35, 0x2CC8, 0xE951, 0xE263, 0x27FA,
	0xAD97, 0x680E, 0x633C, 0xA6A5, 0x7558, 0xB0C1, 0xBBF3, 0x7E6A,
	0x5990, 0x9C09, 0x973B, 0x52A2, 0x815F, 0x44C6, 0x4FF4, 0x8A6D,
	0x5B2E, 0x9EB7, 0x9585, 0x501C, 0x83E1, 0x4678, 0x4D4A, 0x88D3,
	0xAF29, 0x6AB0, 0x6182, 0xA41B, 0x77E6, 0xB27F, 0xB94D, 0x7CD4,
	0xF6B9, 0x3320, 0x3812, 0xFD8B, 0x2E76, 0xEBEF, 0xE0DD, 0x2544,
	0x02BE, 0xC727, 0xCC15, 0x098C, 0xDA71, 0x1FE8, 0x14DA, 0xD143,
	0xF3C5, 0x365C, 0x3D6E, 0xF8F7, 0x2B0A, 0xEE93, 0xE5A1, 0x2038,
	0x07C2, 0xC25B, 0xC969, 0x0CF0, 0xDF0D, 0x1A94, 0x11A6, 0xD43F,
	0x5E52, 0x9BCB, 0x90F9, 0x5560, 0x869D, 0x4304, 0x4836, 0x8DAF,
	0xAA55, 0x6FCC, 0x64FE, 0xA167, 0x729A, 0xB703, 0xBC31, 0x79A8,
	0xA8EB, 0x6D72, 0x6640, 0xA3D9, 0x7024, 0xB5BD, 0xBE8F, 0x7B16,
	0x5CEC, 0x9975, 0x9247, 0x57DE, 0x8423, 0x41BA, 0x4A88, 0x8F11,
	0x057C, 0xC0E5, 0xCBD7, 0x0E4E, 0xDDB3, 0x182A, 0x1318, 0xD681,
	0xF17B, 0x34E2, 0x3FD0, 0xFA49, 0x29B4, 0xEC2D, 0xE71F, 0x2286,
	0xA213, 0x678A, 0x6CB8, 0xA921, 0x7ADC, 0xBF45, 0xB477, 0x71EE,
	0x5614, 0x938D, 0x98BF, 0x5D26, 0x8EDB, 0x4B42, 0x4070, 0x85E9,
	0x0F84, 0xCA1D, 0xC12F, 0x04B6, 0xD74B, 0x12D2, 0x19E0, 0xDC79,
	0xFB83, 0x3E1A, 0x3528, 0xF0B1, 0x234C, 0xE6D5, 0xEDE7, 0x287E,
	0xF93D, 0x3CA4, 0x3796, 0xF20F, 0x21F2, 0xE46B, 0xEF59, 0x2AC0,
	0x0D3A, 0xC8A3, 0xC391, 0x0608, 0xD5F5, 0x106C, 0x1B5E, 0xDEC7,
	0x54AA, 0x9133, 0x9A01, 0x5F98, 0x8C65, 0x49FC, 0x42CE, 0x8757,
	0xA0AD, 0x6534, 0x6E06, 0xAB9F, 0x7862, 0xBDFB, 0xB6C9, 0x7350,
	0x51D6, 0x944F, 0x9F7D, 0x5AE4, 0x8919, 0x4C80, 0x47B2, 0x822B,
	0xA5D1, 0x6048, 0x6B7A, 0xAEE3, 0x7D1E, 0xB887, 0xB3B5, 0x762C,
	0xFC41, 0x39D8, 0x32EA, 0xF773, 0x248E, 0xE117, 0xEA25, 0x2FBC,
	0x0846, 0xCDDF, 0xC6ED, 0x0374, 0xD089, 0x1510, 0x1E22, 0xDBBB,
	0x0AF8, 0xCF61, 0xC453, 0x01CA, 0xD237, 0x17AE, 0x1C9C, 0xD905,
	0xFEFF, 0x3B66, 0x3054, 0xF5CD, 0x2630, 0xE3A9, 0xE89B, 0x2D02,
	0xA76F, 0x62F6, 0x69C4, 0xAC5D, 0x7FA0, 0xBA39, 0xB10B, 0x7492,
	0x5368, 0x96F1, 0x9DC3, 0x585A, 0x8BA7, 0x4E3E, 0x450C, 0x8095
};

// Functions ------------------------------------------------------------------------------------------------------------------

uint16_t ltc681xCalculatePec (uint8_t* data, uint8_t dataCount)
{
	// The Packet Error Code (PEC) is a 15-bit CRC, generated using the following polynomial:
	//   x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1
	//
	// See LTC6811 datasheet, pg.53, or LTC6813 datasheet, pg.54 for more info.

	// Begin with 0b0000 0000 0001 0000 (seed value)
	uint16_t remainder = 0x0010;

	// Traverse each byte of the payload, calculating the remainder using a lookup table.
	for (uint8_t index = 0; index < dataCount; ++index)
	{
		uint8_t addr = ((remainder >> 7) ^ data [index]);
		remainder = (remainder << 8) ^ PEC_LUT [addr];
	}

	// The LSB of the PEC word is a 0, so left shift once to align it.
	return (remainder << 1);
}

bool ltc681xValidatePec (uint8_t* data, uint8_t dataCount, uint16_t pec)
{
	uint16_t actualPec = ltc681xCalculatePec (data, dataCount);
	return pec == actualPec;
}

bool ltc681xPollAdc (ltc681x_t* bottom, sysinterval_t timeout)
{
	// Get the expiration deadline
	systime_t timeStart = chVTGetSystemTimeX ();
	systime_t timeDeadline = chTimeAddX (timeStart, timeout + bottom->config->pollTolerance);

	// Toggle the clock line until the bottom device shifts out a '1'. The value transmitted does not matter, just that the
	// clock line is being toggled.
	uint8_t txByte [1] = { 0xFF };
	uint8_t rxByte [1] = { 0x00 };

	systime_t timeCurrent = timeStart;
	while (chTimeIsInRangeX (timeCurrent, timeStart, timeDeadline))
	{
		spiExchange (bottom->config->spiDriver, sizeof (uint8_t), txByte, rxByte);
		if (rxByte [0] != 0)
		{
			// Non-zero read, success.
			spiUnselect (bottom->config->spiDriver);
			return true;
		}
		timeCurrent = chVTGetSystemTimeX ();
	}

	// No response before expiration, fail the chain.
	ltc681xFailChain (bottom);
	spiUnselect (bottom->config->spiDriver);
	return false;
}

bool ltc681xWriteCommand (ltc681x_t* bottom, uint16_t command, bool unselect)
{
	// Transmit Frame:
	//  0            1            2        3
	// ---------------------------------------------
	// | Command HI | Command LO | PEC HI | PEC LO |
	// ---------------------------------------------
	//
	// See LTC6811 datasheet, pg.58 table.32, or LTC6813 datasheet, pg.59 table.33, for more info.

	// Write the command word followed by the PEC word.
	uint8_t tx [sizeof (uint16_t) * 2] = { command >> 8, command };
	uint16_t pec = ltc681xCalculatePec (tx, 2);
	tx [2] = pec >> 8;
	tx [3] = pec;

	spiSelect (bottom->config->spiDriver);

	uint8_t rx [sizeof (tx)];
	if (spiExchange (bottom->config->spiDriver, sizeof (tx), tx, rx) != MSG_OK)
	{
		spiUnselect (bottom->config->spiDriver);
		ltc681xFailChain (bottom);
		return false;
	}

	if (unselect)
		spiUnselect (bottom->config->spiDriver);

	return true;
}

bool ltc681xWriteRegisterGroups (ltc681x_t* bottom, uint16_t command)
{
	// Transmit Frame:
	//  0            1            2        3
	// ---------------------------------------------
	// | Command HI | Command LO | PEC HI | PEC LO |
	// ---------------------------------------------
	//  4               5                     9               10       11
	// -------------------------------------------------------------------------
	// | Dev N-1 Reg 0 | Dev N-1 Reg 1 | ... | Dev N-1 Reg 5 | PEC HI | PEC LO |
	// -------------------------------------------------------------------------
	//  12              13                    17              18       19
	// -------------------------------------------------------------------------
	// | Dev N-2 Reg 0 | Dev N-2 Reg 1 | ... | Dev N-2 Reg 5 | PEC HI | PEC LO |
	// -------------------------------------------------------------------------
	// Where bytes [12, 19] are repeated down to device 0.
	//
	// See LTC6811 datasheet, pg.58 table.32, or LTC6813 datasheet, pg.59 table.33, for more info.

	if (!ltc681xWriteCommand (bottom, command, false))
		return false;

	// Write each individual device's register group.
	// Note the first written data goes to the last device in the stack (device N-1).
	for (ltc681x_t* device = bottom->topDevice; device != NULL; device = device->lowerDevice)
	{
		uint16_t pec = ltc681xCalculatePec (device->tx, LTC681X_BUFFER_SIZE - sizeof(uint16_t));
		device->tx [LTC681X_BUFFER_SIZE - 2] = pec >> 8;
		device->tx [LTC681X_BUFFER_SIZE - 1] = pec;

		uint8_t rx [LTC681X_BUFFER_SIZE];
		if (spiExchange (bottom->config->spiDriver, LTC681X_BUFFER_SIZE, device->tx, rx)
			!= MSG_OK)
		{
			spiUnselect (bottom->config->spiDriver);
			ltc681xFailChain (bottom);
			return false;
		}
	}

	spiUnselect (bottom->config->spiDriver);

	return true;
}

bool ltc681xReadRegisterGroups (ltc681x_t* bottom, uint16_t command)
{
	// See LTC6811 datasheet, pg.58, or LTC6813 datasheet, pg.59, for more info.

	// Transmit Frame:
	//  0            1            2        3
	// ---------------------------------------------
	// | Command HI | Command LO | PEC HI | PEC LO |
	// ---------------------------------------------
	//
	// Receive Frame:
	//  0             1                   5             6        7
	// -------------------------------------------------------------------
	// | Dev 0 Reg 0 | Dev 0 Reg 1 | ... | Dev 0 Reg 5 | PEC HI | PEC LO |
	// -------------------------------------------------------------------
	//  8             9                   13            14       15
	// -------------------------------------------------------------------
	// | Dev 1 Reg 0 | Dev 1 Reg 1 | ... | Dev 1 Reg 5 | PEC HI | PEC LO |
	// -------------------------------------------------------------------
	// Where bytes [8, 15] are repeated up to device N-1.
	//
	// See LTC6811 datasheet, pg.58 table.34, or LTC6813 datasheet, pg.59 table.34, for more info.

	for (uint16_t attempt = 0; attempt < bottom->config->readAttemptCount; ++attempt)
	{
		if (!ltc681xWriteCommand (bottom, command, false))
			return false;

		// Read each individual device's register group.
		// Note the first read data comes from the first device in the stack (device 0).
		for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		{
			uint8_t rx [LTC681X_BUFFER_SIZE];
			if (spiExchange (bottom->config->spiDriver, LTC681X_BUFFER_SIZE, rx, device->rx)
				!= MSG_OK)
			{
				// If a SPI error occurs, something has failed inside the STM, re-attempting will not help.
				spiUnselect (device->config->spiDriver);
				ltc681xFailChain (bottom);
				return false;
			}
		}

		spiUnselect (bottom->config->spiDriver);

		// Validate the PEC of each device's frame.
		bool valid = true;
		for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		{
			uint16_t pec = device->rx [LTC681X_BUFFER_SIZE - 1] | (device->rx [LTC681X_BUFFER_SIZE - 2] << 8);

			if (!ltc681xValidatePec (device->rx, LTC681X_BUFFER_SIZE - sizeof (uint16_t), pec))
			{
				// If this is not the last attempt, re-attempt the entire operation.
				valid = false;
				if (attempt != bottom->config->readAttemptCount - 1)
					break;

				// If this is the last attempt, fail the device and continue checking the others.
				device->state = LTC681X_STATE_PEC_ERROR;
			}
		}
		if (valid)
			return true;
	}

	return false;
}

void ltc681xFailChain (ltc681x_t* bottom)
{
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		device->state = LTC681X_STATE_FAILED;
}

void ltc681xFailGpio (ltc681x_t* bottom)
{
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		for (uint16_t gpioIndex = 0; gpioIndex < LTC681X_GPIO_COUNT; ++gpioIndex)
		{
			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
			analogSensorFail (sensor);
		}
	}
}