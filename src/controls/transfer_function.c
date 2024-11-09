// Header
#include "transfer_function.h"

float transferFunctionFilter (float x, float* a, float* b, float* w, size_t n)
{
	// Transfer Function Definition:
	//   Y(z) = H(z) * X(z)
	//   y(k) = h(k) ** x(k)
	//     where ** is the convolutional sum.
	//
	//   H(z) = N(z) / D(z) = (b_0 + b_1 * z^-1 + ... + b_n * z^-n) / (a_0 + a_1 * z^-1 + ... + a_n * z^-n)
	//
	//   Y(z) = X(z) * N(z) / D(z)
	//   Y(z) / N(z) = X(z) / D(z)
	//     let W(z) = Y(z) / N(z) = X(z) / D(z)
	//
	//   W(z) = X(z) / D(z)
	//   X(z) = D(z) * W(z)
	//   X(z) = (a_0 + a_1 * z^-1 + ... + a_n * z^-n) W(z)
	//   x(k) = a_0 * w(k) + a_1 * w(k-1) + ... + a_n * w(k-n)
	//   a_0 * w(k) = x(k) - a_1 * w(k-1) - ... - a_n * w(k-n)
	//   w(k) = (x(k) - a_1 * w(k-1) - ... - a_n * w(k-n)) / a_0
	//
	//   W(z) = Y(z) / N(z)
	//   Y(z) = N(z) * W(z)
	//   Y(z) = (b_0 + b_1 * z^-1 + ... + b_n * z^-n) * W(z)
	//   y(k) = b_0 * w(k) + b_1 * w(k-1) + ... + b_n * w(k-n)

	// Delay the state-vector: w(k-i) = w'(k-(i-1))
	for (size_t i = n; i > 0; --i)
		w[i] = w[i - 1];

	// Calculate w(k): w(k) = (x(k) - a_1 * w(k-1) - ... - a_n * w(k-n)) / a_0
	w[0] = x;
	for (size_t i = 1; i < n + 1; ++i)
		w[0] -= a[i] * w[i];
	w[0] /= a[0];

	// Calculate y(k): y(k) = b_0 * w(k) + b_1 * w(k-1) + ... + b_n * w(k-n)
	float y = 0;
	for (size_t i = 0; i < n + 1; ++i)
		y += b[i] * w[i];

	return y;
}