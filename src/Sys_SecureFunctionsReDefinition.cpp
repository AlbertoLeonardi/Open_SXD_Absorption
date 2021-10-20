// System :: Redefinition of standard library functions due to mismatch between unsafe and secure functions
//
// 

#include "Sys_SecureFunctionsReDefinition.h"

#ifdef _SYS_SECURE_FUNCTIONS_REDEFINITION

int my_strcpy_s(char * destination, size_t destsz, const char * source)
{
	if (strlen(source) > destsz - 2)    return 1;
	else { strcpy(destination, source); return 0; }
}

#endif