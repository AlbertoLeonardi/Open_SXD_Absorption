// System :: Redefinition of standard library functions due to mismatch between unsafe and secure functions
//
// 

#ifndef _INCLUDE_SYS_SECURE_FUNCTIONS_REDEFINITION
#define _INCLUDE_SYS_SECURE_FUNCTIONS_REDEFINITION 1

#include <App_Environment.h>

#ifdef _SYS_SECURE_FUNCTIONS_REDEFINITION

int my_strcpy_s(char * destination, size_t destsz, const char * source);

#endif

#endif