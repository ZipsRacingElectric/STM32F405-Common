#ifndef TRANSFER_FUNCTION_H
#define TRANSFER_FUNCTION_H

// Transfer Function Filtering ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.25
//
// Description: Object and functions related to discrete-time transfer functions.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdlib.h>

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Filters a sampled value using the specified transfer function.
 * @param x The sampled point.
 * @param a The coefficients of the denominator of the transfer function. The index indicates the power of the z term it is
 * associated with.
 * @param b The coefficients of the numerator of the transfer function. The index indicates the power of the z term it is
 * associated with.
 * @param w The state vector of the system. The index indicates the time delay each value is associated with.
 * @param n The order of the transfer function. The @c a , @c b , and @c w arrays must be of length @c n + 1.
 * @return The value after filtering.
 */
float transferFunctionFilter (float x, float* a, float* b, float* w, size_t n);

#endif // TRANSFER_FUNCTION_H