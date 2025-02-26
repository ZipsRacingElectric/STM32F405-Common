#ifndef EEPROM_CAN
#define EEPROM_CAN

// EEPROM CAN Functions -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.23
//
// Description: Function for exposing an EEPROM's memory to a CAN bus.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/eeprom.h"

// ChibiOS
#include "hal.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Handles a CAN message containing an EEPROM command.
 * @param command The CAN frame of the command.
 * @param driver The CAN driver to send the response message on.
 * @param eeprom The EEPROM to read/write the data to/from.
 */
void eepromHandleCanCommand (CANRxFrame* command, CANDriver* driver, eeprom_t* eeprom);

#endif // EEPROM_CAN