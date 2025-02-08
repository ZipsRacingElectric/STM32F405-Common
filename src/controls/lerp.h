#ifndef LERP_H
#define LERP_H

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
 */
float bilinearInterpolation (float x3, float y3, float x1, float y1, float x2, float y2,
	float z11, float z12, float z21, float z22);

#endif // LERP_H