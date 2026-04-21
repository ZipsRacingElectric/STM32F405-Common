// Header
#include "vehicle_dynamics.h"

// C Standard Library
#define _USE_MATH_DEFINES
#include "math.h"

float motorSpeedToGroundSpeed (float motorSpeed, float gearRatio, float tireDiameter)
{
	return motorSpeed
		/ gearRatio				// Motor speed (RPM) => Wheel speed (RPM)
		* tireDiameter * M_PI	// Wheel speed (RPM) => Ground speed (Inches/min)
		/ 39370.1f				// Ground speed (Inches/min) => Ground speed (Km/min)
		* 60.0f;				// Ground speed (Km/min) => Ground speed (Km/h)
}