// Application Output - Log Statements
//
// Output running status informations

#ifndef _INCLUDE_APP_OUTPUT_LOG
#define _INCLUDE_APP_OUTPUT_LOG 1

#include <App_Environment.h>

#include <Signal_Warning.h>
#include <Signal_Log.h>

#include <Utility_File_IO.h>

int  App_LogFile_open_default();
int  App_LogFile_move(char Dest_Folder[], char Dest_Name[]);
int  App_LogFile_flush();
int  App_LogFile_close();

#define _OUTPUT_HEAD_0 " *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** \n"
#define _OUTPUT_HEAD_1 "  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  \n"

#endif