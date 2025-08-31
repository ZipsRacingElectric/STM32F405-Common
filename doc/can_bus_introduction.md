# Introdution to the CAN-Bus Communication Protocol - Zips Racing
CAN-bus (or more specifically, CAN 2.0a) is a communication protocol used for connecting multiple control units spread over a large area. Due to its flexibility and reliability, CAN-Bus is a common choice for connecting the various subsystems of a vehicle. For an explanation of the general concepts of CAN-bus, see the below video. Note that parts about OBD2, J1939, NMEA, and other higher-level protocols aren't relevant in our applications.

[CSS-Electronics - CAN-Bus Explained - A Simple Intro (https://www.youtube.com/watch?v=oYps7vT708E)](https://www.youtube.com/watch?v=oYps7vT708E)

## ChibiOS Interface
In ChibiOS, CAN is exposed through the CAN driver. The comprehensive driver documentation can be found at the below URL, however it is quite technical.

[chibios/ChibiOS HAL Reference Manual.pdf](chibios/ChibiOS&#32;HAL&#32;Reference&#32;Manual.pdf), page 83.

An excerpt of the most useful functionality is provided here.

### Transmit Frame
A message (also called a frame) that the user intents to transmit is represented by the `CANTxFrame` struct.
```
struct CANTxFrame
{
	// The data length code of the message (number of used bytes in the
	// payload).
	uint8_t DLC:4;

	// The RTR (remote transmit request) bit of the frame.
	uint8_t RTR:1;

	// The IDE (extended identifier) bit of the frame. If set to CAN_IDE_STD,
	// the SID is used, if set to CAN_IDE_EXT, the EID is used. (See below for
	// what these mean).
	uint8_t IDE:1;

	union
	{
		// The 11-bit standard identifier of the message (only used if
		// IDE = CAN_IDE_STD).
		uint32_t SID:11;

		// The 29-bit extended identifier of the message (only used if
		// IDE = CAN_IDE_EXT).
		uint32_t EID:29;
	};

	// This union is for accessing the payload of the frame, all of the fields point to the same data. The reason different
	// datatypes may be used is for convenience.
	union
	{
		// The payload of the message, accessed as an array of 8 8-bit integers.
		uint8_t data8[8];

		// The payload of the message, accessed as an array of 4 16-bit integers.
		uint16_t data16[4];

		// The payload of the message, accessed as an array of 2 32-bit integers.
		uint32_t data32[2];

		// The payload of the message, accessed as an array of 1 64-bit integer.
		uint64_t data64[1];
	};
}
```

### Transmitting Messages
In order to broadcast (also called transmit) a message, the `canTransmitTimeout` function should be used. The message to be transmitted is a user-created instance of the `CANTxFrame` struct.
```
msg_t canTransmitTimeout(CANDriver* canp, canmbx_t mailbox, const CANTxFrame* ctfp, sysinterval_t timeout)
```
Parameters:
- `canp`	- A pointer to the `CANDriver` object.
- `mailbox`	- The number of the mailbox(s) to use. Use `CAN_ANY_MAILBOX` for the first available mailbox.
- `ctfp`	- A pointer to the `CANTxFrame` to be transmitted.
- `timeout`	- The number of ticks before the operation times out. The following special values are allowed:
	- `TIME_IMMEDIATE` - Timeout immediately.
	- `TIME_INFINITE` - No timeout.

Return Value:
- `MSG_OK`		- The frame was transmitted successfully.
- `MSG_TIMEOUT`	- The operation has timed out.
- `MSG_RESET`	- The driver has been stopped while waiting.

### Receive Frame
A message (also called a frame) that is received by a device is represented by the `CANRxFrame` struct.
```
struct CANRxFrame
{
	// The data length code of the received message (number of bytes written
	// into the payload).
	uint8_t DLC:4;

	// The RTR (remote transmit request) bit of the frame.
	uint8_t RTR:1;

	// The IDE (extended identifier) bit of the frame. If set to CAN_IDE_STD,
	// the SID is used, if set to CAN_IDE_EXT, the EID is used. (See below for
	// what these mean).
	uint8_t IDE:1;

	union
	{
		// The 11-bit standard identifier of the message (only used if
		// IDE = CAN_IDE_STD).
		uint32_t SID:11;

		// The 29-bit extended identifier of the message (only used if
		// IDE = CAN_IDE_EXT).
		uint32_t EID:29;
	};

	// This union is for accessing the payload of the frame, all of the fields point to the same data. The reason different
	// datatypes may be used is for convenience.
	union
	{
		// The payload of the message, accessed as an array of 8 8-bit integers.
		uint8_t data8[8];

		// The payload of the message, accessed as an array of 4 16-bit integers.
		uint16_t data16[4];

		// The payload of the message, accessed as an array of 2 32-bit integers.
		uint32_t data32[2];

		// The payload of the message, accessed as an array of 1 64-bit integer.
		uint64_t data64[1];
	};
};
```

### Receiving Messages
In order to receive a message, the `canReceiveTimeout` function should be used. This function doesn't allow the user to specify *what* message is received, it will simply select the first message received, regardless of its identifier. To check what message was received, the contents of the `CANRxFrame` can be checked.
```
msg_t canReceiveTimeout(CANDriver* canp, canmbx_t mailbox, CANRxFrame* crfp, sysinterval_t timeout)
```
Parameters:
- `canp`	- A pointer to the `CANDriver` object.
- `mailbox`	- The number of the mailbox(s) to use. Use `CAN_ANY_MAILBOX` for the first available mailbox.
- `crfp`	- A pointer to the buffer where the `CANRxFrame` is to be copied.
- `timeout`	- The number of ticks before the operation times out. The following special values are allowed:
	- `TIME_IMMEDIATE`	- Timeout immediately, this is useful in an event driven scenario where a thread never blocks for I/O.
    - `TIME_INFINITE`	- No timeout.

Return Value:
- `MSG_OK`		- A frame has been received and placed in the buffer.
- `MSG_TIMEOUT`	- The operation has timed out.
- `MSG_RESET`	- The driver has been stopped while waiting.

# Examples
## Receiving Messages
File `main.c`:
```
#include "ch.h"
#include "hal.h"

// Configuration of the CAN 1 peripheral. Don’t worry too much
// about the details of this, its a bit out-of-scope for this
// project.

static const CANConfig CAN_DRIVER_CONFIG =
{
	.mcr = CAN_MCR_ABOM |
		CAN_MCR_AWUM |
		CAN_MCR_TXFP,
	.btr = CAN_BTR_SJW (0) |
		CAN_BTR_TS2 (1) |
		CAN_BTR_TS1 (10) |
		CAN_BTR_BRP (2)
};

int main (void)
{
	// ChibiOS initialization.
	halInit ();
	chSysInit ();

	// CAN 1 driver initialization.
	canStart (&CAND1, &CAN_DRIVER_CONFIG);

	// All of ZR's boards have a pin (named CAN1_STBY) for enabling the CAN transceiver.
	palClearLine (LINE_CAN1_STBY);

	// Buffer to put received frames in.
	CANRxFrame frame;

	// Loop for receiving CAN messages.
	while (true)
	{

		// Block execution until a frame is received.
		msg_t result = canReceiveTimeout (&CAND1, CAN_ANY_MAILBOX, &frame,
			TIME_INFINITE);

		// If we didn’t get a frame, try again.
		if (result != MSG_OK)
			continue;

		// Check the SID to determine what frame was received.
		if (frame.SID == 0x500)
		{
			// This signal is a 16-bit unsigned integer, transmitted in little-endian.
			// The STM's native format is little-endian, so we can read the 2 bytes directly into our field.
			// Note: data16 [0] refers to bytes 0 & 1.
			uint16_t littleEndianSignal = frame.data16 [0];

			// This signal is also 16-bit unsigned integer, however it is transmitted in big-endian.
			// Here we have to reverse the data, the __REV16 instruction reverses the endianness
			// of a 16-bit number, which is exactly what we need.
			// Note: data16 [1] refers to bytes 2 & 3.
			uint16_t bigEndianSignal0 = __REV16 (frame.data16 [1]);

			// This signal is a 32-bit unsigned integer, transmitted in big-endian.
			// We have to reverse the endianess here too, however this is a 32-bit number so we have to use the __REV
			// instruction, which operates on 32-bit numbers.
			// Note: data32 [1] refers to bytes 4, 5, 6, & 7.
			uint32_t bigEndianSignal1 = __REV (frame.data32 [1]);

			// Note that all other signals in this example are little-endian, for convenience.
		}

		if (frame.SID == 0x501)
		{
			// Since this is little-endian, we can just copy directly from the buffer as we did with the first message.
			// Note: data32 [0] refers to bytes 0, 1, 2, & 3.
			uint32_t longSignal = frame.data32 [0];

			// This signal is a 16-bit signed integer.
			// For a signed integer, we take the data (which is interpreted as a uint16_t) and cast it into a int16_t.
			// Note: data16 [2] refers to bytes 4 & 5.
			int16_t signedSignal = (int16_t) frame.data16 [2];
		}

		if (frame.SID == 0x502)
		{
			// If the node has a scale factor and offset, we can convert it into a float.
			// This is using a scale factor of 0.1 / LSB and offset of 100.
			// Here we are using a 16-bit signed integer, so we first cast the raw data into the correct type.
			// After casting, we apply our scale factor and offset.
			float floatSignal = ((int16_t) frame.data16 [0]) * 0.1f + 100.0f;

			// For signals that aren't multiples of 8-bit, we'll need to do bitwise operators. This one is a 3-bit unsigned
			// integer, so we use the bitwise AND operator to mask out the 3 bits we are interested in.
			uint8_t smallSignal = frame.data16 [1] & 0b111;

			// This is a 12-bit signal. For signals that don't start at bit 0, we'll need to use a shift operator. If we also
			// need to mask, we can do so after shifting.
			uint16_t mediumSignal = (frame.data16 [1] >> 3) & 0b111111111111;

			// For 1-bit signals, we can treat them as booleans. By masking out the single bit we are interested in and
			// checking its value, it is converted into a bool.
			bool boolSignal0 = (frame.data8 [4] & 0b1) == 0b1;

			// We can also shift before converting into a bool.
			bool boolSignal1 = ((frame.data8 [4] >> 1) & 0b1) == 0b1;
		}

		// We can do something with the data we received here...
	}
}

// We don't care about faults here.
void hardFaultCallback () {}
```