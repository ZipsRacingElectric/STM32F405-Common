#ifndef AMK_INVERTER_H
#define AMK_INVERTER_H

// AMK Inverter ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Object representing the AMK Racing Kit Inverter CAN node.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
	uint16_t		baseId;
} amkInverterConfig_t;

typedef struct
{
	CAN_NODE_FIELDS;
	uint16_t baseId;

	/// @brief Indicates whether the inverter is ready and error-free.
	bool systemReady;

	/// @brief Indicates whether a system error is present.
	bool error;

	/// @brief Indicates whether a system warning is present.
	bool warning;

	/// @brief Acknowledgement of the DC bus being enabled and energized. Is not asserted until the @c dcOn bit is set and
	/// the DC bus voltage exceeds the minimum.
	bool quitDcOn;

	/// @brief Indicates whether the DC bus is enabled or not. See @c quitDcOn for whether the bus is energized.
	bool dcOn;

	/// @brief Acknowledgement of the inverter being enabled and energized. Is not asserted until the DC bus is energized, the
	/// @c inverterOn bit is set, and the system is error-free.
	bool quitInverter;

	/// @brief Indicates whether the inverter controller is enabled or not. See @c quitInverter for whether the system is
	/// energized or not.
	bool inverterOn;

	/// @brief Indicates whether the output torque is being de-rated due to hardware conditions.
	bool derating;

	/// @brief The actual torque being produced / regenerated at the motor shaft (may not match requested torque due to
	/// de-rating).
	float actualTorque;

	/// @brief The actual speed of the motor shaft.
	float actualSpeed;

	/// @brief The measured voltage of the DC bus.
	float dcBusVoltage;

	// TODO(Barach): Docs
	float torqueCurrent;

	/// @brief The actual power consumption of the device.
	float actualPower;
} amkInverter_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, amkInverterConfig_t* config);

msg_t amkSendEnergizationRequest (amkInverter_t* amk, bool energized, sysinterval_t timeout);

msg_t amkSendTorqueRequest (amkInverter_t* amk, float torqueRequest, float torqueLimitPositive, float torqueLimitNegative,
	sysinterval_t timeout);

msg_t amkSendErrorResetRequest (amkInverter_t* amk, sysinterval_t timeout);

#endif // AMK_INVERTER_H