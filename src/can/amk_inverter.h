#ifndef AMK_INVERTER_H
#define AMK_INVERTER_H

// AMK Inverter ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
// 
// Description: Object representing the AMK Racing Kit Inverter CAN node.
//
// To do:
// - Handler / transmit implementations.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct amkInverterConfig
{
	CANDriver*		driver;
	sysinterval_t	timeoutPeriod;
	uint16_t		baseId;
};

typedef struct amkInverterConfig amkInverterConfig_t;

struct amkInverter
{
	CAN_NODE_FIELDS;
	
	uint16_t baseId;
};

typedef struct amkInverter amkInverter_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, amkInverterConfig_t* config);

// TODO(Barach): Better Documentation
/**
 * @brief Sends the specified torque request to an AMK inverter.
 * @param amk The AMK inverter to send the message to.
 * @param inverterEnabled
 * @param dcEnabled
 * @param driverEnabled
 * @param torqueRequest The torque to request from the motor.
 * @param torqueLimitPositive The positive torque limit to specify.
 * @param torqueLimitNegative The negative torque limit to specify.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t amkSendMotorRequest (amkInverter_t* amk, bool inverterEnabled, bool dcEnabled, bool driverEnabled, float torqueRequest,
	float torqueLimitPositive, float torqueLimitNegative, sysinterval_t timeout);

#endif // AMK_INVERTER_H