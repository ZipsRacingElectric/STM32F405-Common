#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

// PID Controller -------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.25
//
// Description: Object and functions related to a proportional-integral-derivative controller.

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	float kp;
	float ki;
	float kd;

	float ySetPoint;
	float iPrime;
	float dPrime;
} pidController_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the output value to set according to the given PID controller.
 * @param pid The PID controller to use.
 * @param y The measured value of the process variable.
 * @param deltaTime The amount of time ellapsed since the last update (in seconds).
 * @return The output value to set.
 */
float pidCalculate (pidController_t* pid, float y, float deltaTime);

/**
 * @brief Calculates the output value to set according to the givent PID controller. Integral anti-windup is used to prevent
 * runaway when output is saturated.
 * @param pid The PID controller to use.
 * @param y The measured value of the process variable.
 * @param deltaTime The amount of time ellapsed since the last update (in seconds).
 * @param xMin The minimum viable value of the output value.
 * @param xMax The maximum viable value of the output value.
 * @return The output value to set.
 */
float pidAntiWindup (pidController_t* pid, float y, float deltaTime, float xMin, float xMax);

#endif // PID_CONTROLLER_H