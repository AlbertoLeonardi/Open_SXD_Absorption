// Operative-System / Compiler Based Library and Function Definitions
//
// Microsoft OS - Dev-C++ Compiler

#ifndef _INCLUDE_OS_MICROSOFT_DEVCpp
#define _INCLUDE_OS_MICROSOFT_DEVCpp 1

#if _BUILD_OS=='D'
// **** Any Compile/OS Environment **** //

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <string.h>

#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>

#ifdef _NVIDIA_CUDA_SUPPORT
	#include "cuda_runtime.h"
	#include "device_launch_parameters.h"
#endif

// **** Specific Compile/OS Environment **** //

#include <unistd.h>
#include <conio.h>
#include <unistd.h>
#include <stdbool.h>

#define SYS_SECURE_FUNCTIONS_REDEFINITION
#include "Sys_SecureFunctionsReDefinition.h"

#define _getch(...)							getch(...)
#define _getcwd(...)						getcwd(...)

#define gets_s(destination,size)			gets(destination)
#define scanf_s(...)						scanf(__VA_ARGS__)

#define strtok_s(str, delim, ptr)			strtok(str,delim)

#define sscanf_s(...)						sscanf(__VA_ARGS__)

#define my_scanf_string(ptr,size)			scanf("%s",ptr)

#define strcat_s(destination,size,source)	strcat(destination,source)
#define strcpy_s(destination,size,source)	my_strcpy_s(destination,size,source) // needs to be redefined because the unsafe version of strcpy return the destination instead of a possible error value
#define sprintf_s(buffer,size,string,...)	sprintf(buffer,string,__VA_ARGS__)

#define fprintf_s(...)						fprintf(__VA_ARGS__)
#define fscanf_s(...)						fscanf(__VA_ARGS__)

#define ctime_s(str,size,time_t)			sprintf(str,"%s",ctime(time_t))

#endif

#endif