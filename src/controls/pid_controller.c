// Header
#include "pid_controller.h"

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
	pid->dPrime = d;

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
	pid->dPrime = d;

	float nonIntegral = pid->kp * p + pid->kd * d;
	float x = nonIntegral + pid->ki * i;

	if (x > xMax)
	{
		pid->iPrime = (xMax - nonIntegral) / pid->ki;
		x = xMax;
	}
	else if (x < xMin)
	{
		pid->iPrime = (xMin - nonIntegral) / pid->ki;
		x = xMin;
	}

	return x;
}