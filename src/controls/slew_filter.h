#ifndef SLEW_FILTER_H
#define SLEW_FILTER_H

// Slew Filtering ------------------------------------------------------------------------------------------------
//
// Author: Jake Nowak
// Date Created: 2026.10.7
//
// Description: Object and functions realted to slew filtering.

// Includes -------------------------------------------------------------------------------------------------------------------

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief 
 * @param target 
 * @param current 
 * @param rate 
 * @param delatTime 
 * @return float 
 */
float slewRateLimit(float target, float current, float applicationRate, float releaseRate, float deltaTime);

#endif // SLEW_FILTER_H