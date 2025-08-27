# Introdution to the I2C Communication Protocol - Zips Racing
I2C is a widely used serial communication protocol aimed at short-distance communication between integrated circuits. Because of its simple nature, I2C is a popular choice for expanding the functionality of microcontrollers. Some common I2C devices are EEPROMs, I/O expanders, DACs, etc.

For a basic introduction to the I2C protocol, see the video below:

[Rohde & Schwarz - Understanding I2C (https://www.youtube.com/watch?v=CAvawEcxoPU)](https://www.youtube.com/watch?v=CAvawEcxoPU)

## ChibiOS Interface
In ChibiOS, I2C is exposed through the I2C driver. The comprehensive driver documentation can be found at the below URL, however it is quite technical.

[common/doc/chibios/ChibiOS HAL Reference Manual.pdf](common/doc/chibios/ChibiOS&#32;HAL&#32;Reference&#32;Manual.pdf), page 250.

An excerpt of the most useful functionality is provided here.

### Configuration
Before a bus can be used, it must first be configured and initialized. The `I2CConfig` structure defines configuration options.
```
struct I2CConfig
{
	// Specifies the I2C mode (I2C or SMBus).
	// - OPMODE_I2C          - Configured for I2C.
	// - OPMODE_SMBUS_DEVICE - Configured for SMBus as a device.
	// - OPMODE_SMBUS_HOST   - Configured for SMBus as a host.
	i2copmode_t op_mode;

	// Specifies the clock frequency, in Hertz. Must be set to a value lower than 400kHz.
	uint32_t clock_speed;

	// Specifies the I2C fast mode duty cycle.
	// - STD_DUTY_CYCLE       - 1:1 duty cycle (standard).
	// - FAST_DUTY_CYCLE_2    - 2:1 duty cycle.
	// - FAST_DUTY_CYCLE_16_9 - 16:9 duty cycle.
	i2cdutycycle_t duty_cycle;
};
```

### Start
After defining a configuration a bus can be initialized using the `i2cStart` function. This function applies the configuration and activates the specified bus. No other operations can be used on a bus until this is performed.
```
msg_t i2cStart (I2CDriver* i2cp, const I2CConfig* config)
```
Parameters:
- `i2cp` - A pointer to the `I2CDriver` object to initialize.
- `config` - A pointer to the `I2CConfig` configuration to use.

Return Value:
- `MSG_OK` - The operation was successful.

### Stop
To deactivate a bus, the `i2cStop` function can be used. No other operations can be on a bus after this operation is performed (except starting the bus again). This function is not typically needed.
```
void i2cStop (I2CDriver* i2cp)
```
Parameters:
- `i2cp` - A pointer to the `I2CDriver` object to stop.

### Write & Read Operation
Once a bus is ready the `i2cMasterTransmitTimeout` function can be used to communicate with a device. This function performs a write operation immediately followed by a read operation (via repeated start).
```
msg_t i2cMasterTransmitTimeout (I2CDriver* i2cp, i2caddr_t addr,
	const uint8_t* txbuf, size_t txbytes,
	uint8_t* rxbuf, size_t rxbytes, sysinterval_t timeout)
```
Parameters:
- `i2cp`	A pointer to the I2CDriver object.
- `addr`	The 7-bit address of the slave device (without R/W bit).
- `txbuf`	A pointer to the buffer to transmit the contents of. Note the byte at index 0 is the first transmitted, followed by the byte at index 1, and so on.
- `txbytes`	The number of bytes to be transmitted.
- `rxbuf`	A pointer to the buffer to write received data into. Note the first received byte is placed at index 0, then the next at index 1, and so on.
- `rxbytes`	The number of bytes to be received, set it to 0 if you want transmit only.
- `timeout`	The number of ticks before the operation times out. Use ```TIME_INFINITE``` for no timeout.

Return Value:
- `MSG_OK`		- The operation was successful.
- `MSG_RESET`	- One or more I2C errors occurred.
- `MSG_TIMEOUT`	- A timeout occurred before the operation could end.

### Read Operation
If the device does not require writing, the `i2cMasterReceiveTimeout` function can be used to perform a single read operation.
```
msg_t i2cMasterReceiveTimeout (I2CDriver* i2cp, i2caddr_t addr,
	uint8_t* rxbuf, size_t rxbytes, sysinterval_t timeout)
```
Parameters:
- `i2cp`	A pointer to the I2CDriver object.
- `addr`	The 7-bit address of the slave device (without R/W bit).
- `rxbuf`	A pointer to the buffer to write received data into. Note the first received byte is placed at index 0, then the next at index 1, and so on.
- `rxbytes`	The number of bytes to be received, set it to 0 if you want transmit only.
- `timeout`	The number of ticks before the operation times out. Use ```TIME_INFINITE``` for no timeout.

Return Value:
- `MSG_OK`		- The operation was successful.
- `MSG_RESET`	- One or more I2C errors occurred.
- `MSG_TIMEOUT`	- A timeout occurred before the operation could end.

## Complete Example
This example shows how all the above functions can be combined. While it doesn't do anything meaningful, more useful applications should be able to be extrapolated from this.
```
// ChibiOS
#include "hal.h"

// C Standard Library
#include <stdint.h>

// Configuration for the I2C1 bus.
static const I2CConfig I2C1_CONFIG =
{
	// I2C mode (can be set for other protocols we aren't using).
	.op_mode = OPMODE_I2C,

	// 1Mbps clock speed (fastest speed all ICs are guaranteed to work with).
	.clock_speed = 100000,

	// Standard duty cycle (can be changed for higher speeds).
	.duty_cycle = STD_DUTY_CYCLE
};

// Entrypoint
int main (void)
{
	// ChibiOS Initialization
	halInit ();
	chSysInit ();

	// Initialize the I2C1 bus. (uses the above configuration).
	i2cStart (&I2CD1, &I2C1_CONFIG);

	// First transaction ------------------------------------------------------------------------------------------------------

	// Transmitting 4 bytes. First byte is 0x01, last byte is 0x67.
	uint8_t txbuf [4] = { 0x01, 0x23, 0x56, 0x67 };

	// Receiving 2 bytes, no need to initialize as the array will be overwritten.
	uint8_t rxbuf [2];

	// Using the I2C1 bus, 7-bit slave address 0x50, 100ms timeout.
	msg_t result = i2cMasterTransmitTimeout (&I2CD1, 0x50, txbuf, 4, rxbuf, 2, TIME_MS2I (100));

	if (result == MSG_OK)
	{
		// Success, do something with rxbuf in here.
	}

	// Second transaction -----------------------------------------------------------------------------------------------------

	// Only transmitting 1 byte, doesn't need to be an array.
	uint8_t registerAddress = 0xAB;

	// Transmit the register address then read 2 bytes.
	result = i2cMasterTransmitTimeout (&I2CD1, 0x50, &registerAddress, sizeof (registerAddress), rxbuf, 2, TIME_MS2I (100));

	if (result == MSG_OK)
	{
		// Success, combine the read values into a 16-bit integer.
		// Note that in this case, the values are little-endian.
		uint16_t value = rxbuf [0] | rxbuf [1] << 8;
	}

	// Third transaction ------------------------------------------------------------------------------------------------------

	// Perform a single byte read (no write operation beforehand).
	result = i2cMasterReceiveTimeout (&I2CD1, 0x50, rxbuf, 1, TIME_MS2I (100));

	if (result == MSG_OK)
	{
		// Success, do something with rxbuf [0] here.
	}

	// Stop the I2C1 bus. This doesn't have to be done, just added here to show how its used.
	i2cStop (&I2CD1);

	// Nothing else to do, infinite loop to keep the program alive.
	while (true)
		chThdSleepMilliseconds (500);
}
```