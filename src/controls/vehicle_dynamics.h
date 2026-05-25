#ifndef VEHICLE_DYNAMICS_H
#define VEHICLE_DYNAMICS_H

// Vehicle Dynamics -----------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.05.25
//
// Description: Library of general physics, geometry, and vehicle dynamics math.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#define _USE_MATH_DEFINES
#include "math.h"

// Unit Conversions -----------------------------------------------------------------------------------------------------------

// Distance

#define KILOMETERS_PER_METER				0.001f
#define METERS_PER_KILOMETER				1000.0f
#define METERS_TO_KILOMETERS(meters)		((meters) * KILOMETERS_PER_METER)
#define KILOMETERS_TO_METERS(kilometers)	((kilometers) * METERS_PER_KILOMETER)

#define METERS_PER_INCH						0.0254f
#define INCHES_PER_METER					(1 / 0.0254f)
#define INCHES_TO_METERS(inches)			((inches) * METERS_PER_INCH)
#define METERS_TO_INCHES(meters)			((meters) * INCHES_PER_METER)

// Time

#define MINUTES_PER_SECOND					(1 / 60.0f)
#define SECONDS_PER_MINUTE					60.0f
#define SECONDS_TO_MINUTES(seconds)			((seconds) * MINUTES_PER_SECOND)
#define MINUTES_TO_SECONDS(minutes)			((minutes) * SECONDS_PER_MINUTE)

#define HOURS_PER_MINUTE					(1 / 60.0f)
#define MINUTES_PER_HOUR					60.0f
#define MINUTES_TO_HOURS(minutes)			((minutes) * HOURS_PER_MINUTE)
#define HOURS_TO_MINUTES(hours)				((hours) * MINUTES_PER_HOUR)

// Angles

#define RADIANS_PER_DEGREE					(M_PI / 180.0f)
#define DEGREES_PER_RADIAN					(180.0f / M_PI)
#define DEGREES_TO_RADIANS(degrees)			((degrees) * RADIANS_PER_DEGREE)
#define RADIANS_TO_DEGREES(radians)			((radians) * DEGREES_PER_RADIAN)

// Geometry -------------------------------------------------------------------------------------------------------------------

#define RADIUS_TO_DIAMETER(radius)			((radius) * 2.0f * M_PI)
#define DIAMETER_TO_RADIUS(diameter)		((diameter) / (2.0f * M_PI))

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts the angle of the vehicle's steering wheel to the angle of the FL wheel.
 * @note This currently does not account for Ackermann angle.
 * @param steeringAngle The angle of the vehicle's steering wheel, in degrees.
 * @param steeringRatio The reduction ratio of the vehicle's steering rack.
 * @return The angle of the FL wheel, in degrees.
 */
static inline float steeringAngleToFlWheelAngle (float steeringAngle, float steeringRatio)
{
	return steeringAngle / steeringRatio;
}

/**
 * @brief Converts the angle of the vehicle's steering wheel to the angle of the FR wheel.
 * @note This currently does not account for Ackermann angle.
 * @param steeringAngle The angle of the vehicle's steering wheel, in degrees.
 * @param steeringRatio The reduction ratio of the vehicle's steering rack.
 * @return The angle of the FR wheel, in degrees.
 */
static inline float steeringAngleToFrWheelAngle (float steeringAngle, float steeringRatio)
{
	return steeringAngle / steeringRatio;
}

/**
 * @brief Converts a motor speed into the ground speed of the vehicle (not accounting for tire slip).
 * @param motorSpeed The speed of the motor, in RPM.
 * @param gearRatio The gear ratio of the gearbox.
 * @param tireRadius The radius of the tire, in inches.
 * @return The ground speed of the vehicle, in km/h.
 */
static inline float motorSpeedToGroundSpeed (float motorSpeed, float gearRatio, float tireRadius)
{
	return motorSpeed											// Motor speed (RPM)
		/ gearRatio												// Convert to wheel speed (RPM)
		* RADIUS_TO_DIAMETER (INCHES_TO_METERS (tireRadius))	// Convert to ground speed (m/min)
		* MINUTES_PER_HOUR										// Convert to ground speed (m/h)
		* KILOMETERS_PER_METER;									// Convert to ground speed (km/h)
}

/**
 * @brief Converts the ground speed of the vehicle into a motor speed (not accounting for tire slip).
 * @param groundSpeed The ground speed of the vehicle, in km/h.
 * @param gearRatio The gear ratio of the gearbox.
 * @param tireRadius The radius of the tire, in inches.
 * @return The ground speed of the vehicle, in km/h.
 */
static inline float groundSpeedToMotorSpeed (float groundSpeed, float gearRatio, float tireRadius)
{
	return groundSpeed											// Ground speed (km/h)
		* METERS_PER_KILOMETER									// Convert to ground speed (m/h)
		* HOURS_PER_MINUTE										// Convert to ground speed (m/min)
		/ RADIUS_TO_DIAMETER (INCHES_TO_METERS (tireRadius))	// Convert to wheel speed (RPM)
		* gearRatio;											// Convert to motor speed (RPM)
}

/**
 * @brief Calculates a scalar that transfers the RL motor's torque request into a moment applied to the vehicle's yaw axis (in
 * the clockwise direction). This value is unitless (Nm of yaw moment per Nm of motor torque).
 * @param trackWidthRear The track width of the vehicle's rear axle, in meters.
 * @param gearRatio The gear ratio of the gearbox.
 * @param wheelRadius The radius of the tire, in inches.
 * @return The calculated transfer scalar.
 */
static inline float motorRlYawMomentTransfer (float trackWidthRear, float gearRatio, float wheelRadius)
{
	return gearRatio * trackWidthRear / (2.0f * INCHES_TO_METERS (wheelRadius));
}

/**
 * @brief Calculates a scalar that transfers the RR motor's torque request into a moment applied to the vehicle's yaw axis (in
 * the clockwise direction). This value is unitless (Nm of yaw moment per Nm of motor torque).
 * @param trackWidthRear The track width of the vehicle's rear axle, in meters.
 * @param gearRatio The gear ratio of the gearbox.
 * @param wheelRadius The radius of the tire, in inches.
 * @return The calculated transfer scalar.
 */
static inline float motorRrYawMomentTransfer (float trackWidthRear, float gearRatio, float wheelRadius)
{
	return -motorRlYawMomentTransfer (trackWidthRear, gearRatio, wheelRadius);
}

/**
 * @brief Calculates a scalar that transfers the FL motor's torque request into a moment applied to the vehicle's yaw axis (in
 * the clockwise direction). This value is unitless (Nm of yaw moment per Nm of motor torque).
 * @param wheelBase The wheel base of the vehicle, in meters.
 * @param frontRearWeightBias The front-to-rear bias of the vehicle's weight distribution. 0 => 100% rearwards, 1 => 100%
 * frontwards.
 * @param trackWidthFront The track width of the vehicle's front axle, in meters.
 * @param gearRatio The gear ratio of the gearbox.
 * @param wheelRadius The radius of the tire, in inches.
 * @param wheelAngle The angle the FL wheel is steering in the clockwise direction, in degrees.
 * @return The calculated transfer scalar.
 */
static inline float motorFlYawMomentTransfer (float wheelBase, float frontRearWeightBias, float trackWidthFront,
	float gearRatio, float wheelRadius, float wheelAngle)
{
	// TODO(Barach): This is very slow, need to optimize.
	float x = trackWidthFront / 2.0f;
	float y = wheelBase * frontRearWeightBias;
	float l = sqrtf (x * x + y * y);
	float a = atanf (y / x);

	return gearRatio * l / INCHES_TO_METERS (wheelRadius) * cosf (a - wheelAngle);
}

/**
 * @brief Calculates a scalar that transfers the FR motor's torque request into a moment applied to the vehicle's yaw axis (in
 * the clockwise direction). This value is unitless (Nm of yaw moment per Nm of motor torque).
 * @param wheelBase The wheel base of the vehicle, in meters.
 * @param frontRearWeightBias The front-to-rear bias of the vehicle's weight distribution. 0 => 100% rearwards, 1 => 100%
 * frontwards.
 * @param trackWidthFront The track width of the vehicle's front axle, in meters.
 * @param gearRatio The gear ratio of the gearbox.
 * @param wheelRadius The radius of the tire, in inches.
 * @param steerAngle The angle the FR wheel is steering in the clockwise direction, in degrees.
 * @return The calculated transfer scalar.
 */
static inline float motorFrYawMomentTransfer (float wheelBase, float frontRearWeightBias, float trackWidthFront,
	float gearRatio, float wheelRadius, float wheelAngle)
{
	return -motorFlYawMomentTransfer (wheelBase, frontRearWeightBias, trackWidthFront, gearRatio, wheelRadius, -wheelAngle);
}

#endif // VEHICLE_DYNAMICS_H