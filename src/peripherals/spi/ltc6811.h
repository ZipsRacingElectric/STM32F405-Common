#ifndef LTC6811_H
#define LTC6811_H

// LTC6811-1 Driver -----------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.02.17
//
// Description: Device driver for the LTC6811-1 BMS ICs.
//
// Note: This code is derivative of the Analog Devices Linduino codebase:
//   https://github.com/analogdevicesinc/Linduino/tree/master.
//
// TODO(Barach): Remove extra cell voltage buffers?
// TODO(Barach): No need to wake chain every operation
// TODO(Barach): Reading 5x the data required.
// TODO(Barach): Combined GPIO and cell voltage command exists.
// TODO(Barach): Mux self test.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/interface/analog_sensor.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define LTC6811_CELL_COUNT		12
#define LTC6811_WIRE_COUNT		(LTC6811_CELL_COUNT + 1)
#define LTC6811_GPIO_COUNT		5
#define LTC6811_BUFFER_SIZE		8

// Datatypes ------------------------------------------------------------------------------------------------------------------

// TODO(Barach): This doesn't support ADCOPT = 1 modes.
typedef enum
{
	LTC6811_ADC_422HZ	= 0b00,
	LTC6811_ADC_27KHZ	= 0b01,
	LTC6811_ADC_7KHZ	= 0b10,
	LTC6811_ADC_26HZ	= 0b11,
} ltc6811AdcMode_t;

/// @brief The maximum amount of time a cell may be discharged without receiving an update.
typedef enum
{
	LTC6811_DISCHARGE_TIMEOUT_DISABLED	= 0x0,
	LTC6811_DISCHARGE_TIMEOUT_30_S		= 0x1,
	LTC6811_DISCHARGE_TIMEOUT_1_MIN		= 0x2,
	LTC6811_DISCHARGE_TIMEOUT_2_MIN		= 0x3,
	LTC6811_DISCHARGE_TIMEOUT_3_MIN		= 0x4,
	LTC6811_DISCHARGE_TIMEOUT_4_MIN		= 0x5,
	LTC6811_DISCHARGE_TIMEOUT_5_MIN		= 0x6,
	LTC6811_DISCHARGE_TIMEOUT_10_MIN	= 0x7,
	LTC6811_DISCHARGE_TIMEOUT_15_MIN	= 0x8,
	LTC6811_DISCHARGE_TIMEOUT_20_MIN	= 0x9,
	LTC6811_DISCHARGE_TIMEOUT_30_MIN	= 0xA,
	LTC6811_DISCHARGE_TIMEOUT_40_MIN	= 0xB,
	LTC6811_DISCHARGE_TIMEOUT_60_MIN	= 0xC,
	LTC6811_DISCHARGE_TIMEOUT_75_MIN	= 0xD,
	LTC6811_DISCHARGE_TIMEOUT_90_MIN	= 0xE,
	LTC6811_DISCHARGE_TIMEOUT_120_MIN	= 0xF
} ltc6811DischargeTimeout_t;

typedef enum
{
	/// @brief Indicates a hardware error has occurred. All other information about the device is void.
	LTC6811_STATE_FAILED = 0,

	/// @brief Indicates a packet with an incorrect PEC was received. All other information about the device is void.
	LTC6811_STATE_PEC_ERROR = 1,

	/// @brief Indicates the device's multiplexor self test failed. All ADC measurements are void.
	LTC6811_STATE_SELF_TEST_FAULT = 2,

	/// @brief Indicates the device is operating normally. Note that this does not mean cell voltages are valid or nominal,
	/// simply that they have been read correctly. For information about cell validities, see @c overvoltageFaults ,
	/// @c undervoltageFaults , and @c openWireFaults .
	LTC6811_STATE_READY = 3
} ltc6811State_t;

typedef struct
{
	/// @brief The SPI bus the daisy chain is connected to.
	SPIDriver* spiDriver;

	/// @brief The SPI configuration of the daisy chain.
	SPIConfig spiConfig;

	/// @brief The number of times to attempt a read operation before failing.
	uint16_t readAttemptCount;

	/// @brief The ADC conversion mode to use for measuring the cell voltages.
	ltc6811AdcMode_t cellAdcMode;

	/// @brief The ADC conversion mode to use for measuring the GPIO voltages.
	ltc6811AdcMode_t gpioAdcMode;

	/// @brief The ADC conversion mode to use for measuring the status values.
	ltc6811AdcMode_t statusAdcMode;

	/// @brief Indicates whether or not dischaging cell should be permitted.
	bool dischargeAllowed;

	/// @brief The maximum amount of time a cell may be discharged without receiving an update.
	ltc6811DischargeTimeout_t dischargeTimeout;

	/// @brief The number of pull-up / pull-down command iterations to perform during the open wire test. This value should be
	/// determined through testing, but cannot be less than 2. Recommended value of 4.
	uint8_t openWireTestIterations;

	/// @brief The amount of time an operation is allowed to run over its expected execution time by.
	sysinterval_t pollTolerance;

	/// @brief Multidimensional array of analog sensors to call upon sampling each device's GPIO. Must be size
	/// [ @c deviceCount ][ @c LTC6811_GPIO_COUNT ].
	analogSensor_t* gpioSensors [][LTC6811_GPIO_COUNT];
} ltc6811Config_t;

struct ltc6811
{
	// Doubly-linked list references.
	struct ltc6811* upperDevice;
	struct ltc6811* lowerDevice;
	struct ltc6811* topDevice;

	// Daisy chain configuration
	const ltc6811Config_t* config;
	bool dischargeAllowed;

	// Per-device configuration
	analogSensor_t* gpioSensors [LTC6811_GPIO_COUNT];

	// Device state
	ltc6811State_t state;

	// ADC measurements
	float cellVoltageSum;
	float cellVoltages [LTC6811_CELL_COUNT];
	float cellVoltagesPullup [LTC6811_CELL_COUNT];
	float cellVoltagesPulldown [LTC6811_CELL_COUNT];
	float cellVoltagesDelta [LTC6811_CELL_COUNT];
	float dieTemperature;
	uint16_t vref2;

	// Discharging
	bool cellsDischarging [LTC6811_CELL_COUNT];

	// Fault conditions
	bool openWireFaults [LTC6811_WIRE_COUNT];

	// Internal
	uint8_t tx [LTC6811_BUFFER_SIZE];
	uint8_t rx [LTC6811_BUFFER_SIZE];
};
typedef struct ltc6811 ltc6811_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes all devices in a daisy chain.
 * @param daisyChain Array of @c ltc6811_t* indicating the order of devices in the chain. The first element indicates the
 * bottom (first) device in the stack.
 * @param deviceCount The number of elements in @c daisyChain .
 * @param config The configuration of the chain.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean that all devices are
 * functional, simply that communication may be attempted.
 */
bool ltc6811Init (ltc6811_t* const* daisyChain, uint16_t deviceCount, const ltc6811Config_t* config);

/**
 * @brief Acquires and starts a daisy chain's SPI driver.
 * @param bottom The bottom (first) device in the chain.
 */
void ltc6811Start (ltc6811_t* bottom);

/**
 * @brief Stops and releases a chain's SPI driver.
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc6811Stop (ltc6811_t* bottom);

/**
 * @brief Wakes up all devices in an LTC6811 daisy chain. This method guarantees all devices are in the ready or standby state,
 * regardless of the previous state of the daisy chain.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc6811WakeupSleep (ltc6811_t* bottom);

/**
 * @brief Writes the configuration to each device in a daisy chain. The configuration includes @c cellVoltageMin ,
 * @c cellVoltageMax , @c dischargeTimeout, and the @c cellsDischarging arrays.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all writes were successful,
 * simply that they didn't all fail.
 */
bool ltc6811WriteConfig (ltc6811_t* bottom);

/**
 * @brief Samples the cell voltages of all devices in a daisy chain.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc6811SampleCells (ltc6811_t* bottom);

/**
 * @brief Samples the die temperature and sum of cells measurements.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc6811SampleStatus (ltc6811_t* bottom);

/**
 * @brief Samples the GPIO voltages of all devices in a daisy chain.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual sensor states to determine so.
 */
bool ltc6811SampleGpio (ltc6811_t* bottom);

/**
 * @brief Performs an open-wire test on all devices in a daisy chain.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc6811OpenWireTest (ltc6811_t* bottom);

/// @brief Sets all devices in a daisy chain to the ready state.
static inline void ltc6811ClearState (ltc6811_t* bottom)
{
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		device->state = LTC6811_STATE_READY;
}

/// @brief Checks whether any device in a daisy chain has an IsoSPI fault present.
static inline bool ltc6811IsospiFault (ltc6811_t* bottom)
{
	bool fault = false;
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		fault |= device->state == LTC6811_STATE_FAILED || device->state == LTC6811_STATE_PEC_ERROR;
	return fault;
}

/// @brief Checks whether any device in a daisy chain has a self-test fault present.
static inline bool ltc6811SelfTestFault (ltc6811_t* bottom)
{
	bool fault = false;
	for (ltc6811_t* device = bottom; device != NULL; device = device->upperDevice)
		fault |= device->state == LTC6811_STATE_SELF_TEST_FAULT;
	return fault;
}

#endif // LTC6811_H