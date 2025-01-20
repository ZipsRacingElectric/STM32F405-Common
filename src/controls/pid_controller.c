// Header
#include "pid_controller.h"

// C Standard Library
#include <math.h>

float pidCalculate (pidController_t* pid, float y, float deltaTime)
{
	// PID Controller Output:
	//   x(k) = k_p * y_p(k) + k_i * y_i(k) + k_d * y_d(k)
	//   where:
	//     x(k) is the control variable (output value),
	//     y(k) is the process variable (input value),
	//     y_sp is the set-point (target value)
	//
	// Proportional Error Term:
	//   yp(k) = y_sp - y(k)
	//
	// Integral Right-Hand Riemann Sum Approximation:
	//   yi(k) = yp(1) * (t(1) - t(0)) + yp(2) * (t(2) - t(1)) + ... + yp(k) * (t(k) - t(k-1))
	//         = yp(k) * (t(k) - t(k-1)) + yi(k-1)
	//         = yp(k) * t_delta(k) + yi(k-1)
	//   where:
	//     t_delta(k) = t(k) - t(k-1)
	//
	// Derivative Difference-Quotient Approximation:
	//   yd(k) = (yp(k) - yp(k-1)) / (t(k) - t(k-1))
	//         = (yp(k) - yp(k-1)) / t_delta(k)

	float yp = pid->ySetPoint - y;

	float yi = yp * deltaTime + pid->yiPrime;
	pid->yiPrime = yi;

	float yd = (yp - pid->ypPrime) / deltaTime;
	pid->ypPrime = yp;

	pid->xp = pid->kp * yp;
	pid->xi = pid->ki * yi;
	pid->xd = pid->kd * yd;
	pid->x = pid->xp + pid->xi + pid->xd;

	// Prevent NaN propogation
	if (isnan (pid->ypPrime) || isinf (pid->ypPrime))
		pid->ypPrime = 0.0f;
	if (isnan (pid->yiPrime) || isinf (pid->yiPrime))
		pid->yiPrime = 0.0f;

	return pid->x;
}

float pidFilterDerivative (pidController_t* pid, float a, float* xdPrime)
{
	// Weighted Rolling Average Low-Pass Filter:
	//   x_dOut(k) = (1 - a) * x_dIn(k) + a * x_dIn(k - 1)

	// Calculate the new derivative term and store the value for the next iteration.
	pid->xd = (1 - a) * pid->xd + a * (*xdPrime);
	*xdPrime = pid->xd;

	// Re-calculate the output after modifying the derivative.
	pid->x = pid->xp + pid->xi + pid->xd;
	return pid->x;
}

float pidApplyAntiWindup (pidController_t* pid, float xMinimum, float xMaximum)
{
	// Anti-windup is a technique applied to prevent integral terms for growing
	// rapidly (running away) when the output value is fully saturated. The
	// simplest approach is to back-calculate a value for the integral term
	// that doesn't saturate the output.

	if (pid->x > xMaximum)
	{
		// Back-calculate the integral term to de-saturate the output.
		if (pid->ki != 0)
			pid->yiPrime = (xMaximum - pid->xp - pid->xd) / pid->ki;
		else
			pid->yiPrime = 0;

		pid->x = xMaximum;
	}
	else if (pid->x < xMinimum)
	{
		// Back-calculate the integral term to de-saturate the output.
		if (pid->ki != 0)
			pid->yiPrime = (xMinimum - pid->xp - pid->xd) / pid->ki;
		else
			pid->yiPrime = 0;

		pid->x = xMinimum;
	}

	return pid->x;
}