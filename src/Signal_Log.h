// Signal Functions :: Log
//
// Note: Log messages are written in log file if _SIGNAL_LOG_FILE is set to 'Y'
//       By default the log-file is pointed by FILE *_SIGNAL_LOG_FILE_POINTER
//       The log-file pointer "name" is redefined with preprocessor function.
//       The log-file pointer is than:
//          FILE *_APP_LOG_FILE_POINTER ... if _SIGNAL_LOG_FILE is set to 'Y'
//          NULL                        ... if _SIGNAL_LOG_FILE is set to 'N'

#ifndef _INCLUDE_SIGNAL_LOG
#define _INCLUDE_SIGNAL_LOG 1

#include <App_Environment.h>

#if _SIGNAL_MESSAGE_EXCLUDE=='N'
#define _LOG_MESSAGE(Message)	Log_Message(Message)
#else
#define _LOG_MESSAGE(Message);
#endif

#if _SIGNAL_MESSAGE_STDOU=='Y'
#define _LOG_MESSAGE_STDOU(Message)	Log_Message_stdou(Message)
#else
#define _LOG_MESSAGE_STDOU(Message);
#endif

#if _SIGNAL_MESSAGE_FILE=='Y'
#define _LOG_MESSAGE_FILE(Message)	Log_Message_file(Message)
#define _SIGNAL_MESSAGE_LOG_FILE_POINTER _APP_LOG_FILE_POINTER 
#else
#define _LOG_MESSAGE_FILE(Message);
#define _SIGNAL_MESSAGE_LOG_FILE_POINTER NULL
#endif


#if _SIGNAL_STREAM_STDOU=='Y'
#define _LOG_STREAM_STDOU(Comment)	Log_Stream_stdou(Comment)
#else
#define _LOG_STREAM_STDOU(Comment);
#endif

#if _SIGNAL_STREAM_FILE=='Y'
#define _LOG_STREAM_FILE(Comment)	Log_Stream_file(Comment)
#define _SIGNAL_STREAM_LOG_FILE_POINTER _APP_LOG_FILE_POINTER 
#else
#define _LOG_STREAM_FILE(Comment);
#define _SIGNAL_STREAM_LOG_FILE_POINTER NULL
#endif



void Log_Message_stdou(const char Message[]);
void Log_Message_file(const char Message[]);
void Log_Message(const char Message[]);

void Log_Stream_stdou(const char Comment[]);
void Log_Stream_file(const char Comment[]);
void Log_Stream(FILE* OutFile, const char Comment[]);

#endif