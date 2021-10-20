// Operative-System / Compiler Based Library and Function Definitions
//
// Linux OS - 

#ifndef _INCLUDE_OS_LINUX
#define _INCLUDE_OS_LINUX 1

#if _BUILD_OS=='L'

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT LIBRARY-DEFINEs  **** //
// **** **** **** **** **** **** **** **** //

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
	#include "cuda_runtime.h"
	#include "device_launch_parameters.h"
#endif

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT INCLUDE           ****//
// **** **** **** **** **** **** **** **** //

#include <unistd.h>
#include <stdbool.h>

#include <termios.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

// **** **** **** **** **** **** **** **** //
// **** OS DEPENDENT DEFINITIONS      **** //
// **** **** **** **** **** **** **** **** //

#define FILE_MODE_F F_OK
#define FILE_MODE_R R_OK
#define FILE_MODE_W W_OK

// **** **** **** **** **** **** **** **** //
// **** SECURE FUNCTION REDEFINITION  **** //
// **** **** **** **** **** **** **** **** //

#define _SYS_LINUX_FUNCTIONS_REDEFINITION
#include "Sys_LinuxFunctionsReDefinition.h"

#define _SYS_SECURE_FUNCTIONS_REDEFINITION
#include "Sys_SecureFunctionsReDefinition.h"

#define _mkdir(path)						mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

#define _getch(...)							getch(__VA_ARGS__)
#define _getcwd(...)						getcwd(__VA_ARGS__)

#define gets_s(destination,size)			fgets(destination,size,stdin)
#define scanf_s(...)						scanf(__VA_ARGS__)

#define strtok_s(str, delim, ptr)			strtok(str,delim)

#define sscanf_s(...)						sscanf(__VA_ARGS__)

#define my_scanf_string(ptr,size)			scanf("%s",ptr)

#define strcat_s(destination,size,source)	strcat(destination,source)
#define strcpy_s(destination,size,source)	my_strcpy_s(destination,size,source) // needs to be redefined because the unsafe version of strcpy return the destination instead of a possible error value
#define sprintf_s(buffer,size,...)			sprintf(buffer,__VA_ARGS__)

#define fprintf_s(...)						fprintf(__VA_ARGS__)
#define fscanf_s(...)						fscanf(__VA_ARGS__)

#define ctime_s(str,size,time_t)			sprintf(str,"%s",ctime(time_t))

#define _access_s(...)						access(__VA_ARGS__)

#endif

#endif