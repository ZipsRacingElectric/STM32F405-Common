#ifndef MC24LC32_CAN_H
#define MC24LC32_CAN_H

// MC24LC32 CAN Functions -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.23
//
// Description: Group of functions for sending / receiving EEPROM-related CAN messages.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/mc24lc32.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Handles an EEPROM command message. Depending on the instruction of the command, the appropriate read / write is
 * executed and, if applicable, a response message is sent.
 * @param frame The received CAN frame of the command message.
 * @param driver The CAN driver to transmit the response on.
 * @param eeprom The EEPROM to read from / write to.
 */
void mc24lc32HandleCanCommand (CANRxFrame* frame, CANDriver* driver, mc24lc32_t* eeprom);

#endif // MC24LC32_CAN_H