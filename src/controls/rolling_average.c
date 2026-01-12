// Header
#include "rolling_average.h"

float rollingAverageCalculate (float x, float* w, size_t n)
{
	// If only 1 sample (or an invalid value of 0) is specified, return without averaging.
	if (n <= 1)
		return x;

	// Sum each value, starting with the most recent.
	float sum = x;

	for (int index = n - 1; index > 0; --index)
	{
		// Shift each element back 1 in the array. Don't shift out the final value, as there is no need.
		if (index != (int) n - 1)
			w [index] = w [index - 1];

		// Add each element to the sum.
		sum += w [index - 1];
	}

	// Set the most recent value
	w [0] = x;

	// Return the average.
	return sum / n;
}