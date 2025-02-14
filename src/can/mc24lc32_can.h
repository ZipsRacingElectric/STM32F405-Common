#ifndef MC24LC32_CAN_H
#define MC24LC32_CAN_H

// MC24LC32 CAN Functions -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.23
//
// Description: Group of functions for sending / receiving EEPROM-related CAN messages.
//
// TODO(Barach): Shorten this handshake.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/mc24lc32.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Addresses above 4096 may be mapped to readonly variables. This callback function is used to request the value of a
 * variable based on the given address.
 * @param addr The address of the variable to get.
 * @param data Should be written to point to the variable.
 * @param dataCount Should be written to contain the size of the variable.
 * @return True if the address is valid, false otherwise.
 */
typedef bool mc24lc32ReadonlyCallback_t (uint16_t addr, void** data, uint8_t* dataCount);

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Handles an EEPROM command message. Depending on the instruction of the command, the appropriate read / write is
 * executed and, if applicable, a response message is sent.
 * @param frame The received CAN frame of the command message.
 * @param driver The CAN driver to transmit the response on.
 * @param eeprom The EEPROM to read from / write to.
 * @param readonlyCallback Hook for accessing readonly variable data. Use @c NULL to disable readonly addresses.
 */
void mc24lc32HandleCanCommand (CANRxFrame* frame, CANDriver* driver, mc24lc32_t* eeprom,
	mc24lc32ReadonlyCallback_t readonlyCallback);

#endif // MC24LC32_CAN_H