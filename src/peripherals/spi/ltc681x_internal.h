#ifndef LTC681X_INTERNAL_H
#define LTC681X_INTERNAL_H

// LTC681X Internal Operations ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.01.26
//
// Description: Common code used internally by LTC681X driver implementations.
//
// Note: This code is derivative of the Analog Devices Linduino codebase:
//   https://github.com/analogdevicesinc/Linduino/tree/master.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "ltc681x.h"

// Constants ------------------------------------------------------------------------------------------------------------------
// See LTC6811 datasheet, pg.8, or LTC6813 datasheet, pg.7.

#define T_READY_MAX						TIME_US2I (10)
#define T_WAKE_MAX						TIME_US2I (400)

/// @brief The total conversion time of the cell voltage ADC / GPIO ADC measuring all cells / GPIO. Indexed by
/// @c ltc681xAdcMode_t .
/// @note See LTC6811 datasheet, pg.25, or LTC6813 datasheet, pg.23.
static const systime_t ADC_MODE_TIMEOUTS [] =
{
	TIME_US2I (12807),	// For 422 Hz mode
	TIME_US2I (1113),	// For 27 kHz mode
	TIME_US2I (2335),	// For 7 kHz mode
	TIME_MS2I (202)		// For 26 Hz mode
};

/// @brief The total conversion time of the status register ADC measuring all values. Indexed by @c ltc681xAdcMode_t .
/// @note See LTC6811 datasheet, pg.29, or LTC6813 datasheet, pg.27.
static const systime_t STATUS_ADC_MODE_TIMEOUTS [] =
{
	TIME_US2I (8537),	// For 422 Hz mode
	TIME_US2I (748),	// For 27 kHz mode
	TIME_US2I (1563),	// For 7 kHz mode
	TIME_MS2I (135),	// For 26 Hz mode
};

// Commands -------------------------------------------------------------------------------------------------------------------
// See LTC6811 datasheet, pg.59, or LTC6813 datasheet, pg.60.

#define COMMAND_WRCFGA					0b00000000001
#define COMMAND_WRCFGB					0b00000100100
#define COMMAND_RDCFGA					0b00000000010
#define COMMAND_RDCFGB					0b00000100110

#define COMMAND_RDCVA					0b00000000100
#define COMMAND_RDCVB					0b00000000110
#define COMMAND_RDCVC					0b00000001000
#define COMMAND_RDCVD					0b00000001010

#define COMMAND_RDAUXA					0b00000001100
#define COMMAND_RDAUXB					0b00000001110

#define COMMAND_RDSTATA					0b00000010000
#define COMMAND_RDSTATB					0b00000010010

#define COMMAND_WRSCTRL					0b00000010100
#define COMMAND_RDSCTRL					0b00000010110

#define COMMAND_WRPWM					0b00000100000
#define COMMAND_RDPWM					0b00000100010

#define COMMAND_STSCTRL					0b00000011001
#define COMMAND_CLRSCTRL				0b00000011000

#define COMMAND_ADCV(ch, md, dcp)		(0b01001100000 | ((md) << 7) | ((dcp) << 4) | (ch))
#define COMMAND_ADOW(ch, md, dcp, pup)	(0b01000101000 | ((md) << 7) | ((dcp) << 4) | (ch) | ((pup) << 6))
#define COMMAND_CVST(md, st)			(0b01000000111 | ((md) << 7) | ((st) << 5))
#define COMMAND_ADOL(md, dcp)			(0b01000000001 | ((md) << 7) | ((dcp) << 4))
#define COMMAND_ADAX(md, chg)			(0b10001100000 | ((md) << 7) | (chg))

#define COMMAND_PLADC					0b11100010100

#define COMMAND_ADSTAT(md, chst)		(0b10001101000 | ((md) << 7) | (chst))

// Conversions ----------------------------------------------------------------------------------------------------------------

#define VUV(vuv)						((uint16_t) (vuv * 625.0f) - 1)
#define VOV(vov)						((uint16_t) (vov * 625.0f))

// 100 uV / LSB
#define CELL_VOLTAGE_FACTOR				0.0001f
#define WORD_TO_CELL_VOLTAGE(word)		(((uint16_t) (word)) * CELL_VOLTAGE_FACTOR)

// Register Groups ------------------------------------------------------------------------------------------------------------

// Configuration Register Group A
// See LTC6811 datasheet, pg.62, or LTC6813 datasheet, pg.63.

#define CFGRA0(gpio5, gpio4, gpio3, gpio2, gpio1, refon, adcopt)															\
	((uint8_t) (((gpio5) << 7) | ((gpio4) << 6) | ((gpio3) << 5) | ((gpio2) << 4) |											\
	((gpio1) << 3) | ((refon) << 2) | (adcopt) << 0))

#define CFGRA1(vuv)																											\
	((uint8_t) VUV (vuv))

#define CFGRA2(vuv, vov)																									\
	((uint8_t) ((VOV (vov) << 4) | (VUV (vuv) >> 8)))

#define CFGRA3(vov)																											\
	((uint8_t) (VOV (vov) >> 4))

// Note these are 1-indexed, not 0-indexed (dcc1 <==> cell[0]).
#define CFGRA4(dcc8, dcc7, dcc6, dcc5, dcc4, dcc3, dcc2, dcc1)																\
	((uint8_t) (((dcc8) << 7) | ((dcc7) << 6) | ((dcc6) << 5) | ((dcc5) << 4) |												\
	((dcc4) << 3) | ((dcc3) << 2) | ((dcc2) << 1) | (dcc1)))

// Note these are 1-indexed, not 0-indexed (dcc9 <==> cell[8]).
#define CFGRA5(dcto, dcc12, dcc11, dcc10, dcc9)																				\
	((uint8_t) (((dcto) << 4) |																								\
	((dcc12) << 3) | ((dcc11) << 2) | ((dcc10) << 1) | (dcc9)))

// Configuration Register Group B
// See LTC6813 datasheet, pg.63.

// Note these are 1-indexed, not 0-indexed (dcc13 <==> cell[12]).
#define CFGRB0(dcc16, dcc15, dcc14, dcc13, gpio9, gpio8, gpio7, gpio6)														\
	((uint8_t) (((dcc16) << 7) | ((dcc15) << 6) | ((dcc14) << 5) | ((dcc13) << 4) |											\
	((gpio9) << 3) | ((gpio8) << 2) | ((gpio7) << 1) | ((gpio6) << 0)))

// Note these are 1-indexed, not 0-indexed (dcc17 <==> cell[16]).
#define CFGRB1(fdrf, ps, dtmen, dcc0, dcc18, dcc17)																			\
	((uint8_t) (((fdrf) << 6) | (((ps) & 0b11) << 4) |																		\
	((dtmen) << 3) | ((dcc0) << 2) | ((dcc18) << 1) | ((dcc17) << 0)))

// Reserved
#define CFGRB2 0x00
#define CFGRB3 0x00
#define CFGRB4 0x00
#define CFGRB5 0x00

// Status Register Group A
// See LTC6811 datasheet, pg.63, or LTC6813 datasheet, pg.66.

#define STAR0_1_SC(star0, star1)		(((uint16_t) (((star1) << 8) | (star0))) * 0.002f)
#define STAR2_3_ITMP(star2, star3)		(((uint16_t) (((star3) << 8) | (star2))) * 0.1f / 7.5f - 273.0f)

// Status Register Group B
// See LTC6811 datasheet, pg.64, or LTC6813 datasheet, pg.66.

#define STBR2_C1UV(stbr2)				((stbr2 & 0b00000001) == 0b00000001)
#define STBR2_C1OV(stbr2)				((stbr2 & 0b00000010) == 0b00000010)
#define STBR2_C2UV(stbr2)				((stbr2 & 0b00000100) == 0b00000100)
#define STBR2_C2OV(stbr2)				((stbr2 & 0b00001000) == 0b00001000)
#define STBR2_C3UV(stbr2)				((stbr2 & 0b00010000) == 0b00010000)
#define STBR2_C3OV(stbr2)				((stbr2 & 0b00100000) == 0b00100000)
#define STBR2_C4UV(stbr2)				((stbr2 & 0b01000000) == 0b01000000)
#define STBR2_C4OV(stbr2)				((stbr2 & 0b10000000) == 0b10000000)

#define STBR3_C5UV(stbr3)				((stbr3 & 0b00000001) == 0b00000001)
#define STBR3_C5OV(stbr3)				((stbr3 & 0b00000010) == 0b00000010)
#define STBR3_C6UV(stbr3)				((stbr3 & 0b00000100) == 0b00000100)
#define STBR3_C6OV(stbr3)				((stbr3 & 0b00001000) == 0b00001000)
#define STBR3_C7UV(stbr3)				((stbr3 & 0b00010000) == 0b00010000)
#define STBR3_C7OV(stbr3)				((stbr3 & 0b00100000) == 0b00100000)
#define STBR3_C8UV(stbr3)				((stbr3 & 0b01000000) == 0b01000000)
#define STBR3_C8OV(stbr3)				((stbr3 & 0b10000000) == 0b10000000)

#define STBR4_C9UV(stbr4)				((stbr4 & 0b00000001) == 0b00000001)
#define STBR4_C9OV(stbr4)				((stbr4 & 0b00000010) == 0b00000010)
#define STBR4_C10UV(stbr4)				((stbr4 & 0b00000100) == 0b00000100)
#define STBR4_C10OV(stbr4)				((stbr4 & 0b00001000) == 0b00001000)
#define STBR4_C11UV(stbr4)				((stbr4 & 0b00010000) == 0b00010000)
#define STBR4_C11OV(stbr4)				((stbr4 & 0b00100000) == 0b00100000)
#define STBR4_C12UV(stbr4)				((stbr4 & 0b01000000) == 0b01000000)
#define STBR4_C12OV(stbr4)				((stbr4 & 0b10000000) == 0b10000000)

// Datatypes ------------------------------------------------------------------------------------------------------------------

/// @brief Indicates which internal buffer to write cell voltages into. Used for open wire test.
typedef enum
{
	CELL_VOLTAGE_DESTINATION_VOLTAGE_BUFFER = 0,
	CELL_VOLTAGE_DESTINATION_PULLUP_BUFFER = 1,
	CELL_VOLTAGE_DESTINATION_PULLDOWN_BUFFER = 2
} cellVoltageDestination_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the packet error code given a frame's contents.
 * @param data The data to calculate the code for.
 * @param dataCount The number of elements in @c data .
 * @return The calculated PEC word. Note this value is already shifted to 0 the LSB.
 */
uint16_t ltc681xCalculatePec (uint8_t* data, uint8_t dataCount);

/**
 * @brief Checks whether the packet error code of a frame is correct.
 * @param data The data to validate the code of.
 * @param dataCount The number of elements in @c data .
 * @param pec The PEC to validate against.
 * @return True if the PEC is correct, false otherwise.
 */
bool ltc681xValidatePec (uint8_t* data, uint8_t dataCount, uint16_t pec);

/**
 * @brief Blocks until a previously scheduled ADC conversion is completed. Note the SPI peripheral should still be selected
 * from the written ADC command.
 * @param bottom The bottom (first) device in the daisy chain.
 * @return True if all device conversions are complete, false if a timeout occurred. Timeouts are considered a fatal error.
 */
bool ltc681xPollAdc (ltc681x_t* bottom, sysinterval_t timeout);

/**
 * @brief Writes a command to each device in a chain.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The command to write.
 * @param unselect Indicates whether to unselect the SPI peripheral or not. If @c false , the caller is responsible for
 * calling @c spiUnselect
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee write success. Note the
 * SPI peripheral is automatically unselected on failure.
 */
bool ltc681xWriteCommand (ltc681x_t* bottom, uint16_t command, bool unselect);

/**
 * @brief Writes to a data register group of each device in a chain.
 * @note The to written to each device should be placed into its @c tx buffer.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The write command of the register group.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee write success.
 */
bool ltc681xWriteRegisterGroups (ltc681x_t* bottom, uint16_t command);

/**
 * @brief Reads from a data register group of each device in a chain.
 * @note The data read from each device is placed into its @c rx buffer.
 * @param bottom The bottom (first) device in the daisy chain.
 * @param command The read command of the register group.
 * @return False if a fatal error occurred, true otherwise. A non-fatal return code does not guarantee read data is valid,
 * check individual device states to determine validity.
 */
bool ltc681xReadRegisterGroups (ltc681x_t* bottom, uint16_t command);

/**
 * @brief Sets all devices in a chain to the @c LTC681X_STATE_FAILED state.
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc681xFailChain (ltc681x_t* bottom);

/**
 * @brief Function to be called when a GPIO sampling operation fails. All GPIO will be put into the
 * @c ANALOG_SENSOR_FAILED state.
 * @param bottom The bottom (first) device in the daisy chain.
 */
void ltc681xFailGpio (ltc681x_t* bottom);

#endif // LTC681X_INTERNAL_H