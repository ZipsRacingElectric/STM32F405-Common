// Header
#include "steinhart_hart.h"

// C Standard Library
#include <math.h>

float steinhartHartTemperature (float measuredResistance, float referenceResistance, float a, float b, float c, float d)
{
	float lnR = logf (measuredResistance / referenceResistance);
	return 1.0f / (a + b * lnR + c * lnR * lnR + d * lnR * lnR * lnR);
}