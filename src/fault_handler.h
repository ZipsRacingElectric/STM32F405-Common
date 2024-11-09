#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

// Fault Handler --------------------------------------------------------------------------------------------------------------
//
// Author: Uli
// Date Created: 2013.12.06
//
// Description: If a serious error occurs, one of the fault exception vectors in this file will be called. This file attempts
//   to aid the unfortunate debugger to blame someone for the crashing code
// 
// Source: https://forum.chibios.org/viewtopic.php?f=3&t=1305&start=20#p31353
//
// Released under the CC0 1.0 Universal (public domain)

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables backtrace.
#define DEBUG_BACKTRACE 0

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Function to be called when an unrecoverable fault occurs. This function is application specific, meaning the user
 * must define this. Unrecoverable faults include a hard fault, memory fault, bus fault, and unhandled NMIs.
 */
void hardFaultCallback (void);

#endif // FAULT_HANDLER_H