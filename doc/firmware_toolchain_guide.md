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
- Clone this repo using github's SSH URL ```git clone <SSH URL>```
- Initialize the repo's ```common``` submodule using ```git submodule init```
- Download the ChibiOS 21.11.3 source from https://github.com/ChibiOS/ChibiOS/releases/tag/ver21.11.3.
- Extract the archive's contents to a permanent location.
- Define the ```CHIBIOS_SOURCE_PATH``` environment variable to point to the location the ChibiOS source (Note: For compatibility with FMPP, this path should use the ```/``` separator rather than ```\```).

### For Windows
Some dependicies of this project are not natively built for Windows. A solution to this is to use MinGW, a POSIX compatibility layer that allows these programs to be run in Windows.

#### MinGW
- Download and install MinGW from https://sourceforge.net/projects/mingw/.
- Install the following MinGW packages:
	```mingw32-base```
	```msys-base```
	```mingw-developer-toolkit```
- Add ```C:\MinGW\bin\``` and ```C:\MinGW\msys\1.0\bin\``` to your system path.
- From a command-line, run ```bash --version``` to validate MinGW has been installed.

#### Make
- Make should be installed with MinGW.
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