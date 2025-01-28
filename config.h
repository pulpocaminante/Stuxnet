#pragma once
// This is the configuration file for the Elevated Ultron Installer


/* -----------------------------------------------------------------------------------*/
/*        Anti-Analysis        */

// If 1, the presence of a debugger or VM will cause a BSOD
#define BSOD 0

// The minimum number of VM indicators needed to trigger a BSOD
#define VM_INDICATOR_MINIMUM 1


/* -----------------------------------------------------------------------------------*/
/*        Config Extras        */

// If in release mode, debug strings will not be compiled
// into the binary
#ifdef _DEBUG
#define ILog(data, ...) printf(data, __VA_ARGS__)
#else
#define ILog(data, ...)
#endif