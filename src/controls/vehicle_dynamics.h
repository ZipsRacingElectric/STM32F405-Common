#ifndef VEHICLE_DYNAMICS_H
#define VEHICLE_DYNAMICS_H

/**
 * @brief Converts a motor speed into the ground speed of the vehicle (not accounting for tire slip).
 * @param motorSpeed The speed of the motor, in RPM.
 * @param gearRatio The gear ratio of the gearbox, ex. 14 for 14:1, 7 for 14:2.
 * @param tireDiameter The diameter of the tire, in inches.
 * @return The ground speed of the vehicle, in km/h.
 */
float motorSpeedToGroundSpeed (float motorSpeed, float gearRatio, float tireDiameter);

#endif // VEHICLE_DYNAMICS_H