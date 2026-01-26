// Header
#include "ltc681x.h"

// Includes
#include "ltc681x_internal.h"

// Functions ------------------------------------------------------------------------------------------------------------------

void ltc681xStartChain (ltc681x_t* bottom, const ltc681xConfig_t* config)
{
	// Initialize the device
	*bottom = (ltc681x_t)
	{
		// Doubly linked-list
		.upperDevice			= NULL,
		.lowerDevice			= NULL,

		// Bottom-only configuration
		.topDevice				= bottom,
		.config					= config,
		.deviceCount			= 1,

		// Default values
		.gpioSensors			= { NULL },
		.state					= LTC681X_STATE_READY
	};
}

void ltc681xAppendChain (ltc681x_t* bottom, ltc681x_t* ltc)
{
	// Initialize the device
	*ltc = (ltc681x_t)
	{
		// Doubly linked-list
		.upperDevice	= NULL,
		.lowerDevice	= bottom->topDevice,

		// Default values
		.gpioSensors	= { NULL },
		.state			= LTC681X_STATE_READY
	};

	// Make this device the new top of the daisy chain
	bottom->topDevice->upperDevice = ltc;
	bottom->topDevice = ltc;
	++bottom->deviceCount;
}

bool ltc681xFinalizeChain (ltc681x_t* bottom)
{
	// Wakeup the chain (assuming sleep mode, as that is very likely) and configures all the devices.

	ltc681xStart (bottom);
	ltc681xWakeupSleep (bottom);

	if (!ltc681xWriteConfig (bottom))
	{
		ltc681xStop (bottom);
		return false;
	}

	ltc681xStop (bottom);
	return false;
}

void ltc681xSetGpioSensor (ltc681x_t* ltc, uint8_t index, analogSensor_t* sensor)
{
	ltc->gpioSensors [index] = sensor;
}

void ltc681xStart (ltc681x_t* bottom)
{
	// Acquire the SPI bus (if enabled)
	#if SPI_USE_MUTUAL_EXCLUSION
	spiAcquireBus (bottom->config->spiDriver);
	#endif // SPI_USE_MUTUAL_EXCLUSION

	// Start the SPI driver
	spiStart (bottom->config->spiDriver, &bottom->config->spiConfig);
}

void ltc681xStop (ltc681x_t* bottom)
{
	// Stop the SPI driver
	spiStop (bottom->config->spiDriver);

	// Release the SPI bus (if enabled)
	#if SPI_USE_MUTUAL_EXCLUSION
	spiReleaseBus (bottom->config->spiDriver);
	#endif // SPI_USE_MUTUAL_EXCLUSION
}

void ltc681xWakeupSleep (ltc681x_t* bottom)
{
	// Sends a wakeup signal using the algorithm described in "Waking a Daisy Chain - Method 2"
	// See LTC6811 datasheet, pg.52, or LTC6813 datasheet, pg.54, for more info.

	// Wake each device individually. If a device in the stack is not in the ready state, it won't propogate the first signal
	// it receives, rather it will wake up and enter the ready state. Once said device is in the ready state, it will propogate
	// the next signal it receives. By sending N signals, we are allowing the first N - 1 devices to not propogate the first
	// signal they receive, in turn guaranteeing that all N devices will receive at least 1 signal.
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Drive CS low for the maximum wakeup time (guarantees this device will wake).
		spiSelect (bottom->config->spiDriver);
		chThdSleep (T_WAKE_MAX);

		// Release CS and allow the device to enter the ready state.
		spiUnselect (bottom->config->spiDriver);
		chThdSleep (T_READY_MAX);
	}
}

void ltc681xWakeupIdle (ltc681x_t* bottom)
{
	// Sends a wakeup signal using the algorithm described in "Waking a Daisy Chain - Method 1"
	// See LTC6811 datasheet, pg.52, or LTC6813 datasheet, pg.53, for more info.

	// Get the number of devices in the daisy chain.
	uint16_t count = 0;
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		++count;

	// Pulse the CS line for a duration of T_READY, then wait for T_READY * N.
	spiSelect (bottom->config->spiDriver);
	chThdSleepMicroseconds (T_READY_MAX);
	spiUnselect (bottom->config->spiDriver);
	chThdSleepMicroseconds (T_READY_MAX * count);
}

bool ltc681xWriteConfig (ltc681x_t* bottom)
{
	// Write the configuration register group A
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// GPIO set to high impedence, reference enabled outside conversion, ADC option 0.
		device->tx [0] = CFGRA0 (1, 1, 1, 1, 1, 1, 0);

		// Undervoltage / overvoltage thresholds. We don't use them, so no need to set.
		device->tx [1] = CFGRA1 (0);
		device->tx [2] = CFGRA2 (0, 0);
		device->tx [3] = CFGRA3 (0);

		// Cells to discharge.
		device->tx [4] = CFGRA4 (
			device->cellsDischarging [7],
			device->cellsDischarging [6],
			device->cellsDischarging [5],
			device->cellsDischarging [4],
			device->cellsDischarging [3],
			device->cellsDischarging [2],
			device->cellsDischarging [1],
			device->cellsDischarging [0]);

		// Cells to discharge and discharge timeout.
		device->tx [5] = CFGRA5 (
			bottom->config->dischargeTimeout,
			device->cellsDischarging [11],
			device->cellsDischarging [10],
			device->cellsDischarging [9],
			device->cellsDischarging [8]);
	}
	bool result = ltc681xWriteRegisterGroups (bottom, COMMAND_WRCFGA);

	// Write the configuration register group B. Note on the LTC6811 this is ignored, as there is only configuration
	// register group A.
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Cells to discharge and GPIO set to high impedence.
		device->tx [0] = CFGRB0 (
			device->cellsDischarging [15],
			device->cellsDischarging [14],
			device->cellsDischarging [13],
			device->cellsDischarging [12],
			1,
			1,
			1,
			1);

		// Normal digital redundancy, no discharge timer monitor, GPIO 9 high impedence, cells to balance.
		device->tx [1] = CFGRB1 (0, 0b00, 0, 0,
			device->cellsDischarging [17],
			device->cellsDischarging [16]);

		// Reserved
		device->tx [2] = CFGRB2;
		device->tx [3] = CFGRB3;
		device->tx [4] = CFGRB4;
		device->tx [5] = CFGRB5;
	}
	result &= ltc681xWriteRegisterGroups (bottom, COMMAND_WRCFGB);

	return result;
}

bool ltc681xSampleStatus (ltc681x_t* bottom)
{
	// Start the ADC measurement for all values.
	if (!ltc681xWriteCommand (bottom, COMMAND_ADSTAT (bottom->config->statusAdcMode, 0b000), false))
		return false;

	// Block until the ADC conversion is complete
	if (!ltc681xPollAdc (bottom, STATUS_ADC_MODE_TIMEOUTS [bottom->config->statusAdcMode]))
		return false;

	// Read the status register group A. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDSTATA);

	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		// Read the sum of cell voltages.
		device->cellVoltageSum = STAR0_1_SC (device->rx [0], device->rx [1]);

		// Read the die temperature.
		device->dieTemperature = STAR2_3_ITMP (device->rx [2], device->rx [3]);
	}

	return true;
}