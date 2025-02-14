// Header
#include "mc24lc32_can.h"

// C Standard Library
#include <string.h>

// Message Packaging ----------------------------------------------------------------------------------------------------------

// Command Message
#define COMMAND_READ_NOT_WRITE(word)		(((word) & 0b00000001) == 0b00000001)
#define COMMAND_DATA_NOT_VALIDATION(word)	(((word) & 0b00000010) == 0b00000010)
#define COMMAND_IS_VALID(word)				(((word) & 0b00000100) == 0b00000100)
#define COMMAND_DATA_COUNT(word)			((((word) & 0b00001100) >> 2) + 1)

// Response Message
#define RESPONSE_READ_NOT_WRITE(rnw)		(((uint16_t) (rnw))	<< 0)
#define RESPONSE_DATA_NOT_VALIDATION(dnv)	(((uint16_t) (dnv))	<< 1)
#define RESPONSE_IS_VALID(iv)				(((uint16_t) (iv))	<< 2)
#define RESPONSE_DATA_COUNT(dc)				(((uint16_t) ((dc) - 1)) << 2)

// Timeouts -------------------------------------------------------------------------------------------------------------------

#define RESPONSE_TIMEOUT TIME_MS2I (100)

// Global Constants -----------------------------------------------------------------------------------------------------------

static const uint32_t INVALID_READ_DATA = 0xFFFFFFFF;

// Function Prototypes --------------------------------------------------------------------------------------------------------

msg_t transmitDataResponse (CANDriver* driver, sysinterval_t timeout, uint16_t id, uint16_t address, const void* data,
	uint8_t dataCount);

msg_t transmitValidationResponse (CANDriver* driver, sysinterval_t timeout, uint16_t id, bool isValid);

// Functions ------------------------------------------------------------------------------------------------------------------

void mc24lc32HandleCanCommand (CANRxFrame* frame, CANDriver* driver, mc24lc32_t* eeprom,
	mc24lc32ReadonlyCallback_t readonlyCallback)
{
	// EEPROM Command Message:
	//   Bytes 0 to 1: Instruction
	//     Bit 0: Read / Not Write
	//     Bit 1: Data / Not Validation
	//     Bit 2: (Validation only) Is Valid
	//     Bits 2 to 3: (Data only) Data Count - 1
	//   Bytes 2 to 3: (Data only) Address
	//   Bytes 4 to 7: (Data only) Data

	uint16_t responseId = frame->SID + 1;

	bool readNotWrite = COMMAND_READ_NOT_WRITE (frame->data16 [0]);
	bool dataNotValidation = COMMAND_DATA_NOT_VALIDATION (frame->data16 [0]);

	if (!dataNotValidation)
	{
		if (readNotWrite)
		{
			// Validation read
			bool isValid = mc24lc32IsValid (eeprom);
			transmitValidationResponse (driver, RESPONSE_TIMEOUT, responseId, isValid);
		}
		else
		{
			// Validation write
			bool isValid = COMMAND_IS_VALID (frame->data16 [0]);

			// Validate / invalidate the EEPROM for next boot.
			if (isValid)
				mc24lc32Validate (eeprom);
			else
				mc34lc32Invalidate (eeprom);

			mc24lc32Write (eeprom);
		}
	}
	else
	{
		uint8_t dataCount = COMMAND_DATA_COUNT (frame->data16 [0]);
		uint16_t address = frame->data16 [1];

		if (readNotWrite)
		{
			if (address >= MC24LC32_SIZE)
			{
				// Readonly variable read
				void* data = NULL;
				uint8_t dataCount;
				if (readonlyCallback != NULL && readonlyCallback (address, &data, &dataCount))
					transmitDataResponse (driver, RESPONSE_TIMEOUT, responseId, address, data, dataCount);
				else
					transmitDataResponse (driver, RESPONSE_TIMEOUT, responseId, address, &INVALID_READ_DATA, 4);
			}
			else
			{
				// Data read
				uint8_t* data = eeprom->cache + address;
				transmitDataResponse (driver, RESPONSE_TIMEOUT, responseId, address, data, dataCount);
			}

		}
		else
		{
			// Data write
			// Write the changes to the EEPROM.
			uint8_t* data = frame->data8 + 4;
			mc24lc32WriteThrough (eeprom, address, data, dataCount);
		}
	}
}

msg_t transmitDataResponse (CANDriver* driver, sysinterval_t timeout, uint16_t id, uint16_t address, const void* data, uint8_t dataCount)
{
	// EEPROM Command Message:
	//   Bytes 0 to 1: Instruction
	//     Bit 0: Read / Not Write
	//     Bit 1: Data / Not Validation
	//     Bit 2: (Validation only) Is Valid
	//     Bits 2 to 3: (Data only) Data Count - 1
	//   Bytes 2 to 3: (Data only) Address
	//   Bytes 4 to 7: (Data only) Data

	uint16_t instruction = RESPONSE_READ_NOT_WRITE (true)
		| RESPONSE_DATA_NOT_VALIDATION (true)
		| RESPONSE_DATA_COUNT (dataCount);

	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= id,
		.data16	=
		{
			instruction,
			address
		}
	};

	memcpy (frame.data8 + 4, data, dataCount);

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitValidationResponse (CANDriver* driver, sysinterval_t timeout, uint16_t id, bool isValid)
{
	// EEPROM Command Message:
	//   Bytes 0 to 1: Instruction
	//     Bit 0: Read / Not Write
	//     Bit 1: Data / Not Validation
	//     Bit 2: (Validation only) Is Valid
	//     Bits 2 to 3: (Data only) Data Count - 1
	//   Bytes 2 to 3: (Data only) Address
	//   Bytes 4 to 7: (Data only) Data

	uint16_t instruction = RESPONSE_READ_NOT_WRITE (true)
		| RESPONSE_DATA_NOT_VALIDATION (false)
		| RESPONSE_IS_VALID (isValid);

	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= id,
		.data16	=
		{
			instruction,
			0x0000,
			0x0000,
			0x0000
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}