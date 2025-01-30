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
 * @brief Performs 2D linear interpolation from the the points (xa, ya) and (xb, yb).
 * @note This can be used for inverse 2D linear interpolation by swapping the x and y coordinates.
 * @param x The input point's x-coordinate.
 * @param xa Point A's x-coordinate.
 * @param ya Point A's y-coordinate.
 * @param xb Point B's x-coordinate.
 * @param yb Point B's y-coordinate.
 * @return The y-coordinate of the interpolated point.
 */
float lerp2d (float x, float xa, float ya, float xb, float yb);

#endif // LERP_H