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

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "ltc681x.h"

// Aliases --------------------------------------------------------------------------------------------------------------------

// General convenience aliases to replace ltc681x function names with ltc6811. All function behaviour is unchanged.

typedef ltc681x_t				ltc6811_t;
typedef ltc681xConfig_t			ltc6811Config_t;

#define ltc6811StartChain		ltc681xStartChain
#define ltc6811AppendChain		ltc681xAppendChain
#define ltc6811FinalizeChain	ltc681xFinalizeChain
#define ltc6811SetGpioSensor	ltc681xSetGpioSensor
#define ltc6811Start			ltc681xStart
#define ltc6811Stop				ltc681xStop
#define ltc6811WakeupSleep		ltc681xWakeupSleep
#define ltc6811WakeupIdle		ltc681xWakeupIdle
#define ltc6811WriteConfig		ltc681xWriteConfig
#define ltc6811SampleStatus		ltc681xSampleStatus
#define ltc6811ClearState		ltc681xClearState
#define ltc6811IsospiFault		ltc681xIsospiFault
#define ltc6811SelfTestFault	ltc681xSelfTestFault

// Constants ------------------------------------------------------------------------------------------------------------------

#define LTC6811_CELL_COUNT		12
#define LTC6811_WIRE_COUNT		(LTC6811_CELL_COUNT + 1)
#define LTC6811_GPIO_COUNT		5

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Samples the cell voltages of all devices in a daisy chain.
 * @note Must be called between @c ltc6811Start and @c ltc6811Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc6811SampleCells (ltc6811_t* bottom);

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

#endif // LTC6811_H