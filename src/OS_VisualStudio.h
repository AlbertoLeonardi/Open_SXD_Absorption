// Operative-System / Compiler Based Library and Function Definitions
//
// Microsoft OS - Visual Studio 13 Compiler

#ifndef _INCLUDE_OS_MICROSOFT_VISUAL_STUDIO
#define _INCLUDE_OS_MICROSOFT_VISUAL_STUDIO 1

#if _BUILD_OS=='V'

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT LIBRARY-DEFINEs  **** //
// **** **** **** **** **** **** **** **** //

// #define _CRT_SECURE_NO_WARNINGS						// Exclude deprecation warnings for older, less secure, functions.
#define _USE_MATH_DEFINES							// Include the definition of math constant such as M_PI
#define _BIND_TO_CURRENT_MFC_VERSION 1				// Bind to the latest (1) version of MFC Microsoft library
#define _BIND_TO_CURRENT_CRT_VERSION 1				// Bind to the latest (1) version of CRT Microsoft library
#define _BIND_TO_CURRENT_ATL_VERSION 1				// Bind to the latest (1) version of ATL Microsoft library
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1			// Bind to the latest (1) version of VCLIBS Microsoft library

// **** **** **** **** **** **** **** **** //
// **** OS IN-DEPENDENT INCLUDEs       ****//
// **** **** **** **** **** **** **** **** //

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>

#ifdef _NVIDIA_CUDA_SUPPORT
	#include <cuda_runtime.h>
	#include <device_launch_parameters.h>
#endif

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT INCLUDE           ****//
// **** **** **** **** **** **** **** **** //

#include <io.h>
#include <conio.h>
#include <direct.h>

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT DEFINITIONS      **** //
// **** **** **** **** **** **** **** **** //

#ifndef HAVE_STRUCT_TIMESPEC
	#define HAVE_STRUCT_TIMESPEC
	#ifndef _TIMESPEC_DEFINED
		#define _TIMESPEC_DEFINED
		struct timespec
		{
			time_t tv_sec;
			long tv_nsec;
		};
	#endif
#endif

#define FILE_MODE_F 00
#define FILE_MODE_R 04
#define FILE_MODE_W 02

// **** **** **** **** **** **** **** **** //
// **** SECURE FUNCTION REDEFINITION  **** //
// **** **** **** **** **** **** **** **** //

#ifdef _CRT_SECURE_NO_WARNINGS
	#define _SYS_SECURE_FUNCTIONS_REDEFINITION
	#include "Sys_SecureFunctionsReDefinition.h"
	
	//#define gets_s(destination,size)			gets(destination)
	#define scanf_s(...)						scanf(__VA_ARGS__)
	
	#define strtok_s(str, delim, ptr)			strtok(str,delim)
	
	#define sscanf_s(...)						sscanf(__VA_ARGS__)
	
	#define my_scanf_string(ptr,size)			scanf("%s",ptr,size)
	
	#define strcat_s(destination,size,source)	strcat(destination,source)
	#define strcpy_s(destination,size,source)	my_strcpy_s(destination,size,source) // needs to be redefined because the unsafe version of strcpy return the destination instead of a possible error value
	#define sprintf_s(buffer,size,string,...)	sprintf(buffer,string,__VA_ARGS__)
	
	#define fprintf_s(...)						fprintf(__VA_ARGS__)
	#define fscanf_s(...)						fscanf(__VA_ARGS__)
	
	#define ctime_s(str,size,time_t)			sprintf(str,"%s",ctime(time_t))
#else
	#define my_scanf_string(ptr,size)			scanf_s("%s",ptr,size)
#endif

#endif

#endif