// Header
#include "pid_controller.h"

// C Standard Library
#include <math.h>

float pidCalculate (pidController_t* pid, float y, float deltaTime)
{
	// PID Controller Output:
	//   x(k) = kp * p(k) + ki * i(k) + kd * d(k)
	//   where:
	//     x(k) is the control variable (output value),
	//     y(k) is the process variable (input value),
	//     y_sp is the set-point (target value)
	//
	// Proportional Error Term:
	//   p(k) = y_sp - y(k)
	//
	// Integral Right-Hand Riemann Sum Approximation:
	//   i(k) = p(1) * (t(1) - t(0)) + p(2) * (t(2) - t(1)) + ... + p(k) * (t(k) - t(k-1))
	//        = p(k) * (t(k) - t(k-1)) + i(k-1)
	//        = p(k) * delta_t(k) + i(k-1)
	//
	// Derivative Difference-Quotient Approximation:
	//   d(k) = (p(k) - p(k-1)) / (t(k) - t(k-1))
	//        = (p(k) - p(k-1)) / delta_t(k)

	float p = pid->ySetPoint - y;

	float i = p * deltaTime + pid->iPrime;
	pid->iPrime = i;

	float d = (p - pid->dPrime) / deltaTime;
	pid->dPrime = p;

	// Prevent NaN propogation
	if (pid->iPrime != pid->iPrime)
		pid->iPrime = 0.0f;
	if (pid->dPrime != pid->dPrime)
		pid->dPrime = 0.0f;

	return pid->kp * p + pid->ki * i + pid->kd * d;
}

float pidAntiWindup (pidController_t* pid, float y, float deltaTime, float xMin, float xMax)
{
	// Anti-windup is a technique applied to prevent integral terms for growing
	// rapidly (running away) when the output value is fully saturated. The
	// simplest approach is to back-calculate a value for the integral term
	// that doesn't saturate the output.

	// See above for PID calculation.

	float p = pid->ySetPoint - y;

	float i = p * deltaTime + pid->iPrime;
	pid->iPrime = i;

	float d = (p - pid->dPrime) / deltaTime;
	pid->dPrime = p;

	// Prevent NaN propogation
	if (isnan (pid->iPrime) || isinf (pid->iPrime))
		pid->iPrime = 0.0f;
	if (isnan (pid->dPrime) || isinf (pid->dPrime))
		pid->dPrime = 0.0f;

	float nonIntegral = pid->kp * p + pid->kd * d;
	float x = nonIntegral + pid->ki * i;

	if (x > xMax)
	{
		if (pid->ki != 0)
			pid->iPrime = (xMax - nonIntegral) / pid->ki;
		else
			pid->iPrime = 0;

		x = xMax;
	}
	else if (x < xMin)
	{
		if (pid->ki != 0)
			pid->iPrime = (xMin - nonIntegral) / pid->ki;
		else
			pid->iPrime = 0;

		x = xMin;
	}

	return x;
}