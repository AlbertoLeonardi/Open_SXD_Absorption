// Operative-System / Compiler Based Library and Function Definitions
//
// Setup external library

#if   _BUILD_OS=='L'
	#include <OS_Linux.h>
#elif _BUILD_OS=='V'
	#include <OS_VisualStudio.h>
#elif _BUILD_OS=='D'
	#include <OS_DevCpp.h>
#endif