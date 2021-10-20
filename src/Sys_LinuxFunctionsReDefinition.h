// System :: Redefinition of standard library functions due to mismatch between unsafe and secure functions
//
// 

#ifndef _INCLUDE_SYS_LINUX_FUNCTIONS_REDEFINITION
#define _INCLUDE_SYS_LINUX_FUNCTIONS_REDEFINITION 1

#include <App_Environment.h>

#ifdef _SYS_LINUX_FUNCTIONS_REDEFINITION

inline bool fopen_s(FILE** IO, const char path[], const char type[]) { *IO = NULL; *IO = fopen(path, type); if (IO == NULL) return true; else return false; }

int getch(void);

int localtime_s(struct tm* buf, const time_t* timer);

#endif
#endif