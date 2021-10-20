// Siignal Functions :: Warnings
//
// Handles Warning Signals

#include "Signal_Warning.h"

void Warning_Message_Log_stdou(const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id)
{
	printf("\n");
	printf(" *** WARNING ! %s : %s [%ld]\n", Library, Function, Id);
	printf(" *** TYPE ...: %s\n", Standard_Type);
	if (strlen(Message) > 0) printf(" *** ISSUE ..: %s // %ld\n", Message, Value);
	printf("\n");
	
	return;
}
void Warning_Message_Log_file(const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id)
{
	char Log[_Nch800], LogEntry[_Nch200];
	
	if (_APP_LOG_FILE_POINTER == NULL) return;
	
	memset(&Log[0], 0, _Nch800*sizeof(char));
	
	memset(&LogEntry[0], 0, _Nch200*sizeof(char));								sprintf_s(&LogEntry[0], _Nch200, "\n");														strcat_s(&Log[0], _Nch800, &LogEntry[0]);
	memset(&LogEntry[0], 0, _Nch200*sizeof(char));								sprintf_s(&LogEntry[0], _Nch200, " *** WARNING ! %s : %s [%ld]\n", Library, Function, Id);	strcat_s(&Log[0], _Nch800, &LogEntry[0]);
	memset(&LogEntry[0], 0, _Nch200*sizeof(char));								sprintf_s(&LogEntry[0], _Nch200, " *** TYPE ...: %s\n", Standard_Type);						strcat_s(&Log[0], _Nch800, &LogEntry[0]);
	memset(&LogEntry[0], 0, _Nch200*sizeof(char));	if (strlen(Message) > 0)	sprintf_s(&LogEntry[0], _Nch200, " *** ISSUE ..: %s // %ld\n", Message, Value);				strcat_s(&Log[0], _Nch800, &LogEntry[0]);
	memset(&LogEntry[0], 0, _Nch200*sizeof(char));								sprintf_s(&LogEntry[0], _Nch200, "\n");														strcat_s(&Log[0], _Nch800, &LogEntry[0]);
	
	fprintf_s(_APP_LOG_FILE_POINTER, "%s",Log);
	
	return;
}
int  Warning_Message_Log(void *Pointer, const char Library[], const char Function[], const char Standard_Type[], const char Message[], long Value, long Id)
{
	if (Pointer == NULL)
	{
		_WARNING_MESSAGE_STDOU(Library, Function, Standard_Type, Message, Value, Id);
		_WARNING_MESSAGE_FILE(Library, Function, Standard_Type, Message, Value, Id);
		return 1;
	}
	return 0;
}