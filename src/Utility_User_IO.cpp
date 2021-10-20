// Utility Functions :: User IO
//
// Function for User Interaction

#include "Utility_User_IO.h"

void User_IO_AskValue(const char Request[], bool NewLine)
{
	printf(" ... %s", Request);
	if (NewLine) printf("\n     > ");
	return;
}

int User_IO_GetValue_Default(void *Value, char Type, long StringSize, const char Default[])
{
	int     Ivoid_t;
	
	long    Lvoid;
	
	long i, j;
	
	char buffer[_Nch100];
	
	switch (Type)
	{
	case 'b': Ivoid_t = 0;  if (!strcmp(Default, "y")) (*(bool*)Value) = true; else (*(bool*)Value) = false;			break;	// bool		... false
	case 'c': Ivoid_t = 0;  if (strlen(Default) >= 1) (*(char*)Value) = Default[0]; else (*(char*)Value) = '-';			break;	// char		... '-'
	case 'i': Ivoid_t = ((int)sscanf_s(&Default[0], "%d", (int*)Value)) - 1;      if (Ivoid_t) (*(int*)Value) = 0;		break;	// int		... 0
	case 'l': Ivoid_t = ((int)sscanf_s(&Default[0], "%ld", (long*)Value)) - 1;    if (Ivoid_t) (*(long*)Value) = 0;		break;	// long		... 0
	case 'f': Ivoid_t = ((int)sscanf_s(&Default[0], "%f", (float*)Value)) - 1;    if (Ivoid_t) (*(float*)Value) = 0.0;	break;	// float	... 0.
#ifndef _USE_LONG_DOUBLE_PRECISON
	case 'd': Ivoid_t = ((int)sscanf_s(&Default[0], "%lf", (double*)Value)) - 1;  if (Ivoid_t) (*(double*)Value) = 0.0;	break;	// double	... 0.
#else
	case 'd': Ivoid_t = ((int)sscanf_s(&Default[0], "%Lf", (double*)Value)) - 1;  if (Ivoid_t) (*(double*)Value) = 0.0;	break;	// double	... 0.
#endif

	case 's': Ivoid_t = strcpy_s((char*)Value, StringSize, Default); if (Ivoid_t) (*(char*)Value) = '\n';	break;	// string	... '\n'
	case 'w':																											// word ... '\n'
		if (StringSize<1) { Ivoid_t = 1; break; }
		else Ivoid_t = 0;
		
		Lvoid = (long)strlen(Default);
		if (Lvoid > StringSize) Lvoid = StringSize;

		((char*)Value)[0] = '\n';

		i = 0; j = 0;
		while ((i < Lvoid) && ((Default[i] == ' ') || (Default[i] == '\t'))) { i++; }
		while ((i < Lvoid) && (Default[i] != ' ') && (Default[i] != '\t') && (Default[i] != '\n') && (j < StringSize - 1)) { ((char*)Value)[j] = Default[i]; j++; i++; }
		((char*)Value)[j] = 0;
		break;
	case 'r':																											// rational number ... 0 (e.g. 0, 1, 2, ... 1/2, 1/3, 1/4, ...)
		Lvoid = (long)strlen(Default);
		
		memset(&buffer[0], 0, _Nch100*sizeof(char));
		
		i = 0; j = 0;
		while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
		while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { buffer[j] = Default[i]; j++; i++; }
		Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[0]) - 1;
		if (Ivoid_t) { (*(long_2*)Value)[0] = 0; (*(long_2*)Value)[1] = 1; break; }

		if (i >= Lvoid)             ((long*)Value)[1] = 1;
		else if (Default[i] == '/')
		{
			memset(&buffer[0], 0, _Nch100*sizeof(char));

			j = 0;
			while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
			while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { buffer[j] = Default[i]; j++; i++; }
			Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[1]) - 1;
			if (Ivoid_t) { (*(long_2*)Value)[0] = 0; (*(long_2*)Value)[1] = 1; break; }
			if ((*(long_2*)Value)[1] == 0) { Ivoid_t = 1; (*(long_2*)Value)[0] = 0; (*(long_2*)Value)[1] = 1; break; }
		}
		else if (Default[i] == '.')
		{
			if ((*(long_2*)Value)[0] != 0)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "User_IO_GetValue_Default", _WARNING_MESSAGE_KEY_VALUE, "Not valid real value", 0, 0);
				_LOG_MESSAGE("Only real number in [0.0 .. 1.0) can be accepted!\n");
				_LOG_MESSAGE("THE NUMERATOR VALUE IS IGNORED!\n");
			}
			memset(&buffer[0], 0, _Nch100*sizeof(char));
			
			j = 0;
			while ((i < Lvoid) && ((Default[i] < 48) || (Default[i] > 57))) { i++; }
			while ((i < Lvoid) && (Default[i] >= 48) && (Default[i] <= 57)) { buffer[j] = Default[i]; j++; i++; }
			Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[0]) - 1;
			if (Ivoid_t) (*(long_2*)Value)[0] = 0;
			(*(long_2*)Value)[1] = 1;
			while ((*(long_2*)Value)[0] / (*(long_2*)Value)[1]) { (*(long_2*)Value)[1] *= 10; }
		}
		else ((long*)Value)[1] = 1;
		break;
	default : Ivoid_t = 1; _APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "User_IO_GetValue_Default", _WARNING_MESSAGE_KEY_VALUE, "Unknown value type.", 0, 1); break;
	}
	
	return Ivoid_t;
}
int User_IO_GetValue(void *Value, char Type, long StringSize)
{
	int     Ivoid_t;
	char    Cvoid_t;
	char    Svoid_t[_Nch200], buffer[_Nch100];
	long    Lvoid;
	
	long i, j;
	
	switch (Type)
	{
	case 'b':																										// bool
		do
		{
			Cvoid_t = 0;
			Ivoid_t = my_scanf_string(&Svoid_t[0], _Nch200); if(strlen(Svoid_t)==1) Cvoid_t = Svoid_t[0];  fflush(stdin);
			switch (Cvoid_t)
			{
			case 'y': (*(bool*)Value) = true;  break;
			case 'n': (*(bool*)Value) = false; break;
			default:
				_APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "User_IO_GetValue", _WARNING_MESSAGE_KEY_USER, "Valid entry are y/n.", 0, 1);
				_USER_IO_ASKVALUE("Eneter the value (y/n): ", false);
				break;
			}
		}
		while ((Cvoid_t != 'y') && (Cvoid_t != 'n'));
		Ivoid_t = 0;
		break;
	case 'c': Ivoid_t = my_scanf_string(&Svoid_t[0], _Nch200) - 1; Cvoid_t = Svoid_t[0];	fflush(stdin); break;	// char
	case 'i': Ivoid_t = ((int)scanf_s("%d", (int*)Value))     - 1;							fflush(stdin); break;	// int
	case 'l': Ivoid_t = ((int)scanf_s("%ld", (long*)Value))   - 1;							fflush(stdin); break;	// long
	case 'f': Ivoid_t = ((int)scanf_s("%f", (float*)Value))   - 1;							fflush(stdin); break;	// float
#ifndef _USE_LONG_DOUBLE_PRECISON
	case 'd': Ivoid_t = ((int)scanf_s("%lf", (double*)Value)) - 1;							fflush(stdin); break;	// double
#else
	case 'd': Ivoid_t = ((int)scanf_s("%Lf", (double*)Value)) - 1;							fflush(stdin); break;	// double
#endif
	case 's':
		if (gets_s((char*)Value, StringSize) != NULL) Ivoid_t = 0;
		else                                          Ivoid_t = -1;
		fflush(stdin); break;																						// double
	case 'w':																										// word
		memset(&Svoid_t[0], 0, _Nch200*sizeof(char));
		Ivoid_t = my_scanf_string(&Svoid_t[0],_Nch200) - 1;									fflush(stdin);			// this should already read only the first word, but to ensure it ...
		if (Ivoid_t) break;
		
		Lvoid = (long)strlen(Svoid_t);
		
		((char*)Value)[0] = '\n';
		
		i = 0; j = 0;
		while ((i < Lvoid) && ((Svoid_t[i] == ' ') || (Svoid_t[i] == '\t'))) { i++; }
		while ((i < Lvoid) && (Svoid_t[i] != ' ') && (Svoid_t[i] != '\t') && (Svoid_t[i] != '\n') && (j < StringSize)) { ((char*)Value)[j] = Svoid_t[i]; j++; i++; }
		((char*)Value)[j] = 0;
		break;
	case 'r':																										// rational number ... 0 (e.g. 0, 1, 2, ... 1/2, 1/3, 1/4, ...)
		memset(&Svoid_t[0], 0, _Nch200*sizeof(char));
		if (gets_s(&Svoid_t[0], _Nch200) != NULL) Ivoid_t = 0;
		else                                  Ivoid_t = -1;
		fflush(stdin);
		if (Ivoid_t) break;
		
		Lvoid = (long)strlen(Svoid_t);
		
		memset(&buffer[0], 0, _Nch100*sizeof(char));
		
		i = 0; j = 0;
		while ((i < Lvoid) && ((Svoid_t[i] < 48) || (Svoid_t[i] > 57))) { i++; }
		while ((i < Lvoid) && (Svoid_t[i] >= 48) && (Svoid_t[i] <= 57)) { buffer[j] = Svoid_t[i]; j++; i++; }
		Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[0]) - 1;
		if (Ivoid_t) break;

		if (i >= Lvoid)             (*(long_2*)Value)[1] = 1;
		else if (Svoid_t[i] == '/')
		{
			memset(&buffer[0], 0, _Nch100*sizeof(char));

			j = 0;
			while ((i < Lvoid) && ((Svoid_t[i] < 48) || (Svoid_t[i] > 57))) { i++; }
			while ((i < Lvoid) && (Svoid_t[i] >= 48) && (Svoid_t[i] <= 57)) { buffer[j] = Svoid_t[i]; j++; i++; }
			Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[1]) - 1;
			if (Ivoid_t) break;
			if ((*(long_2*)Value)[1] == 0) { Ivoid_t = 1; break; }
		}
		else if (Svoid_t[i] == '.')
		{
			if ((*(long_2*)Value)[0] != 0)
			{
				_APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "User_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, "Not valid real value", 0, 0);
				_LOG_MESSAGE("Only real number in [0.0 .. 1.0) can be accepted!\n");
				_LOG_MESSAGE("THE NUMERATOR VALUE IS IGNORED!\n");
			}
			memset(&buffer[0], 0, _Nch100*sizeof(char));
			
			j = 0;
			while ((i < Lvoid) && ((Svoid_t[i] < 48) || (Svoid_t[i] > 57))) { i++; }
			while ((i < Lvoid) && (Svoid_t[i] >= 48) && (Svoid_t[i] <= 57)) { buffer[j] = Svoid_t[i]; j++; i++; }
			Ivoid_t = sscanf_s(buffer, "%ld", &(*(long_2*)Value)[0]) - 1;
			if (Ivoid_t) (*(long_2*)Value)[0] = 0;
			(*(long_2*)Value)[1] = 1;
			while ((*(long_2*)Value)[0] / (*(long_2*)Value)[1]) { (*(long_2*)Value)[1] *= 10; }
		}
		else (*(long_2*)Value)[1] = 1;
		break;
	default : Ivoid_t = 1; _APP_WARNING_MESSAGE(NULL, "Utility_User_IO.cpp", "User_IO_GetValue", _WARNING_MESSAGE_KEY_VALUE, "Unknown value type.", 0, 2); break;
	}
	
	return Ivoid_t;
}
