#ifndef LERP_H
#define LERP_H

// Linear Interpolation -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.01.30
//
// Description: Group of functions related to linear interpolation.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdint.h>
#include <stdbool.h>

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Performs linear interpolation between the values A and B.
 * @param x The input scalar, [0, 1].
 * @param a The minimum value (x = 0 => lerp(x) = a).
 * @param b The maximum value (x = 1 => lerp(x) = b).
 * @return The interpolated value.
 */
float lerp (float x, float a, float b);

/**
 * @brief Performs inverse linear interpolation from the values A and B.
 * @param x The input value, from [A, B].
 * @param a The minimum value (x = A => inverseLerp(x) = 0).
 * @param b The maximum value (x = B => inverseLerp(x) = 1).
 * @return The inverse interpolated value.
 */
float inverseLerp (float x, float a, float b);

/**
 * @brief Performs 2D linear interpolation from the the points A and B.
 * @note This can be used for inverse 2D linear interpolation by swapping the x and y coordinates.
 * @param cx The input point's x-coordinate.
 * @param ax Point A's x-coordinate.
 * @param ay Point A's y-coordinate.
 * @param bx Point B's x-coordinate.
 * @param by Point B's y-coordinate.
 * @return The y-coordinate of the interpolated point.
 */
float lerp2d (float cx, float ax, float ay, float bx, float by);

/**
 * @brief Version of @c lerp2d that saturates the output for values of @c cx
 * outside the range of [ax, bx].
 */
float lerp2dSaturated (float cx, float ax, float ay, float bx, float by);

/**
 * @brief Performs 3D bilinear interpolation on between the points Q11, Q12, Q21, and Q22.
 * @param x3 The input point's x-coordinate.
 * @param y3 The input point's y-coordinate.
 * @param x1 The x-coordinate of points Q11 and Q12.
 * @param y1 The y-coordinate of points Q11 and Q21.
 * @param x2 The x-coordinate of points Q21 and Q22.
 * @param y2 The y-coordinate of points Q12 and Q22.
 * @param z11 The z-coordinate of point Q11.
 * @param z12 The z-coordinate of point Q12.
 * @param z21 The z-coordinate of point Q21.
 * @param z22 The z-coordinate of point Q22.
 * @return The z-coordinate of the interpolated point.
 *
 * @note The X-Y plane looks something like this, keep in mind it's 3D:
 *
 *  Y
 *  |  Q12    Y-Z        Q22
 * y2   o------o----------o  2nd X-Z
 *  |   |      |          |
 *  |   |      |          |
 * y3   |      o          |
 *  |   |      |          |
 *  |   |      |          |
 *  |   |      |          |
 * y1   o------o----------o  1st X-Z
 *  |  Q11               Q21
 *  |
 *  0---x1-----x3---------x2--- X
 *
 */
float bilinearInterpolation (float x3, float y3, float x1, float y1, float x2, float y2,
	float z11, float z12, float z21, float z22);

/**
 * @brief Performs bilinear interpolation on a loopup table.
 * @param x The x value to interpolate for.
 * @param y The y value to interpolate for.
 * @param xMin The minimum value of the x-axis (maps to index @c 0 ).
 * @param xMax The maximum value of the x-axis (maps to index @c xWidth-1 )
 * @param yMin The maximum value of the y-axis (maps to index @c 0 ).
 * @param yMax The maximum value of the y-axis (maps to index @c xWidth-1 )
 * @param table The 2D array to pull data from. Of size [ @c xWidth ][ @c yWidth ].
 * @param xWidth The width of the table along the x-axis.
 * @param yWidth The width of the table along the y-axis.
 * @return The interpolated value.
 */
float bilinearLookupTable (float x, float y, float xMin, float xMax, float yMin, float yMax,
	const float* table, uint16_t xWidth, uint16_t yWidth);

/**
 * @brief Performs inverse linear interpolation, with hysteresis applied to the input. This means that brief deviations in the
 * direction of the input will not affect the output. Reference the below graph for details.
 *
 * Output (C)
 *   |
 * 1 o                 /-<----<-/-<---<-----
 *   |                /        /
 *   |    Decreasing /        / Increasing
 *   |        Curve /        / Curve
 *   |             /        /
 * 0 o ----->--->-/->---->-/
 *   |
 *   o------------o--------o---o------------- Input (X)
 *                A            B
 *                |--hyst--|
 *
 * @param x The point to interpolate. In the same units as A and B.
 * @param a The point to start interpolation at. When decreasing, this is the point at which the output will hit 0.
 * @param b The point to end interpolation at. When increasing, this is the point at which the output will hit 1.
 * @param hyst The amount of hysteresis to use. This defines the maximum amount of deviation in the input value before the
 * output direction changes.
 * @param increasingCurve Buffer to write the increasing/decreasing state into.
 * @param cPrime Buffer to write the last value of C into.
 * @return The output, C. Always in the range [0, 1].
 */
float inverseLerpHysteresis (float x, float a, float b, float hyst, bool* increasingCurve, float* cPrime);

#endif // LERP_H