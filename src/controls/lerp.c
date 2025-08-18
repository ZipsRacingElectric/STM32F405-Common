// Header
#include "lerp.h"

// C Standard Library
#include <stdbool.h>

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
	// The X-Y plane looks something like this, keep in mind it's 3D.
	//
	//  Y
	//  |  Q12    Y-Z        Q22
	// y2   o------o----------o  2nd X-Z
	//  |   |      |          |
	//  |   |      |          |
	// y3   |      o          |
	//  |   |      |          |
	//  |   |      |          |
	//  |   |      |          |
	// y1   o------o----------o  1st X-Z
	//  |  Q11               Q21
	//  |
	//  0---x1-----x3---------x2--- X

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