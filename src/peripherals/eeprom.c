// Header
#include "eeprom.h"

// Function Prototypes --------------------------------------------------------------------------------------------------------

bool virtualEepromWrite (void* object, uint16_t addr, const void* data, uint16_t dataCount);

bool virtualEepromRead (void* object, uint16_t addr, void* data, uint16_t dataCount);

// Functions ------------------------------------------------------------------------------------------------------------------

void virtualEepromInit (virtualEeprom_t* eeprom, const virtualEepromConfig_t* config)
{
	eeprom->writeHandler	= virtualEepromWrite;
	eeprom->readHandler		= virtualEepromRead;
	eeprom->config = config;
}

bool virtualEepromWrite (void* object, uint16_t addr, const void* data, uint16_t dataCount)
{
	// TODO(Barach): No boundary check.

	virtualEeprom_t* eeprom = (virtualEeprom_t*) object;

	// Traverse each EEPROM and check if the write address falls within its mapped memory.
	for (uint16_t index = 0; index < eeprom->config->count; ++index)
	{
		uint16_t addrMin = eeprom->config->addrs [index];
		uint16_t addrMax = addrMin + eeprom->config->sizes [index];

		if (addr >= addrMin && addr < addrMax)
			return eeprom->config->eeproms [index]->writeHandler (eeprom->config->eeproms [index], addr - addrMin, data, dataCount);
	}

	// Failed
	return false;
}

bool virtualEepromRead (void* object, uint16_t addr, void* data, uint16_t dataCount)
{
	// TODO(Barach): No boundary check.

	virtualEeprom_t* eeprom = (virtualEeprom_t*) object;

	// Traverse each EEPROM and check if the read address falls within its mapped memory.
	for (uint16_t index = 0; index < eeprom->config->count; ++index)
	{
		uint16_t addrMin = eeprom->config->addrs [index];
		uint16_t addrMax = addrMin + eeprom->config->sizes [index];

		if (addr >= addrMin && addr < addrMax)
			return eeprom->config->eeproms [index]->readHandler (eeprom->config->eeproms [index], addr - addrMin,
			data, dataCount);
	}

	// Failed
	return false;
}