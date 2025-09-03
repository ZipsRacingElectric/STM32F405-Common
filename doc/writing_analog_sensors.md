# Writing Analog Sensors - Zips Racing
The STM32F405 common library abstracts ChibiOS's ADC subsystem further with the Analog Sensor interface. The main purpose of an analog sensor is to implement  sensor-specific transfer functions.

An example of an analog sensor might be a rotary potentiometer. A rotary potentiometer measures an angle and converts it into a voltage output. An analog sensor implemented in the common library would do the inverse of this calculation, that is, figure out the angle of the potentiometer based on the voltage read by an ADC.

## Declaration
// TODO(Barach)