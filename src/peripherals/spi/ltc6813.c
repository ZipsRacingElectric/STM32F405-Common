// Header
#include "ltc6813.h"

// Includes
#include "ltc681x_internal.h"

/**
 * @brief Samples the cell voltages of each device in a chain.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param destination The destination buffer to use for each device.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee read data is valid,
 * check individual device states to determine validity
 */
static bool sampleCells (ltc6813_t* bottom, cellVoltageDestination_t destination)
{
	// See LTC6813 datasheet section "Measuring Cell Voltages (ADCV Command)", pg.25.

	// Start the cell voltage conversion for all cells, not permitting discharge.
	if (!ltc681xWriteCommand (bottom, COMMAND_ADCV (bottom->config->cellAdcMode, false, 0b000), false))
		return false;

	if (!ltc681xPollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
		return false;

	// Read the first 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVA);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
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

	// Read the next 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVB);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
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

	// Read the next 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVC);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
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

	// Read the next 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVD);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
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

	// Read the next 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVE);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell12 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell13 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell14 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [12] = cell12;
			device->cellVoltages [13] = cell13;
			device->cellVoltages [14] = cell14;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [12] = cell12;
			device->cellVoltagesPullup [13] = cell13;
			device->cellVoltagesPullup [14] = cell14;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [12] = cell12;
			device->cellVoltagesPulldown [13] = cell13;
			device->cellVoltagesPulldown [14] = cell14;
			break;
		}
	}

	// Read the next 3 cell voltage. If this fails, we'd still like to try to read in case only part of the daisy chain is
	// failed.
	ltc681xReadRegisterGroups (bottom, COMMAND_RDCVF);

	for (ltc6813_t* device = bottom; device != NULL; device = device->upperDevice)
	{
		float cell15 = WORD_TO_CELL_VOLTAGE ((device->rx [1] << 8) | device->rx [0]);
		float cell16 = WORD_TO_CELL_VOLTAGE ((device->rx [3] << 8) | device->rx [2]);
		float cell17 = WORD_TO_CELL_VOLTAGE ((device->rx [5] << 8) | device->rx [4]);

		switch (destination)
		{
		case CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER:
			device->cellVoltages [15] = cell15;
			device->cellVoltages [16] = cell16;
			device->cellVoltages [17] = cell17;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER:
			device->cellVoltagesPullup [15] = cell15;
			device->cellVoltagesPullup [16] = cell16;
			device->cellVoltagesPullup [17] = cell17;
			break;
		case CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER:
			device->cellVoltagesPulldown [15] = cell15;
			device->cellVoltagesPulldown [16] = cell16;
			device->cellVoltagesPulldown [17] = cell17;
			break;
		}
	}

	return true;
}

bool ltc6813SampleCells (ltc6813_t* bottom)
{
	// Sample the cell voltages into the cell voltage buffer.
	return sampleCells (bottom, CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER);
}

// bool ltc6811SampleGpio (ltc6811_t* bottom)
// {
// 	// See LTC6811 datasheet section "Auxiliary (GPIO) Measurements (ADAX Command)", pg.26.

// 	// Start the ADC measurement for all GPIO.
// 	if (!ltc681xWriteCommand (bottom, COMMAND_ADAX (bottom->config->gpioAdcMode, 0b000), false))
// 	{
// 		ltc681xFailGpio (bottom);
// 		return false;
// 	}

// 	// Block until the ADC conversion is complete
// 	if (!ltc681xPollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->gpioAdcMode]))
// 	{
// 		ltc681xFailGpio (bottom);
// 		return false;
// 	}

// 	// Read the auxiliary register group B. If this fails, we'd still like to try to read in case only part of the daisy
// 	// chain is failed.
// 	if (!ltc681xReadRegisterGroups (bottom, COMMAND_RDAUXB))
// 		ltc681xFailGpio (bottom);

// 	// Read GPIO 4, GPIO 5, and VREF2
// 	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
// 	{
// 		// Store VREF2
// 		device->vref2 = device->rx [5] << 8 | device->rx [4];

// 		for (uint8_t gpioIndex = 3; gpioIndex < LTC6811_GPIO_COUNT; ++gpioIndex)
// 		{
// 			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
// 			if (sensor == NULL)
// 				continue;

// 			uint16_t sample = device->rx [gpioIndex * 2 - 5] << 8 | device->rx [gpioIndex * 2 - 6];

// 			// Update the sensor with the last sample, providing VREF2 as the analog supply voltage.
// 			analogSensorUpdate (sensor, sample, device->vref2);
// 		}
// 	}

// 	// Read the auxiliary register group A. If this fails, we'd still like to try to read in case only part of the daisy
// 	// chain is failed.
// 	if (!ltc681xReadRegisterGroups (bottom, COMMAND_RDAUXA))
// 		ltc681xFailGpio (bottom);

// 	// Read GPIO 1 to 3
// 	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
// 	{
// 		for (uint8_t gpioIndex = 0; gpioIndex < 3; ++gpioIndex)
// 		{
// 			analogSensor_t* sensor = device->gpioSensors [gpioIndex];
// 			if (sensor == NULL)
// 				continue;

// 			uint16_t sample = device->rx [gpioIndex * 2 + 1] << 8 | device->rx [gpioIndex * 2];

// 			// Update the sensor with the last sample, providing VREF2 as the analog supply voltage.
// 			analogSensorUpdate (sensor, sample, device->vref2);
// 		}
// 	}

// 	return true;
// }

// bool ltc6811OpenWireTest (ltc6811_t* bottom)
// {
// 	// See LTC6811 datasheet section "Open Wire Check (ADOW Command)", pg.34.

// 	// Pull-up measurement
// 	for (uint8_t index = 0; index < bottom->config->openWireTestIterations; ++index)
// 	{
// 		// Send the pull-up command.
// 		if (!ltc681xWriteCommand (bottom, COMMAND_ADOW (0b000, bottom->config->cellAdcMode, false, true), false))
// 			return false;

// 		// Block until complete.
// 		if (!ltc681xPollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
// 			return false;
// 	}

// 	// Sample the cell voltages into the pull-up buffer.
// 	if (!sampleCells (bottom, CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER))
// 		return false;

// 	// Pull-down measurement
// 	for (uint8_t index = 0; index < bottom->config->openWireTestIterations; ++index)
// 	{
// 		// Send the pull-down command.
// 		if (!ltc681xWriteCommand (bottom, COMMAND_ADOW (0b000, bottom->config->cellAdcMode, false, false), false))
// 			return false;

// 		// Block until complete.
// 		if (!ltc681xPollAdc (bottom, ADC_MODE_TIMEOUTS [bottom->config->cellAdcMode]))
// 			return false;
// 	}

// 	// Sample the cell voltages into the pull-down buffer.
// 	if (!sampleCells (bottom, CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER))
// 		return false;

// 	// Check each device, cell-by-cell
// 	// Note that in the datasheet, sense wires are indexed 0 to 12 while cells are indexed 1 to 12.
// 	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
// 	{
// 		// For wire 0, if cell 1 read 0V (1mV tolerance for noise) during pull-up, the wire is open.
// 		device->openWireFaults [0] = device->cellVoltagesPullup [0] < 0.001f && device->cellVoltagesPullup [0] > -0.001f;

// 		// For wire n in [1 to 11], if cell delta (n+1) < -400mV, the wire is open.
// 		for (uint8_t wire = 1; wire < LTC6811_CELL_COUNT - 1; ++wire)
// 		{
// 			device->cellVoltagesDelta [wire] = device->cellVoltagesPullup [wire] - device->cellVoltagesPulldown [wire];
// 			device->openWireFaults [wire] = device->cellVoltagesDelta [wire] < -0.4f;
// 		}

// 		// Note: The datasheet calls out 400mV, but testing shows it as 800mV.
// 		// For wire 11, if cell delta 12 < -800mV, the wire is open.
// 		device->cellVoltagesDelta [11] = device->cellVoltagesPullup [11] - device->cellVoltagesPulldown [11];
// 		device->openWireFaults [11] = device->cellVoltagesDelta [11] < -0.8f;

// 		// For wire 12, if cell 12 read 0V (1mV tolerance for noise) during pull-down, the wire is open.
// 		device->openWireFaults [12] = device->cellVoltagesPulldown [11] < 0.001f && device->cellVoltagesPulldown [11] > -0.001f;
// 	}

// 	return true;
// }