// Header
#include "ltc6811.h"

// Constants / Macros ---------------------------------------------------------------------------------------------------------

#define T_READY_MAX						TIME_US2I (10)
#define T_WAKE_MAX						TIME_US2I (400)

#define COMMAND_WRCFGA					0b00000000001
#define COMMAND_RDCFGA					0b00000000010

#define COMMAND_RDCVA					0b00000000100
#define COMMAND_RDCVB					0b00000000110
#define COMMAND_RDCVC					0b00000001000
#define COMMAND_RDCVD					0b00000001010

#define COMMAND_RDAUXA					0b00000001100
#define COMMAND_RDAUXB					0b00000001110

#define COMMAND_RDSTATA					0b00000010000
#define COMMAND_RDSTATB					0b00000010010

#define COMMAND_WRSCTRL					0b00000010100
#define COMMAND_RDSCTRL					0b00000010110

#define COMMAND_WRPWM					0b00000100000
#define COMMAND_RDPWM					0b00000100010

#define COMMAND_STSCTRL					0b00000011001
#define COMMAND_CLRSCTRL				0b00000011000

#define COMMAND_ADCV(ch, md, dcp)		(0b01001100000 | ((md) << 7) | ((dcp) << 4) | (ch))
#define COMMAND_ADOW(ch, md, dcp, pup)	(0b01000101000 | ((md) << 7) | ((dcp) << 4) | (ch) | ((pup) << 6))
#define COMMAND_CVST(md, st)			(0b01000000111 | ((md) << 7) | ((st) << 5))
#define COMMAND_ADOL(md, dcp)			(0b01000000001 | ((md) << 7) | ((dcp) << 4))
#define COMMAND_ADAX(md, chg)			(0b10001100000 | ((md) << 7) | (chg))

#define COMMAND_PLADC					0b11100010100

#define COMMAND_ADSTAT(md, chst)		(0b10001101000 | ((md) << 7) | (chst))

#define VUV(vuv)						((uint16_t) (vuv * 625.0f) - 1)
#define VOV(vov)						((uint16_t) (vov * 625.0f))

#define CFGR0(gpio5, gpio4, gpio3, gpio2, gpio1, refon, adcopt)																\
	((uint8_t) (((gpio5) << 7) | ((gpio4) << 6) | ((gpio3) << 5) | ((gpio2) << 4) | ((gpio1) << 3) | ((refon) << 2) |		\
	(adcopt)))
#define CFGR1(vuv)						((uint8_t) VUV (vuv))
#define CFGR2(vuv, vov)					((uint8_t) ((VOV (vov) << 4) | (VUV (vuv) >> 8)))
#define CFGR3(vov)						((uint8_t) (VOV (vov) >> 4))
#define CFGR4(dcc8, dcc7, dcc6, dcc5, dcc4, dcc3, dcc2, dcc1)																\
	((uint8_t) (((dcc8) << 7) | ((dcc7) << 6) | ((dcc6) << 5) | ((dcc5) << 4) | ((dcc4) << 3) | ((dcc3) << 2) |				\
	((dcc2) << 1) | (dcc1)))
#define CFGR5(dcto, dcc12, dcc11, dcc10, dcc9)																				\
	((uint8_t) (((dcto) << 4) | ((dcc12) << 3) | ((dcc11) << 2) | ((dcc10) << 1) | (dcc9)))

#define STAR0_1_SC(star0, star1)		(((uint16_t) (((star1) << 8) | (star0))) * 0.002f)
#define STAR2_3_ITMP(star2, star3)		(((uint16_t) (((star3) << 8) | (star2))) * 0.1f / 7.5f - 273.0f)

#define STBR2_C1UV(stbr2)				((stbr2 & 0b00000001) == 0b00000001)
#define STBR2_C1OV(stbr2)				((stbr2 & 0b00000010) == 0b00000010)
#define STBR2_C2UV(stbr2)				((stbr2 & 0b00000100) == 0b00000100)
#define STBR2_C2OV(stbr2)				((stbr2 & 0b00001000) == 0b00001000)
#define STBR2_C3UV(stbr2)				((stbr2 & 0b00010000) == 0b00010000)
#define STBR2_C3OV(stbr2)				((stbr2 & 0b00100000) == 0b00100000)
#define STBR2_C4UV(stbr2)				((stbr2 & 0b01000000) == 0b01000000)
#define STBR2_C4OV(stbr2)				((stbr2 & 0b10000000) == 0b10000000)

#define STBR3_C5UV(stbr3)				((stbr3 & 0b00000001) == 0b00000001)
#define STBR3_C5OV(stbr3)				((stbr3 & 0b00000010) == 0b00000010)
#define STBR3_C6UV(stbr3)				((stbr3 & 0b00000100) == 0b00000100)
#define STBR3_C6OV(stbr3)				((stbr3 & 0b00001000) == 0b00001000)
#define STBR3_C7UV(stbr3)				((stbr3 & 0b00010000) == 0b00010000)
#define STBR3_C7OV(stbr3)				((stbr3 & 0b00100000) == 0b00100000)
#define STBR3_C8UV(stbr3)				((stbr3 & 0b01000000) == 0b01000000)
#define STBR3_C8OV(stbr3)				((stbr3 & 0b10000000) == 0b10000000)

#define STBR4_C9UV(stbr4)				((stbr4 & 0b00000001) == 0b00000001)
#define STBR4_C9OV(stbr4)				((stbr4 & 0b00000010) == 0b00000010)
#define STBR4_C10UV(stbr4)				((stbr4 & 0b00000100) == 0b00000100)
#define STBR4_C10OV(stbr4)				((stbr4 & 0b00001000) == 0b00001000)
#define STBR4_C11UV(stbr4)				((stbr4 & 0b00010000) == 0b00010000)
#define STBR4_C11OV(stbr4)				((stbr4 & 0b00100000) == 0b00100000)
#define STBR4_C12UV(stbr4)				((stbr4 & 0b01000000) == 0b01000000)
#define STBR4_C12OV(stbr4)				((stbr4 & 0b10000000) == 0b10000000)

// 100 uV / LSB
#define CELL_VOLTAGE_FACTOR				0.0001f
#define WORD_TO_CELL_VOLTAGE(word)		(((uint16_t) (word)) * CELL_VOLTAGE_FACTOR)

/// @brief Buffer to read/write irrelevant data from/to. Used in SPI transactions where the data doesn't matter.
static uint8_t nullBuffer [LTC6811_BUFFER_SIZE];

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

/// @brief The total conversion time of the cell voltage ADC / GPIO ADC measuring all cells / GPIO. Indexed by
/// @c ltc6811AdcMode_t .
static const systime_t ADC_MODE_TIMEOUTS [] =
{
	TIME_US2I (12807),	// For 422 Hz mode
	TIME_US2I (1113),	// For 27 kHz mode
	TIME_US2I (2335),	// For 7 kHz mode
	TIME_MS2I (202)		// For 26 Hz mode
};

/// @brief The total conversion time of the status register ADC measuring all values. Indexed by @c ltc6811AdcMode_t .
static const systime_t STATUS_ADC_MODE_TIMEOUTS [] =
{
	TIME_US2I (8537),	// For 422 Hz mode
	TIME_US2I (748),	// For 27 kHz mode
	TIME_US2I (1563),	// For 7 kHz mode
	TIME_MS2I (135),	// For 26 Hz mode
};

typedef enum
{
	CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER = 0,
	CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER = 1,
	CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER = 2
} cellVoltageDestination_t;

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the packet error code given a frame's contents.
 * @param data The data to calculate the code for.
 * @param dataCount The number of elements in @c data .
 * @return The calculated PEC word. Note this value is already shifted to 0 the LSB.
 */
static uint16_t calculatePec (uint8_t* data, uint8_t dataCount);

/**
 * @brief Checks whether the packet error code of a frame is correct.
 * @param data The data to validate the code of.
 * @param dataCount The number of elements in @c data .
 * @param pec The PEC to validate against.
 * @return True if the PEC is correct, false otherwise.
 */
static bool validatePec (uint8_t* data, uint8_t dataCount, uint16_t pec);

/**
 * @brief Wakes up all devices in an LTC6811 daisy chain. This method guarantees all devices are in the ready or standby state,
 * regardless of the previous state of the daisy chain.
 * @param bottom The bottom (first) device in the daisy chain.
 */
static void wakeupSleep (ltc6811_t* bottom);

/**
 * @brief Blocks until a previously scheduled ADC conversion is completed.
 * @param bottom The bottom (first) device in the daisy chain.
 * @return True if all device conversions are complete, false if a timeout occurred. Timeouts may be considered a fatal error.
 */
static bool pollAdc (ltc6811_t* bottom, sysinterval_t timeout);

/**
 * @brief Writes a command to each device in a chain.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The command to write.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee write success.
 */
static bool writeCommand (ltc6811_t* bottom, uint16_t command);

/**
 * @brief Writes to a data register group of each device in a chain.
 * @note The to written to each device should be placed into its @c tx buffer.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The write command of the register group.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee write success.
 */
static bool writeRegisterGroups (ltc6811_t* bottom, uint16_t command);

/**
 * @brief Reads from a data register group of each device in a chain.
 * @note The data read from each device is placed into its @c rx buffer.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The read command of the register group.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee read data is valid,
 * check individual device states to determine validity.
 */
static bool readRegisterGroups (ltc6811_t* bottom, uint16_t command);

/**
 * @brief Samples the cell voltages of each device in a chain.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param destination The destination buffer to use for each device.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee read data is valid,
 * check individual device states to determine validity
 */
static bool sampleCells (ltc6811_t* bottom, cellVoltageDestination_t destination);

/**
 * @brief Updates a fault flag and counter based on a current fault state.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param flags The array of fault flags (ex. @c undervoltageFaults ).
 * @param counters The array of fault counters (ex. @c undervoltageCounters ).
 * @param index The index of the fault within the @c flags & @c counters arrays .
 * @param faulted Whether the current reading is faulted or not.
 */
static void setFault (ltc6811_t* bottom, bool* flags, uint16_t* counters, uint8_t index, bool faulted);

/**
 * @brief Sets all devices in a chain to the @c LTC6811_STATE_FAILED state.
 * @param bottom The bottom (first) device in the daisy chain.
 */
static void failChain (ltc6811_t* bottom);

/**
 * @brief Function to be called when a GPIO sampling operation fails. All GPIO will be put into the
 * @c ANALOG_SENSOR_FAILED state.
 * @param bottom The bottom (first) device in the daisy chain.
 */
static void failGpio (ltc6811_t* bottom);

/**
 * @brief Acquires and starts a daisy chain's SPI driver.
 * @param bottom The bottom (first) device in the chain.
 */
static inline void start (ltc6811_t* bottom)
{
	// Acquire the SPI bus (if enabled)
	#if SPI_USE_MUTUAL_EXCLUSION
	spiAcquireBus (bottom->config->spiDriver);
	#endif // SPI_USE_MUTUAL_EXCLUSION

	// Start the SPI driver
	spiStart (bottom->config->spiDriver, &bottom->config->spiConfig);
}

/**
 * @brief Stops and releases a chain's SPI driver.
 * @param bottom The bottom (first) device in the daisy chain.
 */
static inline void stop (ltc6811_t* bottom)
{
	// Stop the SPI driver
	spiStop (bottom->config->spiDriver);

	// Release the SPI bus (if enabled)
	#if SPI_USE_MUTUAL_EXCLUSION
	spiReleaseBus (bottom->config->spiDriver);
	#endif // SPI_USE_MUTUAL_EXCLUSION
}

// Functions ------------------------------------------------------------------------------------------------------------------

bool ltc6811Init (ltc6811_t* const* daisyChain, uint16_t deviceCount, const ltc6811Config_t* config)
{
	for (uint16_t index = 0; index < deviceCount; ++index)
	{
		// Initialize the doubly linked-list.
		daisyChain [index]->upperDevice = (index != deviceCount - 1) ? daisyChain [index + 1] : NULL;
		daisyChain [index]->lowerDevice = (index != 0) ? daisyChain [index - 1] : NULL;
		daisyChain [index]->topDevice = (index == 0) ? daisyChain [deviceCount - 1] : NULL;

		// Store the configuration in the bottom device
		if (index == 0)
			daisyChain [index]->config = config;
		else
		 	daisyChain [index]->config = NULL;

		// Start from the ready state.
		daisyChain [index]->state = LTC6811_STATE_READY;

		daisyChain [index]->dischargeAllowed = config->dischargeAllowed;

		for (uint16_t cell = 0; cell < LTC6811_CELL_COUNT; ++cell)
		{
			// Set all cells to default
			daisyChain [index]->cellVoltages [cell] = 0.0f;
			daisyChain [index]->cellsDischarging [cell] = false;

			// Reset the cell faults.
			daisyChain [index]->overvoltageCounters [cell] = 0;
			daisyChain [index]->undervoltageCounters [cell] = 0;
		}

		// Reset the wire faults.
		for (uint16_t wire = 0; wire < LTC6811_WIRE_COUNT; ++wire)
			daisyChain [index]->openWireCounters [wire] = 0;

		// TODO(Barach): Not a huge fan of this.
		for (uint8_t gpio = 0; gpio < LTC6811_GPIO_COUNT; ++gpio)
			daisyChain [index]->gpioSensors [gpio] = config->gpioSensors [index][gpio];
	}

	if (!ltc6811WriteConfig (daisyChain [0]))
		return false;

	return ltc6811SampleCells (daisyChain [0]);
}

bool ltc6811WriteConfig (ltc6811_t* bottom)
{
	start (bottom);
	wakeupSleep (bottom);

	// Write the configuration register group A
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// GPIO set to high impedence, reference disabled outside conversion, ADC option 0.
		device->tx [0] = CFGR0 (1, 1, 1, 1, 1, 0, 0);

		// Undervoltage / overvoltage values.
		device->tx [1] = CFGR1 (bottom->config->cellVoltageMin);
		device->tx [2] = CFGR2 (bottom->config->cellVoltageMin, bottom->config->cellVoltageMax);
		device->tx [3] = CFGR3 (bottom->config->cellVoltageMax);

		// Cells to discharge and discharge timeout.
		device->tx [4] = CFGR4 (device->cellsDischarging [7], device->cellsDischarging [6],
			device->cellsDischarging [5], device->cellsDischarging [4], device->cellsDischarging [3],
			device->cellsDischarging [2], device->cellsDischarging [1], device->cellsDischarging [0]);
		device->tx [5] = CFGR5 (bottom->config->dischargeTimeout,
			device->cellsDischarging [11], device->cellsDischarging [10],
			device->cellsDischarging [9], device->cellsDischarging [8]);
	}

	bool result = writeRegisterGroups (bottom, COMMAND_WRCFGA);

	stop (bottom);
	return result;
}

bool ltc6811SampleCells (ltc6811_t* bottom)
{
	start (bottom);
	wakeupSleep (bottom);

	// Sample the cell voltages into the cell voltage buffer.
	if (!sampleCells (bottom, CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER))
	{
		stop (bottom);
		return false;
	}

	stop (bottom);
	return true;
}

bool ltc6811SampleStatus (ltc6811_t* bottom)
{
	start (bottom);
	wakeupSleep (bottom);

	// Start the ADC measurement for all values.
	if (!writeCommand (bottom, COMMAND_ADSTAT (bottom->config->statusAdcMode, 0b000)))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Block until the ADC conversion is complete
	if (!pollAdc (bottom, STATUS_ADC_MODE_TIMEOUTS [bottom->config->statusAdcMode]))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Read the status register group A.
	if (!readRegisterGroups (bottom, COMMAND_RDSTATA))
	{
		stop (bottom);
		return false;
	}

	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Read the sum of cell voltages.
		device->cellVoltageSum = STAR0_1_SC (device->rx [0], device->rx [1]);

		// Read the die temperature.
		device->dieTemperature = STAR2_3_ITMP (device->rx [2], device->rx [3]);
	}

	stop (bottom);
	return true;
}

bool ltc6811SampleCellVoltageFaults (ltc6811_t* bottom)
{
	start (bottom);
	wakeupSleep (bottom);

	// Read the status register group B.
	if (!readRegisterGroups (bottom, COMMAND_RDSTATB))
	{
		stop (bottom);
		return false;
	}

	// Read each device's undervoltage / overvoltage flags.
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// STBR2
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 0, STBR2_C1UV (device->rx [2]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 1, STBR2_C2UV (device->rx [2]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 2, STBR2_C3UV (device->rx [2]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 3, STBR2_C4UV (device->rx [2]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 0, STBR2_C1OV (device->rx [2]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 1, STBR2_C2OV (device->rx [2]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 2, STBR2_C3OV (device->rx [2]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 3, STBR2_C4OV (device->rx [2]));

		// STBR3
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 4, STBR3_C5UV (device->rx [3]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 5, STBR3_C6UV (device->rx [3]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 6, STBR3_C7UV (device->rx [3]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 7, STBR3_C8UV (device->rx [3]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 4, STBR3_C5OV (device->rx [3]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 5, STBR3_C6OV (device->rx [3]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 6, STBR3_C7OV (device->rx [3]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 7, STBR3_C8OV (device->rx [3]));

		// STBR4
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 8, STBR4_C9UV (device->rx [4]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 9, STBR4_C10UV (device->rx [4]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 10, STBR4_C11UV (device->rx [4]));
		setFault (bottom, device->undervoltageFaults, device->undervoltageCounters, 11, STBR4_C12UV (device->rx [4]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 8, STBR4_C9OV (device->rx [4]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 9, STBR4_C10OV (device->rx [4]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 10, STBR4_C11OV (device->rx [4]));
		setFault (bottom, device->overvoltageFaults, device->overvoltageCounters, 11, STBR4_C12OV (device->rx [4]));
	}

	stop (bottom);
	return true;
}

bool ltc6811SampleGpio (ltc6811_t* bottom)
{
	// See LTC6811 datasheet section "Auxiliary (GPIO) Measurements (ADAX Command)", pg.26.

	start (bottom);
	wakeupSleep (bottom);

	// Start the ADC measurement for all GPIO.
	if (!writeCommand (bottom, COMMAND_ADAX (bottom->config->gpioAdcMode, 0b000)))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Block until the ADC conversion is complete
	if (!pollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->gpioAdcMode]))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Read the auxiliary register group B
	if (!readRegisterGroups (bottom, COMMAND_RDAUXB))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Read GPIO 4, GPIO 5, and VREF2
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Store VREF2
		device->vref2 = device->rx [5] << 8 | device->rx [4];

		for (uint8_t gpioIndex = 3; gpioIndex < LTC6811_GPIO_COUNT; ++gpioIndex)
		{
			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
			if (sensor == NULL)
				continue;

			uint16_t sample = device->rx [gpioIndex * 2 - 5] << 8 | device->rx [gpioIndex * 2 - 6];

			// Update the sensor with the last sample, providing VREF2 as the analog supply voltage.
			sensor->callback (sensor, sample, device->vref2);
		}
	}

	// Read the auxiliary register group A
	if (!readRegisterGroups (bottom, COMMAND_RDAUXA))
	{
		stop (bottom);
		failGpio (bottom);
		return false;
	}

	// Read GPIO 1 to 3
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		for (uint8_t gpioIndex = 0; gpioIndex < 3; ++gpioIndex)
		{
			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
			if (sensor == NULL)
				continue;

			uint16_t sample = device->rx [gpioIndex * 2 + 1] << 8 | device->rx [gpioIndex * 2];

			// Update the sensor with the last sample, providing VREF2 as the analog supply voltage.
			sensor->callback (sensor, sample, device->vref2);
		}
	}

	stop (bottom);
	return true;
}

bool ltc6811OpenWireTest (ltc6811_t* bottom)
{
	// See LTC6811 datasheet section "Open Wire Check (ADOW Command)", pg.34.

	start (bottom);
	wakeupSleep (bottom);

	// Pull-up measurement
	for (uint8_t index = 0; index < bottom->config->openWireTestIterations; ++index)
	{
		// Send the pull-up command.
		if (!writeCommand (bottom, COMMAND_ADOW (0b000, bottom->config->cellAdcMode, bottom->dischargeAllowed, true)))
		{
			stop (bottom);
			return false;
		}

		// Block until complete.
		if (!pollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
		{
			stop (bottom);
			return false;
		}
	}

	// Sample the cell voltages into the pull-up buffer.
	if (!sampleCells (bottom, CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER))
	{
		stop (bottom);
		return false;
	}

	// Pull-down measurement
	for (uint8_t index = 0; index < bottom->config->openWireTestIterations; ++index)
	{
		// Send the pull-down command.
		if (!writeCommand (bottom, COMMAND_ADOW (0b000, bottom->config->cellAdcMode, bottom->dischargeAllowed, false)))
		{
			stop (bottom);
			return false;
		}

		// Block until complete.
		if (!pollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
		{
			stop (bottom);
			return false;
		}
	}

	// Sample the cell voltages into the pull-down buffer.
	if (!sampleCells (bottom, CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER))
	{
		stop (bottom);
		return false;
	}

	stop (bottom);

	// Check each device, cell-by-cell
	// Note that in the datasheet, sense wires are indexed 0 to 12 while cells are indexed 1 to 12.
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// For wire 0, if cell 1 read 0V (1mV tolerance for noise) during pull-up, the wire is open.
		setFault (bottom, device->openWireFaults, device->openWireCounters, 0,
			device->cellVoltagesPullup [0] < 0.001f && device->cellVoltagesPullup [0] > -0.001f);

		// For wire n in [1 to 11], if cell delta (n+1) < -400mV, the wire is open.
		for (uint8_t wire = 1; wire < LTC6811_CELL_COUNT - 1; ++wire)
		{
			device->cellVoltagesDelta [wire] = device->cellVoltagesPullup [wire] - device->cellVoltagesPulldown [wire];
			setFault (bottom, device->openWireFaults, device->openWireCounters, wire,
				device->cellVoltagesDelta [wire] < -0.4f);
		}

		// TODO(Barach): The datasheet calls out 400mV, but testing shows it as 800mV. Figure out why.
		// For wire 11, if cell delta 12 < -800mV, the wire is open.
		device->cellVoltagesDelta [11] = device->cellVoltagesPullup [11] - device->cellVoltagesPulldown [11];
		setFault (bottom, device->openWireFaults, device->openWireCounters, 11,
			device->cellVoltagesDelta [11] < -0.8f);

		// For wire 12, if cell 12 read 0V (1mV tolerance for noise) during pull-down, the wire is open.
		setFault (bottom, device->openWireFaults, device->openWireCounters, 12,
			device->cellVoltagesPulldown [11] < 0.001f && device->cellVoltagesPulldown [11] > -0.001f);
	}

	return true;
}

uint16_t calculatePec (uint8_t* data, uint8_t dataCount)
{
	// The Packet Error Code (PEC) is a 15-bit CRC, generated using the following polynomial:
	//   x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1
	//
	// See LTC6811 datasheet, pg.53 for more info.

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

bool validatePec (uint8_t* data, uint8_t dataCount, uint16_t pec)
{
	// TODO(Barach): There is probably a more efficient way to validate this.
	uint16_t actualPec = calculatePec (data, dataCount);
	return pec == actualPec;
}

void wakeupSleep (ltc6811_t* bottom)
{
	// Sends a wakeup signal using the algorithm described in "Waking a Daisy Chain - Method 2"
	// See LTC6811 datasheet, pg.52 for more info.

	// Wake each device individually. If a device in the stack is not in the ready state, it won't propogate the first signal
	// it receives, rather it will wake up and enter the ready state. Once said device is in the ready state, it will propogate
	// the next signal it receives. By sending N signals, we are allowing the first N - 1 devices to not propogate the first
	// signal they receive, in turn guaranteeing that all N devices will receive at least 1 signal.
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Drive CS low for the maximum wakeup time (guarantees this device will wake).
		spiSelect (bottom->config->spiDriver);
		chThdSleep (T_WAKE_MAX);

		// Release CS and allow the device to enter the ready state.
		spiUnselect (bottom->config->spiDriver);
		chThdSleep (T_READY_MAX);
	}
}

bool pollAdc (ltc6811_t* bottom, sysinterval_t timeout)
{
	// TODO(Barach): This should use the writeCommand function.

	// Write the command word followed by the PEC word.
	uint8_t tx [sizeof (uint16_t) * 2] = { COMMAND_PLADC >> 8, (uint8_t) COMMAND_PLADC };
	uint16_t pec = calculatePec (tx, 2);
	tx [2] = pec >> 8;
	tx [3] = pec;

	spiSelect (bottom->config->spiDriver);

	if (spiExchange (bottom->config->spiDriver, sizeof (tx), tx, nullBuffer) != MSG_OK)
	{
		spiUnselect (bottom->config->spiDriver);
		return false;
	}

	// If the conversion has already finished, exit early.
	if (palReadLine (bottom->config->spiMiso))
	{
		spiUnselect (bottom->config->spiDriver);
		return true;
	}

	// Wait for the conversion to finish.
	// palEnableLineEvent (chain->config->spiMiso, PAL_EVENT_MODE_RISING_EDGE);
	// palWaitLineTimeout (chain->config->spiMiso, timeout);
	// palDisableLineEvent (chain->config->spiMiso);

	// TODO(Barach): Polling isn't working
	chThdSleep (timeout);
	return true;

	// // If conversion is finished, success, otherwise failed.
	// bool result = palReadLine (chain->config->spiMiso);
	// spiUnselect (chain->config->spiDriver);
	// return result;
}

bool writeCommand (ltc6811_t* bottom, uint16_t command)
{
	// Transmit Frame:
	//  0            1            2        3
	// ---------------------------------------------
	// | Command HI | Command LO | PEC HI | PEC LO |
	// ---------------------------------------------

	// Write the command word followed by the PEC word.
	uint8_t tx [sizeof (uint16_t) * 2] = { command >> 8, command };
	uint16_t pec = calculatePec (tx, 2);
	tx [2] = pec >> 8;
	tx [3] = pec;

	spiSelect (bottom->config->spiDriver);

	if (spiExchange (bottom->config->spiDriver, sizeof (tx), tx, nullBuffer) != MSG_OK)
	{
		spiUnselect (bottom->config->spiDriver);
		failChain (bottom);
		return false;
	}

	spiUnselect (bottom->config->spiDriver);

	return true;
}

bool writeRegisterGroups (ltc6811_t* bottom, uint16_t command)
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

	// Write the command word followed by the PEC word.
	uint8_t tx [sizeof (uint16_t) * 2] = { command >> 8, command };
	uint16_t pec = calculatePec (tx, 2);
	tx [2] = pec >> 8;
	tx [3] = pec;

	spiSelect (bottom->config->spiDriver);

	if (spiExchange (bottom->config->spiDriver, sizeof (tx), tx, nullBuffer) != MSG_OK)
	{
		spiUnselect (bottom->config->spiDriver);
		failChain (bottom);
		return false;
	}

	// Write each individual device's register group.
	// Note the first written data goes to the last device in the stack (device N-1).
	for (ltc6811_t* device = bottom->topDevice; device != NULL; device = device->lowerDevice)
	{
		pec = calculatePec (device->tx, LTC6811_BUFFER_SIZE - sizeof(uint16_t));
		device->tx [LTC6811_BUFFER_SIZE - 2] = pec >> 8;
		device->tx [LTC6811_BUFFER_SIZE - 1] = pec;

		// TODO(Barach): Null buffer is non-reentrant?
		if (spiExchange (bottom->config->spiDriver, LTC6811_BUFFER_SIZE, device->tx, nullBuffer)
			!= MSG_OK)
		{
			spiUnselect (bottom->config->spiDriver);
			failChain (bottom);
			return false;
		}
	}

	spiUnselect (bottom->config->spiDriver);

	return true;
}

bool readRegisterGroups (ltc6811_t* bottom, uint16_t command)
{
	// See LTC6811 datasheet, pg.58 for more info.

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

	for (uint16_t attempt = 0; attempt < bottom->config->readAttemptCount; ++attempt)
	{
		// TODO(Barach): The control of this is messy.

		// Write the command word followed by the PEC word.
		uint8_t tx [sizeof (uint16_t) * 2] = { command >> 8, command };
		uint16_t pec = calculatePec (tx, 2);
		tx [2] = pec >> 8;
		tx [3] = pec;

		spiSelect (bottom->config->spiDriver);

		// TODO(Barach): Null buffer is nonreentrant?
		if (spiExchange (bottom->config->spiDriver, sizeof (tx), tx, nullBuffer) != MSG_OK)
		{
			// If a SPI error occurs, something has failed inside the STM, re-attempting will not help.
			spiUnselect (bottom->config->spiDriver);
			failChain (bottom);
			return false;
		}

		// Read each individual device's register group.
		// Note the first read data comes from the first device in the stack (device 0).
		for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		{
			if (spiExchange (bottom->config->spiDriver, LTC6811_BUFFER_SIZE, nullBuffer, device->rx)
				!= MSG_OK)
			{
				// If a SPI error occurs, something has failed inside the STM, re-attempting will not help.
				spiUnselect (device->config->spiDriver);
				failChain (bottom);
				return false;
			}
		}

		spiUnselect (bottom->config->spiDriver);

		// Validate the PEC of each device's frame.
		for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		{
			pec = device->rx [LTC6811_BUFFER_SIZE - 1] | (device->rx [LTC6811_BUFFER_SIZE - 2] << 8);

			if (!validatePec (device->rx, LTC6811_BUFFER_SIZE - sizeof (uint16_t), pec))
			{
				// If this is not the last attempt, re-attempt the entire operation.
				if (attempt != bottom->config->readAttemptCount - 1)
					break;

				// If this is the last attempt, fail the device and continue checking the others.
				device->state = LTC6811_STATE_PEC_ERROR;
			}
		}
	}

	return true;
}

bool sampleCells (ltc6811_t* bottom, cellVoltageDestination_t destination)
{
	// See LTC6811 datasheet section "Measuring Cell Voltages (ADCV Command)", pg.25.

	// Start the cell voltage conversion for all cells, conditionally permitting discharge.
	if (!writeCommand (bottom, COMMAND_ADCV (bottom->config->cellAdcMode, bottom->dischargeAllowed, 0b000)))
		return false;

	if (!pollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
		return false;

	if (!readRegisterGroups (bottom, COMMAND_RDCVA))
		return false;

	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell0 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell1 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell2 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [0] = cell0;
			device->cellVoltages [1] = cell1;
			device->cellVoltages [2] = cell2;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [0] = cell0;
			device->cellVoltagesPullup [1] = cell1;
			device->cellVoltagesPullup [2] = cell2;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [0] = cell0;
			device->cellVoltagesPulldown [1] = cell1;
			device->cellVoltagesPulldown [2] = cell2;
			break;
		}
	}

	if (!readRegisterGroups (bottom, COMMAND_RDCVB))
		return false;

	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell3 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell4 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell5 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [3] = cell3;
			device->cellVoltages [4] = cell4;
			device->cellVoltages [5] = cell5;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [3] = cell3;
			device->cellVoltagesPullup [4] = cell4;
			device->cellVoltagesPullup [5] = cell5;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [3] = cell3;
			device->cellVoltagesPulldown [4] = cell4;
			device->cellVoltagesPulldown [5] = cell5;
			break;
		}
	}

	if (!readRegisterGroups (bottom, COMMAND_RDCVC))
		return false;

	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell6 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell7 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell8 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [6] = cell6;
			device->cellVoltages [7] = cell7;
			device->cellVoltages [8] = cell8;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [6] = cell6;
			device->cellVoltagesPullup [7] = cell7;
			device->cellVoltagesPullup [8] = cell8;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [6] = cell6;
			device->cellVoltagesPulldown [7] = cell7;
			device->cellVoltagesPulldown [8] = cell8;
			break;
		}
	}

	if (!readRegisterGroups (bottom, COMMAND_RDCVD))
		return false;

	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell9 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell10 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell11 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [9] = cell9;
			device->cellVoltages [10] = cell10;
			device->cellVoltages [11] = cell11;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [9] = cell9;
			device->cellVoltagesPullup [10] = cell10;
			device->cellVoltagesPullup [11] = cell11;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [9] = cell9;
			device->cellVoltagesPulldown [10] = cell10;
			device->cellVoltagesPulldown [11] = cell11;
			break;
		}
	}

	return true;
}

void setFault (ltc6811_t* bottom, bool* flags, uint16_t* counters, uint8_t index, bool faulted)
{
	if (faulted)
	{
		if (counters [index] < bottom->config->faultCount)
		{
			// If the counter hasn't filled yet, increment it.
			++(counters [index]);
		}
		else
		{
			// If the counter is filled, set the flag.
			flags [index] = true;
		}
	}
	else
	{
		// If no fault is present, reset both the flag and counter.
		counters [index] = 0;
		flags [index] = false;
	}
}

void failChain (ltc6811_t* bottom)
{
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		device->state = LTC6811_STATE_FAILED;
}

void failGpio (ltc6811_t* bottom)
{
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		for (uint16_t gpioIndex = 0; gpioIndex < LTC6811_GPIO_COUNT; ++gpioIndex)
		{
			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
			if (sensor != NULL)
				sensor->state = ANALOG_SENSOR_FAILED;
		}
	}
}