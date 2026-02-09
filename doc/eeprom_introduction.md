# EEPROM Introduction - Zips Racing

An EEPROM (or Electronically-Erasable Read-Only Memory) is a type of non-volatile storage commonly used in embedded systems. Non-volatile, in the context of memory, refers to the fact that the memory is not subject to losing information when powered off. This is contrasted with volatile memory, which does not retain information when powered off.

The on-board memory of the STM32, what stores variables and temporary information, is volatile memory, meaning it cannot store anything permanently. To enable the STM32 with permanent storage, an EEPROM can be employed. In our applications, EEPROMs are typically used for storing sensor calibrations, control system tunings, and other values that may be changed "on-the-go".

## The MC24LC32 EEPROM

For simplicity, the majority of Zips Racing's PCBs that use EEPROMs use the same one, that is, the Microchip 24LC32 EEPROM.

To simplify its usage, a driver for this EEPROM has been written, found in the STM common library (common/src/peripherals/i2c/mc24lc32.h).

In order to use the EEPROM, it must be configured and initialized. An example of this is given below.

```
mc24lc32_t physicalEeprom;

/// @brief Configuration for the on-board EEPROM.
static const mc24lc32Config_t PHYSICAL_EEPROM_CONFIG =
{
	// The I2C address of the device. Almost always going to be 0x50.
	.addr = 0x50,

	// The I2C bus the device is on. Here we are using I2C bus 1.
	.i2c = &I2CD1,

	// The amount of time to allow for device polling. 500ms is pretty
	// generous.
	.timeout = TIME_MS2I (500),

	// The string used to validate the contents of the EEPROM. If this exact
	// string isn't read, we assume the EEPROM's contents aren't valid (device
	// enters the MC24LC32_STATE_INVALID state).
	.magicString = "EX_2026.02.09",

	// Callback for when the EEPROM's contents are modified. Here we are not
	// using this.
	.dirtyHook = NULL
};

void peripheralsInit (void)
{
	// Call the EEPROM's initialize function. If it fails AND the EEPROM is in
	// the MC24LC32_STATE_FAILED state, we know a hardware failure occurred.
	if (!mc24lc32Init (&physicalEeprom, &PHYSICAL_EEPROM_CONFIG)
		&& physicalEeprom.state == MC24LC32_STATE_FAILED)
	{
		// Hardware issue, do something here.
	}

	// Otherwise, we know the hardware works. You may still want to check if it
	// is in the MC24LC32_STATE_READY or MC24LC32_STATE_INVALID state.
}
```

### Read Operations

For reading the contents of the device, the driver exposes the `cache` field. This field is an array of bytes (`uint8_t`), matching the exact size of the EEPROM's memory. Upon initialization, the complete contents of the EEPROM are copied into this field.

Any value from the EEPROM can be directly accessed via the `cache` field, no operation or function call required. For convenience, this is typically accessed through a "map", discussed in the *Cache Mapping* section.

### Write Operations

For writing to the contents of the device, the `cache` field alone cannot be used. If one writes to the `cache`, the actual EEPROM contents are not affected, meaning the change is lost when the device powers down. To accomodate this, the `mc24lc32Write` function can be used. This function writes the requested change to both the `cache` field and the actual contents of the EEPROM, guaranteeing that both the change was made and the contents of the `cache` are still consistent with the actual device.

## Cache Mapping

Accessing an EEPROM's contents via an array of bytes is often inconvenient. Preferrably, we can store any arbitrary datatype in the EEPROM and access it directly as said datatype. This can be achieved by cache mapping.

Cache mapping is the process of defining a structure that "maps" the memory of the EEPROM to C datatypes. Functionally, this is just a C struct.

```
typedef struct
{
	// 16 bytes of padding. Must be here, as we are not allowed to use the
	// first 16 bytes of the EEPROM (reserved for the magic string).
	uint8_t pad0 [16]; // Address 0x0000

	// First usable value, located at EEPROM address 0x0010.
	uint8_t firstValue;

	// Second usable value, located at EEPROM address 0x0011 (1 byte after the
	// last).
	uint8_t secondValue;

	// 2 bytes of padding here due to C datatype alignment requirements.

	// Third value we use. Note that this is subject to C datatype alignment,
	// so it is located at address 0x0014 (not 0x0012 as you might expect).
	float thirdValue;

	// Fourth value we use. This is located at EEPROM address 0x0018 (4 bytes
	// after the last).
	uint8_t fourthValue;

	// 3 bytes of padding here due to C datatype alignment requirements.

	// Fifth value we use, located at EEPROM address 0x001C.
	float fifthValue;

	// Sixth value we use, located at EEPROM address 0x0020.
	uint16_t sixthValue;

	// Seventh value we use, located at EEPROM address 0x0022 (2 bytes after last).
	uint16_t seventhValue;
} eepromMap_t;
```

In order to use this struct, we can "trick" the C compiler into letting us interpret the `cache` field as if it were an instance of the `eepromMap_t` structure.

```
// Create a new pointer (of type eepromMap_t*) that points to the same address
// as the EEPROM cache field.
eepromMap_t* physicalEepromMap = (eepromMap_t*) physicalEeprom.cache;

// Now if we access an element of this struct, it will actually access the
// corresponding location in the EEPROM's cache field.
float a = physicalEepromMap->firstValue;
```

Note that this approach only works under the condition that strict aliasing is disabled (via the `-fno-strict-aliasing` GCC option). This option is specified by default in the STM common library.

## CAN Bus Access

Being able to read/write to the EEPROM from an application is convenient, however it is common to want to modify an EEPROM's contents via CAN bus. While one could implement a setter / getter CAN message for every desired field, that is inconvenient. A more general solution is provided by the `common/src/can/eeprom_can` module of the common library. The entire purpose of this module is to directly expose an EEPROM to the CAN bus for random access.

To use the module, only two things are required:

- Two unused, consecutive CAN IDs the application can use.
- A thread for handling received CAN messages.

The latter can be achieved via the `common/src/can/can_thread` module.

An example demonstrating the module's usage is provided below:

```
// The lower of the two consecutive IDs the application reserves. Here we use 0x700 and 0x701.
#define EEPROM_COMMAND_MESSAGE_ID 0x700

int8_t receiveMessage (void* config, CANRxFrame* frame)
{
	// Config is required for the can_thread module, here we just ignore it.
	(void) config;

	if (frame->SID == EEPROM_COMMAND_MESSAGE_ID)
	{
		// If the command message was received, call the handler.
		eepromHandleCanCommand (frame, config->driver, (eeprom_t*) &physicalEeprom);
		return 0;
	}

	return -1;
}
```

By using this module, the application will now correctly respond to EEPROM command CAN messages. Importantly, this allows the EEPROM to be used with the `can-eeprom-cli` application of the ZRE-CAN-Tools project.

### CAN EEPROM CLI Configuration

The `can-eeprom-cli` application requires a configuration file to work with an EEPROM. This file must provide the CAN ID of the EEPROM command (lower of the two consecutive CAN IDs), a name for the device, and the map of the EEPROM's contents. Care must be taken to guarentee the configuration's map exactly matches the EEPROM map defined in the firmware.

An example configuration that matches the example map and code snippet is provided below:

```
{
	"name":				"EXAMPLE",
	"canId":			"0x700",
	"variables":
	[
		{
			"address":	"0x0000",
			"name":		"EEPROM_STATUS",
			"type":		"uint8_t",
			"comment":	"The status of the device's EEPROM. 0 => Hardware failure, 1 => Memory invalid, 3 => Memory valid."
		},
		{
			"address":	"0x0010",
			"name":		"FIRST_VALUE",
			"type":		"uint8_t"
		},
		{
			"address":	"0x0011",
			"name":		"SECOND_VALUE",
			"type":		"uint8_t"
		},
		{
			"address":	"0x0014",
			"name":		"THIRD_VALUE",
			"type":		"float"
		},
		{
			"address":	"0x0018",
			"name":		"FOURTH_VALUE",
			"type":		"uint8_t"
		},
		{
			"address":	"0x001C",
			"name":		"FIFTH_VALUE",
			"type":		"float"
		},
		{
			"address":	"0x0020",
			"name":		"SIXTH_VALUE",
			"type":		"uint16_t"
		},
		{
			"address":	"0x0022",
			"name":		"SEVENTH_VALUE",
			"type":		"uint16_t"
		},
	]
}
```