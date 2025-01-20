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
	/// @brief The proportional coefficient, in x-Units / y-Unit.
	float kp;

	/// @brief The integral coefficient, in x-Units / (y-Unit * seconds).
	float ki;

	/// @brief The derivative coefficient, in x-Units / (y-Units / second).
	float kd;

	/// @brief The set-point to target, in y-Units.
	float ySetPoint;

	/// @brief The previous proportional term, should be initialized to 0.
	float ypPrime;

	/// @brief The running integral term, should be initialized to 0.
	float yiPrime;

	/// @brief The output of the controller. Does not need initialized.
	float x;

	/// @brief The output contribution of the proportional term. Does not need initialized.
	float xp;

	/// @brief The output contribution of the integral term. Does not need initialized.
	float xi;

	/// @brief The output contribution of the derivative term. Does not need initialized.
	float xd;
} pidController_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the output value to set according to the given PID controller.
 * @param pid The PID controller to use.
 * @param y The measured value of the process variable.
 * @param deltaTime The amount of time elapsed since the last update (in seconds).
 * filtering, 0.5 => 50/50 split between current input and previous input.
 * @return The output value to set.
 */
float pidCalculate (pidController_t* pid, float y, float deltaTime);

/**
 * @brief Applies a low-pass filter to the derivative term of a PID controller's output.
 * @note The PID controller should already have its output calculated via @c pidCalculate .
 * @param pid The PID controller to use.
 * @param a The measurment gain of the filter to apply, specifies how much of the output should consist of the current
 * measurement vs. the previous measurements. (0 => no filtering, 0.5 => 50% current & 50% previous). Must be in the range
 * [0, 1), not including 1.
 * @param xdPrime Float storing the previous value of the derivative term, written to contain the current derivative term. Must
 * initialized to 0 and preserved between iterations.
 * @return The output value after filtering.
 */
float pidFilterDerivative (pidController_t* pid, float a, float* xdPrime);

/**
 * @brief Back-calculates a PID controller's integral term to de-saturate the output, if it is saturated. This is used to
 * prevent integral runaway for a saturated system.
 * @note The PID controller should already have its output calculated via @c pidCalculate .
 * @param pid The PID controller to apply the anti-windup to.
 * @param xMinimum The minimum viable value of the output.
 * @param xMaximum The maximum viable value of the output.
 * @return The output value after applying anti-windup.
 */
float pidApplyAntiWindup (pidController_t* pid, float xMinimum, float xMaximum);

#endif // PID_CONTROLLER_H