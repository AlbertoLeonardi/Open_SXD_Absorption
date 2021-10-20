// Utility Functions :: File IO functions
//
// Provides standard and application default access to files

#ifndef _INCLUDE_UTILITY_FILE_IO
#define _INCLUDE_UTILITY_FILE_IO 1

#include <App_Environment.h>

#include <Signal_Warning.h>
#include <Signal_Log.h>

#include <Utility_User_IO.h>
#include <Utility_Statistics.h>

FILE* File_IO_Open(const char Path[], const char Type[]);
FILE *File_IO_Move(const char OldPath[], const char NewPath[], FILE *OldFilePointer, const char Type[]);
int   File_IO_Copy(const char OldPath[], const char NewPath[]);

int   File_IO_Directory_Create(const char Path[], long Path_Size);

const char *File_IO_BoolToString(bool Bvoid);
bool  File_IO_StringToBool(const char Entry[]);
char  File_IO_BoolToChar(bool Bvoid);
bool  File_IO_CharToBool(char Entry);

int File_IO_GetValue_Default(long Start_Id, long Number, char Type, void *Data, const char Default[], long StringSize);
int File_IO_GetValue(const char Name[], const char Default[], FILE *File_my, const char S_KEY[], const char I_KEY[], const char E_KEY[], long Number, char Type, void *Data, long StringSize, char *LastCharRead);													// Spaces, Tabs, and New-Lines are ignored // However, the use of separator characters (I_KEY) is suggested

int File_IO_GetDistribution(const char Name[], const char Default[], FILE *File_my, const char S_KEY[], const char I_KEY[], const char E_KEY[], const char Sequence[], struct Statistics_Distribution *Data, double ScaleFactor, double IntegralProbability);

int File_IO_Search_KeyPhrase(FILE* File_my, const char Entry_Key[], const char PassPhrase[], bool JumpToNextLine, bool Warning_EndOfFile);																															// Spaces, Tabs, and New-Lines before the Pass Phrase are ignored
int File_IO_Search_KeyWord(FILE *File_my, const char Entry_Key[], const char PassWord[], bool JumpToNextLine, bool Exclude_Warning);																																// Spaces and Tabs between key- and pass-words are ignored
int File_IO_Search_KeyLine(FILE *File_my, const char Entry_Key[], bool Exclude_Warning);

void File_IO_JumpToNextLine(FILE *File_my);
long File_IO_Get_FileLength(FILE *File_my);

#endif