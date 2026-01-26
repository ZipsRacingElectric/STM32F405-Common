#ifndef LTC681X_H
#define LTC681X_H

// LTC6811 / LTC6813 Common Driver --------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.01.26
//
// Description: Common code used by both the LTC6811 and LTC6813 device drivers.
//
// Note: This code is derivative of the Analog Devices Linduino codebase:
//   https://github.com/analogdevicesinc/Linduino/tree/master.
//
// TODO(Barach):
// - Mux self test?

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

// Maximum number of cells, wires, and GPIO used by any LTC driver.
#define LTC681X_CELL_COUNT		18
#define LTC681X_WIRE_COUNT		(LTC681X_CELL_COUNT + 1)
#define LTC681X_GPIO_COUNT		5		// TODO(Barach): 9

#define LTC681X_BUFFER_SIZE		8

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	LTC681X_ADC_422HZ	= 0b00,
	LTC681X_ADC_27KHZ	= 0b01,
	LTC681X_ADC_7KHZ	= 0b10,
	LTC681X_ADC_26HZ	= 0b11,
} ltc681xAdcMode_t;

/// @brief The maximum amount of time a cell may be discharged without receiving an update.
typedef enum
{
	LTC681X_DISCHARGE_TIMEOUT_DISABLED	= 0x0,
	LTC681X_DISCHARGE_TIMEOUT_30_S		= 0x1,
	LTC681X_DISCHARGE_TIMEOUT_1_MIN		= 0x2,
	LTC681X_DISCHARGE_TIMEOUT_2_MIN		= 0x3,
	LTC681X_DISCHARGE_TIMEOUT_3_MIN		= 0x4,
	LTC681X_DISCHARGE_TIMEOUT_4_MIN		= 0x5,
	LTC681X_DISCHARGE_TIMEOUT_5_MIN		= 0x6,
	LTC681X_DISCHARGE_TIMEOUT_10_MIN	= 0x7,
	LTC681X_DISCHARGE_TIMEOUT_15_MIN	= 0x8,
	LTC681X_DISCHARGE_TIMEOUT_20_MIN	= 0x9,
	LTC681X_DISCHARGE_TIMEOUT_30_MIN	= 0xA,
	LTC681X_DISCHARGE_TIMEOUT_40_MIN	= 0xB,
	LTC681X_DISCHARGE_TIMEOUT_60_MIN	= 0xC,
	LTC681X_DISCHARGE_TIMEOUT_75_MIN	= 0xD,
	LTC681X_DISCHARGE_TIMEOUT_90_MIN	= 0xE,
	LTC681X_DISCHARGE_TIMEOUT_120_MIN	= 0xF
} ltc681xDischargeTimeout_t;

typedef enum
{
	/// @brief Indicates a hardware error has occurred. All other information about the device is void.
	LTC681X_STATE_FAILED = 0,

	/// @brief Indicates a packet with an incorrect PEC was received. All other information about the device is void.
	LTC681X_STATE_PEC_ERROR = 1,

	/// @brief Indicates the device's multiplexor self test failed. All ADC measurements are void.
	LTC681X_STATE_SELF_TEST_FAULT = 2,

	/// @brief Indicates the device is operating normally. Note that this does not mean cell voltages are valid or nominal,
	/// simply that they have been read correctly. For information about cell validities, see @c overvoltageFaults ,
	/// @c undervoltageFaults , and @c openWireFaults .
	LTC681X_STATE_READY = 3
} ltc681xState_t;

typedef struct
{
	/// @brief The SPI bus the daisy chain is connected to.
	SPIDriver* spiDriver;

	/// @brief The SPI configuration of the daisy chain.
	SPIConfig spiConfig;

	/// @brief The number of times to attempt a read operation before failing.
	uint16_t readAttemptCount;

	/// @brief The ADC conversion mode to use for measuring the cell voltages.
	ltc681xAdcMode_t cellAdcMode;

	/// @brief The ADC conversion mode to use for measuring the GPIO voltages.
	ltc681xAdcMode_t gpioAdcMode;

	/// @brief The ADC conversion mode to use for measuring the status values.
	ltc681xAdcMode_t statusAdcMode;

	/// @brief Indicates whether or not dischaging cell should be permitted.
	bool dischargeAllowed;

	/// @brief The maximum amount of time a cell may be discharged without receiving an update.
	ltc681xDischargeTimeout_t dischargeTimeout;

	/// @brief The number of pull-up / pull-down command iterations to perform during the open wire test. This value should be
	/// determined through testing, but cannot be less than 2. Recommended value of 4.
	uint8_t openWireTestIterations;

	/// @brief The amount of time an operation is allowed to run over its expected execution time by.
	sysinterval_t pollTolerance;
} ltc681xConfig_t;

struct ltc681x
{
	// Doubly-linked list references.
	struct ltc681x* upperDevice;
	struct ltc681x* lowerDevice;

	// Bottom device configuration (uninitialized for all other devices).
	struct ltc681x* topDevice;
	const ltc681xConfig_t* config;
	uint16_t deviceCount;

	// Per-device configuration
	analogSensor_t* gpioSensors [LTC681X_GPIO_COUNT];

	// Device state
	ltc681xState_t state;

	// ADC measurements
	float cellVoltageSum;
	float cellVoltages [LTC681X_CELL_COUNT];
	float cellVoltagesPullup [LTC681X_CELL_COUNT];
	float cellVoltagesPulldown [LTC681X_CELL_COUNT];
	float cellVoltagesDelta [LTC681X_CELL_COUNT];
	float dieTemperature;
	uint16_t vref2;

	// Discharging
	bool cellsDischarging [LTC681X_CELL_COUNT];

	// Fault conditions
	bool openWireFaults [LTC681X_WIRE_COUNT];

	// Internal
	uint8_t tx [LTC681X_BUFFER_SIZE];
	uint8_t rx [LTC681X_BUFFER_SIZE];
};
typedef struct ltc681x ltc681x_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the bottom device of an LTC daisy chain.
 * @note This does not test the device communication, and therefore cannot fail. See @c ltc681xFinalizeChain to test the
 * device.
 * @param bottom The bottom (first) device in the stack.
 * @param config The configuration of the chain.
 */
void ltc681xStartChain (ltc681x_t* bottom, const ltc681xConfig_t* config);

/**
 * @brief Initializes and appends an LTC to the end of an LTC daisy chain.
 * @note This does not test the device communication, and therefore cannot fail. See @c ltc681xFinalizeChain to test the
 * device.
 * @param bottom The bottom (first) device in the stack.
 * @param ltc The LTC to initialize and append.
 */
void ltc681xAppendChain (ltc681x_t* bottom, ltc681x_t* ltc);

/**
 * @brief Finalizes modifications to an LTC daisy chain made by @c ltc681xStartChain and @c ltc681xAppendChain . This function
 * performs the final configuration and tests all device communications.
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean that all devices are
 * functional, simply that communication may be attempted.
 */
bool ltc681xFinalizeChain (ltc681x_t* bottom);

/**
 * @brief Links an analog sensor to the callback of an LTC's GPIO ADC.
 * @param ltc The LTC to link the sensor to.
 * @param index The index of the GPIO to link to.
 * @param sensor The analog sensor to link.
 */
void ltc681xSetGpioSensor (ltc681x_t* ltc, uint8_t index, analogSensor_t* sensor);

/**
 * @brief Acquires and starts a daisy chain's SPI driver.
 * @param bottom The bottom (first) device in the chain.
 */
void ltc681xStart (ltc681x_t* bottom);

/**
 * @brief Stops and releases a chain's SPI driver.
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc681xStop (ltc681x_t* bottom);

/**
 * @brief Wakes up all devices in an LTC681X daisy chain from the sleep state. This method guarantees all devices are in the
 * standby state, regardless of the previous state of the daisy chain. The LTC681X core enters the sleep state after 2 seconds
 * of inactivity.
 * @note Must be called between @c ltc681xStart and @c ltc681xStop .
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc681xWakeupSleep (ltc681x_t* bottom);

/**
 * @brief Wakes up all devices in an LTC681X daisy chain from the idle state. This method guarantees all devices are in the
 * ready state. The LTC681X IsoSPI port enters the idle state after 4.3 ms of inactivity.
 * @note Must be called between @c ltc681xStart and @c ltc681xStop .
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc681xWakeupIdle (ltc681x_t* bottom);

/**
 * @brief Writes the configuration to each device in a daisy chain. The configuration includes @c dischargeTimeout, and the
 * @c cellsDischarging arrays.
 * @note Must be called between @c ltc681xStart and @c ltc681xStop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all writes were successful,
 * simply that they didn't all fail.
 */
bool ltc681xWriteConfig (ltc681x_t* bottom);

/**
 * @brief Samples the die temperature and sum of cells measurements.
 * @note Must be called between @c ltc681xStart and @c ltc681xStop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc681xSampleStatus (ltc681x_t* bottom);

/// @brief Sets all devices in a daisy chain to the ready state.
static inline void ltc681xClearState (ltc681x_t* bottom)
{
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		device->state = LTC681X_STATE_READY;
}

/// @brief Checks whether any device in a daisy chain has an IsoSPI fault present.
static inline bool ltc681xIsospiFault (ltc681x_t* bottom)
{
	bool fault = false;
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		fault |= device->state == LTC681X_STATE_FAILED || device->state == LTC681X_STATE_PEC_ERROR;
	return fault;
}

/// @brief Checks whether any device in a daisy chain has a self-test fault present.
static inline bool ltc681xSelfTestFault (ltc681x_t* bottom)
{
	bool fault = false;
	for (ltc681x_t* device = bottom; device != NULL; device = device->upperDevice)
		fault |= device->state == LTC681X_STATE_SELF_TEST_FAULT;
	return fault;
}

#endif // LTC681X_H