# STM32F405 Common Library
The STM32F405 common library is a library of common code used by Zips Racing's firmware. The main purpose of this is to prevent code duplication across projects, especially between different revisions of the same firmware. Even if a piece of software isn't currently used in multiple places, the potential that it may be in the future can be incentive enough to move it into here.

For documentation on specific sections of this library, see the header of the relevant source file.

For documentation of device specifications, see the [doc/datasheets](doc/datasheets) directory.

## Directory Structure
```
.
├── make                - Directory of Makefile includes.
│   ├── board.mk        - Include defining the board.h and board.c targets,
│   │                     used by ChibiOS.
│   ├── chibios.mk      - Include defining the application target and linking
│   │                     ChibiOS
│   ├── clangd.mk       - Include defining the compile_commands.json target for
│   │                     Clangd.
│   └── openocd.mk      - Include defining make targets for OpenOCD commands.
├── makefile            - Common makefile includes.
├── src                 - C source / include files.
│   ├── algorithm       - Code for generic algorithms & functions.
│   ├── can             - Code related to CAN-bus. Defines CAN nodes that can
│   │                     be included to interface with other devices on the
│   │                     vehicle's bus.
│   ├── controls        - Code related to control systems.
│   └── peripherals     - Code related to board hardware and peripherals.
│       │                 Specifically, this refers to electrical & mechanical
│       │                 hardware that is 'owned' by a board.
│       ├── adc         - Analog sensors and ADC-based hardware. These devices
│       │                 take an analog measurment and apply a transfer
│       │                 function to it.
│       ├── i2c         - I2C based device drivers. These devices may implement
│       │                 a variety of interfaces.
│       ├── interface   - Common interfaces a variety of devices may implement.
│       └── spi         - SPI based device drivers. These devices may implement
│                         a variety of interfaces.
└── stm32f405.svd       - SVD file for the STM32F405 microcontroller. Used for
                          the debugger.
```