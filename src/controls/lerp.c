// Header
#include "lerp.h"

float lerp (float x, float a, float b)
{
	return x * (b - a) + a;
}

float inverseLerp (float x, float a, float b)
{
	return (x - a) / (b - a);
}

float lerp2d (float x, float xa, float ya, float xb, float yb)
{
	// Perform inverse lerp then lerp.
	return (x - xa) / (xb - xa) * (yb - ya) + ya;
}