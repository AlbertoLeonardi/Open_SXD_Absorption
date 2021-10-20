// Signal Functions :: Warnings
//
// Note: Warning messages are written in log file if _SIGNAL_WARNING_LOG_FILE is set to 'Y'
//       By default the log-file is pointed by FILE *_SIGNAL_WARNING_LOG_FILE_POINTER
//       The log-file pointer "name" is redefined with preprocessor function.
//       The log-file pointer is than:
//          FILE *_APP_LOG_FILE_POINTER ... if _SIGNAL_WARNING_LOG_FILE is set to 'Y'
//          NULL                        ... if _SIGNAL_WARNING_LOG_FILE is set to 'N'

#ifndef _INCLUDE_SIGNAL_WARNING
#define _INCLUDE_SIGNAL_WARNING 1

#include <App_Environment.h>

// Standard Error Key signal Messages
// Memory
#define _WARNING_MESSAGE_KEY_ALLOC			"Memory allocation issue: any"
#define _WARNING_MESSAGE_KEY_CALLOC			"Memory allocation issue: calloc"
#define _WARNING_MESSAGE_KEY_MALLOC			"Memory allocation issue: malloc"
#define _WARNING_MESSAGE_KEY_REALLOC		"Memory allocation issue: realloc"
// Data
#define _WARNING_MESSAGE_KEY_VALUE			"Unexpected / Not valid data value"
#define _WARNING_MESSAGE_KEY_USER			"Unexpected / Not valid user entry"
// File
#define _WARNING_MESSAGE_KEY_FILE_Fn		"File not existing"
#define _WARNING_MESSAGE_KEY_FILE_Fy		"File existing"
#define _WARNING_MESSAGE_KEY_FILE_R			"File not accessible for reading"
#define _WARNING_MESSAGE_KEY_FILE_W			"File not accessible for writing"
#define _WARNING_MESSAGE_KEY_FILE_X			"File not accessible for either read or write"
// Directory
#define _WARNING_MESSAGE_KEY_PATH_R			"Directory path not found"
#define _WARNING_MESSAGE_KEY_PATH_W			"Directory path not created"
// Function Error
#define _WARNING_MESSAGE_KEY_UNKNOWN		"Unknown function behavior or output"
// Pthread Error
#define _WARNING_MESSAGE_KEY_THREAD_CREATE	"Unknown pthread_create error"
// CUDA Error
#define _WARNING_MESSAGE_KEY_CUDA_DEVICE	"CUDA: Device not available"
#define _WARNING_MESSAGE_KEY_CUDA_MALLOC	"CUDA: Memory allocation issue"
#define _WARNING_MESSAGE_KEY_CUDA_MEMSET	"CUDA: Memory memset issue"
#define _WARNING_MESSAGE_KEY_CUDA_MEMCPY	"CUDA: Memory copy issue"
//

#if _SIGNAL_WARNING_EXCLUDE=='N'
#define _APP_WARNING_MESSAGE(...)	Warning_Message_Log(__VA_ARGS__)
#else
#define _APP_WARNING_MESSAGE(...)	0
#endif

#if _SIGNAL_WARNING_STDOU=='Y'
#define _WARNING_MESSAGE_STDOU(...)	Warning_Message_Log_stdou(__VA_ARGS__)
#else
#define _WARNING_MESSAGE_STDOU(...)
#endif

#if _SIGNAL_WARNING_FILE=='Y'
#define _WARNING_MESSAGE_FILE(...)	Warning_Message_Log_file(__VA_ARGS__)
#define _SIGNAL_WARNING_LOG_FILE_POINTER _APP_LOG_FILE_POINTER 
#else
#define _WARNING_MESSAGE_FILE(...)
#define _SIGNAL_WARNING_LOG_FILE_POINTER NULL
#endif

void Warning_Message_Log_stdou(const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id);
void Warning_Message_Log_file(const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id);
int  Warning_Message_Log(void *Pointer, const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id);

#endif