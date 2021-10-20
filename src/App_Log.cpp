// Application Output - Log Statements
//
// Output running status informations

#include "App_Log.h"

int  App_LogFile_open_default()
{
	char  buffer[_Nch600];
	
	memset(&buffer[0], 0, _Nch600*sizeof(char));
	
	_getcwd(&buffer[0], _Nch600*sizeof(char));
	
	strcat_s(&buffer[0], _Nch600, "/");
	strcat_s(&buffer[0], _Nch600, _APP_INPUT_LOCATION_SUB_FOLDER);
	strcat_s(&buffer[0], _Nch600, "/log.");
	strcat_s(&buffer[0], _Nch600, _APP_INPUT_LOCATION_NAME);
	
	if ((_APP_LOG_FILE_POINTER = File_IO_Open(buffer, "w")) != NULL) return 0; else return 1;
}
int  App_LogFile_move(char Dest_Folder[], char Dest_Name[])
{
	char  OldPath[_Nch500], NewPath[_Nch500];
	
	memset(&OldPath[0], 0, _Nch500*sizeof(char));
	
	_getcwd(&OldPath[0], _Nch500*sizeof(char));
	
	strcat_s(&OldPath[0], _Nch500, "/");
	strcat_s(&OldPath[0], _Nch500, _APP_INPUT_LOCATION_SUB_FOLDER);
	strcat_s(&OldPath[0], _Nch500, "/");
	strcat_s(&OldPath[0], _Nch500, _SN_LOG);
	strcat_s(&OldPath[0], _Nch500, _APP_INPUT_LOCATION_NAME);
	
	memset(&NewPath[0], 0, _Nch500 *sizeof(char));
	sprintf_s(&NewPath[0], _Nch500, "%s/%s%s%s", Dest_Folder, _SN_LOG, Dest_Name, _EXT_TXT);
	
	if ((_APP_LOG_FILE_POINTER = File_IO_Move(OldPath, NewPath, _APP_LOG_FILE_POINTER, "a")) != NULL) return 0; else return 1;
}
int  App_LogFile_flush()           { if (_APP_LOG_FILE_POINTER != NULL) return fflush(_APP_LOG_FILE_POINTER); else return 1; }
int  App_LogFile_close()
{
	time_t RunTime;
	char  RunTimeDate[_Nch100];
	
	char  buffer[_Nch600];
	
	time(&RunTime);
	ctime_s(&RunTimeDate[0], _Nch100, &RunTime);
	
	Log_Stream(NULL, "\n");
	Log_Stream(NULL, _OUTPUT_HEAD_0);						  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(NULL, _OUTPUT_HEAD_0);						  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(NULL, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "                    %s", RunTimeDate);																			Log_Stream(NULL, buffer);
	Log_Stream(NULL, "\n");
	Log_Stream(NULL, _OUTPUT_HEAD_0);						  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(NULL, _OUTPUT_HEAD_0);						  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(NULL, "\n");
	
	if (_APP_LOG_FILE_POINTER != NULL) fclose(_APP_LOG_FILE_POINTER);
	
	return 0;
}

