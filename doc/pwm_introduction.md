# PWM Introduction - Zips Racing

## ChibiOS Interface

In ChibiOS pulse-width modulation (or PWM) can be implemented via the PWM driver.

The PWM driver is a single timer, but it may control multiple outputs (called channels). On the STM32F405, each PWM driver has 4 output channels. All channels controlled by a PWM driver have a single PWM frequency (due to sharing a timer), but may have independent duty cycles.

A PWM driver consists of a single timer, a single counter, and **N** comparison registers (1 per channel). On every tick of the timer, the counter is incremented by 1. Once the timer value of the timer reaches a certain value (called the PWM period), it resets to 0 and starts counting again.

The output of each PWM channel is determined by whether the value in its register (called the width) is greater than or less than the value of the counter. If the value is less than that of the counter, the channel's output is active, otherwise the channel's output is inactive. This effectively means the duty cycle of the channel can be set by setting the width of the channel.

```
dutyCycle = width / period
```

or

```
width = dutyCycle * period
```

Note that, as both the channel's width and the PWM period are integers, this implies the resolution of the duty cycle (how close you can get to a desired value) is dependent on the value of the PWM period. This is important to keep in mind when selecting a value of the PWM period.

### Starting the PWM Driver

Like almost all drivers in ChibiOS, in order to use the PWM driver it must first be configured.

The `pwmStart` function is used to configure and start a specific PWM driver. The `config` structure determines how the driver will behave. See *Configuration Structure* for how this is defined.
```
msg_t pwmStart(PWMDriver* pwmp, const PWMConfig* config);
```

### Configuration Structure

The important parameters of a PWM controller are:
- Clock Frequency: The frequency of the clock implementing the controller. This clock acts like a counter, increasing **M** times per second.
- PWM Period: The value at which the clock's counter is reset. This determines how long the total PWM wave is (both low time and high time).

The PWM period can be calculated from the Clock Frequency and the PWM Period:

```
pwmFrequency = frequency / period
```

Where:
- `pwmFrequency` is the PWM frequency.
- `frequency` is the clock frequency.
- `period` is the PWM period.

The PWM driver configuration structure is defined as follows:
```
typedef struct
{

	// Timer clock in Hz. Note this must be an integer divisor of the APB1
	// clock speed (typically configured as 42MHz).
	uint32_t frequency;

	// The total length of the PWM period, in timer ticks.
	pwmcnt_t period;

	// Callback invoked on reset of the PWM counter. Set to NULL to disable.
	pwmcallback_t callback;

	// Channel-specific configurations. On the STM32F405, this array has 4
	// elements. See below for structure definition.
	PWMChannelConfig channels [PWM_CHANNELS];

} PWMConfig;
```

The configuration for each channel is defined as follows:
```
typedef struct {

	// Channel output logic level. Can be either:
	// - PWM_OUTPUT_ACTIVE_HIGH => High output when active (common).
	// - PWM_OUTPUT_ACTIVE_LOW => Low output when active.
	pwmmode_t mode;

	// Callback invoked when the channel switches output state. Set to NULL to
	// disable.
	pwmcallback_t callback;

} PWMChannelConfig;
```

### Enabling / Writing To a Channel

The `pwmEnableChannel` function is used to enable a channel (if not already) and write a specific duty cycle to said channel.

```
void pwmEnableChannel(PWMDriver *pwmp, pwmchannel_t channel, pwmcnt_t width);
```

Parameters:
- `pwmp`    - Pointer to the PWM driver to use.
- `channel` - The index of the channel to modify in range [0, PWM_CHANNELS - 1]
- `width`   - The width of the high cycle, in clock tick.

## Examples

File `main.c`
```
// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "ch.h"
#include "hal.h"

// Configuration --------------------------------------------------------------------------------------------------------------

static const PWMConfig PWM_3_CONFIG =
{
	// Clock frequency of 80kHz and PWM period of 255. This gives us a PWM frequency of ~313 Hz and 256 possible output
	// values.
	.frequency = 80000,
	.period = 255,

	// No callbacks.
	.callback = NULL,

	// Per channel configuration
	.channels =
	{
		{
			// Channel 0 is unused
			.mode = PWM_OUTPUT_DISABLED,
			.callback = NULL
		},
		{
			// Channel 1 is unused
			.mode = PWM_OUTPUT_DISABLED,
			.callback = NULL
		},
		{
			// Channel 2 used. In this example we are using active low output, meaning if our duty cycle is 75%, the signal is
			// low for 75% of the cycle.
			.mode = PWM_OUTPUT_ACTIVE_LOW,
			.callback = NULL
		},
		{
			// Channel 3 is unused
			.mode = PWM_OUTPUT_DISABLED,
			.callback = NULL
		}
	}
};

// Entrypoint -----------------------------------------------------------------------------------------------------------------

int main (void)
{
	// ChibiOS Initialization
	halInit ();
	chSysInit ();

	// Debug Initialization
	ioline_t ledLine = LINE_LED_HEARTBEAT;
	debugHeartbeatStart (&ledLine, LOWPRIO);
	debugSerialInit (&SD1, NULL);

	// Start the PWM driver
	pwmStart (&PWMD3, &PWM_3_CONFIG);

	// Main loop. i is the value we write to the PWM channel
	uint8_t i = 0;
	while (true)
	{
		// Write i to channel 2.
		pwmEnableChannel (&PWMD3, 2, i);
		++i;

		// Print for debugging
		debugPrintf ("i = %u\r\n", i);

		// Wait 100ms
		chThdSleepMilliseconds (100);
	}

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}

void hardFaultCallback (void)
{
	// We don't care about faults in this example.
}
```