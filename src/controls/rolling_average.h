#ifndef ROLLING_AVERAGE_H
#define ROLLING_AVERAGE_H

// Rolling Average ------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.01.11
//
// Description: Code for performing a rolling average (or 'boxcar') filter.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdlib.h>

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the rolling average of a signal.
 * @param x The most recent value of the sample.
 * @param w Array of the signal's history. Must be at least of size @c n-1 .
 * @param n The number of values to average.
 * @return The most recent value of the rolling average.
 */
float rollingAverageCalculate (float x, float* w, size_t n);

#endif // ROLLING_AVERAGE_H
