// Utility Functions :: Shape 3D Scan
//
// Mash STL is reconstruct from a 360 deg image sequence 

#include "Utility_Shape3DScan.h"

// Image Processing

int Shape3DScan_FrameRecovery(char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[])
{
	long i;
	
	char Out_Path_Dir[_Nch500];
	char In_Path[_Nch800], Out_Path[_Nch800];
	
	long Image_Id, Image_Count;
	
	FILE* LOG;
	
	memset(&Out_Path_Dir[0], 0, _Nch500 * sizeof(char));
	
	strcat_s(&Out_Path_Dir[0], _Nch500, Path_Ou);
	strcat_s(&Out_Path_Dir[0], _Nch500, _SEQUENCE_BMP);
	
	if ((!strcmp(Path_In, Out_Path_Dir)) && (!strcmp(Name_In, Name_Ou)))
	{
		memset(&In_Path[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&In_Path[0], _Nch800, "%s/%s%s", Path_In, Name_In, _EXT_TXT);
		
		LOG = File_IO_Open(In_Path, "r");
		if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_FrameRecovery", _WARNING_MESSAGE_KEY_VALUE, In_Path, 0, 0)) { _USER_IO_WAIT(); return 1; }
		
		rewind(LOG);
		Image_Count = File_IO_Get_FileLength(LOG) - 1;
		
		rewind(LOG);
		File_IO_JumpToNextLine(LOG);
		
		for (i = 0; i < Image_Count; i++)
		{
			if (fscanf_s(LOG, "%ld", &Image_Id))
			{
				if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_FrameRecovery", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return 1; }
			
				memset(&In_Path[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BMP);
				
				if (_access_s(In_Path, FILE_MODE_F)) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Log Image-Sequence file not found", _WARNING_MESSAGE_KEY_FILE_Fn, In_Path, 0, 0); _USER_IO_WAIT(); return 1; }
			}
			
			File_IO_JumpToNextLine(LOG);
		}
		
		fclose(LOG);
		
		return 0;
	}
	
	if (File_IO_Directory_Create(Out_Path_Dir, _Nch500)) return 1;
	
	memset(&In_Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&In_Path[0], _Nch800, "%s/%s%s", Path_In, Name_In, _EXT_TXT);
	
	memset(&Out_Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Out_Path[0], _Nch800, "%s/%s%s", Out_Path_Dir, Name_Ou, _EXT_TXT);
	
	if (File_IO_Copy(In_Path, Out_Path)) return 1;
	
	LOG = File_IO_Open(Out_Path, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_FrameRecovery", _WARNING_MESSAGE_KEY_VALUE, Out_Path, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
	
	rewind(LOG);
	File_IO_JumpToNextLine(LOG);
	
	for (i = 0; i < Image_Count; i++)
	{
		if (fscanf_s(LOG, "%ld", &Image_Id))
		{
			if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_FrameRecovery", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return 1; }
			
			memset(&In_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BMP);
			
			memset(&Out_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Out_Path[0], _Nch800, "%s/%s_%ld%s", Out_Path_Dir, Name_Ou, Image_Id, _EXT_BMP);
			
			if (File_IO_Copy(In_Path, Out_Path)) return 1;
		}
		
		File_IO_JumpToNextLine(LOG);
	}
	
	fclose(LOG);
	
	return 0;
}
int Shape3DScan_FrameSampling(char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[], char Token[], long Digits_Count, long Step_Id_0, long Step_Id_1, long Frame_Id_0, long Frame_Id_1, double RevolutionAngleError)
{
	long Step_Count  = Step_Id_1 - Step_Id_0;
	long Frame_Count = Frame_Id_1 - Frame_Id_0;
	
	double Angle;
	double AnglePerStep = (360.0 + RevolutionAngleError) / ((double)Step_Count);
	
	long Sequence_Setp_Id;
	long Frame_Id;
	
	char Out_Path_Dir[_Nch500];
	char Out_Path[_Nch800], In_Path[_Nch800];
	char Out_Log[_Nch800];
	
	FILE* LOG;
	
	memset(&Out_Path_Dir[0], 0, _Nch500 * sizeof(char));
	
	strcat_s(&Out_Path_Dir[0], _Nch500, Path_Ou);
	strcat_s(&Out_Path_Dir[0], _Nch500, _SEQUENCE_BMP);
	
	if (File_IO_Directory_Create(Out_Path_Dir, _Nch500)) return 1;
	
	memset(&Out_Log[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Out_Log[0], _Nch800, "%s/%s%s", Out_Path_Dir, Name_Ou, _EXT_TXT);
	
	LOG = File_IO_Open(Out_Log, "w");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_FrameSampling", _WARNING_MESSAGE_KEY_VALUE, Out_Log, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	fprintf(LOG, "Sequence_Id\tFrame_Id\tAngle(deg)");
	
	for (Sequence_Setp_Id = 0; Sequence_Setp_Id <= Step_Count; Sequence_Setp_Id++)
	{
		Angle = AnglePerStep * (double)Sequence_Setp_Id;
		
		Frame_Id = Frame_Id_0 + (Frame_Count * Sequence_Setp_Id + Step_Count / 2) / Step_Count;
		
		memset(&In_Path[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&In_Path[0], _Nch800, "%s/%s%s%0*d%s", Path_In, Name_In, Token, Digits_Count, Frame_Id, _EXT_BMP);
		
		memset(&Out_Path[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&Out_Path[0], _Nch800, "%s/%s_%d%s", Out_Path_Dir, Name_Ou, Sequence_Setp_Id, _EXT_BMP);
		
		if (File_IO_Copy(In_Path, Out_Path)) { fclose(LOG); return 1; }

		fprintf(LOG, "\n%ld\t%ld\t%lf", Sequence_Setp_Id, Frame_Id, Angle);
	}
	
	fclose(LOG);

	return 0;
}

int Shape3DScan_Get_FrameSequence(bool Use_Preprocessed_Sequence, char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[], char Token[], long Digits_Count, long Step_Id_0, long Step_Id_1, long Frame_Id_0, long Frame_Id_1, double RevolutionAngleError)
{
	if (Use_Preprocessed_Sequence) return Shape3DScan_FrameRecovery(Path_In, Path_Ou, Name_In, Name_Ou);
	else                           return Shape3DScan_FrameSampling(Path_In, Path_Ou, Name_In, Name_Ou, Token, Digits_Count, Step_Id_0, Step_Id_1, Frame_Id_0, Frame_Id_1, RevolutionAngleError);
}

struct Threading_GetMask
{
	long Thread_id;
	long Thread_Count;
	
	char *Path;
	char *Name;
	
	rgb_t* Mask_color;
	double Mask_Transparency;
	bool   MaskFilterOverlap;
	
	long Filter_Count;
	struct ImageFilters_Setting* FilterTool;
	struct MaskFilter* Filter;
};
void* Shape3DScan_Get_MaskSequence_Thread(void* ptr)
{
	struct  Threading_GetMask* TH = (struct  Threading_GetMask*)ptr;
	
	long i, j;
	long Image_Id, Image_Count;
	
	char Path[_Nch800];
	
	FILE *LOG, *BIN;
	
	memset(&Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path[0], _Nch800, "%s%s/%s%s", (*TH).Path, _SEQUENCE_BMP, (*TH).Name, _EXT_TXT);
	
	LOG = File_IO_Open(Path, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, Path, 0, 0)) return NULL;
		
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
		
	rewind(LOG);
	File_IO_JumpToNextLine(LOG);
	
	for (j = 0; j < (*TH).Thread_id; j++) File_IO_JumpToNextLine(LOG);
	
	for (i = (*TH).Thread_id; i < Image_Count; i+=(*TH).Thread_Count)
	{
		if (fscanf_s(LOG, "%ld", &Image_Id))
		{
			if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return NULL; }
			
			memset(&Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_BMP, (*TH).Name, Image_Id, _EXT_BMP);
				
			bitmap_image image(Path);
			
			if (!image) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_FILE_R, "Image not found", 0, 1); _USER_IO_WAIT();  fclose(LOG); return NULL; }
			else
			{
				if (ImageFilters_ResetFilter((*TH).Filter, &image)) return NULL;
				
				for (j = 0; j < (*TH).Filter_Count; j++) (*TH).FilterTool[j].Tool(&(*TH).FilterTool[j], (*TH).Filter);
				
				if ((*TH).MaskFilterOverlap) { if (ImageFilters_MergeMaskFilter((*TH).Filter, &image, (*(*TH).Mask_color), (*TH).Mask_Transparency)) return NULL; }
				else                         { if (ImageFilters_MergeMask((*TH).Filter, &image, (*(*TH).Mask_color), (*TH).Mask_Transparency)) return NULL; }
				
				ImageFilters_GetHollowMask((*TH).Filter);
				
				if ((*TH).MaskFilterOverlap) { if (ImageFilters_MergeMaskFilter((*TH).Filter, &image, (*(*TH).Mask_color), 0.00)) return NULL; }
				else                         { if (ImageFilters_MergeMask((*TH).Filter, &image, (*(*TH).Mask_color), 0.00)) return NULL; }
				
				memset(&Path[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_MSK, (*TH).Name, Image_Id, _EXT_BMP);
				
				image.save_image(Path);
				
				//
				
				memset(&Path[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_POL, (*TH).Name, Image_Id, _EXT_BIN);
				
				fopen_s(&BIN, Path, "wb");
				if (_APP_WARNING_MESSAGE(BIN, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, Path, 0, 1)) return NULL;
				
				if (ImageFilters_GetMaskDataSet((*TH).Filter, BIN)) return NULL;
				
				fclose(BIN);
			} 
		}
		
		for(j=0;j< (*TH).Thread_Count;j++) File_IO_JumpToNextLine(LOG);
	}
	
	fclose(LOG);
	
	return NULL;
}
int Shape3DScan_Get_MaskSequence(char Path[], char Name[], long_3 RGB_Mask_color, double Mask_Transparency, bool MaskFilterOverlap, long Filter_Count, struct ImageFilters_Setting* FilterTool, long Threads_Count)
{
	long i, j;
	
	char Local_Path[_Nch800];
	
	rgb_t Mask_color;
	struct MaskFilter** Filter;
	
	struct  Threading_GetMask* TH = NULL;
	pthread_t* Thread = NULL;
	
	Mask_color.red   = (unsigned char)RGB_Mask_color[0];
	Mask_color.green = (unsigned char)RGB_Mask_color[1];
	Mask_color.blue  = (unsigned char)RGB_Mask_color[2];
	
	memset(&Local_Path[0], 0, _Nch800 * sizeof(char));
	
	strcat_s(&Local_Path[0], _Nch800, Path);
	strcat_s(&Local_Path[0], _Nch800, _SEQUENCE_MSK);
	
	if (File_IO_Directory_Create(Local_Path, _Nch800)) return 1;
	
	memset(&Local_Path[0], 0, _Nch800 * sizeof(char));
	
	strcat_s(&Local_Path[0], _Nch800, Path);
	strcat_s(&Local_Path[0], _Nch800, _SEQUENCE_POL);
	
	if (File_IO_Directory_Create(Local_Path, _Nch800)) return 1;
	
	memset(&Local_Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Local_Path[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_BMP, Name, 0, _EXT_BMP);
	
	bitmap_image image(Local_Path);
	
	Filter = (struct MaskFilter**)calloc(Threads_Count, sizeof(struct MaskFilter*));
	if (_APP_WARNING_MESSAGE(Filter, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_CALLOC, "\"Filter\" to", Threads_Count, 0)) { _USER_IO_WAIT(); return 1; }
	
	Filter[0] = ImageFilters_GetFilter(&image); // it is assumed all the snaphots have the same size
	if(Filter[0] == NULL) { _USER_IO_WAIT(); return 1; }
	
	for (i = 1; i < Threads_Count; i++) { if (ImageFilters_CopyFilter(&Filter[i], &Filter[0])) { _USER_IO_WAIT(); for (j = 0; j < i; j++) { ImageFilters_FreeFilter(Filter[i]); } return 1; } }
	
	// Threading
	
	Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_GetMask*)calloc(Threads_Count, sizeof(struct Threading_GetMask));
	if (_APP_WARNING_MESSAGE(TH, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
		TH[i].Mask_color = &Mask_color;
		TH[i].Mask_Transparency = Mask_Transparency;
		TH[i].MaskFilterOverlap = MaskFilterOverlap;
		
		TH[i].Filter_Count = Filter_Count;
		TH[i].FilterTool = FilterTool;
		TH[i].Filter = Filter[i];
		
		TH[i].Path = &Path[0];
		TH[i].Name = &Name[0];
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, Shape3DScan_Get_MaskSequence_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_THREAD_CREATE, "Shape3DScan_Get_MaskSequence", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) Shape3DScan_Get_MaskSequence_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);
	
	// End Threading
	
	for (i = 0; i < Threads_Count; i++) if (ImageFilters_FreeFilter(Filter[i])) return 1;
	free(Filter);
	
	return 0;
}

// Image Shape Detection

int Shape3DScan_Get_2DConvexHull(long Node_Count, double_2* Node, bool Exclude_Up, bool Exclude_Do, bool Exclude_Lt, bool Exclude_Rt, FILE *BIN, bitmap_image *image, rgb_t* Mask_color)
{
	long i, j, k, l;
	
	long k0, k1, dk;
	long l0, l1, dl;
	long l_start;

	double Dvoid;
	
	long Iw[2];
	long Ih[2];
	
	double vv[2];
	double dd[2];
	double side[2];
	
	long Segment_Count;
	double_2 *Segment_normal;
	long_2   *Segment_vertex;
	
	bool Test, Exclude;
	
	long Segment_Count_alloc = 10;
	Segment_normal = (double_2*)calloc(Segment_Count_alloc, sizeof(double_2));
	Segment_vertex = (long_2*)  calloc(Segment_Count_alloc, sizeof(long_2));
	
	if (_APP_WARNING_MESSAGE(Segment_normal, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_2DConvexHull", _WARNING_MESSAGE_KEY_CALLOC, "\"Segment_normal\" to", Segment_Count_alloc, 0)) return NULL;
	if (_APP_WARNING_MESSAGE(Segment_vertex, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_2DConvexHull", _WARNING_MESSAGE_KEY_CALLOC, "\"Segment_vertex\" to", Segment_Count_alloc, 0)) return NULL;
	
	Iw[0] = 0;
	Iw[1] = 0;
	for (i = 1; i < Node_Count; i++)
	{
		if (Node[i][0] < Node[Iw[0]][0]) Iw[0] = i;
		if (Node[i][0] > Node[Iw[1]][0]) Iw[1] = i;
	}
	
	if (abs(Iw[0] - Iw[1]) < FLT_EPSILON) return 1; // No width region
	
	Ih[0] = 0;
	Ih[1] = 0;
	for (i = 1; i < Node_Count; i++)
	{
		if (Node[i][1] < Node[Ih[0]][1]) Ih[0] = i;
		if (Node[i][1] > Node[Ih[1]][1]) Ih[1] = i;
	}
	
	if (abs(Ih[0] - Ih[1]) < FLT_EPSILON) return 1; // no hieght region
	
	Segment_Count = 0;
	
	Segment_normal[Segment_Count][0] = Node[Iw[1]][1] - Node[Iw[0]][1];
	Segment_normal[Segment_Count][1] = Node[Iw[0]][0] - Node[Iw[1]][0];
	Segment_vertex[Segment_Count][0] = Iw[0];
	Segment_vertex[Segment_Count][1] = Iw[1];
	for (i = 0; i < Node_Count; i++)
	{
		if ((i != Segment_vertex[Segment_Count][0]) && (i != Segment_vertex[Segment_Count][1]))
		{
			side[0] = Node[i][0] - Node[Segment_vertex[Segment_Count][0]][0];
			side[1] = Node[i][1] - Node[Segment_vertex[Segment_Count][0]][1];
			
			if (Segment_normal[Segment_Count][0] * (side[0]) + Segment_normal[Segment_Count][1] * (side[1]) > FLT_EPSILON)
			{
				Segment_vertex[Segment_Count][1] = i;
				Segment_normal[Segment_Count][0] = +side[1];
				Segment_normal[Segment_Count][1] = -side[0];
			}
		}
	}
	
	side[0] = Node[Segment_vertex[Segment_Count][1]][0] - Node[Segment_vertex[Segment_Count][0]][0];
	side[1] = Node[Segment_vertex[Segment_Count][1]][1] - Node[Segment_vertex[Segment_Count][0]][1];

	dd[0] = 0.0;
	dd[1] = side[0] * side[0] + side[1] * side[1];

	for (i = 0; i < Node_Count; i++)
	{
		vv[0] = Node[i][0] - Node[Iw[0]][0];
		vv[1] = Node[i][1] - Node[Iw[0]][1];
		
		if (abs(vv[0] * Segment_normal[Segment_Count][0] + vv[1] * Segment_normal[Segment_Count][1]) < FLT_EPSILON)
		{
			Dvoid = side[0] * vv[0] + side[1] * vv[1];
			if (Dvoid < dd[0]) { dd[0] = Dvoid; Segment_vertex[Segment_Count][0] = i; }
			if (Dvoid > dd[1]) { dd[1] = Dvoid; Segment_vertex[Segment_Count][1] = i; }
		}
	}

	do {
		Segment_Count++;
		if (Segment_Count % 10 == 0)
		{
			Segment_Count_alloc += 10;
			Segment_normal = (double_2*)realloc(Segment_normal, Segment_Count_alloc * sizeof(double_2));
			Segment_vertex = (long_2*)realloc(Segment_vertex, Segment_Count_alloc * sizeof(long_2));
			
			if (_APP_WARNING_MESSAGE(Segment_normal, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_2DConvexHull", _WARNING_MESSAGE_KEY_REALLOC, "\"Segment_normal\" to", Segment_Count_alloc, 0)) return NULL;
			if (_APP_WARNING_MESSAGE(Segment_vertex, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_2DConvexHull", _WARNING_MESSAGE_KEY_REALLOC, "\"Segment_vertex\" to", Segment_Count_alloc, 0)) return NULL;
		}
		
		Segment_vertex[Segment_Count][0] = Segment_vertex[Segment_Count - 1][1];
		Segment_vertex[Segment_Count][1] = 0;
		Test = true;
		while ((Segment_vertex[Segment_Count][1] == Segment_vertex[Segment_Count - 1][0]) ||
			   (Segment_vertex[Segment_Count][1] == Segment_vertex[Segment_Count - 1][1]) || Test )
		{
			side[0] = Node[Segment_vertex[Segment_Count][1]][0] - Node[Segment_vertex[Segment_Count][0]][0];
			side[1] = Node[Segment_vertex[Segment_Count][1]][1] - Node[Segment_vertex[Segment_Count][0]][1];
			
			Segment_normal[Segment_Count][0] = +side[1];
			Segment_normal[Segment_Count][1] = -side[0];
			if (abs((Segment_normal[Segment_Count - 1][0] * Segment_normal[Segment_Count][1]) -
				    (Segment_normal[Segment_Count - 1][1] * Segment_normal[Segment_Count][0])) < FLT_EPSILON) Segment_vertex[Segment_Count][1]++;
			else Test = false;
		}
		
		if (Segment_normal[Segment_Count][0] * (Node[Segment_vertex[Segment_Count - 1][0]][0] - Node[Segment_vertex[Segment_Count][0]][0]) +
			Segment_normal[Segment_Count][1] * (Node[Segment_vertex[Segment_Count - 1][0]][1] - Node[Segment_vertex[Segment_Count][0]][1]) > 0.0) { Segment_normal[Segment_Count][0] *= -1.0; Segment_normal[Segment_Count][1] *= -1.0; }
		
		for (i = 0; i < Node_Count; i++)
		{
			if ( (i != Segment_vertex[Segment_Count - 1][0]) &&
				 (i != Segment_vertex[Segment_Count][0])     &&
				 (i != Segment_vertex[Segment_Count][1])     )
			{
				vv[0] = Node[i][0] - Node[Segment_vertex[Segment_Count][0]][0];
				vv[1] = Node[i][1] - Node[Segment_vertex[Segment_Count][0]][1];
				
				if ( (abs(Segment_normal[Segment_Count - 1][0] * vv[0] +
					      Segment_normal[Segment_Count - 1][1] * vv[1]) > FLT_EPSILON) &&
				     (Segment_normal[Segment_Count][0] * vv[0] +
					  Segment_normal[Segment_Count][1] * vv[1] > FLT_EPSILON) )
				{
					Segment_vertex[Segment_Count][1] = i;
					Segment_normal[Segment_Count][0] = +vv[1];
					Segment_normal[Segment_Count][1] = -vv[0];
					side[0] = vv[0];
					side[1] = vv[1];
				}
			}
		}
		
		dd[1] = side[0] * side[0] + side[1] * side[1];
		
		for (i = 0; i < Node_Count; i++)
		{
			vv[0] = Node[i][0] - Node[Segment_vertex[Segment_Count][0]][0];
			vv[1] = Node[i][1] - Node[Segment_vertex[Segment_Count][0]][1];
			
			if (abs(vv[0] * Segment_normal[Segment_Count][0] + vv[1] * Segment_normal[Segment_Count][1]) < FLT_EPSILON)
			{
				Dvoid = side[0] * vv[0] + side[1] * vv[1];
				if (Dvoid > dd[1]) { dd[1] = Dvoid; Segment_vertex[Segment_Count][1] = i; }
			}
		}
	} while (Segment_vertex[Segment_Count][1] != Segment_vertex[0][0]);
	Segment_Count++;

	for (i = 0; i < Segment_Count; i++)
	{
		if (fabs(Segment_normal[i][1]) > fabs(Segment_normal[i][0]))
		{
			if (Segment_normal[i][1] > 0.0) Exclude = Exclude_Up;
			else                            Exclude = Exclude_Do;
		}
		else
		{
			if (Segment_normal[i][0] > 0.0) Exclude = Exclude_Rt;
			else                            Exclude = Exclude_Lt;
		}
		
		if(Exclude)
		{
			Segment_Count--;
			Segment_normal[i][0] = Segment_normal[Segment_Count][0];
			Segment_normal[i][1] = Segment_normal[Segment_Count][1];
			Segment_vertex[i][0] = Segment_vertex[Segment_Count][0];
			Segment_vertex[i][1] = Segment_vertex[Segment_Count][1];
			i--;
		}
	}

	if (BIN != NULL)
	{
		fwrite(&Segment_Count, sizeof(long), 1, BIN);
		fwrite(Segment_vertex, sizeof(long_2), Segment_Count, BIN);
		fwrite(Segment_normal, sizeof(double_2), Segment_Count, BIN);

		fflush(BIN);
	}
	
	// Save Polygon Image

	if (image != NULL)
	{
		for (i = 0; i < Segment_Count; i++)
		{
			k0 = (long)Node[Segment_vertex[i][0]][0];
			k1 = (long)Node[Segment_vertex[i][1]][0];
			l0 = (long)Node[Segment_vertex[i][0]][1];
			l1 = (long)Node[Segment_vertex[i][1]][1];
			
			dk = k1 - k0;
			dl = l1 - l0;
			
			if (labs(dk) > labs(dl))
			{
				if (dk < 0)
				{
					dk = k0; k0 = k1; k1 = dk; dk = k1 - k0;
					dl = l0; l0 = l1; l1 = dl; dl = l1 - l0;
				}
				
				l_start = (*image).height() - l0;
				for (j = 0; j <= dk; j++)
				{
					k = k0 + j;
					l = l_start - (dl * j) / dk;
					(*image).set_pixel(k, l, (*Mask_color));
				}
			}
			else if(dl)
			{
				if (dl < 0)
				{
					dk = k0; k0 = k1; k1 = dk; dk = k1 - k0;
					dl = l0; l0 = l1; l1 = dl; dl = l1 - l0;
				}
				
				l_start = (*image).height() - l0;
				for (j = 0; j <= dl; j++)
				{
					k = k0 + (dk * j) / dl;
					l = l_start - j;
				
					(*image).set_pixel(k, l, (*Mask_color));
				}
			}
			else { } // empty segment!
		}
	}
	
	free(Segment_vertex);
	free(Segment_normal);
	
	return 0;
}

struct Threading_GetPolygon
{
	long Thread_id;
	long Thread_Count;
	
	char* Path;
	char* Name;

	long  Skip_Side_Up_Count;
	long  Skip_Side_Do_Count;
	long  Skip_Side_Lt_Count;
	long  Skip_Side_Rt_Count;
	long_2* Skip_Side_Up;
	long_2* Skip_Side_Do;
	long_2* Skip_Side_Lt;
	long_2* Skip_Side_Rt;
	
	rgb_t* Mask_color;
};
void* Shape3DScan_Get_PolygonSequence_Thread(void* ptr)
{
	struct  Threading_GetPolygon* TH = (struct  Threading_GetPolygon*)ptr;
	
	long i, j, k;
	long Image_Id, Image_Count;
	
	long Node_Count;
	double_2 *Node;
	
	bool Exclude_Up, Exclude_Do, Exclude_Lt, Exclude_Rt;

	char Path[_Nch800];
	FILE *LOG, *BIN;
	
	memset(&Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path[0], _Nch800, "%s%s/%s%s", (*TH).Path, _SEQUENCE_BMP, (*TH).Name, _EXT_TXT);
	
	LOG = File_IO_Open(Path, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, Path, 0, 0)) return NULL;
		
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
		
	rewind(LOG);
	File_IO_JumpToNextLine(LOG);
	
	for (j = 0; j < (*TH).Thread_id; j++) File_IO_JumpToNextLine(LOG);
	
	Node = (double_2*)calloc(1, sizeof(double_2));
	if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_PolygonSequence_Thread", _WARNING_MESSAGE_KEY_CALLOC, "\"Node\" to", 1, 0)) return NULL;
	
	for (i = (*TH).Thread_id; i < Image_Count; i+=(*TH).Thread_Count)
	{
		if (fscanf_s(LOG, "%ld", &Image_Id))
		{
			if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return NULL; }
			
			memset(&Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_POL, (*TH).Name, Image_Id, _EXT_BIN);
			
			BIN = File_IO_Open(Path, "rb");
			if (BIN == NULL) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_PolygonSequence_Thread", _WARNING_MESSAGE_KEY_FILE_R, "Image data-set not found", 0, 1); _USER_IO_WAIT();  fclose(LOG); return NULL; }
			else
			{
				fseek(BIN, sizeof(double_2), SEEK_SET);
				fread(&Node_Count, sizeof(long), 1, BIN);
				if (Node_Count > 3)
				{
					Node = (double_2*)realloc(Node, Node_Count * sizeof(double_2));
					if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_PolygonSequence_Thread", _WARNING_MESSAGE_KEY_REALLOC, "\"Node\" to", Node_Count, 0)) { _USER_IO_WAIT();  fclose(LOG); fclose(BIN); return NULL; }
					
					memset(Node, 0, Node_Count * sizeof(double_2));
					fread(Node, sizeof(double_2), Node_Count, BIN);

					fclose(BIN);
					BIN = File_IO_Open(Path, "ab");
					/**/
					memset(&Path[0], 0, _Nch800 * sizeof(char));
					sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_BMP, (*TH).Name, Image_Id, _EXT_BMP);
					
					bitmap_image image(Path);
					
					if (!image) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_FILE_R, "Image not found", 0, 1); _USER_IO_WAIT();  fclose(LOG); return NULL; }
					else
					{
						Exclude_Up = false;  for (k = 0; k < (*TH).Skip_Side_Up_Count; k++) if ((Image_Id >= (*TH).Skip_Side_Up[k][0]) && (Image_Id <= (*TH).Skip_Side_Up[k][1])) { Exclude_Up = true; k = (*TH).Skip_Side_Up_Count; }
						Exclude_Do = false;  for (k = 0; k < (*TH).Skip_Side_Do_Count; k++) if ((Image_Id >= (*TH).Skip_Side_Do[k][0]) && (Image_Id <= (*TH).Skip_Side_Do[k][1])) { Exclude_Do = true; k = (*TH).Skip_Side_Do_Count; }
						Exclude_Lt = false;  for (k = 0; k < (*TH).Skip_Side_Lt_Count; k++) if ((Image_Id >= (*TH).Skip_Side_Lt[k][0]) && (Image_Id <= (*TH).Skip_Side_Lt[k][1])) { Exclude_Lt = true; k = (*TH).Skip_Side_Lt_Count; }
						Exclude_Rt = false;  for (k = 0; k < (*TH).Skip_Side_Rt_Count; k++) if ((Image_Id >= (*TH).Skip_Side_Rt[k][0]) && (Image_Id <= (*TH).Skip_Side_Rt[k][1])) { Exclude_Rt = true; k = (*TH).Skip_Side_Rt_Count; }
						/**/
						if (Shape3DScan_Get_2DConvexHull(Node_Count, Node, Exclude_Up, Exclude_Do, Exclude_Lt, Exclude_Rt, BIN, &image, (*TH).Mask_color)) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_PolygonSequence_Thread", _WARNING_MESSAGE_KEY_VALUE, "No Convex-Hull defined", 0, 1); _USER_IO_WAIT(); fclose(LOG); fclose(BIN); return NULL; }
						/**/
					}
					
					memset(&Path[0], 0, _Nch800 * sizeof(char));
					sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_POL, (*TH).Name, Image_Id, _EXT_BMP);
					
					image.save_image(Path);
					/**/
				}
				else
				{
					memset(&Path[0], 0, _Nch800 * sizeof(char));
					sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_BMP, (*TH).Name, Image_Id, _EXT_BMP);
					
					bitmap_image image(Path);
					
					if (!image) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence_Thread", _WARNING_MESSAGE_KEY_FILE_R, "Image not found", 0, 1); _USER_IO_WAIT();  fclose(LOG); return NULL; }
					
					memset(&Path[0], 0, _Nch800 * sizeof(char));
					sprintf_s(&Path[0], _Nch800, "%s%s/%s_%ld%s", (*TH).Path, _SEQUENCE_POL, (*TH).Name, Image_Id, _EXT_BMP);
					
					image.save_image(Path);
				}
				
				fclose(BIN);
			} 
		}
		
		for(j=0;j< (*TH).Thread_Count;j++) File_IO_JumpToNextLine(LOG);
	}
	
	free(Node);
	
	fclose(LOG);
	
	return NULL;
}
int Shape3DScan_Get_PolygonSequence(char Path[], char Name[],
									long Skip_Side_Up_Count, long_2* Skip_Side_Up,
									long Skip_Side_Do_Count, long_2* Skip_Side_Do,
									long Skip_Side_Lt_Count, long_2* Skip_Side_Lt,
									long Skip_Side_Rt_Count, long_2* Skip_Side_Rt,
									long_3 RGB_Mask_color, long Threads_Count)
{
	long i, j;
	
	rgb_t Mask_color;
	
	struct  Threading_GetPolygon* TH = NULL;
	pthread_t* Thread = NULL;
	
	Mask_color.red   = (unsigned char)RGB_Mask_color[0];
	Mask_color.green = (unsigned char)RGB_Mask_color[1];
	Mask_color.blue  = (unsigned char)RGB_Mask_color[2];
	
	// Threading
	
	Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_GetPolygon*)calloc(Threads_Count, sizeof(struct Threading_GetPolygon));
	if (_APP_WARNING_MESSAGE(TH, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
		TH[i].Mask_color = &Mask_color;
		
		TH[i].Path = &Path[0];
		TH[i].Name = &Name[0];

		TH[i].Skip_Side_Up_Count = Skip_Side_Up_Count;
		TH[i].Skip_Side_Do_Count = Skip_Side_Do_Count;
		TH[i].Skip_Side_Lt_Count = Skip_Side_Lt_Count;
		TH[i].Skip_Side_Rt_Count = Skip_Side_Rt_Count;

		TH[i].Skip_Side_Up = Skip_Side_Up;
		TH[i].Skip_Side_Do = Skip_Side_Do;
		TH[i].Skip_Side_Lt = Skip_Side_Lt;
		TH[i].Skip_Side_Rt = Skip_Side_Rt;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, Shape3DScan_Get_PolygonSequence_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MaskSequence", _WARNING_MESSAGE_KEY_THREAD_CREATE, "Shape3DScan_Get_MaskSequence", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) Shape3DScan_Get_PolygonSequence_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);
	
	// End Threading
	
	return 0;
}

int Shape3DScan_Recover_PolygonSequence(char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[])
{
	long i;
	
	char Out_Path_Dir[_Nch500];
	char In_Path[_Nch800], Out_Path[_Nch800];
	
	long Image_Id, Image_Count;
	
	FILE* LOG;
	
	memset(&Out_Path_Dir[0], 0, _Nch500 * sizeof(char));
	
	strcat_s(&Out_Path_Dir[0], _Nch500, Path_Ou);
	strcat_s(&Out_Path_Dir[0], _Nch500, _SEQUENCE_POL);
	
	if ((!strcmp(Path_In, Out_Path_Dir)) && (!strcmp(Name_In, Name_Ou)))
	{
		memset(&In_Path[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&In_Path[0], _Nch800, "%s%s/%s%s", Path_Ou, _SEQUENCE_BMP, Name_Ou, _EXT_TXT);
		
		LOG = File_IO_Open(In_Path, "r");
		if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_VALUE, In_Path, 0, 0)) { _USER_IO_WAIT(); return 1; }
		
		rewind(LOG);
		Image_Count = File_IO_Get_FileLength(LOG) - 1;
		
		rewind(LOG);
		File_IO_JumpToNextLine(LOG);
		
		for (i = 0; i < Image_Count; i++)
		{
			if (fscanf_s(LOG, "%ld", &Image_Id))
			{
				if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return 1; }
			
				memset(&In_Path[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BMP);
				
				if (_access_s(In_Path, FILE_MODE_F)) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_FILE_Fn, In_Path, 0, 0); _USER_IO_WAIT(); return 1; }
				
				memset(&In_Path[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BIN);
				
				if (_access_s(In_Path, FILE_MODE_F)) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_FILE_Fn, In_Path, 0, 0); _USER_IO_WAIT(); return 1; }
			}
			
			File_IO_JumpToNextLine(LOG);
		}
		
		fclose(LOG);
		
		return 0;
	}
	
	memset(&In_Path[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&In_Path[0], _Nch800, "%s%s/%s%s", Path_Ou, _SEQUENCE_BMP, Name_Ou, _EXT_TXT);
	
	LOG = File_IO_Open(In_Path, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_VALUE, In_Path, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
	
	rewind(LOG);
	File_IO_JumpToNextLine(LOG);
	
	for (i = 0; i < Image_Count; i++)
	{
		if (fscanf_s(LOG, "%ld", &Image_Id))
		{
			if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Recover_PolygonSequence", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); return 1; }
			
			//
			memset(&In_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BMP);
			
			memset(&Out_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Out_Path[0], _Nch800, "%s/%s_%ld%s", Out_Path_Dir, Name_Ou, Image_Id, _EXT_BMP);
			
			if (File_IO_Copy(In_Path, Out_Path)) return 1;
			
			memset(&In_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&In_Path[0], _Nch800, "%s/%s_%ld%s", Path_In, Name_In, Image_Id, _EXT_BIN);
			
			memset(&Out_Path[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Out_Path[0], _Nch800, "%s/%s_%ld%s", Out_Path_Dir, Name_Ou, Image_Id, _EXT_BIN);
			
			if (File_IO_Copy(In_Path, Out_Path)) return 1;
		}
		
		File_IO_JumpToNextLine(LOG);
	}
	
	fclose(LOG);
	
	return 0;
}

int Shape3DScan_Get_RotationAxes(char Path[], char Name[], long_3 Mask_color, long Skip_Count, long_2* Skip_Id, bool CenterShift_Detection, double CenterShift_Error, double* CenterShift)
{
	long i, j, k;
	
	rgb_t color;
	
	bool First;
	long Count;
	
	long Image_Count, RotationRow;
	
	double_2 Center;
	
	long      Node_Count;
	double_2* Node;
	
	double    Min, Max, Mid;
	
	char Path_Local[_Nch800];
	FILE* LOG, * BIN;
	
	memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s%s", Path, _SEQUENCE_BMP, Name, _EXT_TXT);
	
	LOG = File_IO_Open(Path_Local, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_FILE_R, Path_Local, 0, 0)) return 1;
	
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
	
	fclose(LOG);
	
	Node = (double_2*)calloc(1, sizeof(double_2));
	if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_CALLOC, "\"Node\" to", 1, 0)) return 1;
	
	k = 0;
	Count = 0;
	First = true;
	for (i = 0; i < Image_Count; i++)
	{
		if (k < Skip_Count) { if (i == Skip_Id[k][0]) { i = Skip_Id[k][1]; k++; continue; } }
		Count++;

		memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_POL, Name, i, _EXT_BIN);
		
		BIN = File_IO_Open(Path_Local, "rb");
		if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_FILE_R, Path_Local, 0, 0)) { return 1; }
		
		fread(&Center, sizeof(double_2), 1, BIN);
		fread(&Node_Count, sizeof(long), 1, BIN);
		
		if (CenterShift_Detection)
		{
			Node = (double_2*)realloc(Node, Node_Count * sizeof(double_2));
			if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_REALLOC, "\"Node\" to", Node_Count, 0)) { fclose(BIN); return 1; }
			
			fread(&Node[0], sizeof(double_2), Node_Count, BIN);
			
			if (First)
			{
				Min = Node[0][1];
				Max = Node[0][1];
				Mid = Center[1];
				First = false;
			}
			else
			{
				if (Node[0][1] > Max) Max = Node[0][1];
				if (Node[0][1] < Min) Min = Node[0][1];
				Mid += Center[1];
			}
			
			for (j = 1; j < Node_Count; j++)
			{
				if (Node[j][1] > Max) { Max = Node[j][1]; }
				if (Node[j][1] < Min) { Min = Node[j][1]; }
			}
		}
		else
		{
			if (First)
			{
				Min = Center[1];
				Max = Center[1];
				Mid = Center[1];
				First = false;
			}
			else
			{
				if (Center[1] > Max) Max = Center[1];
				if (Center[1] < Min) Min = Center[1];
				Mid += Center[1];
			}
		}
		
		fclose(BIN);
	}
	
	free(Node);
	
	if (First) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_VALUE, "None valid polygon node detected!", 0, 0); return 1;	}
	
	Mid /= (double)Count;
	
	*CenterShift = 0.50 * (Min + Max) - Mid + CenterShift_Error;

	RotationRow = (long)round(Mid + *CenterShift);
	
	color.red   = (unsigned char)Mask_color[0];
	color.green = (unsigned char)Mask_color[1];
	color.blue  = (unsigned char)Mask_color[2];
	
	for (i = 0; i < Image_Count; i++)
	{
		memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
		sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_POL, Name, i, _EXT_BMP);
		
		bitmap_image image(Path_Local);
		
		if (!image) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_RotationAxes", _WARNING_MESSAGE_KEY_FILE_X, "Image not found", 0, 1); _USER_IO_WAIT(); }
		else
		{
			for (j = 0; j < (long)image.width(); j++) image.set_pixel(j, image.height() - RotationRow, (color));
			image.save_image(Path_Local);
		}
	}
	
	return 0;
}

// 3D Mesh Reconstruction

struct Mesh* Shape3DScan_Get_MeshFacets(char Path[], char Name[], long Skip_Count, long_2* Skip_Id, double Focal_Distance, double Rotation_Shift)
{
	struct Mesh* Shape = NULL;
	
	long i, j, k;
	long s, f;
	
	long Lvoid;
	double Dvoid;
	
	long Image_Count, Image_Id;
	long Skip;
	
	double Rotation_Angle;
	
	double_2 Center;
	
	long      Node_Count;
	double_2* Node;
	
	long      Segment_Count;
	long_2*   Vertex;
	double_2* Normal;

	double cosT, sinT;
	double_2  ax;
	double_3  nn;
	
	double Segment_Distance;
	
	char Path_Local[_Nch800];
	FILE *LOG, *BIN;
	
	Node = (double_2*)calloc(1, sizeof(double_2));
	if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Node\" to", 1, 0)) return NULL;
	
	Vertex = (long_2*)calloc(1, sizeof(long_2));
	Normal = (double_2*)calloc(1, sizeof(double_2));
	if (_APP_WARNING_MESSAGE(Vertex, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Segment_vertex\" to", 1, 0)) return NULL;
	if (_APP_WARNING_MESSAGE(Normal, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Segment_normal\" to", 1, 0)) return NULL;
	
	memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s%s", Path, _SEQUENCE_BMP, Name, _EXT_TXT);
	
	LOG = File_IO_Open(Path_Local, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_FILE_R, Path_Local, 0, 0)) return NULL;
	
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
	
	rewind(LOG);
	
	Shape = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(Shape, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape\" to", 1, 0)) return NULL;
	
	Mesh_Reset(Shape);
	
	Shape->Face = (struct Mesh_Face*)calloc(1, sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", 1, 0)) return NULL;
	
	j = 0;
	for (i = 0; i < Image_Count; i++)
	{
		File_IO_JumpToNextLine(LOG);
		
		Skip = 0;
		if (j < Skip_Count) { if (i == Skip_Id[j][0]) { i = Skip_Id[j][1]; Skip = Skip_Id[j][1] - Skip_Id[j][0]; j++; } }
		
		if (Skip > 0) { for (k = 0; k < Skip; k++) File_IO_JumpToNextLine(LOG); }
		else
		{
			if (fscanf_s(LOG, "%ld", &Image_Id))
			{
				if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); Mesh_Free_All(Shape); return NULL; }
				
				fscanf_s(LOG, "%ld", &Lvoid);
				fscanf_s(LOG, "%lf", &Rotation_Angle);
				
				cosT = cos(Rotation_Angle * _CONST_DegToRad);
				sinT = sin(Rotation_Angle * _CONST_DegToRad);
				
				memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
				sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_POL, Name, Image_Id, _EXT_BIN);
				
				BIN = File_IO_Open(Path_Local, "rb");
				if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_FILE_R, Path_Local, 0, 0)) { fclose(LOG); Mesh_Free_All(Shape); return NULL; }
				
				fread(&Center, sizeof(double_2), 1, BIN);
				fread(&Node_Count, sizeof(long), 1, BIN);
				
				Node = (double_2*)realloc(Node, Node_Count * sizeof(double_2));
				if (_APP_WARNING_MESSAGE(Node, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_REALLOC, "\"Node\" to", Node_Count, 0)) { fclose(LOG); fclose(BIN); Mesh_Free_All(Shape); return NULL; }
				
				fread(&Node[0], sizeof(double_2), Node_Count, BIN);
				
				fread(&Segment_Count, sizeof(long), 1, BIN);
				
				if (Segment_Count > 0)
				{
					Vertex = (long_2*)realloc(Vertex, Segment_Count * sizeof(long_2));
					Normal = (double_2*)realloc(Normal, Segment_Count * sizeof(double_2));
					if (_APP_WARNING_MESSAGE(Vertex, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Segment_vertex\" to", Segment_Count, 0)) { fclose(LOG); fclose(BIN); Mesh_Free_All(Shape); return NULL; }
					if (_APP_WARNING_MESSAGE(Normal, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Segment_normal\" to", Segment_Count, 0)) { fclose(LOG); fclose(BIN); Mesh_Free_All(Shape); return NULL; }

					fread(&Vertex[0], sizeof(long_2), Segment_Count, BIN);
					fread(&Normal[0], sizeof(double_2), Segment_Count, BIN);

					f = Shape->Face_Count;
					Shape->Face_Count += Segment_Count;
					Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
					if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MeshFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face\" to", Shape->Face_Count, 0)) { fclose(LOG); fclose(BIN); Mesh_Free_All(Shape); return NULL; }

					for (s = 0; s < Segment_Count; s++)
					{
						Dvoid = 1.0 / sqrt(Normal[s][0] * Normal[s][0] + Normal[s][1] * Normal[s][1]);

						Normal[s][0] *= Dvoid;
						Normal[s][1] *= Dvoid;
						/**/
						if (Focal_Distance > FLT_EPSILON)
						{
							Segment_Distance = Normal[s][0] * (Node[Vertex[s][0]][0] - Center[0]) + Normal[s][1] * (Node[Vertex[s][0]][1] - Center[1]);

							ax[0] = Normal[s][0] * Segment_Distance;
							ax[1] = Normal[s][1] * (Segment_Distance - Rotation_Shift);

							// The focal distance is assumed in the normal direction out of the image with projection at the center of the image itself
							Dvoid = Focal_Distance / sqrt(Segment_Distance * Segment_Distance + Focal_Distance * Focal_Distance);
							nn[0] = (Dvoid * Normal[s][0]);
							nn[1] = (Dvoid * Normal[s][1]);
							nn[2] = (Dvoid * Segment_Distance) / Focal_Distance;

							Shape->Face[f].Normal[0] = nn[0];
							Shape->Face[f].Normal[1] = nn[1] * cosT - nn[2] * sinT;
							Shape->Face[f].Normal[2] = nn[1] * sinT + nn[2] * cosT;
							Shape->Face[f].Normal[3] = nn[0] * ax[0] + nn[1] * ax[1];
						}
						else
						{
							Segment_Distance = Normal[s][0] * (Node[Vertex[s][0]][0] - Center[0]) + Normal[s][1] * (Node[Vertex[s][0]][1] - Center[1] - Rotation_Shift);

							// The focal distance is assumed in the normal direction out of the image with projection at the center of the image itself
							Shape->Face[f].Normal[0] = Normal[s][0];
							Shape->Face[f].Normal[1] = Normal[s][1] * cosT;
							Shape->Face[f].Normal[2] = Normal[s][1] * sinT;
							Shape->Face[f].Normal[3] = Segment_Distance;
						}

						f++;
					}
				}
				fclose(BIN);
			}
		}
	}
	
	fclose(LOG);
	
	free(Normal);
	free(Vertex);
	free(Node);
	
	return Shape;
}

struct Mesh* Shape3DScan_Get_FakeMesh(double Size)
{
	struct Mesh* Shape = NULL;
	
	Shape = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(Shape, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape\" to", 1, 0)) return NULL;
	
	Mesh_Reset(Shape);
	
	Shape->Face = (struct Mesh_Face*)calloc(6, sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", 1, 0)) return NULL;
	
	Shape->Face_Count = 6;
	
	Shape->Face[0].Normal[0] = +0.0;
	Shape->Face[0].Normal[1] = -1.0;
	Shape->Face[0].Normal[2] = +0.0;
	Shape->Face[0].Normal[3] = Size;
	
	Shape->Face[1].Normal[0] = +1.0;
	Shape->Face[1].Normal[1] = +0.0;
	Shape->Face[1].Normal[2] = +0.0;
	Shape->Face[1].Normal[3] = Size;
	
	Shape->Face[2].Normal[0] = +0.0;
	Shape->Face[2].Normal[1] = +1.0;
	Shape->Face[2].Normal[2] = +0.0;
	Shape->Face[2].Normal[3] = Size;
	
	Shape->Face[3].Normal[0] = -1.0;
	Shape->Face[3].Normal[1] = +0.0;
	Shape->Face[3].Normal[2] = +0.0;
	Shape->Face[3].Normal[3] = Size;
	
	Shape->Face[4].Normal[0] = +0.0;
	Shape->Face[4].Normal[1] = +0.0;
	Shape->Face[4].Normal[2] = +1.0;
	Shape->Face[4].Normal[3] = Size;
	
	Shape->Face[5].Normal[0] = +0.0;
	Shape->Face[5].Normal[1] = +0.0;
	Shape->Face[5].Normal[2] = -1.0;
	Shape->Face[5].Normal[3] = Size;
	
	return Shape;
}

// 3D Mesh Projection

int Shape3DScan_MeshProjection(struct Mesh* Shape, char Path[], char Name[], double Pixel_mm, double Focal_Distance, double Rotation_Shift, long_3 RGB_Mask_color)
{
	long i, j, k;
	
	long Lvoid;
	
	bool Exclude;
	long Image_Count, Image_Id, Vertex_Count;
	
	double Rotation_Angle, cosT, sinT;
	
	bool   Focal;
	double Focal_Distance_I;

	double    OutOfPlane;
	double_2* Vertex;
	
	double_2  Image_Center, dd;
	
	rgb_t Mask_color;
	
	Mask_color.red   = (unsigned char)RGB_Mask_color[0];
	Mask_color.green = (unsigned char)RGB_Mask_color[1];
	Mask_color.blue  = (unsigned char)RGB_Mask_color[2];
	
	char Path_Local[_Nch800];
	FILE* LOG;
	
	memset(&Path_Local[0], 0, _Nch800 * sizeof(char));

	strcat_s(&Path_Local[0], _Nch800, Path);
	strcat_s(&Path_Local[0], _Nch800, _SEQUENCE_USE);

	if (File_IO_Directory_Create(Path_Local, _Nch800)) return 1;

	memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s%s", Path, _SEQUENCE_BMP, Name, _EXT_TXT);
	
	LOG = File_IO_Open(Path_Local, "r");
	if (_APP_WARNING_MESSAGE(LOG, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MashFacets", _WARNING_MESSAGE_KEY_FILE_R, Path_Local, 0, 0)) return 1;
	
	rewind(LOG);
	Image_Count = File_IO_Get_FileLength(LOG) - 1;
	
	rewind(LOG);
	
	Vertex = (double_2*)calloc(Shape->Node_Count, sizeof(double_2));
	if (_APP_WARNING_MESSAGE(Vertex, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MashFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Vertex\" to", Shape->Node_Count, 0)) return 1;
	
	if (Focal_Distance > FLT_EPSILON) { Focal = true; Focal_Distance_I = 1.0 / Focal_Distance; }
	else Focal = false;

	for (i = 0; i < Image_Count; i++)
	{
		File_IO_JumpToNextLine(LOG);
		
		if (fscanf_s(LOG, "%ld", &Image_Id))
		{
			if (Image_Id != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_MashFacets", _WARNING_MESSAGE_KEY_VALUE, "Image_Id not consistent", 0, 1); _USER_IO_WAIT(); fclose(LOG); free(Vertex); return 1; }
			
			memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_POL, Name, i, _EXT_BMP);
			
			bitmap_image image(Path_Local);
			
			Image_Center[0] = 0.50 + (0.50 * (double)image.width());
			Image_Center[1] = 0.50 + (0.50 * (double)image.height());
			
			fscanf_s(LOG, "%ld", &Lvoid);
			fscanf_s(LOG, "%lf", &Rotation_Angle);
			
			cosT = cos(Rotation_Angle * _CONST_DegToRad);
			sinT = sin(Rotation_Angle * _CONST_DegToRad);
			
			Vertex_Count = 0;
			for (j = 0; j < Shape->Node_Count; j++)
			{
				Vertex[Vertex_Count][0] = Pixel_mm * (Shape->Node[j].x[0]);
				Vertex[Vertex_Count][1] = Pixel_mm * (Shape->Node[j].x[1] * cosT + Shape->Node[j].x[2] * sinT) + Rotation_Shift;
				OutOfPlane              = Pixel_mm * (Shape->Node[j].x[2] * cosT - Shape->Node[j].x[1] * sinT);
				
				if (Focal)
				{
					Vertex[Vertex_Count][0] *= (1.0 + OutOfPlane * Focal_Distance_I);
					Vertex[Vertex_Count][1] *= (1.0 + OutOfPlane * Focal_Distance_I);
				}

				Vertex[Vertex_Count][0] += Image_Center[0];
				Vertex[Vertex_Count][1] += Image_Center[1];

				Exclude = false;
				for (k = 0; k < Vertex_Count; k++)
				{
					dd[0] = Vertex[Vertex_Count][0] - Vertex[k][0];
					dd[1] = Vertex[Vertex_Count][1] - Vertex[k][1];
					if (dd[0] * dd[0] + dd[1] * dd[1] < FLT_EPSILON) { Exclude = true; break; }
				}
				if (!Exclude) Vertex_Count++;
			}
			
			Shape3DScan_Get_2DConvexHull(Vertex_Count, Vertex, false, false, false, false, NULL, &image, &Mask_color);
			
			memset(&Path_Local[0], 0, _Nch800 * sizeof(char));
			sprintf_s(&Path_Local[0], _Nch800, "%s%s/%s_%ld%s", Path, _SEQUENCE_USE, Name, i, _EXT_BMP);
			
			image.save_image(Path_Local);
		}
	}
	
	free(Vertex);
	
	return 0;
}