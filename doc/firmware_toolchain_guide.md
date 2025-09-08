# Firmware Toolchain Guide - Zips Racing
This guide details the basic setup and structure of an STM32F405 project.

## Dependencies
The toolchain is dependent on the following 3rd party software. The 'Setup' section details the installation of each of these packages.
- ChibiOS 21.11.3
- make
- arm-none-eabi-gcc
- openocd
- FMPP - FreeMark Pre-Processor

## Setup
- Clone this project's repository using GitHub's SSH URL ```git clone <SSH URL>```.
- Change directories into the project's newly created directory ```cd <Project directory name>```.
- Clone the project's ```common``` submodule using ```git submodule init``` followed by ```git submodule update```.
- Download the ChibiOS 21.11.3 source from https://github.com/ChibiOS/ChibiOS/releases/tag/ver21.11.3.
- Extract the archive's contents to a permanent location.
- Define the ```CHIBIOS_SOURCE_PATH``` environment variable to point to the location the ChibiOS source (Note: For compatibility with FMPP, this path should use the ```/``` separator rather than ```\```).

### For Windows
Some dependencies of this project are not natively built for Windows. A solution to this is to use MSYS2, a collection of tools and libraries that provide a POSIX-like development environment for Windows.

#### MSYS2
- Download and run the MSYS2 installer from [Github](https://github.com/msys2/msys2-installer/releases/).
- When finished, a terminal should open, if not, open one by searching 'MSYS2 UCRT64' from the start menu.
- In said terminal, run `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`. When prompted, select default (all) packages.
- Add the `msys64\ucrt64\bin` and `msys64\usr\bin` directories to your system path.

#### Make
- Make should be installed with MSYS2.
- From a command-line, run ```make -v``` to validate make has been installed.

#### ARM GNU Toolchain
- Download the ARM GNU toolchain for 32-bit bare-metal targets (arm-none-eabi) from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads.
- Extract the archive's contents to a permanent location and add the ```bin``` directory to your system path.
- From a command-line, run ```arm-none-eabi-gcc -v``` to validate the ARM GNU toolchain has been installed.

#### OpenOCD
- Download OpenOCD for windows (https://github.com/openocd-org/openocd/)
- Extract the archive's contents to a permanent location and add the ```bin``` directory to your system path.
- From a command-line, run ```openocd -v``` to validate OpenOCD has been installed.

#### FMPP
- If your system does not have the Java runtime environment installed, install the latest version.
- Download FMPP from https://sourceforge.net/projects/fmpp/.
- Extract the archive's contents to a permanent location and add the ```bin``` directory to your system path.
- From a command-line, run ```fmpp --version``` to validate FMPP has been installed.

### Recommended VS-Code Extensions
- C/C++
- Clangd
- Cortex-Debug
- Doxygen
- Doxygen Documentation Generator
- RedHat XML

## Usage
### Compilation
- Use ```make``` to compile the program. Files that have not been modified will not re-compiled.
- Use ```make clean``` to delete all build files.

### Programming
- Use ```make flash``` to call OpenOCD. If modifications were made, the program will be recompiled.

### Debugging
- If not already, ensure the code has been compiled. Note that starting debugging does not automatically re-compile the application.
- In VS-code, use ```Run Debugger``` to launch a debug session using the cortex debug extension.

## Basic Directory Structure
```
.
├── build                               - Directory for compilation output.
├── common                              - STM32 common library, see the readme in here for more details.
├── config                              - ChibiOS configuration files.
│   ├── board.chcfg                     - Defines the pin mapping and clock frequency of the board.
│   ├── chconf.h                        - ChibiOS RT configuration.
│   ├── halconf.h                       - ChibiOS HAL configuration.
│   ├── mcuconf.h                       - ChibiOS HAL driver configuration.
├── doc                                 - Documentation folder.
│   ├── chibios                         - ChibiOS documentation.
│   ├── datasheets                      - Datasheets of important components on this board.
│   ├── schematics                      - Schematics of this and related boards.
│   └── software                        - Software documentation.
├── makefile                            - Makefile for this application.
└── src                                 - C source / include files.
    ├── can                             - Code related to this device's CAN interface. This defines the messages this board
    │                                     transmits and receives.
    ├── controls                        - Code related to control systems. Torque vectoring implementations are defined here.
    └── peripherals                     - Code related to board hardware and peripherals.
```