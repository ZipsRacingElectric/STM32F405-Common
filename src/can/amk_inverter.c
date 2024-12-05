// Header
#include "amk_inverter.h"

// Conversions -----------------------------------------------------------------------------------------------------------------

// Torque values (unit Nm)
#define TORQUE_FACTOR			0.0098f
#define TORQUE_INVERSE_FACTOR	102.040816326530f
#define TORQUE_TO_WORD(torque)	(int16_t) ((torque) * TORQUE_INVERSE_FACTOR)
#define WORD_TO_TORQUE(word)	(((int16_t) (word)) * TORQUE_FACTOR)

// Speed values (unit RPM)
// TODO(Barach): Are you sure this is correct? It looked to be proportional in testing.
#define SPEED_FACTOR			0.00001f
#define WORD_TO_SPEED(word)		(SPEED_FACTOR / ((int32_t) (word)))

// Voltage values (unit V)
#define WORD_TO_VOLTAGE(word)	((uint16_t) (word))

// Current values (unit A)
#define WORD_TO_CURRENT(word)	((int16_t) (word))

// Power values (unit W)
#define WORD_TO_POWER(word)		((uint32_t) (word))

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define MOTOR_REQUEST_ID_OFFSET		0x000
#define MOTOR_FEEDBACK_ID_OFFSET	0x004
#define POWER_CONSUMPTION_ID_OFFSET	0x008

// Message Flags --------------------------------------------------------------------------------------------------------------

#define MOTOR_FEEDBACK_FLAG_POS 	0x00
#define POWER_CONSUMPTION_FLAG_POS	0x01

// Message Packing ------------------------------------------------------------------------------------------------------------

// AMK Control Word
#define CONTROL_WORD_INVERTER_ON(bit)	(((uint16_t) (bit)) << 8)
#define CONTROL_WORD_DC_ON(bit)			(((uint16_t) (bit)) << 9)
#define CONTROL_WORD_ENABLE(bit)		(((uint16_t) (bit)) << 10)
#define CONTROL_WORD_ERROR_RESET(bit)	(((uint16_t) (bit)) << 11)

// AMK Status Word
#define STATUS_WORD_SYSTEM_READY(word)	((((word) >> 8) & 0b1) == 0b1)
#define STATUS_WORD_ERROR(word)			((((word) >> 9) & 0b1) == 0b1)
#define STATUS_WORD_WARNING(word)		((((word) >> 10) & 0b1) == 0b1)
#define STATUS_WORD_QUIT_DC_ON(word)	((((word) >> 11) & 0b1) == 0b1)
#define STATUS_WORD_DC_ON(word)			((((word) >> 12) & 0b1) == 0b1)
#define STATUS_WORD_QUIT_INVERTER(word)	((((word) >> 13) & 0b1) == 0b1)
#define STATUS_WORD_INVERTER_ON(word)	((((word) >> 14) & 0b1) == 0b1)
#define STATUS_WORD_DERATING(word)		((((word) >> 15) & 0b1) == 0b1)

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Sends the specified torque request to an AMK inverter.
 * @param amk The AMK inverter to send the message to.
 * @param inverterEnabled Indicates whether the inverter controller should be enabled (cannot be set until quitDcOn is
 * asserted).
 * @param dcEnabled Indicates whether the DC bus should be enabled (can be asserted any time).
 * @param driverEnabled Indicates whether the inverter driver should be enabled (cannot be asserted until quitInverter is
 * asserted).
 * @param errorReset Indicates whether any present errors should be reset (can only be asserted if all setpoints are 0).
 * @param torqueRequest The torque to request from the motor.
 * @param torqueLimitPositive The positive torque limit to specify.
 * @param torqueLimitNegative The negative torque limit to specify.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t amkSendMotorRequest (amkInverter_t* amk, bool inverterEnabled, bool dcEnabled, bool driverEnabled, bool errorReset,
	float torqueRequest, float torqueLimitPositive, float torqueLimitNegative, sysinterval_t timeout);

int8_t amkReceiveHandler (void* node, CANRxFrame* frame);

// Functions ------------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, amkInverterConfig_t* config)
{
	// Store the configuration
	amk->baseId = config->baseId;

	// Initialize the node
	canNodeConfig_t canConfig =
	{
		.driver			= config->driver,
		.receiveHandler	= amkReceiveHandler,
		.timeoutHandler	= NULL,
		.timeoutPeriod	= config->timeoutPeriod,
		.messageCount	= 2
	};
	canNodeInit ((canNode_t*) amk, &canConfig);
}

// Transmit Functions ---------------------------------------------------------------------------------------------------------

msg_t amkSendEnergizationRequest (amkInverter_t* amk, bool energized, sysinterval_t timeout)
{
	// In order to energize, all setpoints must be set to 0.
	return amkSendMotorRequest (amk, energized, true, energized, false, 0, 0, 0, timeout);
}

msg_t amkSendTorqueRequest (amkInverter_t* amk, float torqueRequest, float torqueLimitPositive, float torqueLimitNegative,
	sysinterval_t timeout)
{
	canNodeLock ((canNode_t*) amk);
	bool energized = amk->state == CAN_NODE_VALID && amk->quitInverter;
	canNodeUnlock ((canNode_t*) amk);

	// If the inverter isn't energized, send the request to energize.
	if (!energized)
		return amkSendEnergizationRequest (amk, true, timeout);

	// Otherwise, send the torque request.
	return amkSendMotorRequest (amk, true, true, true, false, torqueRequest, torqueLimitPositive, torqueLimitNegative,
		timeout);
}

msg_t amkSendErrorResetRequest (amkInverter_t* amk, sysinterval_t timeout)
{
	// Preserve the current settings.
	canNodeLock ((canNode_t*) amk);
	bool inverterEnabled	= amk->state == CAN_NODE_VALID && amk->inverterOn;
	bool dcEnabled			= amk->state == CAN_NODE_VALID && amk->dcOn;
	bool driverEnabled		= inverterEnabled && dcEnabled;
	canNodeUnlock ((canNode_t*) amk);

	return amkSendMotorRequest (amk, inverterEnabled, dcEnabled, driverEnabled, true, 0, 0, 0, timeout);
}

msg_t amkSendMotorRequest (amkInverter_t* amk, bool inverterEnabled, bool dcEnabled, bool driverEnabled, bool errorReset,
	float torqueRequest, float torqueLimitPositive, float torqueLimitNegative, sysinterval_t timeout)
{
	// Motor Request Message: (ID Offset 0x000)
	//   Bytes 0 to 1: Control word (uint16_t).
	//     Bits 0 to 7: Reserved
	//     Bit 8: Inverter enable
	//     Bit 9: DC enable
	//     Bit 10: Driver enable
	//     Bit 11: Error reset
	//     Bits 12 to 15: Reserved
	//   Bytes 2 to 3: Torque setpoint (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)
	//   Bytes 4 to 5: Positive torque limit (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)
	//   Bytes 6 to 7: Negative torque limit (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)

	uint16_t controlWord = CONTROL_WORD_INVERTER_ON (inverterEnabled) | CONTROL_WORD_DC_ON (dcEnabled) |
		CONTROL_WORD_ENABLE (driverEnabled) | CONTROL_WORD_ERROR_RESET (errorReset);
	int16_t torqueRequestInt		= TORQUE_TO_WORD (torqueRequest);
	int16_t torqueLimitPositiveInt	= TORQUE_TO_WORD (torqueLimitPositive);
	int16_t torqueLimitNegativeInt	= TORQUE_TO_WORD (torqueLimitNegative);

	CANTxFrame transmit =
	{
		.DLC = 8,
		.IDE = CAN_IDE_STD,
		.SID = amk->baseId + MOTOR_REQUEST_ID_OFFSET,
		.data16 =
		{
			controlWord,
			torqueRequestInt,
			torqueLimitPositiveInt,
			torqueLimitNegativeInt
		}
	};

	msg_t result = canTransmitTimeout (amk->driver, CAN_ANY_MAILBOX, &transmit, timeout);
	if (result != MSG_OK)
		canFaultCallback (result);
	return result;
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void amkHandleMotorFeedback (amkInverter_t* amk, CANRxFrame* frame)
{
	// Motor Feedback Message: (ID Offset 0x004)
	//   Bytes 0 to 1: Status word (uint16_t)
	//     Bit 0 to 7: Reserved
	//     Bit 8: System ready
	//     Bit 9: Error
	//     Bit 10: Warning
	//     Bit 11: Quit DC on
	//     Bit 12: DC on
	//     Bit 13: Quit inverter on
	//     Bit 14: Inverter on
	//     Bit 15: Derating
	//   Bytes 2 to 3: Actual torque (int16_t)
	//   Bytes 4 to 7: Actual speed (int32_t)

	uint16_t statusWord = frame->data16 [0];
	amk->systemReady	= STATUS_WORD_SYSTEM_READY (statusWord);
	amk->error			= STATUS_WORD_ERROR (statusWord);
	amk->warning		= STATUS_WORD_WARNING (statusWord);
	amk->quitDcOn		= STATUS_WORD_QUIT_DC_ON (statusWord);
	amk->dcOn			= STATUS_WORD_DC_ON (statusWord);
	amk->quitInverter	= STATUS_WORD_QUIT_INVERTER (statusWord);
	amk->inverterOn		= STATUS_WORD_INVERTER_ON (statusWord);
	amk->derating		= STATUS_WORD_DERATING (statusWord);

	amk->actualTorque	= WORD_TO_TORQUE (frame->data16 [1]);
	amk->actualSpeed	= WORD_TO_SPEED (frame->data32 [1]);
}

void amkHandlePowerConsumption (amkInverter_t* amk, CANRxFrame* frame)
{
	// Power Consumption Message: (ID Offset 0x008)
	//   Bytes 0 to 1: DC bus voltage (uint16_t)
	//   Bytes 2 to 3: Torque current (int16_T)
	//   Bytes 4 to 7: Actual power (uint32_t)

	amk->dcBusVoltage	= WORD_TO_VOLTAGE (frame->data16 [0]);
	amk->torqueCurrent	= WORD_TO_CURRENT (frame->data16 [1]);
	amk->actualPower	= WORD_TO_POWER (frame->data32 [1]);
}

int8_t amkReceiveHandler (void* node, CANRxFrame* frame)
{
	amkInverter_t* amk = (amkInverter_t*) node;
	uint16_t id = frame->SID;

	// Identify and handle the message.
	if (id == amk->baseId + MOTOR_FEEDBACK_ID_OFFSET)
	{
		// Motor feedback message.
		amkHandleMotorFeedback (amk, frame);
		return MOTOR_FEEDBACK_FLAG_POS;
	}
	if (id == amk->baseId + POWER_CONSUMPTION_ID_OFFSET)
	{
		// Power consumption message.
		amkHandlePowerConsumption (amk, frame);
		return POWER_CONSUMPTION_FLAG_POS;
	}
	else
	{
		// Message doesn't belong to this node.
		return -1;
	}
}