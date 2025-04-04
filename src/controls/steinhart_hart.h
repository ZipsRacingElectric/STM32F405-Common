#ifndef STEINHART_HART_H
#define STEINHART_HART_H

// TODO(Barach): This version or only 3 coefficients?

/**
 * @brief Converts a thermistor's resistance to a temperature based on the thermistor's Steinhart-Hart coefficients.
 * @note The version of the Steinhart-Hart equation implemented uses 4 coefficients, taking the form:
 * 1/T = A + B * ln (R / R_Ref) + C * ln (R / R_Ref)^2 + D * ln (R / R_Ref)^3
 * @param measuredResistance The resistance to convert.
 * @param referenceResistance The reference resistance of the thermistor. Use 1 if the coefficients do not incorporate this.
 * @param a The A coefficient (ln^0).
 * @param b The B coefficient (ln^1)
 * @param c The C coefficient (ln^2)
 * @param d The D coefficient (ln^3)
 * @return The converted temperature, in degrees Celsius.
 */
float steinhartHartTemperature (float measuredResistance, float referenceResistance, float a, float b, float c, float d);

#endif // STEINHART_HART_H