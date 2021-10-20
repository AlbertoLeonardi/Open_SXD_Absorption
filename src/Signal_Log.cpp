// Signal Functions :: Log
//
// Handles Message Signals

#include "Signal_Log.h"

void Log_Message_stdou(const char Message[])
{
	printf(" ... %s\n", Message);
	return;
}
void Log_Message_file(const char Message[])
{
	if (_SIGNAL_MESSAGE_LOG_FILE_POINTER == NULL) return;
	
	fprintf_s(_SIGNAL_MESSAGE_LOG_FILE_POINTER, " ... %s", Message);
	
	return;
}
void Log_Message(const char Message[])
{
	if (strlen(Message)>0)
	{
		_LOG_MESSAGE_STDOU(Message);
		_LOG_MESSAGE_FILE(Message);
	}
	return;
}

void Log_Stream_stdou(const char Comment[])
{
	printf("%s", Comment);
	return;
}
void Log_Stream_file(const char Comment[])
{
	if (_SIGNAL_STREAM_LOG_FILE_POINTER == NULL) return;
	
	fprintf_s(_SIGNAL_STREAM_LOG_FILE_POINTER, "%s", Comment);
	
	return;
}
void Log_Stream(FILE* OutFile, const char Comment[])
{
	if (strlen(Comment)>0)
	{
		_LOG_STREAM_STDOU(Comment);
		_LOG_STREAM_FILE(Comment);
		if (OutFile != NULL)fprintf_s(OutFile, "%s", Comment);
	}
	return;
}