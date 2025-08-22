# STM32F405 Common Library
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
│       ├── adc         - Analog sensors and ADC-based hardware.
│       ├── i2c         - I2C based device drivers.
│       ├── interface   - Common interfaces a variety of devices may implement.
│       └── spi         - SPI based device drivers.
└── stm32f405.svd       - SVD file for the STM32F405 microcontroller. Used for
                          the debugger.
```