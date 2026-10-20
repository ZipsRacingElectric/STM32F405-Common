#include "slew_filter.h"

float slewRateLimit(float target, float current, float applicationRate, float releaseRate, float deltaTime)
{
    float rate;

    // Moving toward zero torque: allow a faster rate.
    if ((current > 0.0f && target < current) ||
        (current < 0.0f && target > current)) 
    {
        rate = releaseRate;
    }
    else 
    {
        rate = applicationRate; 
    }

    float maxChange = rate * deltaTime;
    float error = target - current;

    if (error > maxChange)
        return current + maxChange;

    if (error < -maxChange)  
        return current - maxChange;

    return target;
}