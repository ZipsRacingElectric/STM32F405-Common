# The Zips Racing Firmware Developement Reference Manual
Author: Cole Barach

Date Created: 2025.07.10

Date Updated: 2025.07.11

## Introduction
Most PCBs made by Zips Racing feature a microcontroller running custom firmware. As these PCBs are custom designs, they must use custom firmware.

### What This Manual Covers
- The development, maintenance, and usage of said firmware.
- General programming guidelines and standards.

### What This Manual Does Not Cover
- Application-specific firmware - These have their own documentation in their respective repositories.
- The STMF405 Common Library - This has its own documentation [../readme.md](../readme.md).
- Specific details of ChibiOS - This has its own reference manual [chibios/ChibiOS HAL Reference Manual.pdf](chibios/ChibiOS%20HAL%20Reference%20Manual.pdf).
- Specific details of the STM32F405 - This has its own hardware datasheet [datasheets/STM32F405RG Datasheet.pdf](datasheets/STM32F405RG%20Datasheet.pdf) and software reference manual [datasheets/STM32F405 Reference Manual.pdf](datasheets/STM32F405%20Reference%20Manual.pdf)

### Terminology
- Developer - The person writing the software in question. In OOP, typically an object is designed and developed by a single developer. This is opposed to other developers using software.
- User - The developers (real or potential, present or future) that may use a piece of software. In OOP, this refers to anyone instancing, using, configuring an object. This is opposed to the original developer of the software.
- End User - Developers or non-developers mean to interact with a piece of software. This may be anyone expected in interact with the vehicle or the charger.
- Module - The combination of a header file (```.h```), source file (```.c```), and optionally, makefile (```.mk```), that define a portion of software. While not a formal definition, this is the least ambiguous way of referring to such.

### Acronyms
- PCB - Printed circuit board
- OOP - Object-Oriented Programming
- HAL - Hardware abstraction layer
- RTOS - Real-time operating system
- IC - Integrated circuit
- GPIO - General purpose input / output
- ADC - Analog to digital converter
- DAC - Digital to analog converter
- I2C - Inter-integrated circuit
- CAN - Controller area network

## Object-Oriented Programming Paradigm
To write modular, reliable, and maintainable software, the object-oriented programming (OOP) paradigm is used by both ChibiOS and Zips Racing's firmware. As all the firmware is written in C, objects are represented as the collection of structures, functions, and macros that relate to it. For the sake of simplicity, 1 object is typically associated with a single module.

### Structures
A ```struct``` is a collection of primative datatypes. All information related to a specific object should be grouped into a struct to simplify syntax, reduce code duplication, and allow multiple instantiations.

It is generally poor practice to expect the user of an object to directly manipulate its members. The reasoning for this is as follows:
- It may not be obvious what the user is meant to modify or how to modify it.
- If the user is not aware of the implementation, they may introduce inconsistencies within the object.
- Certain members may not be intended to be modified by the user. C does not offer compile-time protection as C++ does.

There are certainly exceptions to this rule. It may be necessary to have the user directly operate on the object's members to avoid buffering and unnecessary operations. When such exceptions arise, it should be clearly documented what members are meant to be modified and what the restrictions are.

Reading directly from a structure's members is more acceptable. The alternative would be to create an accessor function for every individual member, which is not only impractical, but may have negative performance implications.

### Functions
A structure only defines the data associated with an object. To manipulate said data, functions are required. Because of the reasons outlined above, it is best to use functions for modifying the data of an object and direct access for reading data from an object. For reading data that is not immediately available, (ex. something that is extrapolated from the object's data), functions may be ideal.

Because C does not support namespaces, all externally linked function names must be unique. To prevent naming collisions, every function name should be prefixed by the name of the object it is intended to operate on. As these functions are not explict members of their associated object, they must be provided with a reference to the object they must operate on. This should always be done in the form of a pointer, as it prevents unnecessary copy operations. If the function does not need to modify the state of the object, it should use a ```const``` pointer to indicate such.

An example of an object's member functions might look like the following. The object is of the ```exampleObject_t``` type. It has an ```enum``` associated with it called ```exampleObjectState_t```. As the state of the object is not directly stored within it, but rather inferred from its data members, an accessor function must be used to read its state. Regardless of whether or not the state is stored within the object, a mutator function is used to prevent the user from assigning an invalid state. Both the ```enum``` and the function are related to the ```exampleObject_t``` so their names are prefixed as to indicate such.

```exampleObjectState_t exampleObjectGetState (const exampleObject_t* obj);```

```void exampleObjectSetState (exampleObject_t* obj, exampleObjectState_t state);```

### Initialization and Configuration
Because it is seen as bad practice to have the user directly modify object data, the question of how said object is initialized must be raised. As some objects have initializations that may fail, initializations dependent on other objects, or initialization that may need re-performed, a standalone function is the best fit.

To promote the reusability of objects, most data should be injected into the object during initialization, as opposed to being hard-coded into the object itself. This subset of the object's data is referred to as its configuration, and must be specified by the user.

Some examples of what to prefer in the configuration:
- References to other objects.
- Calibration values.
- Any parameters correlating to physical values.
- I2C addresses.
- Programmable CAN addresses (including all addresses belonging to Zips Racing's firmware).
- Any other types of external identifiers or addresses.
- Anything else that may need to differ between instances of an object.

Some examples of what to prefer in the object implementation
- Known constants common to all potential usages (datasheet parameters).
- Hard-coded CAN addresses.
- Protocol-specific details (Ex. bitmasks and bit-shift amounts).
- Boundary values used for validation.

Like with objects, configurations should implemented as a ```struct```. The key difference between a configuration and an object is that the user is responsible for initializing every member. It is the responsibility of the developer to document each member well enough to prevent misuse. The configuration should be named ```config```, prefixed by the object's name.

Example:
```
typedef struct
{
	/// @brief The 7-bit I2C address of the device.
	uint8_t addr;

	/// @brief The ideal minimum output value, in Volts.
	float minValue;

	/// @brief The ideal maximum output value, in Volts.
	float maxValue;
} exampleObjectConfig_t;
```

Like with other member functions, the initialization should be prefixed with the object's name. The name of the function itself should be ```init```, and it should accept a reference to the object, and, if applicable, a reference to the configuration.

```
/**
 * @brief Initializes an @c exampleObject_t .
 * @param obj The object to init.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool exampleObjectInit (exampleObject_t* obj, exampleObjectConfig_t* config);
```

### Polymorphism
<TODO(Barach)>

### Incomplete Software or Missing Features
<TODO(Barach)>

## The STM32F405 Microcontroller
The STM32F405 microcontrollers is the primary controller used by Zips Racing. Chosen for its power and versatility, this micro features a 32-bit ARM-based processor, 2 CAN peripherals, a 16-channel ADC, 3 I2C peripherals, and much more. With the vast number of peripherals this micro has, it is possible to use the same type of micro for all of Zips Racing's PCBs. This is ideal to minimize time spent developing toolchains and to increase software portability.

Due to its high number of peripherals, the micro must use the same physical pins for multiple peripherals. Due to this multiplexing, not all peripherals may be used at the same time. In the STM32F405 datasheet, a series of tables outline every pin and its possible configurations. Table 7 (page 47) outlines each pin (pin name) and its possible functionality. A pin may be configured as a GPIO (pin type I/O), as an ADC input or DAC output (under additional functions), or as a peripheral-specific pin (under alternate functions). If an alternate function is selected, the mapping is specified under Table 9 (page 62). For details on how to perform this configuration, see <TODO(Barach)>.

## ChibiOS
ChibiOS is a combination real-time operating system (RTOS) and hardware abstraction layer (HAL) utilized by all modern Zips Racing embedded systems.

### The Hardware Abstraction Layer (HAL)
ChibiOS's HAL provides a simplified and object-oriented interface for interacting with the micro's peripherals. Each peripheral is mapped to a 'driver' object, which encapsulates the peripheral's state and acts as a handler for performing operations.

### The Real-Time Operating System (RTOS)
The RTOS provides objects and functions for writing multi-threaded code that can execute with precise timing.