// Utility Functions :: File IO functions
//
// Provides standard and application default access to files

#include "Utility_File_IO.h"

FILE *File_IO_Open_Reading(const char Path[], const char Type[])
{
	bool Bvoid;
	
	char FilePath[_Nch800], Message[_Nch1000];
	
	if (Type == NULL) return NULL;
	
	FILE *OUT = NULL;
	
	memset(&FilePath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&FilePath[0], _Nch800, Path);
	
	do
	{
		if (_access_s(FilePath, FILE_MODE_F)) // Not existing file
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Reading", _WARNING_MESSAGE_KEY_FILE_Fn, FilePath, (long)(*Type), 0);
			_USER_IO_ASKVALUE("Do you want try again? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (!Bvoid)
			{
				_USER_IO_ASKVALUE("Do you want try a different path? ", false);
				_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
				if (Bvoid)
				{
					memset(&FilePath[0], 0, _Nch800*sizeof(char));
					_USER_IO_ASKVALUE("Write the full file path: ", true);
					_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
				}
				else return NULL;
			}
		}
		else Bvoid = false;
	}
	while (Bvoid);
	
	do
	{
		if (_access_s(FilePath, FILE_MODE_R)) // Not accessible for reading
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Reading", _WARNING_MESSAGE_KEY_FILE_R, FilePath, (long)(*Type), 1);
			_USER_IO_ASKVALUE("Do you want try again? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (!Bvoid)
			{
				_USER_IO_ASKVALUE("Do you want try a different path? ", false);
				_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
				if (Bvoid)
				{
					memset(&FilePath[0], 0, _Nch800*sizeof(char));
					_USER_IO_ASKVALUE("Write the full file path: ", true);
					_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
				}
				else return NULL;
			}
		}
		else Bvoid = false;
	}
	while (Bvoid);
	
	fopen_s(&OUT, FilePath, Type);
	
	memset(&Message[0], 0, _Nch1000*sizeof(char));
	sprintf_s(&Message[0], _Nch1000, "File Open: %s\n", FilePath);
	_LOG_MESSAGE(Message);
	
	return OUT;
}
FILE *File_IO_Open_Writing(const char Path[], const char Type[])
{
	bool Bvoid;
	
	char FilePath[_Nch800], Message[_Nch1000];
	
	if (Type == NULL) return NULL;
	
	FILE *OUT = NULL;
	
	memset(&FilePath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&FilePath[0], _Nch800, Path);
	
	do
	{
		if (!_access_s(FilePath, FILE_MODE_F)) // File existing
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Writing", _WARNING_MESSAGE_KEY_FILE_Fy, FilePath, (long)(*Type), 0);
			_USER_IO_ASKVALUE("Do you want try owerwrite the exiting file? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (!Bvoid)
			{
				_USER_IO_ASKVALUE("Do you want try a different path? ", false);
				_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
				if (Bvoid)
				{
					memset(&FilePath[0], 0, _Nch800*sizeof(char));
					_USER_IO_ASKVALUE("Write the full file path: ", true);
					_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
				}
				else return NULL;
			}
			else
			{
				if (_access_s(FilePath, FILE_MODE_W)) // Not accessible fro writing
				{
					do
					{
						_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Writing", _WARNING_MESSAGE_KEY_FILE_W, FilePath, (long)(*Type), 1);
						_USER_IO_ASKVALUE("Do you want try again? ", false);
						_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
						if (!Bvoid)
						{
							_USER_IO_ASKVALUE("Do you want try a different path? ", false);
							_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
							if (Bvoid)
							{
								memset(&FilePath[0], 0, _Nch800*sizeof(char));
								_USER_IO_ASKVALUE("Write the full file path: ", true);
								_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
							}
							else return NULL;
						}
					}
					while (Bvoid);
				}
				else Bvoid = false;
			}
		}
		else Bvoid = false;
	}
	while (Bvoid);
	
	fopen_s(&OUT, FilePath, Type);
	
	memset(&Message[0], 0, _Nch1000*sizeof(char));
	sprintf_s(&Message[0], _Nch1000, "File Open: %s\n", FilePath);
	_LOG_MESSAGE(Message);
	
	return OUT;
}
FILE *File_IO_Open_Appending(const char Path[], const char Type[])
{
	bool Bvoid;
	
	char FilePath[_Nch800], Message[_Nch1000];
	
	if (Type == NULL) return NULL;
	
	FILE *OUT = NULL;
	
	memset(&FilePath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&FilePath[0], _Nch800, Path);
	
	if (!_access_s(FilePath, FILE_MODE_F)) // Existing file
	{
		do
		{
			if (_access_s(FilePath, FILE_MODE_R)) // Accesible for read
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Appending", _WARNING_MESSAGE_KEY_FILE_R, FilePath, (long)(*Type), 0);
				_USER_IO_ASKVALUE("Do you want try again? ", false);
				_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
				if (!Bvoid)
				{
					_USER_IO_ASKVALUE("Do you want try a different path? ", false);
					_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
					if (Bvoid)
					{
						memset(&FilePath[0], 0, _Nch800*sizeof(char));
						_USER_IO_ASKVALUE("Write the full file path: ", true);
						_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
					}
					else return NULL;
				}
			}
			else Bvoid = false;
		}
		while (Bvoid);
		do
		{
			if (_access_s(FilePath, FILE_MODE_W)) // Accesible for write
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open_Appending", _WARNING_MESSAGE_KEY_FILE_W, FilePath, (long)(*Type), 0);
				_USER_IO_ASKVALUE("Do you want try again? ", false);
				_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
				if (!Bvoid)
				{
					_USER_IO_ASKVALUE("Do you want try a different path? ", false);
					_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
					if (Bvoid)
					{
						memset(&FilePath[0], 0, _Nch800*sizeof(char));
						_USER_IO_ASKVALUE("Write the full file path: ", true);
						_USER_IO_GETVALUE(&FilePath[0], 's', _Nch800, "\n");
					}
					else return NULL;
				}
			}
			else Bvoid = false;
		}
		while (Bvoid);
	}
	
	fopen_s(&OUT, FilePath, Type);
	
	memset(&Message[0], 0, _Nch1000*sizeof(char));
	sprintf_s(&Message[0], _Nch1000, "File Open: %s\n", FilePath);
	_LOG_MESSAGE(Message);
	
	return OUT;
}

FILE *File_IO_Open(const char Path[], const char Type[])
{
	if (Type == NULL) return NULL;
	
	if      ((!strcmp(Type, "r")) || (!strcmp(Type, "rb"))) return(File_IO_Open_Reading(Path, Type));
	else if ((!strcmp(Type, "w")) || (!strcmp(Type, "wb"))) return(File_IO_Open_Writing(Path, Type));
	else if ((!strcmp(Type, "a")) || (!strcmp(Type, "ab"))) return(File_IO_Open_Appending(Path, Type));
	else { _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Open", _WARNING_MESSAGE_KEY_VALUE, "Unknown value type.", 0, 0); return NULL; }
}
FILE *File_IO_Move(const char OldPath[], const char NewPath[], FILE *OldFilePointer, const char Type[])
{
	bool Bvoid;
	bool FileAlreadyOpen;
	
	char Local_OldPath[_Nch800], Local_NewPath[_Nch800];

	if (!strcmp(OldPath, NewPath)) return OldFilePointer;
	
	memset(&Local_NewPath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&Local_NewPath[0], _Nch800, NewPath);
	
	while (!_access_s(Local_NewPath, FILE_MODE_F)) // File existing
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_Fy, Local_NewPath, 0, 1);
		_USER_IO_ASKVALUE("Do you want overwrite the exiting file? ", false);
		_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
		if (!Bvoid)
		{
			_USER_IO_ASKVALUE("Do you want try a different destination path? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (Bvoid)
			{
				memset(&Local_NewPath[0], 0, _Nch800*sizeof(char));
				_USER_IO_ASKVALUE("Write the full source file path: ", true);
				_USER_IO_GETVALUE(&Local_NewPath[0], 's', _Nch800, "\n");
			}
			else return NULL;
		}
		else if (remove(Local_NewPath)) { _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_W, Local_NewPath, 0, 2); return NULL; }
	}

	memset(&Local_OldPath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&Local_OldPath[0], _Nch800, OldPath);
		
	while (_access_s(Local_OldPath, FILE_MODE_F)) // File not existing
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_Fn, Local_OldPath, 0, 3);
		_USER_IO_ASKVALUE("Do you want try a different source path? ", false);
		_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
		if (Bvoid)
		{
			memset(&Local_OldPath[0], 0, _Nch800*sizeof(char));
			_USER_IO_ASKVALUE("Write the full source file path: ", true);
			_USER_IO_GETVALUE(&Local_OldPath[0], 's', _Nch800, "\n");
		}
		else return NULL;
	}
	
	if (OldFilePointer == NULL)    FileAlreadyOpen = false;
	else { fclose(OldFilePointer); FileAlreadyOpen = true; }
	
	if (rename(OldPath, NewPath)) return NULL;
	
	if (FileAlreadyOpen) return File_IO_Open(NewPath, Type);
	else                 return NULL;
}
int   File_IO_Copy(const char OldPath[], const char NewPath[])
{
	bool Bvoid;
	
	char Local_OldPath[_Nch800], Local_NewPath[_Nch800];
	
	size_t size;
	void* buffer;
	
	FILE* ptr_old, *ptr_new;
	
	if (!strcmp(OldPath, NewPath)) return 0;
	
	memset(&Local_NewPath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&Local_NewPath[0], _Nch800, NewPath);
	
	while (!_access_s(Local_NewPath, FILE_MODE_F)) // File existing
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_Fy, Local_NewPath, 0, 1);
		_USER_IO_ASKVALUE("Do you want overwrite the exiting file? ", false);
		_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
		if (!Bvoid)
		{
			_USER_IO_ASKVALUE("Do you want try a different destination path? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (Bvoid)
			{
				memset(&Local_NewPath[0], 0, _Nch800*sizeof(char));
				_USER_IO_ASKVALUE("Write the full source file path: ", true);
				_USER_IO_GETVALUE(&Local_NewPath[0], 's', _Nch800, "\n");
			}
			else return 1;
		}
		else if (!remove(Local_NewPath)) { _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_W, Local_NewPath, 0, 2); return NULL; }
	}

	memset(&Local_OldPath[0], 0, _Nch800*sizeof(char));
	strcpy_s(&Local_OldPath[0], _Nch800, OldPath);
		
	while (_access_s(Local_OldPath, FILE_MODE_F)) // File not existing
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Move", _WARNING_MESSAGE_KEY_FILE_Fn, Local_OldPath, 0, 3);
		_USER_IO_ASKVALUE("Do you want try a different source path? ", false);
		_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
		if (Bvoid)
		{
			memset(&Local_OldPath[0], 0, _Nch800*sizeof(char));
			_USER_IO_ASKVALUE("Write the full source file path: ", true);
			_USER_IO_GETVALUE(&Local_OldPath[0], 's', _Nch800, "\n");
		}
		else return 1;
	}
	
	if (fopen_s(&ptr_old, &Local_OldPath[0], "rb")) return 1;
	if (fopen_s(&ptr_new, &Local_NewPath[0], "wb")) return 1;
	
	fseek(ptr_old, 0, SEEK_END);	// seek to end of file
	size = ftell(ptr_old);			// get current file pointer, i.e., file size in byte
	
	if (size <= 0)
	{
		fclose(ptr_new);
		fclose(ptr_old);
		return 0;
	}
	
	buffer = malloc(size);
	
	rewind(ptr_old);
	fread(buffer, size, 1, ptr_old);
	fwrite(buffer, size, 1, ptr_new);
	fflush(ptr_new);
	
	fclose(ptr_new);
	fclose(ptr_old);
	
	free(buffer);
	
	return 0;
}

int   File_IO_Directory_Create(const char Path[], long Path_Size)
{
	bool Bvoid;
	void* ptr = (void*)(&Path[0]);
	
	while (_access_s(Path, FILE_MODE_F)) // Path not existing
	{
		if (_mkdir(Path))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Directory_Create", _WARNING_MESSAGE_KEY_PATH_W, Path, 0, 0);
			_USER_IO_ASKVALUE("Do you want try a different path? ", false);
			_USER_IO_GETVALUE(&Bvoid, 'b', 0, "n");
			if (Bvoid)
			{
				memset(ptr, 0, Path_Size * sizeof(char));
				_USER_IO_ASKVALUE("Write the full source path: ", true);
				_USER_IO_GETVALUE(ptr, 's', Path_Size, "\n");
			}
			else return 1;
		}
	}
	
	return 0;
}

const char *File_IO_BoolToString(bool Bvoid)
{
	if (Bvoid) return "yes";
	else return "no";
}
bool  File_IO_StringToBool(const char Entry[])
{
	if      (strcmp(Entry, "yes")) return true;
	else if (strcmp(Entry, "no"))  return false;
	else return NULL;
}
char  File_IO_BoolToChar(bool Bvoid)
{
	if (Bvoid) return 'y';
	else return 'n';
}
bool  File_IO_CharToBool(char Entry)
{
	if      (Entry=='y') return true;
	else if (Entry=='n')  return false;
	else return NULL;
}

int File_IO_GetValue_Default(long Start_Id, long Number, char Type, void *Data, const char Default[], long StringSize)
{
	char   *Cvoid_a;
	
	int     Ivoid_t;
	long    Lvoid;
	char    Svoid[_Nch100];
	
	long i, j;
	
	if (Start_Id < 0) return 1;
	
	switch (Type)
	{
	case 'b':	// bool		... false
		if      (!strcmp(Default, "y")) { Ivoid_t = 0; for (i = Start_Id; i < Number; i++) ((bool*)Data)[i] = true; }
		else if (!strcmp(Default, "n")) { Ivoid_t = 0; for (i = Start_Id; i < Number; i++) ((bool*)Data)[i] = false; }
		else                            { Ivoid_t = 1; for (i = Start_Id; i < Number; i++) ((bool*)Data)[i] = false; }
		break;	
	case 'c':	// char		... '-'
		if (strlen(Default) >= 1) { Ivoid_t = 0; for (i = Start_Id; i < Number; i++) ((char*)Data)[i] = Default[0]; }
		else                      { Ivoid_t = 1; for (i = Start_Id; i < Number; i++) ((char*)Data)[i] = '-'; }
		break;
	case 'i':	// int		... 0
		Ivoid_t = ((int)sscanf_s(Default, "%d", &((int*)Data)[Start_Id])) - 1;
		if (Ivoid_t) ((int*)Data)[Start_Id] = 0;
		for (i = Start_Id + 1; i < Number; i++) ((int*)Data)[i] = ((int*)Data)[Start_Id];
		break;
	case 'l':	// long		... 0
		Ivoid_t = ((int)sscanf_s(Default, "%ld", &((long*)Data)[Start_Id])) - 1;
		if (Ivoid_t) ((long*)Data)[Start_Id] = 0;
		for (i = Start_Id + 1; i < Number; i++) ((long*)Data)[i] = ((long*)Data)[Start_Id];
		break;
	case 'f':	// float	... 0.
		Ivoid_t = ((int)sscanf_s(Default, "%f", &((float*)Data)[Start_Id])) - 1;
		if (Ivoid_t) ((float*)Data)[Start_Id] = 0.0;
		for (i = Start_Id + 1; i < Number; i++) ((float*)Data)[i] = ((float*)Data)[Start_Id];
		break;
	case 'd':	// double	... 0.
#ifndef _USE_LONG_DOUBLE_PRECISON
		Ivoid_t = ((int)sscanf_s(Default, "%lf", &((double*)Data)[Start_Id])) - 1;
#else
		Ivoid_t = ((int)sscanf_s(Default, "%Lf", &((double*)Data)[Start_Id])) - 1;
#endif
		if (Ivoid_t) ((double*)Data)[Start_Id] = 0.0;
		for (i = Start_Id + 1; i < Number; i++) ((double*)Data)[i] = ((double*)Data)[Start_Id];
		break;
	case 's':	// string	...	'\n'
		if (Number < 2) { Ivoid_t = strcpy_s((char*)Data, StringSize, Default); if (Ivoid_t) ((char*)Data)[0] = '\n'; }
		else            { Ivoid_t = strcpy_s(((char**)Data)[Start_Id], StringSize, Default); if (Ivoid_t) ((char**)Data)[Start_Id][0] = '\n'; }
		for (i = Start_Id + 1; i < Number; i++) strcpy_s(((char**)Data)[i], StringSize, ((char**)Data)[Start_Id]);
		break;
	case 'w':	// word
		if (Number < 2) Cvoid_a = (char*)Data;
		else            Cvoid_a = ((char**)Data)[Start_Id];
		
		Lvoid = (long)strlen(Default);
		if (Lvoid < 1) Ivoid_t = 1;
		else           Ivoid_t = 0;
		
		Cvoid_a[0] = '\n';
		
		i = 0; j = 0;
		while ((i < Lvoid) && ((Default[i] == ' ') || (Default[i] == '\t') || (Default[i] == '\n'))) { i++; }
		while ((i < Lvoid) &&  (Default[i] != ' ') && (Default[i] != '\t') && (Default[i] != '\n'))  { Cvoid_a[j] = Default[i]; j++; i++; }
		Cvoid_a[j] = 0;
		
		for (i = Start_Id + 1; i < Number; i++) strcpy_s(((char**)Data)[i], StringSize, ((char**)Data)[Start_Id]);
		break;
	case 'W':	// word length (long)
		Lvoid = (long)strlen(Default);
		
		if (Lvoid > 0)
		{
			i = 0; j = 0;
			while ((i < Lvoid) && ((Default[i] == ' ') || (Default[i] == '\t') || (Default[i] == '\n'))) { i++; }
			while ((i < Lvoid) && (Default[i] != ' ') && (Default[i] != '\t') && (Default[i] != '\n')) { j++; i++; }
			Lvoid = j;
		}
		
		for (i = Start_Id + 1; i < Number; i++) ((long*)Data)[i] = Lvoid;
		break;
	case 'r':	// rational (long[2])
		Lvoid = (long)strlen(Default);
		memset(&Svoid[0], 0, _Nch100*sizeof(char));
		
		i = 0; j = 0;
		while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
		while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { Svoid[j] = Default[i]; j++; i++; }
		Ivoid_t = ((int)sscanf_s(Svoid, "%ld", &((long_2*)Data)[Start_Id][0])) - 1;
		if (Ivoid_t) ((long_2*)Data)[Start_Id][0] = 0;

		if (i>=Lvoid) ((long_2*)Data)[Start_Id][1] = 1;
		else if (Default[i] == '/')
		{
			memset(&Svoid[0], 0, _Nch100*sizeof(char));
			
			j = 0;
			while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
			while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { Svoid[j] = Default[i]; j++; i++; }
			Ivoid_t = sscanf_s(Svoid, "%ld", &((long_2*)Data)[Start_Id][1]) - 1;
			if (Ivoid_t) ((long_2*)Data)[Start_Id][1] = 1;
			if (((long_2*)Data)[Start_Id][1] == 0) { ((long_2*)Data)[Start_Id][0] = 1; Ivoid_t = 1; }
		}
		else if (Default[i] == '.')
		{
			if (((long_2*)Data)[Start_Id][0] != 0)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue_Default", _WARNING_MESSAGE_KEY_VALUE, "Not valid real value", 0, 0);
				_LOG_MESSAGE("Only real number in [0.0 .. 1.0) can be accepted!\n");
				_LOG_MESSAGE("THE NUMERATOR VALUE IS IGNORED!\n");
			}
			memset(&Svoid[0], 0, _Nch100*sizeof(char));
			
			j = 0;
			while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
			while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { Svoid[j] = Default[i]; j++; i++; }
			Ivoid_t = sscanf_s(Svoid, "%ld", &((long_2*)Data)[Start_Id][0]) - 1;
			if (Ivoid_t) ((long_2*)Data)[Start_Id][0] = 0;
			((long_2*)Data)[Start_Id][1] = 1;
			while (((long_2*)Data)[Start_Id][0] / ((long_2*)Data)[Start_Id][1]) { ((long_2*)Data)[Start_Id][1] *= 10; }
		}
		else ((long_2*)Data)[Start_Id][1] = 1;
		
		for (i = Start_Id + 1; i < Number; i++) ((long_2*)Data)[i][0] = ((long_2*)Data)[Start_Id][0];
		for (i = Start_Id + 1; i < Number; i++) ((long_2*)Data)[i][1] = ((long_2*)Data)[Start_Id][1];
		break;
	default: Ivoid_t = 1; _APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "File_IO_GetValue_Default", _WARNING_MESSAGE_KEY_VALUE, "Unknown value type.", 0, 1); break;
	}

	return Ivoid_t;
}
int File_IO_GetValue(const char Name[], const char Default[], FILE *File_my, const char S_KEY_ptr[], const char I_KEY_ptr[], const char E_KEY_ptr[], long Number, char Type, void *Data, long StringSize, char *LastCharRead)
{
	long i, j;
	
	char   *Cvoid_a;
	
	int     Ivoid_t;
	char    Cvoid_t;
	char    Svoid[_Nch100];
	
	char    S_KEY, I_KEY, E_KEY;
	bool    S_KEY_f, I_KEY_f, E_KEY_f;
	
	if (S_KEY_ptr != NULL) { S_KEY = S_KEY_ptr[0]; S_KEY_f = true; } else { S_KEY = '!'; S_KEY_f = false; }
	if (I_KEY_ptr != NULL) { I_KEY = I_KEY_ptr[0]; I_KEY_f = true; } else { I_KEY = '!'; I_KEY_f = false; }
	if (E_KEY_ptr != NULL) { E_KEY = E_KEY_ptr[0]; E_KEY_f = true; } else { E_KEY = '!'; E_KEY_f = false; }
	
	Cvoid_t = 0;
	
	if (S_KEY_f)
	{
		do { Cvoid_t = getc(File_my); } while ((Cvoid_t != S_KEY) && (Cvoid_t != EOF) && (!feof(File_my)));
		if (Cvoid_t == EOF)
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 0);
			_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
			return (File_IO_GetValue_Default(0, Number, Type, Data, Default, StringSize));
		}
	}
	
	for (i = 0; i < Number; i++)
	{
		switch (Type)
		{
		case 'b':	// bool
			Cvoid_t = ' ';
			do { Cvoid_t = getc(File_my); } while (((Cvoid_t == ' ') || (Cvoid_t == '\t') || (Cvoid_t == '\n')) && (Cvoid_t != EOF));
			if (Cvoid_t == EOF)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 1);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			
			switch (Cvoid_t)
			{
			case 'y': ((bool*)Data)[i] = true;  break;
			case 'n': ((bool*)Data)[i] = false; break;
			default:
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, Name, i, 2);
				_LOG_MESSAGE("A 'y' OR NOR VALUE IS ASSIGNED!\n");
				if (!strcmp(Default, "y")) ((bool*)Data)[i] = true;
				else                       ((bool*)Data)[i] = false;
				break;
			}
			break;	
		case 'c':	// char
			Cvoid_t = ' ';
			do { Cvoid_t = getc(File_my); } while (((Cvoid_t == ' ') || (Cvoid_t == '\t') || (Cvoid_t == '\n')) && (Cvoid_t != EOF));
			if (Cvoid_t == EOF)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 3);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			((char*)Data)[i] = Cvoid_t;
			break;
		case 'i':	// int
			Ivoid_t = fscanf_s(File_my, "%d", &((int*)Data)[i]);
			if (Ivoid_t != 1)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 4);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			break;
		case 'l':	// long
			Ivoid_t = fscanf_s(File_my, "%ld", &((long*)Data)[i]);
			if (Ivoid_t != 1)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 5);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			break;
		case 'f':	// float
			Ivoid_t = fscanf_s(File_my, "%f", &((float*)Data)[i]);
			if (Ivoid_t != 1)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 6);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			break;
		case 'd':	// double
#ifndef _USE_LONG_DOUBLE_PRECISON
			Ivoid_t = fscanf_s(File_my, "%lf", &((double*)Data)[i]);
#else
			Ivoid_t = fscanf_s(File_my, "%Lf", &((double*)Data)[i]);
#endif
			if (Ivoid_t != 1)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 7);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			break;
		case 's':	// string
			j = 0;
			if (Number < 2) Cvoid_a = (char*)Data;
			else            Cvoid_a = ((char**)Data)[i];
			
			memset(Cvoid_a, 0, StringSize*sizeof(char));
			
			do
			{
				Cvoid_t = getc(File_my);
				if (Cvoid_t == EOF)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 8);
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}
				
				if (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f))) { Cvoid_a[j] = Cvoid_t; j++; }
			}
			while (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f)) && (Cvoid_t != EOF));
			Cvoid_a[j] = 0;
			break;
		case 'w':	// word
			if (Number < 2) Cvoid_a = (char*)Data;
			else            Cvoid_a = ((char**)Data)[i];
			
			memset(Cvoid_a, 0, StringSize*sizeof(char));
			
			Cvoid_t = ' ';
			do { Cvoid_t = getc(File_my); } while (((Cvoid_t == ' ') || (Cvoid_t == '\t') || (Cvoid_t == '\n')) && (Cvoid_t != EOF));
			if (Cvoid_t == EOF)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 9);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			
			Cvoid_a[0] = Cvoid_t;
			
			j = 1;
			do
			{
				Cvoid_t = getc(File_my);
				if (Cvoid_t == EOF)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 10);
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}

				if (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f)) && (Cvoid_t != ' ') && (Cvoid_t != '\t') && (Cvoid_t != '\n') && (Cvoid_t != EOF))
				{
					if (j >= StringSize) return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
					Cvoid_a[j] = Cvoid_t; j++;
				}
			}
			while (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f)) && (Cvoid_t != ' ') && (Cvoid_t != '\t') && (Cvoid_t != '\n') && (Cvoid_t != EOF));
			Cvoid_a[j] = 0;
			break;
		case 'W':	// word length (long)
			Cvoid_t = ' ';
			do { Cvoid_t = getc(File_my); } while (((Cvoid_t == ' ') || (Cvoid_t == '\t') || (Cvoid_t == '\n')) && (Cvoid_t != EOF));
			if (Cvoid_t == EOF)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 9);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			
			((long*)Data)[0] = 1;
			do
			{
				Cvoid_t = getc(File_my);
				if (Cvoid_t == EOF)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 10);
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}
				
				if (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f)) && (Cvoid_t != ' ') && (Cvoid_t != '\t') && (Cvoid_t != '\n') && (Cvoid_t != EOF))
				{
					((long*)Data)[0]++;
				}
			}
			while (((Cvoid_t != I_KEY) || (!I_KEY_f)) && ((Cvoid_t != E_KEY) || (!E_KEY_f)) && (Cvoid_t != ' ') && (Cvoid_t != '\t') && (Cvoid_t != '\n') && (Cvoid_t != EOF));
			
			break;
		case 'r':	// rational (long[2])
			memset(&Svoid[0], 0, _Nch100*sizeof(char));
			
			Cvoid_t = ' '; j = 0;
			while (((Cvoid_t < 48) || (Cvoid_t > 57)) && (Cvoid_t != EOF)) { Cvoid_t = getc(File_my); }
			while ((Cvoid_t >= 48) && (Cvoid_t <= 57) && (Cvoid_t != EOF)) { Svoid[j] = Cvoid_t; j++; Cvoid_t = getc(File_my); j++; }
			Ivoid_t = sscanf_s(Svoid, "%ld", &((long_2*)Data)[i][0]) - 1;
			if (Ivoid_t)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 11);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
			}
			
			if (Cvoid_t == '/') 
			{
				memset(&Svoid[0], 0, _Nch100*sizeof(char));
				
				j = 0;
				while (((Cvoid_t < 48) || (Cvoid_t > 57)) && (Cvoid_t != EOF)) { Cvoid_t = getc(File_my); }
				while ((Cvoid_t >= 48) && (Cvoid_t <= 57) && (Cvoid_t != EOF)) { Svoid[j] = Cvoid_t; j++; Cvoid_t = getc(File_my); j++; }
				Ivoid_t = sscanf_s(Svoid, "%ld", &((long_2*)Data)[i][1]) - 1;
				if ((Ivoid_t) || (((long_2*)Data)[i][1] == 0))
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 12);
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}
			}
			else if (Cvoid_t == '.')
			{
				if (((long_2*)Data)[i][0]!=0)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, Name, i, 13);
					_LOG_MESSAGE("Only real number in [0.0 .. 1.0) can be accepted!\n");
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}
				memset(&Svoid[0], 0, _Nch100*sizeof(char));
				
				j = 0;
				while (((Cvoid_t < 48) || (Cvoid_t > 57)) && (Cvoid_t != EOF)) { Cvoid_t = getc(File_my); }
				while ((Cvoid_t >= 48) && (Cvoid_t <= 57) && (Cvoid_t != EOF)) { Svoid[j] = Cvoid_t; j++; Cvoid_t = getc(File_my); }
				Ivoid_t = sscanf_s(Svoid, "%ld", &((long_2*)Data)[i][0]) - 1;
				if (Ivoid_t)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_UNKNOWN, Name, i, 14);
					_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
					return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
				}
				((long_2*)Data)[i][1] = 1;
				while (((long_2*)Data)[i][0] / ((long_2*)Data)[i][1]) { ((long_2*)Data)[i][1] *= 10; }
			}
			else ((long_2*)Data)[i][1] = 1;
			break;
		default :
			_APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "File_IO_GetValue_Default", _WARNING_MESSAGE_KEY_VALUE, "Unknown value type.", i, 15);
			_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
			return (File_IO_GetValue_Default(i, Number, Type, Data, Default, StringSize));
		}
		
		if ((i < Number - 1) && (I_KEY_f) && (Type!='s') && (Type!='w') && (Type!='W'))
		{
			Cvoid_t = 0;
			
			while ((Cvoid_t != I_KEY) && (Cvoid_t != EOF)) { Cvoid_t = getc(File_my); }
			if (Cvoid_t == EOF)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, "Separation key not found.", 0, 16);
				_LOG_MESSAGE("THE DEFAULT VALUE IS ASSIGNED!\n");
				return (File_IO_GetValue_Default(i + 1, Number, Type, Data, Default, StringSize));
			}
		}
	}
	
	if (E_KEY_f) while ((Cvoid_t != E_KEY) && (Cvoid_t != EOF)) Cvoid_t = getc(File_my);
	
	if (LastCharRead!=NULL) *LastCharRead = Cvoid_t;
	
	return 0;
}

int File_IO_GetDistribution(const char Name[], const char Default[], FILE *File_my, const char S_KEY[], const char I_KEY[], const char E_KEY[], const char Sequence[], struct Statistics_Distribution *Data, double ScaleFactor, double IntegralProbability)
{
	size_t Size;
	char *DefaultS;

	char* Default_token, *token;
	char buffer[_Nch100];
	
	Size = strlen(Default) + 1;
	DefaultS = (char*)calloc(Size, sizeof(char));

	strcpy_s(DefaultS, Size, Default);

	if (ScaleFactor < FLT_EPSILON)  ScaleFactor = 1.0;
	
	if (!strcmp(Sequence, "ty_mu_sg"))
	{
		Default_token = strtok_s(DefaultS, " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, S_KEY, NULL, I_KEY, 1, 'w', &buffer[0], _Nch100, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, I_KEY, 1, 'd', &(*Data).mu, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, E_KEY, 1, 'd', &(*Data).sigma, 0, NULL);
		free(DefaultS);
		
		(*Data).mu    *= ScaleFactor;
		(*Data).sigma *= ScaleFactor;
		
		if (Statistics_DistributionParameters_IdentifyType(Data, buffer))		{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_UNKNOWN, Name, 0, 0); return 1; }
		if ((*Data).type == 'c') (*Data).sigma = 0.0;
		if (Statistics_DistributionParameters_MuSigma_to_MeanSDvariance(Data))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 1); return 1; }
		if (Statistics_IntegralProbability_Bounds(Data, IntegralProbability))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 1); return 1; }
		return 0;
	}
	else if (!strcmp(Sequence, "mu_sg_ty"))
	{
		Default_token = strtok_s(DefaultS, " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, I_KEY, 1, 'd', &(*Data).mu, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, E_KEY, 1, 'd', &(*Data).sigma, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, S_KEY, NULL, I_KEY, 1, 'w', &buffer[0], _Nch100, NULL);
		free(DefaultS);
		
		(*Data).mu    *= ScaleFactor;
		(*Data).sigma *= ScaleFactor;
		
		if (Statistics_DistributionParameters_IdentifyType(Data, buffer))		{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_UNKNOWN, Name, 0, 2); return 1; }
		if ((*Data).type == 'c') (*Data).sigma = 0.0;
		if (Statistics_DistributionParameters_MuSigma_to_MeanSDvariance(Data))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 3); return 1; }
		if (Statistics_IntegralProbability_Bounds(Data, IntegralProbability))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 1); return 1; }
		return 0;
	}
	else if (!strcmp(Sequence, "ty_me_sd"))
	{
		Default_token = strtok_s(DefaultS, " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, S_KEY, NULL, I_KEY, 1, 'w', &buffer[0], _Nch100, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, I_KEY, 1, 'd', &(*Data).mean, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, E_KEY, 1, 'd', &(*Data).sd, 0, NULL);
		free(DefaultS);
		
		(*Data).mean  *= ScaleFactor;
		(*Data).sd    *= ScaleFactor;
		
		if (Statistics_DistributionParameters_IdentifyType(Data, buffer))		{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_UNKNOWN, Name, 0, 4); return 1; }
		if ((*Data).type == 'c') (*Data).sd = 0.0;
		if (Statistics_DistributionParameters_MeanSD_to_MuSigmaVariance(Data))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 5); return 1; }
		if (Statistics_IntegralProbability_Bounds(Data, IntegralProbability))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 1); return 1; }
		return 0;
	}
	else if (!strcmp(Sequence, "me_sd_ty"))
	{
		Default_token = strtok_s(DefaultS, " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, I_KEY, 1, 'd', &(*Data).mean, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, NULL, NULL, E_KEY, 1, 'd', &(*Data).sd, 0, NULL);
		Default_token = strtok_s(NULL,     " :\t\v", &token); File_IO_GetValue(Name, Default_token, File_my, S_KEY, NULL, I_KEY, 1, 'w', &buffer[0], _Nch100, NULL);
		free(DefaultS);
		
		(*Data).mean  *= ScaleFactor;
		(*Data).sd    *= ScaleFactor;
		
		if (Statistics_DistributionParameters_IdentifyType(Data, buffer))		{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_UNKNOWN, Name, 0, 6); return 1; }
		if ((*Data).type == 'c') (*Data).sd = 0.0;
		if (Statistics_DistributionParameters_MeanSD_to_MuSigmaVariance(Data))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 7); return 1; }
		if (Statistics_IntegralProbability_Bounds(Data, IntegralProbability))	{ _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_VALUE, Name, 0, 1); return 1; }
		return 0;
	}
	else
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_GetDistribution", _WARNING_MESSAGE_KEY_UNKNOWN, Name, 0, 8);
		free(DefaultS);
		return 1;
	}
}

int File_IO_Search_KeyPhrase(FILE *File_my, const char Entry_Key[], const char PassPhrase[], bool JumpToNextLine, bool Warning_EndOfFile) // Spaces, Tabs, and New-Lines before the Pass Phrase are ignored
{
	char   Cvoid_t = 0;
	
	long   i;
	
	size_t Size;
	char  *Test_Phrase = NULL;
	
	if (Entry_Key != NULL)
	{
		do { Cvoid_t = getc(File_my); } while ((Cvoid_t != Entry_Key[0]) && (Cvoid_t != EOF));
		if (Cvoid_t == EOF)
		{
			if (Warning_EndOfFile) _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_VALUE, "Entry key not found!", 0, 0);
			return 1;
		}
	}
	
	Size = strlen(PassPhrase); // skippa spazi e simili se la frase successiva non matcha la passphrase serach for a new key and repeat!
	if (Size >= 1)
	{
		Cvoid_t = ' ';
		do { Cvoid_t = getc(File_my); } while (((Cvoid_t == ' ') || (Cvoid_t == '\t') || (Cvoid_t == '\n')) && (Cvoid_t != EOF));
		if (Cvoid_t == EOF)
		{
			if (Warning_EndOfFile) _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_UNKNOWN, "Pass Phrase not found!", 0, 1);
			return 1;
		}
		
		Test_Phrase = (char*)calloc(Size + 1, sizeof(char));
		if (Test_Phrase == NULL) { _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_CALLOC, "\"Test_Phrase\" to", (long)Size, 1); return 1; }
		
		memset(&Test_Phrase[0], 0, Size*sizeof(char));
		
		Test_Phrase[0] = Cvoid_t;
		for (i = 1; i < (long)Size; i++)
		{
			Test_Phrase[i] = getc(File_my);
			if (Test_Phrase[i] == EOF)
			{
				if (Warning_EndOfFile) _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_VALUE, "Pass Phrase not found!", 0, 2);
				return 1;
			}
		}
		
		while (strcmp(Test_Phrase, PassPhrase))
		{
			for (i = 0; i < (long)Size - 1; i++)Test_Phrase[i] = Test_Phrase[i + 1];
			Test_Phrase[Size - 1] = getc(File_my);
			if (Test_Phrase[Size - 1] == EOF)
			{
				if (Warning_EndOfFile) _APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_VALUE, "Pass Phrase not found!", 0, 3);
				return 1;
			}
		}
		
		if ((JumpToNextLine) && (Test_Phrase[Size - 1] != '\n')) File_IO_JumpToNextLine(File_my);
		
		free(Test_Phrase);
	}
	else
	{
		if ((JumpToNextLine) && (Cvoid_t != '\n')) File_IO_JumpToNextLine(File_my);
	}
	
	return 0;
}
int File_IO_Search_KeyWord(FILE *File_my, const char Entry_Key[], const char PassWord[], bool JumpToNextLine, bool Exclude_Warning) // Spaces and Tabs between key- and pass-words are ignored
{
	char   Cvoid_t;
	
	long SizeWord;
	char  *TestWord = NULL;
	
	SizeWord = (long)strlen(PassWord);
	TestWord = (char*)calloc((size_t)SizeWord + 1, sizeof(char));
	if (_APP_WARNING_MESSAGE(TestWord, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_CALLOC, "\"TestWord\" to", (long)SizeWord, 0)) { _USER_IO_WAIT(); return 1; }
	
	do
	{
		memset(&TestWord[0], 0, (size_t)SizeWord + 1);
		
		if (Entry_Key != NULL)
		{
			do { Cvoid_t = getc(File_my); } while ((Cvoid_t != Entry_Key[0]) && (Cvoid_t != EOF));
			if (Cvoid_t == EOF)
			{
				free(TestWord);
				
				if (!Exclude_Warning)
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_Key", _WARNING_MESSAGE_KEY_VALUE, "Entry key not found!", 0, 1);
					_USER_IO_WAIT();
				}
				return 1;
			}
		}
		File_IO_GetValue("Password", "\n", File_my, NULL, NULL, NULL, 1, 'w', TestWord, SizeWord, &Cvoid_t);
		if (feof(File_my))
		{
			free(TestWord);
			
			if (!Exclude_Warning)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_KeyWord", _WARNING_MESSAGE_KEY_VALUE, "Pass Word not found!", 0, 2);
				_USER_IO_WAIT();
			}
			return 1;
		}
	}
	while (strcmp(TestWord, PassWord));
	
	if ((JumpToNextLine) && (Cvoid_t != '\n')) File_IO_JumpToNextLine(File_my);
	
	free(TestWord);
	
	return 0;
}
int File_IO_Search_KeyLine(FILE *File_my, const char Entry_Key[], bool Exclude_Warning)
{
	char   Cvoid_t;
	
	while (!feof(File_my))
	{
		do { Cvoid_t = getc(File_my); } while ((Cvoid_t == ' ') || (Cvoid_t == '\t'));
		
		if (Entry_Key != NULL) { if (Cvoid_t == Entry_Key[0]) return 0; }
		
		if (Cvoid_t != '\n') File_IO_JumpToNextLine(File_my);
	}
	
	if (!Exclude_Warning)
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_File_IO.cpp", "File_IO_Search_KeyLine", _WARNING_MESSAGE_KEY_VALUE, "Entry key empty!", 0, 0);
		_USER_IO_WAIT();
	}
	
	return 1;
}

void File_IO_JumpToNextLine(FILE *File_my)
{
	char   Cvoid_t;
	do { Cvoid_t = getc(File_my); } while ((Cvoid_t != '\n') && (Cvoid_t != EOF));
	return;
}
long File_IO_Get_FileLength(FILE *File_my)
{
	char   Cvoid_t;
	
	long N_lines = 0;
	while (!feof(File_my))
	{
		Cvoid_t = getc(File_my);
		if (Cvoid_t != EOF)
		{
			N_lines++;
			do { Cvoid_t = getc(File_my); } while ((Cvoid_t != '\n') && (Cvoid_t != EOF));
		}
	}
	return N_lines;
}