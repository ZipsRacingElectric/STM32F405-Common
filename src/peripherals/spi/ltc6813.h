#ifndef LTC6813_H
#define LTC6813_H

// LTC6813-1 Driver -----------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.01.26
//
// Description: Device driver for the LTC6813-1 BMS ICs.
//
// Note: This code is derivative of the Analog Devices Linduino codebase:
//   https://github.com/analogdevicesinc/Linduino/tree/master.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "ltc681x.h"

// Aliases --------------------------------------------------------------------------------------------------------------------

// General convenience aliases to replace ltc681x function names with ltc6813. All function behaviour is unchanged.

typedef ltc681x_t				ltc6813_t;
typedef ltc681xConfig_t			ltc6813Config_t;

#define ltc6813StartChain		ltc681xStartChain
#define ltc6813AppendChain		ltc681xAppendChain
#define ltc6813FinalizeChain	ltc681xFinalizeChain
#define ltc6813SetGpioSensor	ltc681xSetGpioSensor
#define ltc6813Start			ltc681xStart
#define ltc6813Stop				ltc681xStop
#define ltc6813WakeupSleep		ltc681xWakeupSleep
#define ltc6813WakeupIdle		ltc681xWakeupIdle
#define ltc6813WriteConfig		ltc681xWriteConfig
#define ltc6813SampleStatus		ltc681xSampleStatus
#define ltc6813ClearState		ltc681xClearState
#define ltc6813IsospiFault		ltc681xIsospiFault
#define ltc6813SelfTestFault	ltc681xSelfTestFault

// Constants ------------------------------------------------------------------------------------------------------------------

#define LTC6813_CELL_COUNT		18
#define LTC6813_WIRE_COUNT		(LTC6813_CELL_COUNT + 1)
#define LTC6813_GPIO_COUNT		5 // TODO(Barach): 9

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Samples the cell voltages of all devices in a daisy chain.
 * @note Must be called between @c ltc6813Start and @c ltc6813Stop .
 * @param bottom The bottom (first) device in the stack.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
 * check individual device states to determine so.
 */
bool ltc6813SampleCells (ltc6813_t* bottom);

// /**
//  * @brief Samples the GPIO voltages of all devices in a daisy chain.
//  * @note Must be called between @c ltc6813Start and @c ltc6813Stop .
//  * @param bottom The bottom (first) device in the stack.
//  * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
//  * check individual sensor states to determine so.
//  */
// bool ltc6813SampleGpio (ltc6813_t* bottom);

// /**
//  * @brief Performs an open-wire test on all devices in a daisy chain.
//  * @note Must be called between @c ltc6813Start and @c ltc6813Stop .
//  * @param bottom The bottom (first) device in the stack.
//  * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not mean all measurements are valid,
//  * check individual device states to determine so.
//  */
// bool ltc6813OpenWireTest (ltc6813_t* bottom);

#endif // LTC6813_H