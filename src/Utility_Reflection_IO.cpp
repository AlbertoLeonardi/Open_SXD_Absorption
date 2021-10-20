// Utility Functions :: Reflection_IO
//

#include "Utility_Reflection_IO.h"

void Reflection_IDL_info_Free(struct Reflection_List_IDL_info* IDL_info)
{
	if (IDL_info != NULL)
	{
		if (IDL_info->Ab_id != NULL) free(IDL_info->Ab_id);
		free(IDL_info);
	}
	return;
}

void Reflection_IO_Free(struct Reflection_List_DataSet* Reflection_List)
{
	long i;
	
	if (Reflection_List != NULL)
	{
		if (Reflection_List->Reflection != NULL)
		{
			for (i = 0; i < Reflection_List->Reflection_Count; i++) { if (Reflection_List->Reflection[i].Absorption != NULL) free(Reflection_List->Reflection[i].Absorption); }
			free(Reflection_List->Reflection);
		}
		free(Reflection_List);
	}
	return;
}
int  Reflection_IO_Stream(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], long Id)
{
	time_t RunTime;
	
	long i, j;
	
	char  RunTimeDate[_Nch100];
	char  value[_Nch20];
	char  buffer[_Nch600];
	FILE *OUT = NULL;
	
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "%s/%s_%ld.xd%s", Working_Path, Working_Name, Id, _EXT_TXT);
	if((OUT = File_IO_Open(buffer, "w")) == NULL) return 1;
	
	time(&RunTime);
	ctime_s(&RunTimeDate[0], _Nch100, &RunTime);
	
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "## SXD Reflections'List\n");																				Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "## %s", RunTimeDate);																					Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "## Line format: @ Beam_in->(x,y,z) Beam_out->(x,y,z) Mu->(...)\n");										Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		sprintf_s(&buffer[0], _Nch600, "@ (%lf %lf %lf) (%lf %lf %lf) (%lf", Reflection_List->Reflection[i].Beam_In[0], Reflection_List->Reflection[i].Beam_In[1], Reflection_List->Reflection[i].Beam_In[2],
																			 Reflection_List->Reflection[i].Beam_Ou[0], Reflection_List->Reflection[i].Beam_Ou[1], Reflection_List->Reflection[i].Beam_Ou[2],
																			 Reflection_List->Reflection[i].Absorption[0].Mu);
		Log_Stream(OUT, buffer);
		for (j = 1; j < Reflection_List->Reflection[i].Absorption_Count; j++)
		{
			memset(&value[0], 0, _Nch20 * sizeof(char));
			sprintf_s(&value[0], _Nch20, " %lf", Reflection_List->Reflection[i].Absorption[j].Mu);
			Log_Stream(OUT, value);
		}
		memset(&value[0], 0, _Nch20 * sizeof(char));
		sprintf_s(&value[0], _Nch20, ")\n");
		Log_Stream(OUT, value);
	}
	Log_Stream(OUT, "\n");
	
	fclose(OUT);

	return 0;
}

int  Reflection_IO_Dump_TXT(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], char Name_Extension[], long Id)
{
	time_t RunTime;
	
	long i, j;
	
	char  RunTimeDate[_Nch100];
	char  buffer[_Nch600];
	FILE* OUT = NULL;
	
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "%s/%s_%ld.ab.%s%s", Working_Path, Working_Name, Id, Name_Extension, _EXT_TXT);
	if ((OUT = File_IO_Open(buffer, "w")) == NULL) return 1;
	
	time(&RunTime);
	ctime_s(&RunTimeDate[0], _Nch100, &RunTime);
	
	fprintf(OUT, "## SXD Reflections'List ## Transmission Ceofficients\n");
	fprintf(OUT, "## %s", RunTimeDate);
	fprintf(OUT, "## Line format: Beam_in->(x,y,z) : Beam_out->(x,y,z) : Mu->(...) : Transmission->(...)\n\n");
	
	fprintf(OUT, "# Lattice parameters\t[%lf %lf %lf %lf %lf %lf]\n\n", Reflection_List->UnitCell[0], Reflection_List->UnitCell[1], Reflection_List->UnitCell[2],
																		Reflection_List->UnitCell[0] * _CONST_RadToDeg, Reflection_List->UnitCell[1] * _CONST_RadToDeg, Reflection_List->UnitCell[2] * _CONST_RadToDeg);
	
	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		fprintf(OUT,"%lf %lf %lf : %lf %lf %lf : %lf",Reflection_List->Reflection[i].Beam_In[0], Reflection_List->Reflection[i].Beam_In[1], Reflection_List->Reflection[i].Beam_In[2],
													  Reflection_List->Reflection[i].Beam_Ou[0], Reflection_List->Reflection[i].Beam_Ou[1], Reflection_List->Reflection[i].Beam_Ou[2],
													  Reflection_List->Reflection[i].Absorption[0].Mu);
		for (j = 1; j < Reflection_List->Reflection[i].Absorption_Count; j++) fprintf(OUT, " %lf", Reflection_List->Reflection[i].Absorption[j].Mu);
		fprintf(OUT, " :");
		for (j = 0; j < Reflection_List->Reflection[i].Absorption_Count; j++) fprintf(OUT, " %lf", Reflection_List->Reflection[i].Absorption[j].Transmission);
		fprintf(OUT, "\n");
	}
	
	fclose(OUT);
	
	return 0;
}
int  Reflection_IO_Dump_IDL(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], char Name_Extension[], long Id, char IDL_Path[], struct Reflection_List_IDL_info* IDL_info)
{
	long i, j, k;
	
	long_3 c_range[3];

	char  value[_Nch20];
	char  buffer[_Nch2000];
	char* s_token, * token;
	
	bool Test;
	
	FILE* OUT = NULL;
	FILE* DAT = NULL;
	
	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		for (j = 0; j < Reflection_List->Reflection[i].Absorption_Count; j++)
		{
			Reflection_List->Reflection[i].Absorption[j].Intensity[0] /= Reflection_List->Reflection[i].Absorption[j].Transmission;
			Reflection_List->Reflection[i].Absorption[j].Intensity[1] /= Reflection_List->Reflection[i].Absorption[j].Transmission;
		}
	}

	memset(&buffer[0], 0, _Nch2000 * sizeof(char)); sprintf_s(&buffer[0], _Nch2000, "%s/%s_%ld.ab.%s%s", Working_Path, Working_Name, Id, Name_Extension, _EXT_IDL);
	if ((OUT = File_IO_Open(buffer, "w")) == NULL) return 1;
	
	DAT = File_IO_Open(IDL_Path, "r"); if (DAT == NULL) return NULL;
	
	Test = false;
	do 
	{
		memset(&buffer[0], 0, _Nch2000 * sizeof(char));
		File_IO_GetValue("IDL line-by-line", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
		fprintf(OUT, "%s\n", buffer);
		
		s_token = strtok_s(buffer, " :\t\v", &token); if (!strcmp(s_token, "#"))  {
		s_token = strtok_s(NULL,   " :\t\v", &token); if (!strcmp(s_token, "No")) {
		s_token = strtok_s(NULL,   " :\t\v", &token); if (!strcmp(s_token, "of")) {
		s_token = strtok_s(NULL,   " :\t\v", &token); if (!strcmp(s_token, "peaks")) { Test = true; }}}}
	}
	while ((!feof(DAT)) && (!Test));
	
	memset(&buffer[0], 0, _Nch2000 * sizeof(char));
	File_IO_GetValue("IDL line-by-line", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
	fprintf(OUT, "%s\n", buffer);
	
	memset(&buffer[0], 0, _Nch2000 * sizeof(char));
	File_IO_GetValue("IDL line-by-line", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
	
	c_range[0][1] = 0;
	for (i = 0; i <= IDL_info->Fsqr_cd; i++)
	{
		c_range[0][0] = c_range[0][1];
		while ((buffer[c_range[0][1]] == ' ') || (buffer[c_range[0][1]] == '\t')) c_range[0][1]++;
		while ((buffer[c_range[0][1]] != ' ') && (buffer[c_range[0][1]] != '\t') && (buffer[c_range[0][1]] != '\n')) c_range[0][1]++;
	}
	c_range[0][2] = c_range[0][1] - c_range[0][0];
	c_range[1][1] = 0;
	for (i = 0; i <= IDL_info->sFsqr_cd; i++)
	{
		c_range[1][0] = c_range[1][1];
		while ((buffer[c_range[1][1]] == ' ') || (buffer[c_range[1][1]] == '\t')) c_range[1][1]++;
		while ((buffer[c_range[1][1]] != ' ') && (buffer[c_range[1][1]] != '\t') && (buffer[c_range[1][1]] != '\n')) c_range[1][1]++;
	}
	c_range[1][2] = c_range[1][1] - c_range[1][0];
	c_range[2][1] = 0;
	for (i = 0; i <= IDL_info->Tbar_cd; i++)
	{
		c_range[2][0] = c_range[2][1];
		while ((buffer[c_range[2][1]] == ' ') || (buffer[c_range[2][1]] == '\t')) c_range[2][1]++;
		while ((buffer[c_range[2][1]] != ' ') && (buffer[c_range[2][1]] != '\t') && (buffer[c_range[2][1]] != '\n')) c_range[2][1]++;
	}
	c_range[2][2] = c_range[2][1] - c_range[2][0];
	
	for (k = c_range[0][0]; k < c_range[0][1]; k++) buffer[k] = ' ';
	for (k = c_range[1][0]; k < c_range[1][1]; k++) buffer[k] = ' ';
	for (k = c_range[2][0]; k < c_range[2][1]; k++) buffer[k] = ' ';

	memset(&value[0], 0, _Nch20 * sizeof(char));
	if (Reflection_List->Reflection[IDL_info->Ab_id[0][0]].Absorption[IDL_info->Ab_id[0][1]].Intensity[0] < 0.0) sprintf_s(&value[0], _Nch20, "unobserved");
	else sprintf_s(&value[0], _Nch20, "%.2lf", Reflection_List->Reflection[IDL_info->Ab_id[0][0]].Absorption[IDL_info->Ab_id[0][1]].Intensity[0]);
	k = c_range[0][1] - (long)strlen(&value[0]); if (k < c_range[0][0] + 1) k = c_range[0][0] + 1; j = 0; while (k < c_range[0][1]) { buffer[k] = value[j]; j++; k++; }
	
	memset(&value[0], 0, _Nch20 * sizeof(char));
	if (Reflection_List->Reflection[IDL_info->Ab_id[0][0]].Absorption[IDL_info->Ab_id[0][1]].Intensity[0] < 0.0) sprintf_s(&value[0], _Nch20, "----");
	else sprintf_s(&value[0], _Nch20, "%.2lf", Reflection_List->Reflection[IDL_info->Ab_id[0][0]].Absorption[IDL_info->Ab_id[0][1]].Intensity[1]);
	k = c_range[1][1] - (long)strlen(&value[0]); if (k < c_range[1][0] + 1) k = c_range[1][0] + 1; j = 0; while (k < c_range[1][1]) { buffer[k] = value[j]; j++; k++; }
	
	memset(&value[0], 0, _Nch20 * sizeof(char));
	sprintf_s(&value[0], _Nch20, "%.4lf", Reflection_List->Reflection[IDL_info->Ab_id[0][0]].Absorption[IDL_info->Ab_id[0][1]].Transmission);
	k = c_range[2][0] + 4; j = 0; while (k < c_range[2][1]) { buffer[k] = value[j]; j++; k++; }
	
	fprintf(OUT, "%s\n", buffer);
	
	for (i = 1; i < IDL_info->Reflection_Count; i++)
	{
		memset(&buffer[0], 0, _Nch2000 * sizeof(char));
		File_IO_GetValue("IDL line-by-line", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
		
		memset(&value[0], 0, _Nch20 * sizeof(char));
		if (Reflection_List->Reflection[IDL_info->Ab_id[i][0]].Absorption[IDL_info->Ab_id[i][1]].Intensity[0] < 0.0) sprintf_s(&value[0], _Nch20, "unobserved");
		else sprintf_s(&value[0], _Nch20, "%.2lf", Reflection_List->Reflection[IDL_info->Ab_id[i][0]].Absorption[IDL_info->Ab_id[i][1]].Intensity[0]);
		k = c_range[0][1] - (long)strlen(&value[0]); if (k < c_range[0][0] + 1) k = c_range[0][0] + 1; j = 0; while (k < c_range[0][1]) { buffer[k] = value[j]; j++; k++; }
		
		memset(&value[0], 0, _Nch20 * sizeof(char));
		if (Reflection_List->Reflection[IDL_info->Ab_id[i][0]].Absorption[IDL_info->Ab_id[i][1]].Intensity[0] < 0.0) sprintf_s(&value[0], _Nch20, "----");
		else sprintf_s(&value[0], _Nch20, "%.2lf", Reflection_List->Reflection[IDL_info->Ab_id[i][0]].Absorption[IDL_info->Ab_id[i][1]].Intensity[1]);
		k = c_range[1][1] - (long)strlen(&value[0]); if (k < c_range[1][0] + 1) k = c_range[1][0] + 1; j = 0; while (k < c_range[1][1]) { buffer[k] = value[j]; j++; k++; }
		
		memset(&value[0], 0, _Nch20 * sizeof(char));
		sprintf_s(&value[0], _Nch20, "%.4lf", Reflection_List->Reflection[IDL_info->Ab_id[i][0]].Absorption[IDL_info->Ab_id[i][1]].Transmission);
		k = c_range[2][0] + 4; j = 0; while (k < c_range[2][1]) { buffer[k] = value[j]; j++; k++; }
		
		fprintf(OUT, "%s\n", buffer);
	}
	
	fclose(DAT);
	fclose(OUT);
	
	return 0;
}

struct Reflection_List_DataSet* Reflection_IO_Read_ReflectionList_TXT(char Path[])
{
	struct Reflection_List_DataSet* Reflection_List;
	
	char buffer[_Nch500];
	char* s_token, * token;
	
	FILE* DAT;
	
	DAT = File_IO_Open(Path, "r"); if (DAT == NULL) return NULL;
	
	Reflection_List = (struct Reflection_List_DataSet*)calloc(1, sizeof(struct Reflection_List_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	rewind(DAT); while (File_IO_Search_KeyPhrase(DAT, "#", "Lattice parameters", false, false)) { if (feof(DAT)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Lattice parameters\" not found!", 0, 0); return NULL; } }
	
	File_IO_GetValue("Parameters", "0.0", DAT, "[", NULL, "]", 6, 'd', &Reflection_List->UnitCell[0], 0, NULL);
	
	Reflection_List->UnitCell[3] *= _CONST_DegToRad;
	Reflection_List->UnitCell[4] *= _CONST_DegToRad;
	Reflection_List->UnitCell[5] *= _CONST_DegToRad;
	
	Reflection_List->Reflection = (struct Reflection_DataSet*)calloc(10, sizeof(struct Reflection_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List->Reflection, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List->Reflection\" to", 10, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
	
	memset(&Reflection_List->SampleRotation[0],        0, 3 * sizeof(double));
	memset(&Reflection_List->SampleRotation_Matrix[0], 0, 9 * sizeof(double));
	
	Reflection_List->Reflection_Count = 0;
	
	rewind(DAT);
	while (!feof(DAT))
	{ 
		if(!File_IO_Search_KeyLine(DAT, "@", true)) 
		{
			memset(&Reflection_List->Reflection[Reflection_List->Reflection_Count].hkl[0], 0, sizeof(long_3));
			memset(&Reflection_List->Reflection[Reflection_List->Reflection_Count].PlaneNorm[0], 0, sizeof(double_3));
			
			memset(&buffer[0], 0, _Nch500 * sizeof(char));
			File_IO_GetValue("Beam_In", "1.0", DAT, "(", NULL, ")", 3, 'd', &Reflection_List->Reflection[Reflection_List->Reflection_Count].Beam_In[0], 0, NULL);
			File_IO_GetValue("Beam_Ou", "1.0", DAT, "(", NULL, ")", 3, 'd', &Reflection_List->Reflection[Reflection_List->Reflection_Count].Beam_Ou[0], 0, NULL);
			File_IO_GetValue("Mu_list", "0.0", DAT, "(", NULL, ")", 1, 's', &buffer[0], _Nch500, NULL);
			
			Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption = (struct Absorption_DataSet*)calloc(10, sizeof(struct Absorption_DataSet));
			if (_APP_WARNING_MESSAGE(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption\" to", 10, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
			
			Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count = 0;
			
			s_token = strtok_s(buffer, " :\t\v", &token);
			while (s_token != NULL)
			{
				sscanf_s(s_token, "%lf", &Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption[Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count].Mu);
				Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count++;
				
				if (Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count % 10 == 0)
				{
					Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption = (struct Absorption_DataSet*)realloc(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption, (Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count + 10) * sizeof(struct Absorption_DataSet));
					if (_APP_WARNING_MESSAGE(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption\" to", Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count + 10, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
				}
				
				s_token = strtok_s(NULL, " :\t\v", &token);
			}
			
			if (Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count > 0)
			{
				Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption = (struct Absorption_DataSet*)realloc(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption, Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count * sizeof(struct Absorption_DataSet));
				if (_APP_WARNING_MESSAGE(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption\" to", Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
				
				Reflection_List->Reflection_Count++;
				
				if (Reflection_List->Reflection_Count % 10 == 0)
				{
					Reflection_List->Reflection = (struct Reflection_DataSet*)realloc(Reflection_List->Reflection, (Reflection_List->Reflection_Count + 10) * sizeof(struct Reflection_DataSet));
					if (_APP_WARNING_MESSAGE(Reflection_List->Reflection, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection\" to", Reflection_List->Reflection_Count + 10, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
				}
			}
			else free(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption);
		}
	}
	
	if(Reflection_List->Reflection_Count>0)
	{
		Reflection_List->Reflection = (struct Reflection_DataSet*)realloc(Reflection_List->Reflection, Reflection_List->Reflection_Count * sizeof(struct Reflection_DataSet));
		if (_APP_WARNING_MESSAGE(Reflection_List->Reflection, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_TXT", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection\" to", Reflection_List->Reflection_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
	}
	else free(Reflection_List->Reflection);
	
	fclose(DAT);
	
	return Reflection_List;
}
struct Reflection_List_DataSet* Reflection_IO_Read_ReflectionList_IDL(char Path[], struct Reflection_List_IDL_info ** IDL_info) // Lambda values are assumed as Mu!!!
{
	struct Reflection_List_DataSet* Reflection_List;
	
	long *Map_Id;

	long i, j;
	long max_cd;
	long Detector;
	
	char buffer[_Nch2000];
	char* s_token, * token;
	
	double_2 lla;
	double_3 hkl;
	double Norm;
	
	FILE* DAT;
	
	DAT = File_IO_Open(Path, "r"); if (DAT == NULL) return NULL;
	
	*IDL_info = (struct Reflection_List_IDL_info*)calloc(1, sizeof(struct Reflection_List_IDL_info));
	if (_APP_WARNING_MESSAGE(*IDL_info, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"IDL_info\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	//IDL = &IDL_info;

	Reflection_List = (struct Reflection_List_DataSet*)calloc(1, sizeof(struct Reflection_List_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	rewind(DAT); while (File_IO_Search_KeyPhrase(DAT, "#", "Phi, Chi, Omega", false, false)) { if (feof(DAT)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "\"Phi, Chi, Omega\" key not found!", 0, 0); return NULL; } }
	
	fscanf_s(DAT, "%lf", &Reflection_List->SampleRotation[0]); Reflection_List->SampleRotation[0] *= _CONST_DegToRad;
	fscanf_s(DAT, "%lf", &Reflection_List->SampleRotation[1]); Reflection_List->SampleRotation[1] *= _CONST_DegToRad;
	fscanf_s(DAT, "%lf", &Reflection_List->SampleRotation[2]); Reflection_List->SampleRotation[2] *= _CONST_DegToRad;
	
	RotationMatrix_3D_Euler_ZXZ(Reflection_List->SampleRotation[0], Reflection_List->SampleRotation[1], Reflection_List->SampleRotation[2], &Reflection_List->SampleRotation_Matrix[0]);
	
	rewind(DAT); while (File_IO_Search_KeyPhrase(DAT, "#", "Lattice parameters for detector", false, false)) { if (feof(DAT)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "\"UB and ABC matrix for detector\" key not found!", 0, 0); return NULL; } }
	
	fscanf_s(DAT, "%ld", &Detector);
	
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[0]);
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[1]);
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[2]);
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[3]); Reflection_List->UnitCell[3] *= _CONST_DegToRad;
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[4]); Reflection_List->UnitCell[4] *= _CONST_DegToRad;
	fscanf_s(DAT, "%lf", &Reflection_List->UnitCell[5]); Reflection_List->UnitCell[5] *= _CONST_DegToRad;
	
	rewind(DAT); while (File_IO_Search_KeyPhrase(DAT, "#", "UB and ABC matrix for detector", false, false)) { if (feof(DAT)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "\"UB and ABC matrix for detector\" key not found!", 0, 0); return NULL; } }
	
	if (File_IO_Search_KeyLine(DAT, "#", true)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "Incomplete UB/ABC Matrix definition!", 0, 0); return NULL; }
	fscanf_s(DAT, "%lf", &(*IDL_info)->UB[0][0]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[0][1]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[0][2]);
	fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[0][0]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[0][1]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[0][2]);
	
	if (File_IO_Search_KeyLine(DAT, "#", true)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "Incomplete UB/ABC Matrix definition!", 0, 0); return NULL; }
	fscanf_s(DAT, "%lf", &(*IDL_info)->UB[1][0]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[1][1]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[1][2]);
	fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[1][0]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[1][1]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[1][2]);
	
	if (File_IO_Search_KeyLine(DAT, "#", true)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "Incomplete UB/ABC Matrix definition!", 0, 0); return NULL; }
	fscanf_s(DAT, "%lf", &(*IDL_info)->UB[2][0]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[2][1]); fscanf_s(DAT, "%lf", &(*IDL_info)->UB[2][2]);
	fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[2][0]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[2][1]);   fscanf_s(DAT, "%lf", &(*IDL_info)->ABC[2][2]);
	
	// Read possibly different UB/ABC matrix per detector ...
	
	rewind(DAT); while (File_IO_Search_KeyPhrase(DAT, "#", "No of peaks", false, false)) { if (feof(DAT)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "\"No of peaks\" key not found!", 0, 0); return NULL; } }
	
	fscanf_s(DAT, "%ld", &Reflection_List->Reflection_Count);
	(*IDL_info)->Reflection_Count = Reflection_List->Reflection_Count;
	
	if (Reflection_List->Reflection_Count < 1) { fclose(DAT); return(Reflection_List); }
	
	Reflection_List->Reflection = (struct Reflection_DataSet*)calloc(Reflection_List->Reflection_Count, sizeof(struct Reflection_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List->Reflection, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List->Reflection\" to", Reflection_List->Reflection_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
	
	(*IDL_info)->Ab_id = (long_2*)calloc(Reflection_List->Reflection_Count, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*IDL_info)->Ab_id, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List->Reflection\" to", Reflection_List->Reflection_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }

	Map_Id = (long*)calloc(Reflection_List->Reflection_Count, sizeof(long));
	if (_APP_WARNING_MESSAGE(Map_Id, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"Map_Id\" to", Reflection_List->Reflection_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }

	File_IO_JumpToNextLine(DAT);
	if (File_IO_Search_KeyLine(DAT, "#", true)) { Reflection_IO_Free(Reflection_List); _APP_WARNING_MESSAGE(NULL, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_VALUE, "None reflection columne definition found!", 0, 0); return NULL; }
	
	memset(&buffer[0], 0, _Nch2000 * sizeof(char));
	File_IO_GetValue("Reflection colums name", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
	
	j = 0;
	s_token = strtok_s(buffer, " :\t\v", &token);
	while (s_token != NULL)
	{
		if      (!strcmp(s_token, "h"))			{ (*IDL_info)->h_cd = j; }
		else if (!strcmp(s_token, "k"))			{ (*IDL_info)->k_cd = j; }
		else if (!strcmp(s_token, "l"))			{ (*IDL_info)->l_cd = j; }
		else if (!strcmp(s_token, "Fsqr"))		{ (*IDL_info)->Fsqr_cd = j; }
		else if (!strcmp(s_token, "s(Fsqr)"))	{ (*IDL_info)->sFsqr_cd = j; }
		else if (!strcmp(s_token, "Lambda"))	{ (*IDL_info)->Lambda_cd = j; }
		else if (!strcmp(s_token, "Tbar"))		{ (*IDL_info)->Tbar_cd = j; }
		else if (!strcmp(s_token, "Latitude"))	{ (*IDL_info)->latitude_cd = j; }
		else if (!strcmp(s_token, "Longitude"))	{ (*IDL_info)->longitude_cd = j; }
		
		j++;
		s_token = strtok_s(NULL, " :\t\v", &token);
	}
	
	max_cd = (*IDL_info)->h_cd;
	if (max_cd < (*IDL_info)->k_cd)         max_cd = (*IDL_info)->k_cd;
	if (max_cd < (*IDL_info)->l_cd)         max_cd = (*IDL_info)->l_cd;
	if (max_cd < (*IDL_info)->Tbar_cd)      max_cd = (*IDL_info)->Tbar_cd;
	if (max_cd < (*IDL_info)->Lambda_cd)    max_cd = (*IDL_info)->Lambda_cd;
	if (max_cd < (*IDL_info)->latitude_cd)  max_cd = (*IDL_info)->latitude_cd;
	if (max_cd < (*IDL_info)->longitude_cd) max_cd = (*IDL_info)->longitude_cd;
	
	//long detector;
	//FILE* MY;
	//fopen_s(&MY, "C:/Users/stk79123/Shared/OutBeam.txt", "w");
	//fprintf(MY, "1278\n");

	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		Reflection_List->Reflection[i].Absorption_Count = 1;
		
		Reflection_List->Reflection[i].Absorption = (struct Absorption_DataSet*)calloc(Reflection_List->Reflection[i].Absorption_Count, sizeof(struct Absorption_DataSet));
		if (_APP_WARNING_MESSAGE(Reflection_List->Reflection[i].Absorption, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List->Reflection[i].Absorption\" to", 1, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
		
		memset(&buffer[0], 0, _Nch2000 * sizeof(char));
		File_IO_GetValue("Reflection colums name", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch2000, NULL);
		
		j = 0;
		s_token = strtok_s(buffer, " \t\v", &token);
		while ((s_token != NULL) && (j <= max_cd))
		{
			if      (j == (*IDL_info)->h_cd)		 sscanf_s(s_token, "%ld", &Reflection_List->Reflection[i].hkl[0]);
			else if (j == (*IDL_info)->k_cd)		 sscanf_s(s_token, "%ld", &Reflection_List->Reflection[i].hkl[1]);
			else if (j == (*IDL_info)->l_cd)		 sscanf_s(s_token, "%ld", &Reflection_List->Reflection[i].hkl[2]);
			else if (j == (*IDL_info)->latitude_cd)  sscanf_s(s_token, "%lf", &lla[0]);
			else if (j == (*IDL_info)->longitude_cd) sscanf_s(s_token, "%lf", &lla[1]);
			//else if (j == 5) sscanf_s(s_token, "%ld", &detector);
			// Read index of the detector ...
			else if (j == (*IDL_info)->Fsqr_cd)		{ if ((s_token[0] > 42) && (s_token[0] < 58)) sscanf_s(s_token, "%lf", &Reflection_List->Reflection[i].Absorption[0].Intensity[0]); else Reflection_List->Reflection[i].Absorption[0].Intensity[0] = -1.0; }
			else if (j == (*IDL_info)->sFsqr_cd)	{ if ((s_token[0] > 42) && (s_token[0] < 58)) sscanf_s(s_token, "%lf", &Reflection_List->Reflection[i].Absorption[0].Intensity[1]); else Reflection_List->Reflection[i].Absorption[0].Intensity[1] = -1.0; }
			else if (j == (*IDL_info)->Lambda_cd)	 sscanf_s(s_token, "%lf", &Reflection_List->Reflection[i].Absorption[0].Mu); // Convert from Lambda to Mu !!!
			
			j++;
			s_token = strtok_s(NULL, " :\t\v", &token);
		}
		
		// Apply possibly different UB/ABC matrix per detector with detector index ...
		/*
		aa[0] = (*IDL_info)->ABC[0][0]; aa[1] = (*IDL_info)->ABC[1][0]; aa[2] = (*IDL_info)->ABC[2][0];
		bb[0] = (*IDL_info)->ABC[0][1]; bb[1] = (*IDL_info)->ABC[1][1]; bb[2] = (*IDL_info)->ABC[2][1];
		cc[0] = (*IDL_info)->ABC[0][2]; cc[1] = (*IDL_info)->ABC[1][2]; cc[2] = (*IDL_info)->ABC[2][2];
		
		if (fabs(hkl[0]) > FLT_EPSILON) { Norm = 1.0 / hkl[0]; aa[0] *= Norm; aa[1] *= Norm; aa[2] *= Norm; }
		if (fabs(hkl[1]) > FLT_EPSILON) { Norm = 1.0 / hkl[1]; bb[0] *= Norm; bb[1] *= Norm; bb[2] *= Norm; }
		if (fabs(hkl[2]) > FLT_EPSILON) { Norm = 1.0 / hkl[2]; cc[0] *= Norm; cc[1] *= Norm; cc[2] *= Norm; }
		
		if (fabs(hkl[0]) < FLT_EPSILON) // 0??
		{
			if (fabs(hkl[1]) < FLT_EPSILON) // 00?
			{
				if (fabs(hkl[2]) < FLT_EPSILON) { } // 000
				else // 00l
				{
					Reflection_List->Reflection[i].Beam_Ou[0] = cc[0];
					Reflection_List->Reflection[i].Beam_Ou[0] = cc[1];
					Reflection_List->Reflection[i].Beam_Ou[0] = cc[2];
				}
			}
			else if (fabs(hkl[2]) < FLT_EPSILON) // 0k0
			{
				Reflection_List->Reflection[i].Beam_Ou[0] = bb[0];
				Reflection_List->Reflection[i].Beam_Ou[0] = bb[1];
				Reflection_List->Reflection[i].Beam_Ou[0] = bb[2];
			}
			else // 0kl
			{
				cc[0] -= bb[0];
				cc[1] -= bb[1];
				cc[2] -= bb[2];
				
				Reflection_List->Reflection[i].Beam_Ou[0] = aa[1] * cc[2] - aa[2] * cc[1];
				Reflection_List->Reflection[i].Beam_Ou[1] = aa[2] * cc[0] - aa[0] * cc[2];
				Reflection_List->Reflection[i].Beam_Ou[2] = aa[0] * cc[1] - aa[1] * cc[0];
				
				if (Reflection_List->Reflection[i].Beam_Ou[0] * bb[0] + Reflection_List->Reflection[i].Beam_Ou[1] * bb[1] + Reflection_List->Reflection[i].Beam_Ou[2] * bb[2] < 0.0)
				{
					Reflection_List->Reflection[i].Beam_Ou[0] *= -1.0;
					Reflection_List->Reflection[i].Beam_Ou[1] *= -1.0;
					Reflection_List->Reflection[i].Beam_Ou[2] *= -1.0;
				}
			}
		}
		else // h??
		{
			if (fabs(hkl[1]) < FLT_EPSILON) // h0?
			{
				if (fabs(hkl[2]) < FLT_EPSILON) // h00
				{
					Reflection_List->Reflection[i].Beam_Ou[0] = aa[0];
					Reflection_List->Reflection[i].Beam_Ou[0] = aa[1];
					Reflection_List->Reflection[i].Beam_Ou[0] = aa[2];
				}
				else // h0l
				{
					cc[0] -= aa[0];
					cc[1] -= aa[1];
					cc[2] -= aa[2];
					
					Reflection_List->Reflection[i].Beam_Ou[0] = bb[1] * cc[2] - bb[2] * cc[1];
					Reflection_List->Reflection[i].Beam_Ou[1] = bb[2] * cc[0] - bb[0] * cc[2];
					Reflection_List->Reflection[i].Beam_Ou[2] = bb[0] * cc[1] - bb[1] * cc[0];
					
					if (Reflection_List->Reflection[i].Beam_Ou[0] * aa[0] + Reflection_List->Reflection[i].Beam_Ou[1] * aa[1] + Reflection_List->Reflection[i].Beam_Ou[2] * aa[2] < 0.0)
					{
						Reflection_List->Reflection[i].Beam_Ou[0] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[1] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[2] *= -1.0;
					}
				}
			}
			else // hk?
			{
				if (fabs(hkl[2]) < FLT_EPSILON) // hk0
				{
					bb[0] -= aa[0];
					bb[1] -= aa[1];
					bb[2] -= aa[2];
					
					Reflection_List->Reflection[i].Beam_Ou[0] = cc[1] * bb[2] - cc[2] * bb[1];
					Reflection_List->Reflection[i].Beam_Ou[1] = cc[2] * bb[0] - cc[0] * bb[2];
					Reflection_List->Reflection[i].Beam_Ou[2] = cc[0] * bb[1] - cc[1] * bb[0];
					
					if (Reflection_List->Reflection[i].Beam_Ou[0] * aa[0] + Reflection_List->Reflection[i].Beam_Ou[1] * aa[1] + Reflection_List->Reflection[i].Beam_Ou[2] * aa[2] < 0.0)
					{
						Reflection_List->Reflection[i].Beam_Ou[0] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[1] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[2] *= -1.0;
					}
				}
				else // hkl
				{
					bb[0] -= aa[0];
					bb[1] -= aa[1];
					bb[2] -= aa[2];
					
					cc[0] -= aa[0];
					cc[1] -= aa[1];
					cc[2] -= aa[2];
					
					Reflection_List->Reflection[i].Beam_Ou[0] = bb[1] * cc[2] - bb[2] * cc[1];
					Reflection_List->Reflection[i].Beam_Ou[1] = bb[2] * cc[0] - bb[0] * cc[2];
					Reflection_List->Reflection[i].Beam_Ou[2] = bb[0] * cc[1] - bb[1] * cc[0];
					
					if (Reflection_List->Reflection[i].Beam_Ou[0] * aa[0] + Reflection_List->Reflection[i].Beam_Ou[1] * aa[1] + Reflection_List->Reflection[i].Beam_Ou[2] * aa[2] < 0.0)
					{
						Reflection_List->Reflection[i].Beam_Ou[0] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[1] *= -1.0;
						Reflection_List->Reflection[i].Beam_Ou[2] *= -1.0;
					}
				}
			}
		}
		*/
		lla[0] *= _CONST_DegToRad;
		lla[1] *= _CONST_DegToRad;
		
		Reflection_List->Reflection[i].Beam_Ou[0] = cos(lla[0]) * cos(lla[1]);
		Reflection_List->Reflection[i].Beam_Ou[1] = cos(lla[0]) * sin(lla[1]);
		Reflection_List->Reflection[i].Beam_Ou[2] = sin(lla[0]);
		
		Norm = 1.0 / sqrt(Reflection_List->Reflection[i].Beam_Ou[0] * Reflection_List->Reflection[i].Beam_Ou[0] +
						  Reflection_List->Reflection[i].Beam_Ou[1] * Reflection_List->Reflection[i].Beam_Ou[1] +
						  Reflection_List->Reflection[i].Beam_Ou[2] * Reflection_List->Reflection[i].Beam_Ou[2]);
		
		Reflection_List->Reflection[i].Beam_Ou[0] *= Norm;
		Reflection_List->Reflection[i].Beam_Ou[1] *= Norm;
		Reflection_List->Reflection[i].Beam_Ou[2] *= Norm;
		
		Map_Id[i] = i;
		(*IDL_info)->Ab_id[i][0] = i;
		(*IDL_info)->Ab_id[i][1] = 0;
		//fprintf(MY, "\n%lf\t%lf\t%lf\t%ld\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", hkl[0], hkl[1], hkl[2], detector, Reflection_List->Reflection[i].Beam_Ou[0], Reflection_List->Reflection[i].Beam_Ou[1], Reflection_List->Reflection[i].Beam_Ou[2],
		//	hkl[0] * (*IDL_info)->UB[0][0] + hkl[1] * (*IDL_info)->UB[1][0] + hkl[2] * (*IDL_info)->UB[2][0],
		//	hkl[0] * (*IDL_info)->UB[0][1] + hkl[1] * (*IDL_info)->UB[1][1] + hkl[2] * (*IDL_info)->UB[2][1],
		//	hkl[0] * (*IDL_info)->UB[0][2] + hkl[1] * (*IDL_info)->UB[1][2] + hkl[2] * (*IDL_info)->UB[2][2]);
	}
	
	//fclose(MY);

	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		for (j = i + 1; j < Reflection_List->Reflection_Count; j++)
		{
			if (fabs(Reflection_List->Reflection[i].Beam_Ou[0] * Reflection_List->Reflection[j].Beam_Ou[0] +
					 Reflection_List->Reflection[i].Beam_Ou[1] * Reflection_List->Reflection[j].Beam_Ou[1] +
					 Reflection_List->Reflection[i].Beam_Ou[2] * Reflection_List->Reflection[j].Beam_Ou[2] - 1.0) < FLT_EPSILON)
			{
				Reflection_List->Reflection[i].Absorption_Count ++;
				
				Reflection_List->Reflection[i].Absorption = (struct Absorption_DataSet*)realloc(Reflection_List->Reflection[i].Absorption, Reflection_List->Reflection[i].Absorption_Count * sizeof(struct Absorption_DataSet));
				if (_APP_WARNING_MESSAGE(Reflection_List->Reflection[i].Absorption, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionList_IDL", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection[i].Absorption\" to", 1, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
				
				Reflection_List->Reflection[i].Absorption[Reflection_List->Reflection[i].Absorption_Count - 1].Intensity[0] = Reflection_List->Reflection[j].Absorption->Intensity[0];
				Reflection_List->Reflection[i].Absorption[Reflection_List->Reflection[i].Absorption_Count - 1].Intensity[1] = Reflection_List->Reflection[j].Absorption->Intensity[1];
				Reflection_List->Reflection[i].Absorption[Reflection_List->Reflection[i].Absorption_Count - 1].Mu = Reflection_List->Reflection[j].Absorption->Mu;
				
				(*IDL_info)->Ab_id[Map_Id[j]][0] = i;
				(*IDL_info)->Ab_id[Map_Id[j]][1] = Reflection_List->Reflection[i].Absorption_Count - 1;
				
				Reflection_List->Reflection_Count--;
				if (j < Reflection_List->Reflection_Count)
				{
					Reflection_List->Reflection[j].Beam_Ou[0] = Reflection_List->Reflection[Reflection_List->Reflection_Count].Beam_Ou[0];
					Reflection_List->Reflection[j].Beam_Ou[1] = Reflection_List->Reflection[Reflection_List->Reflection_Count].Beam_Ou[1];
					Reflection_List->Reflection[j].Beam_Ou[2] = Reflection_List->Reflection[Reflection_List->Reflection_Count].Beam_Ou[2];
					Reflection_List->Reflection[j].Absorption[0].Intensity[0] = Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption[0].Intensity[0];
					Reflection_List->Reflection[j].Absorption[0].Intensity[1] = Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption[0].Intensity[1];
					Reflection_List->Reflection[j].Absorption[0].Mu = Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption[0].Mu;
					free(Reflection_List->Reflection[Reflection_List->Reflection_Count].Absorption);
					Map_Id[j] = Map_Id[Reflection_List->Reflection_Count];
					(*IDL_info)->Ab_id[Map_Id[Reflection_List->Reflection_Count]][0] = j;
					j--;
				} 
			}
		}
	}
	
	Reflection_List->Reflection = (struct Reflection_DataSet*)realloc(Reflection_List->Reflection, Reflection_List->Reflection_Count * sizeof(struct Reflection_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List->Reflection, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_REALLOC, "\"Reflection_List->Reflection\" to", Reflection_List->Reflection_Count, 0)) { _USER_IO_WAIT(); Reflection_IO_Free(Reflection_List); return NULL; }
	
	for (i = 0; i < Reflection_List->Reflection_Count; i++)
	{
		Reflection_List->Reflection[i].Beam_In[0] = 1.0;
		Reflection_List->Reflection[i].Beam_In[1] = 0.0;
		Reflection_List->Reflection[i].Beam_In[2] = 0.0;
		
		// SolveRotation_3D(&Reflection_List->Reflection[i].Beam_Ou[0],&Reflection_List->SampleRotation_Matrix[0]);

		Reflection_List->Reflection[i].PlaneNorm[0] = Reflection_List->Reflection[i].Beam_Ou[0] - Reflection_List->Reflection[i].Beam_In[0];
		Reflection_List->Reflection[i].PlaneNorm[1] = Reflection_List->Reflection[i].Beam_Ou[1] - Reflection_List->Reflection[i].Beam_In[1];
		Reflection_List->Reflection[i].PlaneNorm[2] = Reflection_List->Reflection[i].Beam_Ou[2] - Reflection_List->Reflection[i].Beam_In[2];

		Norm = 1.0 / sqrt(Reflection_List->Reflection[i].PlaneNorm[0] * Reflection_List->Reflection[i].PlaneNorm[0] +
						  Reflection_List->Reflection[i].PlaneNorm[1] * Reflection_List->Reflection[i].PlaneNorm[1] +
						  Reflection_List->Reflection[i].PlaneNorm[2] * Reflection_List->Reflection[i].PlaneNorm[2]);
		
		Reflection_List->Reflection[i].PlaneNorm[0] *= Norm;
		Reflection_List->Reflection[i].PlaneNorm[1] *= Norm;
		Reflection_List->Reflection[i].PlaneNorm[2] *= Norm;
	}
	
	fclose(DAT);
	
	free(Map_Id);
	
	return Reflection_List;
}
struct Reflection_List_DataSet* Reflection_Get_EmptyReflectionSet()
{
	struct Reflection_List_DataSet* Reflection_List;
	
	Reflection_List = (struct Reflection_List_DataSet*)calloc(1, sizeof(struct Reflection_List_DataSet));
	if (_APP_WARNING_MESSAGE(Reflection_List, "Utility_Reflection_IO.cpp", "Reflection_IO_Read_ReflectionsList", _WARNING_MESSAGE_KEY_CALLOC, "\"Reflection_List\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	Reflection_List->Reflection_Count = 0;
	
	memset(&Reflection_List->SampleRotation[0],        0, 3 * sizeof(double));
	memset(&Reflection_List->SampleRotation_Matrix[0], 0, 9 * sizeof(double));
	
	Reflection_List->Reflection = NULL;
	
	return Reflection_List;
}
