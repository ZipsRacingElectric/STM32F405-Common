// Header
#include "lerp.h"

// C Standard Library
#include <math.h>

float lerp (float x, float a, float b)
{
	// Scale X by range, then offset by min.
	return x * (b - a) + a;
}

float inverseLerp (float x, float a, float b)
{
	// Prevent division-by-zero. If both points are identical, this is the most useful answer.
	if (b == a)
		return 0;

	// Offset x by min, then divide by range.
	return (x - a) / (b - a);
}

float lerp2d (float cx, float ax, float ay, float bx, float by)
{
	// Prevent division-by-zero. If both points are identical, this is the most useful answer.
	if (bx == ax)
		return ay;

	// Perform inverse lerp then lerp.
	return (cx - ax) / (bx - ax) * (by - ay) + ay;
}

float lerp2dSaturated (float cx, float ax, float ay, float bx, float by)
{
	// Determine which value is the minimum
	bool aIsMin = ax < bx;

	// Min saturation
	if (cx < (aIsMin ? ax : bx))
		return aIsMin ? ay : by;

	// Max saturation
	if (cx > (aIsMin ? bx : ax))
		return aIsMin ? by : ay;

	// Within bounds, use normal form.
	return lerp2d (cx, ax, ay, bx, by);
}

float bilinearInterpolation (float x3, float y3, float x1, float y1, float x2, float y2,
	float z11, float z12, float z21, float z22)
{
	// Normalize the input coordinates (x1,y1 => 0, x2,y2 => 1)
	float x = inverseLerp (x3, x1, x2);
	float y = inverseLerp (y3, y1, y2);

	// Lerp the 1st X-Z plane
	float z1 = lerp (x, z11, z21);

	// Lerp the 2nd X-Z plane
	float z2 = lerp (x, z12, z22);

	// Lerp the central Y-Z plane
	return lerp (y, z1, z2);
}

float bilinearLookupTable (float x, float y, float xMin, float xMax, float yMin, float yMax,
	const float* table, uint16_t xWidth, uint16_t yWidth)
{
	// Get the floating-point x & y indices
	float x3 = lerp2dSaturated (x, xMin, 0, xMax, xWidth - 1);
	float y3 = lerp2dSaturated (y, yMin, 0, yMax, yWidth - 1);

	// Get the upper and lower bounds on the x & y indices
	uint8_t x1 = floorf (x3);
	uint8_t x2 = ceilf  (x3);
	uint8_t y1 = floorf (y3);
	uint8_t y2 = ceilf  (y3);

	// Get the 4 points to interpolate at.
	float z11 = table [x1 + y1 * xWidth];
	float z12 = table [x1 + y2 * xWidth];
	float z21 = table [x2 + y1 * xWidth];
	float z22 = table [x2 + y2 * xWidth];

	// Perform bilinear interpolation between the 4 points.
	return bilinearInterpolation (x3, y3, x1, y1, x2, y2, z11, z12, z21, z22);
}

float inverseLerpHysteresis (float x, float a, float b, float hyst, bool* increasingCurve, float* cPrime)
{
	if (x < a)
	{
		// If before the start of the interpolation, return 0.
		*increasingCurve = true;
		*cPrime = 0;
		return 0;
	}
	else if (x > b)
	{
		// If after the end of interpolation, return 1.
		*increasingCurve = false;
		*cPrime = 1;
		return 1;
	}
	else
	{
		// Compute the outputs for both the increasing and decreasing curves
		float cIncreasing = (x - a - hyst) / (b - a - hyst);
		float cDecreasing = (x - a) / (b - hyst - a);

		if (*increasingCurve)
		{
			if (cIncreasing > *cPrime)
			{
				// If on the increasing curve and the output has increased, return said output.
				*cPrime = cIncreasing;
				return cIncreasing;
			}
			else if (cDecreasing < *cPrime)
			{
				// If on the increasing curve and the input has decreased enough to hit the decreasing curve, switch curves
				// and return said output.
				*increasingCurve = false;
				*cPrime = cDecreasing;
				return cDecreasing;
			}
		}
		else
		{
			if (cDecreasing < *cPrime)
			{
				// If on the decreasing curve and the output has decreased, return said output.
				*cPrime = cDecreasing;
				return cDecreasing;
			}
			else if (cIncreasing > *cPrime)
			{
				// If on the decreasing curve and the input has increased enough to hit the increasing curve, switch curves
				// and return said output.
				*increasingCurve = true;
				*cPrime = cIncreasing;
				return cIncreasing;
			}
		}
	}

	// If the output has not changed, return the last value.
	return *cPrime;
}