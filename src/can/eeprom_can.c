// Header
#include "eeprom_can.h"

// C Standard Library
#include <string.h>

// Constants / Macros ---------------------------------------------------------------------------------------------------------

#define WORD_TO_ADDRESS(word)	((word) & 0x7FFF)
#define WORD_TO_RW(word)		(((word) >> 15) & 0b1)

/// @brief The value of the RW bit for a read operation.
#define RW_READ 1

/// @brief The timeout for transmitting a response message.
#define RESPONSE_TIMEOUT TIME_MS2I (100)

// Functions ------------------------------------------------------------------------------------------------------------------

void eepromHandleCanCommand (CANRxFrame* command, CANDriver* driver, eeprom_t* eeprom)
{
	// Parse the command
	uint16_t address = WORD_TO_ADDRESS (command->data16 [0]);
	bool rw = WORD_TO_RW (command->data16 [0]);
	uint8_t count = command->DLC - 2;

	// Start the response, use same DLC, address, and R/W.
	CANTxFrame response =
	{
		.DLC	= command->DLC,
		.IDE	= CAN_IDE_STD,
		.SID	= command->SID + 1,
		.data16	=
		{
			command->data16 [0]
		}
	};

	bool result;
	if (rw == RW_READ)
	{
		// Read operation
		result = eeprom->readHandler (eeprom, address, response.data8 + 2, count);
	}
	else
	{
		// Write operation
		result = eeprom->writeHandler (eeprom, address, command->data8 + 2, count);
		memcpy (response.data8 + 2, command->data8 + 2, count);
	}

	// If the operation failed, send an invalid response.
	if (!result)
		response.data64 [0] = 0xFFFFFFFFFFFF;

	// Send the response
	canTransmitTimeout (driver, CAN_ANY_MAILBOX, &response, RESPONSE_TIMEOUT);
}