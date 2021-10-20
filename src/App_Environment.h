// Application Environment
//
// Background environment to compile and run the application modules 

#ifndef _INCLUDE_APP_ENVIRONMENT
#define _INCLUDE_APP_ENVIRONMENT 1

#include <App_Setup_Compile.h>
#include <App_Setup_Update.h>

#include <App_OS_Compiler.h>
#include <App_System.h>

// **** Application Release ID and date **** //

#define _APP_VERSION_ID		"00.00"											// Format  :: 00.00
#define _APP_VERSION_DATE	"03.Mar.2021"									// Format  :: 00.Jan.2000

// **** Application default Work-Path and Work-Name **** //

#define _APP_INPUT_LOCATION_BASE_FOLDER		""								// (including the initial '/')						// If it was not defined, the software running/compiling location is used instead
#define _APP_INPUT_LOCATION_SUB_FOLDER		"SXD_Absorption_Input"			// (without the separator '/')						// It has to be defined
#define _APP_INPUT_LOCATION_NAME			"SXD_Absorption_Input.txt"		// (without the separator '/', but final extension)	// It has to be defined
#define _APP_LINEAR_ABSORPTION_NAME         "SXD_LinearAbsorption.txt"		// (without the separator '/', but final extension)	// It has to be defined

#endif
