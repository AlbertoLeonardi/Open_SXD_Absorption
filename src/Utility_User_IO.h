// Utility Functions :: User IO
//
// Function for User Interaction

#ifndef _INCLUDE_UTILITY_USER_IO
#define _INCLUDE_UTILITY_USER_IO 1

#include <App_Environment.h>

#include <Signal_Warning.h>
#include <Signal_Log.h>

#if _UTILITY_USER_IO_EXCLUDE=='Y'
#define _USER_IO_WAIT();
#define _USER_IO_ASKVALUE(Request,NewLine);
#define _USER_IO_GETVALUE(Value,Type,StringSize,Default);	User_IO_GetValue_Default(Value,Type,StringSize,Default);
#else
#define _USER_IO_WAIT();									_getch();
#define _USER_IO_ASKVALUE(Request,NewLine);					User_IO_AskValue(Request,NewLine);
#define _USER_IO_GETVALUE(Value,Type,StringSize,Default);	User_IO_GetValue(Value,Type,StringSize);
#endif

void User_IO_AskValue(const char Request[], bool NewLine);

int User_IO_GetValue_Default(void *Value, char Type, long StringSize, const char Default[]);
int User_IO_GetValue(void *Value, char Type, long StringSize);

#endif