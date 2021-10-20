// Application System
//
// Collect imput parameters from external text file

#include "SXD_Absorption_InOut.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Input

struct SXD_Absorption_Data* SXD_Absorption_Input_Get_RunSettings_DataStructure()
{
	struct SXD_Absorption_Data* InData = NULL;
	
	InData = (struct SXD_Absorption_Data*)calloc(1, sizeof(struct SXD_Absorption_Data));
	if (_APP_WARNING_MESSAGE(InData, "App_Input.cpp", "App_Input_Get_RunSettings_DataStructure", _WARNING_MESSAGE_KEY_CALLOC, "\"InData\" to", 1, 0)) { _USER_IO_WAIT(); return NULL; }
	
	SXD_Absorption_Input_RunSettings_Reset(InData);
	
	return InData;
}
void SXD_Absorption_Input_RunSettings_Reset(struct SXD_Absorption_Data *InData)
{
	//
#ifdef _NVIDIA_CUDA_SUPPORT
	struct GPU_Device GPU;									// GPU device Name
#endif
	//
	(*InData).User_Seed[0] = 0;
	(*InData).User_Seed[1] = 0;
	//
	// ||||||||||  GENERAL SETTINGS  ||||||||||
	//
	memset(&(*InData).App_Input_BaseFolder[0],		 0, _Nch300 * sizeof(char));
	memset(&(*InData).App_Input_SubFolder[0],		 0, _Nch100 * sizeof(char));
	memset(&(*InData).App_Input_Name[0],			 0, _Nch100 * sizeof(char));
	memset(&(*InData).App_LinearAbsorption[0],       0, _Nch100 * sizeof(char));
	//
	memset(&(*InData).Working_Path[0],				 0, _Nch300 * sizeof(char));
	memset(&(*InData).Working_Name[0],				 0, _Nch100 * sizeof(char));
	//
	(*InData).ShapeFromFile = false;
	(*InData).DumpShape = false;
	(*InData).Break_StepSequence = false;
	(*InData).Break_PolygonSequence = false;
	//
	// |||||||||| 360 FRAME SEQUENCE ||||||||||
	//
	(*InData).Use_Preprocessed_ImageSequence = false;
	//
	memset(&(*InData).ImageSequence_Path[0], 0, _Nch300 * sizeof(char));
	memset(&(*InData).ImageSequence_Name[0], 0, _Nch100 * sizeof(char));
	//
	memset(&(*InData).ImageSequence_Token[0],		 0, _Nch10  * sizeof(char));
	(*InData).ImageSequence_Digits_Count = 0;
	(*InData).ImageSequence_Step_Id[0] = 0;
	(*InData).ImageSequence_Step_Id[1] = 1;
	(*InData).ImageSequence_Frame_Id[0] = 0;
	(*InData).ImageSequence_Frame_Id[1] = 0;
	(*InData).ImageSequence_RevolutionAngleError = 0.0;
	//
	// ||||||||||  IMAGE  FILTERING  ||||||||||
	//
	(*InData).Use_Prefiltered_ImageSequence = false;
	//
	memset(&(*InData).PreFilteredData_Path[0], 0, _Nch300 * sizeof(char));
	memset(&(*InData).PreFilteredData_Name[0], 0, _Nch100 * sizeof(char));
	//
	(*InData).Filter = NULL;					// it is assumed that the pointer has been already freed or never allocated!
	//
	(*InData).RGB_Mask_Color[0] = 0;
	(*InData).RGB_Mask_Color[1] = 0;
	(*InData).RGB_Mask_Color[2] = 0;
	(*InData).Mask_Transparency = 0.0;
	(*InData).MaskFilterOverlap = true;
	//
	// |||||||||| 3D SHAPE DETECTION ||||||||||
	//
	(*InData).Shape_RGB_Mask_Color[0] = 0;
	(*InData).Shape_RGB_Mask_Color[1] = 0;
	(*InData).Shape_RGB_Mask_Color[2] = 0;
	(*InData).Pixel_mm = 1;
	(*InData).PixelSize = 1.0;
	(*InData).Camera_FocalDistance = 0.0;
	(*InData).Image_RotationAxesDetection = true;
	(*InData).Image_RotationAxesError = 0.0;
	(*InData).Image_RotationAxesShift =0.0;
	(*InData).Mesh_Approximation_Threshold = 0.0;
	//
	memset(&(*InData).STL_type[0], 0, _Nch10 * sizeof(char));
	//
	(*InData).Skip_Count = 0;
	(*InData).Skip_Image = NULL;
	//
	(*InData).Skip_Side_Up_Count = 0;
	(*InData).Skip_Side_Do_Count = 0;
	(*InData).Skip_Side_Lt_Count = 0;
	(*InData).Skip_Side_Rt_Count = 0;
	(*InData).Skip_Side_Up = NULL;
	(*InData).Skip_Side_Do = NULL;
	(*InData).Skip_Side_Lt = NULL;
	(*InData).Skip_Side_Rt = NULL;
	//
	// |||||||||| ABSORPTION FACTORs ||||||||||
	//
	memset(&(*InData).Ab_Algorithm_Code[0], 0, _Nch20 * sizeof(char));
	(*InData).Ab_Algorithm = -1;
	(*InData).Ab_Resolution = FLT_EPSILON;
	(*InData).Ab_Iterations = 0;
	(*InData).Ab_Convergence = 0.0;
	(*InData).Ab_Lattice_StepNumber = 0;
	(*InData).Ab_Lattice_StepSize = 0.0;
	//
	memset(&(*InData).SampleReferenceAxes[0], 0, _Nch10 * sizeof(char));
	memset(&(*InData).SampleReferenceAxes[1], 0, _Nch10 * sizeof(char));
	memset(&(*InData).SampleReferenceAxes[2], 0, _Nch10 * sizeof(char));
	(*InData).SampleOrientation[0] = 0.0;
	(*InData).SampleOrientation[1] = 0.0;
	(*InData).SampleOrientation[2] = 0.0;
	//
	memset(&(*InData).ReflectionList_FileType_Code[0], 0, _Nch20 * sizeof(char));
	(*InData).ReflectionList_FileType = 0;
	(*InData).ReflectionList_Count = 0;
	(*InData).ReflectionList_Path = NULL;
	(*InData).Reflection_List = NULL;
	(*InData).IDL_info = NULL;
	//
	(*InData).Elements_Count = 0;
	(*InData).Element = NULL;
	//
	return;
}
void SXD_Absorption_Input_RunSettings_Free(struct SXD_Absorption_Data *InData)
{
	long i;
	
	if (InData != NULL)
	{
		if ((*InData).Filter != NULL)
		{
			for (i = 0; i < (*InData).Filter_Count; i++) { if ((*InData).Filter[i].Palette.color != NULL) free((*InData).Filter[i].Palette.color); }
			free((*InData).Filter);
		}
		if ((*InData).Skip_Image != NULL) free((*InData).Skip_Image);

		if ((*InData).Skip_Side_Up != NULL) free((*InData).Skip_Side_Up);
		if ((*InData).Skip_Side_Do != NULL) free((*InData).Skip_Side_Do);
		if ((*InData).Skip_Side_Lt != NULL) free((*InData).Skip_Side_Lt);
		if ((*InData).Skip_Side_Rt != NULL) free((*InData).Skip_Side_Rt);
		
		Reflection_IO_Free((*InData).Reflection_List);
		Reflection_IDL_info_Free((*InData).IDL_info);
		
		if ((*InData).ReflectionList_Path != NULL)
		{
			for (i = 0; i < (*InData).ReflectionList_Count; i++) if ((*InData).ReflectionList_Path[i] != NULL) free((*InData).ReflectionList_Path[i]);
			free((*InData).ReflectionList_Path);
		}
		
		if ((*InData).Element != NULL) free((*InData).Element);
		
		free(InData);
	}
	
	return;
}

int SXD_Absorption_Input_RunSettings_LineCommand_Preset(struct SXD_Absorption_Data *InData, int argc, char **argv)
{
	long i;

	char code[_Nch100];

	for (i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			memset(&code[0], 0, _Nch100*sizeof(char));
			strcpy_s(&code[0], _Nch100, argv[i]); // sscanf_s(argv[i], "%s", &code[0]);

			if (!strcmp(code, "-h"))
			{
				printf("\n");
				printf("**** **** **** ****  **** **** **** ****\n");
				printf("**** **** Line command options **** ****\n");
				printf("\n");
				printf("Application Settings:\n\n");
				printf(" -App_bf ... Application input file: base-folder path (including the initial '/')\n");
				printf(" -App_sf ... Application input file: sub-folder path  (without the separator '/')\n");
				printf(" -App_na ... Application input file: name path        (without the separator '/', but with final extension)\n");
				printf("\n");
				printf("Working Settings:\n");
				printf("\n");
				printf(" -Seed ... Random generator seeds (pair of integers)\n");
				printf("\n");
				printf("---- ---- ---- ----  ---- ---- ---- ----\n");
				printf("\n");
				printf(" -Run_wf ... Output (working) directory path   (with initial '/')\n");
				printf(" -Run_wn ... Output (working) files' root-name (with initial '/')\n");
				printf("\n");
				printf(" -Abs_wn ... Linear Absorption coefficients file name (without initial '/')\n");
				printf("\n");
				printf(" -ShapeF ... Inport crystal shape from STL file (y/n)\n");
				printf(" -ShapeS ... Rotation axes shift (pixels)\n");
				printf("\n");
				printf(" -ShapeD ... skip any shape processing (y/n)\n");
				printf("\n");
				printf(" -Break_S ... Break after step frame-sequence (y/n)\n");
				printf(" -Break_P ... Break after polygon frame-sequence (y/n)\n");
				printf("\n");
				printf("---- ---- ---- ----  ---- ---- ---- ----\n");
				printf("\n");
				printf(" -360_up ... Use preprocessed image sequence (y/n)\n");
				printf(" -360_sf ... Image sequence directory path\n");
				printf(" -360_sn ... Image sequence name (without / or extension)\n");
				printf(" -360_it ... Image sequence token\n");
				printf(" -360_ip ... Image sequence index # of 0s padding\n");
				printf(" -360_si ... Image sequence step-id extremes (start and end)\n");
				printf(" -360_fi ... Image sequence frame-id extremes (start and end)\n");
				printf(" -360_ra ... Image sequence revolution angel error (deg)\n");
				printf("\n");
				printf("---- ---- ---- ----  ---- ---- ---- ----\n");
				printf("\n");
				printf(" -Mask_p ... Use prefiltered image data (y/n, Path, Name)\n");
				printf(" -Mask_c ... RGB mask color (three integers)\n");
				printf(" -Mask_t ... RGB mask transparency\n");
				printf(" -Mask_o ... RGB mask overlap to the filters (y/n)\n");
				printf("\n");
				printf(" -MSK ... Add an Image Filter / Mask to the list (see documentation for the additional parameters)\n");
				printf(" -MSK -M_swap ... Mask swap\n");
				printf(" -MSK -M_ex_r ... Mask exclude region\n");
				printf(" -MSK -M_in_r ... Mask include region\n");
				printf(" -MSK -M_expa ... Mask expand\n");
				printf(" -MSK -M_cont ... Mask contract\n");
				printf(" -MSK -M_aver ... Mask average\n");
				printf(" -MSK -M_RGBc ... Mask via select a color\n");
				printf(" -MSK -M_RGBm ... Mask via select rgb - color mismatch\n");
				printf(" -MSK -M_RGBe ... Mask via select ellipse of colors\n");
				printf(" -MSK -M_RGBp ... Mask via select palette of colors\n");
				printf(" -MSK -M_red ... Mask via select red color\n");
				printf(" -MSK -M_green ... Mask via select green color\n");
				printf(" -MSK -M_blue ... Mask via select blue color\n");
				printf(" -MSK -M_gray ... Mask via select gray color\n");
				printf(" -MSK -M_white ... Mask via select white color\n");
				printf(" -MSK -M_black ... Mask via select black color\n");
				printf("\n");
				printf(" -MSK -I_gray ... Image-Filter grayscale\n");
				printf(" -MSK -I_coSc ... Image-Filter color scale\n");
				printf(" -MSK -I_coSS ... Image-Filter color shift scale\n");
				printf(" -MSK -I_coSh ... Image-Filter color shift\n");
				printf(" -MSK -I_post ... Image-Filter posterize\n");
				printf(" -MSK -I_Ben3 ... Image-Filter border enhance (range 3)\n");
				printf(" -MSK -I_Ben5 ... Image-Filter border enhance (range 5)\n");
				printf(" -MSK -I_iden ... Image-Filter identity\n");
				printf(" -MSK -I_shar ... Image-Filter sharpen\n");
				printf(" -MSK -I_blB3 ... Image-Filter blure box 3\n");
				printf(" -MSK -I_blB5 ... Image-Filter blure box 5\n");
				printf(" -MSK -I_blG3 ... Image-Filter blure Gauss 3\n");
				printf(" -MSK -I_blG5 ... Image-Filter blure Gauss 5\n");
				printf(" -MSK -I_edgL ... Image-Filter edges soft\n");
				printf(" -MSK -I_edgM ... Image-Filter edges midium\n");
				printf(" -MSK -I_edgH ... Image-Filter edges strong\n");
				printf("\n");
				printf("---- ---- ---- ----  ---- ---- ---- ----\n");
				printf("\n");
				printf(" -Shape_c  ... Shape RGB mask color (three integers)\n");
				printf(" -Pixel_mm ... Image # of pixels per mm\n");
				printf(" -Camera_F ... Camera focal distance (mm)\n");
				printf(" -RotAxs_M ...  Rotation-Axes autodetection (y/n)\n");
				printf(" -RotAxs_E ...  Rotation-Axes error (pixels)\n");
				printf(" -Mesh_apr ... (%%) threshold minumum surface & volume per facet\n");
				printf(" -STL_type ... 'ASCII'(default) / 'BINARY' / 'NONE' STL file type\n");
				printf(" -Skip_add ... Add range to the list of images to ignore (Id_start Id_end)\n");
				printf(" -SS_Up_add ... Add range to the list of polygons' images for which to ignore the Up side (Id_start Id_end)\n");
				printf(" -SS_Do_add ... Add range to the list of polygons' images for which to ignore the Down side (Id_start Id_end)\n");
				printf(" -SS_Lt_add ... Add range to the list of polygons' images for which to ignore the Left side  (Id_start Id_end)\n");
				printf(" -SS_Rt_add ... Add range to the list of polygons' images for which to ignore the Right side (Id_start Id_end)\n");
				printf("\n");
				printf("---- ---- ---- ----  ---- ---- ---- ----\n");
				printf("\n");
				printf(" -A_algorithm  ... Absorption Algorithm (None/Any/Analytic/Discrete/MonteCarlo/Beam_MC)\n");
				printf(" -A_resolution ... Vertex resolution for the analytic algorithm\n");
				printf(" -A_iterations ... Max number of iterations for MC algorithms\n");
				printf(" -A_converge   ... Convergence threshold for MC algorithms\n");
				printf(" -A_Lattice_s  ... Number of lattice step for the discrete algorithm\n");
				printf(" -A_Lattice_d  ... Lattice step size for the discrete algorithm\n");
				printf("\n");
				printf(" -A_Sample_Ax ... Instrument vs. Sample axes match (e.g., x/-x y/-y z/-z)\n");
				printf(" -A_Sample_Or ... Instrument vs. Sample orientation match (Alpha, Beta, Gamma)\n");
				printf("\n");
				printf(" -Ref_ty ... Reflections'list dat file-type (txt/idl)\n");
				printf(" -Ref_fp ... Reflections'list data file full path (add new path)\n");
				printf("\n");
				printf(" -Elemnt_ad ... Add new element data (Name, Occupancy)\n");
				printf(" -Elemnt_rm ... Remove exitsiting element (Name)\n");
				printf("\n");
				printf("**** **** **** ****  **** **** **** ****\n");
				return 1;
			}
			else
			{
				if      (!strcmp(code, "-App_bf")) { memset(&(*InData).App_Input_BaseFolder[0], 0, _Nch300*sizeof(char)); strcpy_s(&(*InData).App_Input_BaseFolder[0],   _Nch300, argv[i + 1]); i++; }
				else if (!strcmp(code, "-App_sf")) { memset(&(*InData).App_Input_SubFolder[0],  0, _Nch100*sizeof(char)); strcpy_s(&(*InData).App_Input_SubFolder[0],    _Nch100, argv[i + 1]); i++; }
				else if (!strcmp(code, "-App_na")) { memset(&(*InData).App_Input_Name[0],       0, _Nch100*sizeof(char)); strcpy_s(&(*InData).App_Input_Name[0],         _Nch100, argv[i + 1]); i++; }
				else if (!strcmp(code, "-App_la")) { memset(&(*InData).App_LinearAbsorption[0], 0, _Nch100 * sizeof(char)); strcpy_s(&(*InData).App_LinearAbsorption[0], _Nch100, argv[i + 1]); i++; }
			}
		}
	}
	return 0;
}

int SXD_Absorption_Input_RunSettings_LineCommand(struct SXD_Absorption_Data *InData, int argc, char **argv)
{
	long i;
	
	bool test;
	char code[_Nch100];
	
	rgb_t Mask_color;
	
	for (i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			memset(&code[0], 0, _Nch100*sizeof(char));
			strcpy_s(&code[0], _Nch100, argv[i]); // sscanf_s(argv[i], "%s", &code[0]);
			//
			if      (!strcmp(code, "-Seed"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).User_Seed[0]);
													  sscanf_s(argv[i + 1], "%ld", &(*InData).User_Seed[1]); }
			//
			else if (!strcmp(code, "-Abs_wn"))		{ memset(&(*InData).App_LinearAbsorption[0], 0, _Nch100 * sizeof(char)); strcpy_s(&(*InData).App_LinearAbsorption[0], _Nch100, argv[i + 1]); }
			//
			else if (!strcmp(code, "-Run_wf"))		{ memset(&(*InData).Working_Path[0], 0, _Nch300 * sizeof(char)); strcpy_s(&(*InData).Working_Path[0], _Nch300, argv[i + 1]); }
			else if (!strcmp(code, "-Run_wn"))		{ memset(&(*InData).Working_Name[0], 0, _Nch100 * sizeof(char)); strcpy_s(&(*InData).Working_Name[0], _Nch100, argv[i + 1]); }
			//
			else if (!strcmp(code, "-ShapeF"))		{ if (argv[i + 1][0] == 'y') (*InData).ShapeFromFile = true; else (*InData).ShapeFromFile = false; }
			else if (!strcmp(code, "-ShapeS"))		{ sscanf_s(argv[i + 1], "%lf", &(*InData).Image_RotationAxesShift); }
			//
			else if (!strcmp(code, "-ShapeD"))	{ if (argv[i + 1][0] == 'y') (*InData).DumpShape = true; else (*InData).DumpShape = false; }
			//
			else if (!strcmp(code, "-Break_S"))		{ if (argv[i + 1][0] == 'y') (*InData).Break_StepSequence = true; else (*InData).Break_StepSequence = false; }
			else if (!strcmp(code, "-Break_P"))		{ if (argv[i + 1][0] == 'y') (*InData).Break_PolygonSequence = true; else (*InData).Break_PolygonSequence = false; }
			//
			else if (!strcmp(code, "-360_up"))		{ if (argv[i + 1][0] == 'y') (*InData).Use_Preprocessed_ImageSequence = true; else (*InData).Use_Preprocessed_ImageSequence = false; }
			else if (!strcmp(code, "-360_pf"))		{ memset(&(*InData).ImageSequence_Path[0], 0, _Nch300 * sizeof(char)); strcpy_s(&(*InData).ImageSequence_Path[0], _Nch300, argv[i + 1]); }
			else if (!strcmp(code, "-360_pn"))		{ memset(&(*InData).ImageSequence_Name[0], 0, _Nch100 * sizeof(char)); strcpy_s(&(*InData).ImageSequence_Name[0], _Nch100, argv[i + 1]); }
			else if (!strcmp(code, "-360_it"))		{ memset(&(*InData).ImageSequence_Token[0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).ImageSequence_Token[0], _Nch10, argv[i + 1]); }
			else if (!strcmp(code, "-360_ip"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).ImageSequence_Digits_Count); }
			else if (!strcmp(code, "-360_si"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).ImageSequence_Step_Id[0]);
													  sscanf_s(argv[i + 2], "%ld", &(*InData).ImageSequence_Step_Id[1]); }
			else if (!strcmp(code, "-360_fi"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).ImageSequence_Frame_Id[0]);
													  sscanf_s(argv[i + 2], "%ld", &(*InData).ImageSequence_Frame_Id[1]); }
			else if (!strcmp(code, "-360_ra"))		{ sscanf_s(argv[i + 1], "%lf", &(*InData).ImageSequence_RevolutionAngleError); }
			else if (!strcmp(code, "-Mask_p"))		{ if (argv[i + 1][0] == 'y') (*InData).Use_Prefiltered_ImageSequence = true; else (*InData).Use_Prefiltered_ImageSequence = false;
													  memset(&(*InData).PreFilteredData_Path[0], 0, _Nch300 * sizeof(char)); strcpy_s(&(*InData).PreFilteredData_Path[0], _Nch300, argv[i + 2]);
													  memset(&(*InData).PreFilteredData_Name[0], 0, _Nch100 * sizeof(char)); strcpy_s(&(*InData).PreFilteredData_Name[0], _Nch100, argv[i + 3]); }
			else if (!strcmp(code, "-Mask_c"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).RGB_Mask_Color[0]);
													  sscanf_s(argv[i + 2], "%ld", &(*InData).RGB_Mask_Color[1]);
													  sscanf_s(argv[i + 3], "%ld", &(*InData).RGB_Mask_Color[2]); }
			else if (!strcmp(code, "-Mask_t"))		{ sscanf_s(argv[i + 1], "%lf", &(*InData).Mask_Transparency); }
			else if (!strcmp(code, "-Mask_o"))		{ if (argv[i + 1][0] == 'y') (*InData).MaskFilterOverlap = true; else (*InData).MaskFilterOverlap = false; }
			else if (!strcmp(code, "-MSK"))			{ i++;
													  memset(&code[0], 0, _Nch100*sizeof(char));
													  strcpy_s(&code[0], _Nch100, argv[i]); // sscanf_s(argv[i], "%s", &code[0]);
													  test = false;
													  //
													  if      (!strcmp(code, "-M_swap"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_Swap; }
													  else if (!strcmp(code, "-M_ex_r"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_RegionExclude_Square;
																							  if (argv[i + 1][0] == 'y') (*InData).Filter[(*InData).Filter_Count].in_side = true; else (*InData).Filter[(*InData).Filter_Count].in_side = false;
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][0]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][1]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][1]); }
													  else if (!strcmp(code, "-M_in_r"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_RegionInclude_Square;
																							  if (argv[i + 1][0] == 'y') (*InData).Filter[(*InData).Filter_Count].in_side = true; else (*InData).Filter[(*InData).Filter_Count].in_side = false;
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][0]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][1]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][1]); }
													  else if (!strcmp(code, "-M_expa"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_SquareExpand;
																							  sscanf_s(argv[i + 1], "%ld", &(*InData).Filter[(*InData).Filter_Count].Shell_Thicknes); }
													  else if (!strcmp(code, "-M_cont"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_SquareContract;
																							  sscanf_s(argv[i + 1], "%ld", &(*InData).Filter[(*InData).Filter_Count].Shell_Thicknes); }
													  else if (!strcmp(code, "-M_aver"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Mask_SquareAverage;
																							  sscanf_s(argv[i + 1], "%ld", &(*InData).Filter[(*InData).Filter_Count].Shell_Thicknes); }
													  else if (!strcmp(code, "-M_RGBc"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_SelectColor;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[0]);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[1]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[2]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[1]);
																							  sscanf_s(argv[i + 6], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[2]);
																							  sscanf_s(argv[i + 7], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance); }
													  else if (!strcmp(code, "-M_RGBm"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_SelectColorRGBmismatch;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_mismatch[0]);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_mismatch[1]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_mismatch[2]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[1]);
																							  sscanf_s(argv[i + 6], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[2]);
																							  sscanf_s(argv[i + 7], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance); }
													  else if (!strcmp(code, "-M_RGBe"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_SelectColor_EllipsoidSet;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][0]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][1]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][1]);
																							  strcpy_s(&(*InData).Filter[(*InData).Filter_Count].Path[0], _Nch100, argv[i + 6]);
																							  
																							  if ((*InData).RGB_Mask_Color[0] < 0.0) Mask_color.red   = 0; else if ((*InData).RGB_Mask_Color[0] > 255.0) Mask_color.red   = 255; else Mask_color.red   = (unsigned char)(*InData).RGB_Mask_Color[0];
																							  if ((*InData).RGB_Mask_Color[1] < 0.0) Mask_color.green = 0; else if ((*InData).RGB_Mask_Color[1] > 255.0) Mask_color.green = 255; else Mask_color.green = (unsigned char)(*InData).RGB_Mask_Color[1];
																							  if ((*InData).RGB_Mask_Color[2] < 0.0) Mask_color.blue  = 0; else if ((*InData).RGB_Mask_Color[2] > 255.0) Mask_color.blue  = 255; else Mask_color.blue  = (unsigned char)(*InData).RGB_Mask_Color[2];
																							  ImageFilters_GetColorSet_Ellipsoid((*InData).Filter[(*InData).Filter_Count].Path,
																																				  (*InData).Filter[(*InData).Filter_Count].Region[0][0], (*InData).Filter[(*InData).Filter_Count].Region[0][1],
																																				  (*InData).Filter[(*InData).Filter_Count].Region[1][0], (*InData).Filter[(*InData).Filter_Count].Region[1][1],
																																				  &(*InData).Filter[(*InData).Filter_Count].Ellipse, Mask_color); }
													  else if (!strcmp(code, "-M_RGBp"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_SelectColor_Palette;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][0]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[0][1]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][0]);
																							  sscanf_s(argv[i + 5], "%lf", &(*InData).Filter[(*InData).Filter_Count].Region[1][1]);
																							  sscanf_s(argv[i + 6], "%ld", &(*InData).Filter[(*InData).Filter_Count].Shell_Thicknes);
																							  strcpy_s(&(*InData).Filter[(*InData).Filter_Count].Path[0], _Nch100, argv[i + 7]);
																							  
																							  if ((*InData).RGB_Mask_Color[0] < 0.0) Mask_color.red   = 0; else if ((*InData).RGB_Mask_Color[0] > 255.0) Mask_color.red   = 255; else Mask_color.red   = (unsigned char)(*InData).RGB_Mask_Color[0];
																							  if ((*InData).RGB_Mask_Color[1] < 0.0) Mask_color.green = 0; else if ((*InData).RGB_Mask_Color[1] > 255.0) Mask_color.green = 255; else Mask_color.green = (unsigned char)(*InData).RGB_Mask_Color[1];
																							  if ((*InData).RGB_Mask_Color[2] < 0.0) Mask_color.blue  = 0; else if ((*InData).RGB_Mask_Color[2] > 255.0) Mask_color.blue  = 255; else Mask_color.blue  = (unsigned char)(*InData).RGB_Mask_Color[2];
																							  ImageFilters_GetColorSet_Palette((*InData).Filter[(*InData).Filter_Count].Path,
																																				(*InData).Filter[(*InData).Filter_Count].Region[0][0], (*InData).Filter[(*InData).Filter_Count].Region[0][1],
																																				(*InData).Filter[(*InData).Filter_Count].Region[1][0], (*InData).Filter[(*InData).Filter_Count].Region[1][1],
																																				&(*InData).Filter[(*InData).Filter_Count].Palette, (*InData).Filter[(*InData).Filter_Count].Shell_Thicknes, Mask_color); }
													  else if (!strcmp(code, "-M_red"))		{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickRed;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-M_green"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickGreen;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-M_blue"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickBlue;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-M_gray"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickGray;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-M_white"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickWhite;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-M_black"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_MaskUpdate_PickBlack;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  //
													  else if (!strcmp(code, "-I_gray"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_GrayScale; }
													  else if (!strcmp(code, "-I_coSc"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_ColorGradient_Scale;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_coSS"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_ColorShift_Scale;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[0]);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[1]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_delta[2]);
																							  sscanf_s(argv[i + 4], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_coSh"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_ColorShift;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[0]);
																							  sscanf_s(argv[i + 2], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[1]);
																							  sscanf_s(argv[i + 3], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb[2]); }
													  else if (!strcmp(code, "-I_post"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Posterize;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].rgb_distance); }
													  else if (!strcmp(code, "-I_Ben3"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_BorderEnhance_3;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_Ben5"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_BorderEnhance_5;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_iden"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Identity;
																							  (*InData).Filter[(*InData).Filter_Count].Scale = 1.0; }
													  else if (!strcmp(code, "-I_shar"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Sharpen;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_blB3"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Blure_Box_3;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_blB5"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Blure_Box_5;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_blG3"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Blure_Gauss_3;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_blG5"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Blure_Gauss_5;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_edgL"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Edge_3low;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_edgM"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Edge_3mid;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  else if (!strcmp(code, "-I_edgH"))	{ (*InData).Filter[(*InData).Filter_Count].Tool = &ImageFilters_Edge_3hig;
																							  sscanf_s(argv[i + 1], "%lf", &(*InData).Filter[(*InData).Filter_Count].Scale); }
													  //
													  if (test)
													  {
														  (*InData).Filter_Count++;
														  (*InData).Filter = (struct ImageFilters_Setting*)realloc((*InData).Filter, ((*InData).Filter_Count + 1) * sizeof(struct ImageFilters_Setting));
														  if (_APP_WARNING_MESSAGE((*InData).Filter, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Filter\" to", (*InData).Filter_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  
														  ImageFilters_Reset_Settings(&(*InData).Filter[(*InData).Filter_Count]);
													  } }
			else if (!strcmp(code, "-Shape_c"))		{ sscanf_s(argv[i + 1], "%ld", &(*InData).Shape_RGB_Mask_Color[0]);
													  sscanf_s(argv[i + 2], "%ld", &(*InData).Shape_RGB_Mask_Color[1]);
													  sscanf_s(argv[i + 3], "%ld", &(*InData).Shape_RGB_Mask_Color[2]); }
			else if (!strcmp(code, "-Pixel_mm"))	{ sscanf_s(argv[i + 1], "%ld", &(*InData).Pixel_mm); }
			else if (!strcmp(code, "-Camera_F"))	{ sscanf_s(argv[i + 1], "%lf", &(*InData).Camera_FocalDistance); }
			else if (!strcmp(code, "-RotAxs_M"))	{ if (argv[i + 1][0] == 'y') (*InData).Image_RotationAxesDetection = true; else (*InData).Image_RotationAxesDetection = false; }
			else if (!strcmp(code, "-RotAxs_E"))	{ sscanf_s(argv[i + 1], "%lf", &(*InData).Image_RotationAxesError); }
			else if (!strcmp(code, "-Mesh_apr"))    { sscanf_s(argv[i + 1], "%lf", &(*InData).Mesh_Approximation_Threshold); }
			else if (!strcmp(code, "-STL_type"))	{ memset(&(*InData).STL_type[0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).STL_type[0], _Nch10, argv[i + 1]); }
			else if (!strcmp(code, "-Skip_add"))	{ if((*InData).Skip_Image==NULL)
													  {
														  (*InData).Skip_Count = 1;
														  (*InData).Skip_Image = (long_2*)calloc((*InData).Skip_Count, sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Image\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Image[0][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Image[0][1]);
													  }
													  else
													  {
														  (*InData).Skip_Image = (long_2*)realloc((*InData).Skip_Image, ((*InData).Skip_Count + 1) * sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Image\" to", (*InData).Skip_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Image[(*InData).Skip_Count][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Image[(*InData).Skip_Count][1]);
														  (*InData).Skip_Count++;
													  }
													  }
			else if (!strcmp(code, "-SS_Up_add"))	{ if((*InData).Skip_Side_Up==NULL)
													  {
														  (*InData).Skip_Side_Up_Count = 1;
														  (*InData).Skip_Side_Up = (long_2*)calloc((*InData).Skip_Side_Up_Count, sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Up\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Up[0][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Up[0][1]);
													  }
													  else
													  {
														  (*InData).Skip_Side_Up = (long_2*)realloc((*InData).Skip_Side_Up, ((*InData).Skip_Side_Up_Count + 1) * sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Up\" to", (*InData).Skip_Side_Up_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][1]);
														  (*InData).Skip_Side_Up_Count++;
													  }
													  }
			else if (!strcmp(code, "-SS_Do_add"))	{ if((*InData).Skip_Side_Do==NULL)
													  {
														  (*InData).Skip_Side_Do_Count = 1;
														  (*InData).Skip_Side_Do = (long_2*)calloc((*InData).Skip_Side_Do_Count, sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Do\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Do[0][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Do[0][1]);
													  }
													  else
													  {
														  (*InData).Skip_Side_Do = (long_2*)realloc((*InData).Skip_Side_Do, ((*InData).Skip_Side_Do_Count + 1) * sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Do\" to", (*InData).Skip_Side_Do_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][1]);
														  (*InData).Skip_Side_Do_Count++;
													  }
													  }
			else if (!strcmp(code, "-SS_Lt_add"))	{ if((*InData).Skip_Side_Lt==NULL)
													  {
														  (*InData).Skip_Side_Lt_Count = 1;
														  (*InData).Skip_Side_Lt = (long_2*)calloc((*InData).Skip_Side_Lt_Count, sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Lt\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Lt[0][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Lt[0][1]);
													  }
													  else
													  {
														  (*InData).Skip_Side_Lt = (long_2*)realloc((*InData).Skip_Side_Lt, ((*InData).Skip_Side_Lt_Count + 1) * sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Lt\" to", (*InData).Skip_Side_Lt_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][1]);
														  (*InData).Skip_Side_Lt_Count++;
													  }
													  }
			else if (!strcmp(code, "-SS_Rt_add"))	{ if((*InData).Skip_Side_Rt==NULL)
													  {
														  (*InData).Skip_Side_Rt_Count = 1;
														  (*InData).Skip_Side_Rt = (long_2*)calloc((*InData).Skip_Side_Rt_Count, sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Rt\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Rt[0][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Rt[0][1]);
													  }
													  else
													  {
														  (*InData).Skip_Side_Rt = (long_2*)realloc((*InData).Skip_Side_Rt, ((*InData).Skip_Side_Rt_Count + 1) * sizeof(long_2));
														  if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Rt\" to", (*InData).Skip_Side_Rt_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
														  sscanf_s(argv[i + 1], "%ld", &(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][0]);
														  sscanf_s(argv[i + 2], "%ld", &(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][1]);
														  (*InData).Skip_Side_Rt_Count++;
													  }
													  }
			else if (!strcmp(code, "-A_algorithm")) { memset(&(*InData).Ab_Algorithm_Code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&(*InData).Ab_Algorithm_Code[0], _Nch20, argv[i + 1]); }
			else if (!strcmp(code, "-A_resolution")){ sscanf_s(argv[i + 1], "%lf", &(*InData).Ab_Resolution); }
			else if (!strcmp(code, "-A_iterations")){ sscanf_s(argv[i + 1], "%ld", &(*InData).Ab_Iterations); }
			else if (!strcmp(code, "-A_converge"))	{ sscanf_s(argv[i + 1], "%lf", &(*InData).Ab_Convergence); }
			else if (!strcmp(code, "-A_Lattice_s"))	{ sscanf_s(argv[i + 1], "%ld", &(*InData).Ab_Lattice_StepNumber); }
			else if (!strcmp(code, "-A_Lattice_d"))	{ sscanf_s(argv[i + 1], "%lf", &(*InData).Ab_Lattice_StepSize); }
			else if (!strcmp(code, "-A_Sample_Ax")) { memset(&(*InData).SampleReferenceAxes[0][0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).SampleReferenceAxes[0][0], _Nch10, argv[i + 1]);
													  memset(&(*InData).SampleReferenceAxes[0][0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).SampleReferenceAxes[0][0], _Nch10, argv[i + 2]);
													  memset(&(*InData).SampleReferenceAxes[0][0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).SampleReferenceAxes[0][0], _Nch10, argv[i + 3]); }
			else if (!strcmp(code, "-A_Sample_Or")) { sscanf_s(argv[i + 1], "%lf", &(*InData).SampleOrientation[0]);
													  sscanf_s(argv[i + 2], "%lf", &(*InData).SampleOrientation[1]);
													  sscanf_s(argv[i + 3], "%lf", &(*InData).SampleOrientation[2]); }
			else if (!strcmp(code, "-Ref_fp"))		{ memset(&(*InData).ReflectionList_FileType_Code[0], 0, _Nch500 * sizeof(char)); strcpy_s(&(*InData).ReflectionList_FileType_Code[0], _Nch20, argv[i + 1]); }
			else if (!strcmp(code, "-Ref_fp"))		{ if((*InData).ReflectionList_Path!=NULL)    (*InData).ReflectionList_Path = (char**)realloc((*InData).ReflectionList_Path, ((*InData).ReflectionList_Count+1) * sizeof(char*));
													  else { (*InData).ReflectionList_Count = 0; (*InData).ReflectionList_Path = (char**)calloc(1, sizeof(char*)); }
													  if (_APP_WARNING_MESSAGE((*InData).ReflectionList_Path, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_ALLOC, "\"(*InData).ReflectionList_Path\" to", (*InData).ReflectionList_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
													  (*InData).ReflectionList_Path[(*InData).ReflectionList_Count] = (char*)calloc(_Nch500, sizeof(char));
													  if (_APP_WARNING_MESSAGE((*InData).ReflectionList_Path[(*InData).ReflectionList_Count], "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).ReflectionList_Path[(*InData).ReflectionList_Count]\" to", _Nch500, 0)) { _USER_IO_WAIT(); return 1; }
													  memset(&(*InData).ReflectionList_Path[(*InData).ReflectionList_Count][0], 0, _Nch500 * sizeof(char)); strcpy_s(&(*InData).ReflectionList_Path[(*InData).ReflectionList_Count][0], _Nch500, argv[i + 1]);
													  (*InData).ReflectionList_Count++; }
			else if (!strcmp(code, "-Element_ad"))	{ (*InData).Element = (struct LinearAbsorption_Element*)realloc((*InData).Element, ((*InData).Elements_Count + 1) * sizeof(struct LinearAbsorption_Element));
													  if (_APP_WARNING_MESSAGE((*InData).Element, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Element\" to", (*InData).Elements_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
													  memset(&(*InData).Element[(*InData).Elements_Count].Name[0], 0, _Nch20 * sizeof(char)); strcpy_s(&(*InData).Element[(*InData).Elements_Count].Name[0], _Nch20, argv[i + 1]);
													  (*InData).Elements_Count++; }
			else if (!strcmp(code, "-Element_rm"))	{ for (i = 0; i < (*InData).Elements_Count; i++) { if(!strcmp((*InData).Element[i].Name, argv[i + 1])) {
														(*InData).Elements_Count--;
														if (i != (*InData).Elements_Count)
														{
															strcpy_s(&(*InData).Element[i].Name[0], _Nch20, &(*InData).Element[(*InData).Elements_Count].Name[0]);
															(*InData).Element[i].Occupancy = (*InData).Element[(*InData).Elements_Count].Occupancy;
															(*InData).Element = (struct LinearAbsorption_Element*)realloc((*InData).Element, ((*InData).Elements_Count) * sizeof(struct LinearAbsorption_Element));
															if (_APP_WARNING_MESSAGE((*InData).Element, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Element\" to", (*InData).Elements_Count, 0)) { _USER_IO_WAIT(); return 1; }
														}
														i = (*InData).Elements_Count;
													  } } }
		}
	}
	return 0;
}

void SXD_Absorption_Input_RunSettings_Data_Location(struct SXD_Absorption_Data *InData)
{
	if (strlen((*InData).App_Input_BaseFolder) < 1)
	{
		if ((_APP_INPUT_USE_COMPILE_LOCATION == 'Y') || (strlen(_APP_INPUT_LOCATION_BASE_FOLDER)<1)) _getcwd(&(*InData).App_Input_BaseFolder[0], _Nch300*sizeof(char));
		else sprintf_s(&(*InData).App_Input_BaseFolder[0], _Nch300, "%s", _APP_INPUT_LOCATION_BASE_FOLDER);
	}

	if (strlen((*InData).App_Input_SubFolder) < 1)
	{
		if ((_APP_INPUT_USE_COMPILE_LOCATION == 'Y') && (strlen(_APP_INPUT_LOCATION_SUB_FOLDER)>=1)) sprintf_s(&(*InData).App_Input_SubFolder[0], _Nch100, "%s", _APP_INPUT_LOCATION_SUB_FOLDER);
	}

	if (strlen((*InData).App_Input_Name) < 1)
	{
		if ((_APP_INPUT_USE_COMPILE_LOCATION == 'Y') && (strlen(_APP_INPUT_LOCATION_NAME)>=1)) sprintf_s(&(*InData).App_Input_Name[0], _Nch100, "%s", _APP_INPUT_LOCATION_NAME);
	}
	
	if (strlen((*InData).App_LinearAbsorption) < 1)
	{
		if ((_APP_INPUT_USE_COMPILE_LOCATION == 'Y') && (strlen(_APP_LINEAR_ABSORPTION_NAME) >= 1)) sprintf_s(&(*InData).App_LinearAbsorption[0], _Nch100, "%s", _APP_LINEAR_ABSORPTION_NAME);
	}
	
	return;
}

int SXD_Absorption_Input_ImageFilters_Upload(FILE* DAT, struct ImageFilters_Setting* FilterSet, long *Color)
{
	long i, j;
	
	char buffer[_Nch100];

	rgb_t Mask_color;

	if (Color[0] < 0.0) Mask_color.red   = 0; else if (Color[0] > 255.0) Mask_color.red   = 255; else Mask_color.red   = (unsigned char)Color[0];
	if (Color[1] < 0.0) Mask_color.green = 0; else if (Color[1] > 255.0) Mask_color.green = 255; else Mask_color.green = (unsigned char)Color[1];
	if (Color[2] < 0.0) Mask_color.blue  = 0; else if (Color[2] > 255.0) Mask_color.blue  = 255; else Mask_color.blue  = (unsigned char)Color[2];
	
	memset(&buffer[0], 0, _Nch100 * sizeof(char)); File_IO_GetValue("\"Image-Filter\"", "None", DAT, NULL, NULL, ":", 1, 's', &buffer[0], _Nch100, NULL);
	
	i = 0; while (((buffer[i] == ' ') || (buffer[i] == '\t')) && (i < _Nch100 - 1)) i++;
	
	if (i > 0) { j = 0; while (i < _Nch100) { buffer[j] = buffer[i]; i++; j++; }}
	
	i = _Nch100 - 1; while (((buffer[i] == ' ') || (buffer[i] == '\t') || (buffer[i] == '\n') || (buffer[i] == 0) || (buffer[i] == ':')) && (i > 0)) i--;
	for (j = i + 1; j < _Nch100; j++) buffer[j] = 0;
	
	// Mask
	
	if      (!strcmp(buffer, "image-mask swap"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_Swap;
	}
	else if (!strcmp(buffer, "image-mask exclude region"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_RegionExclude_Square;
		if (File_IO_GetValue("image-mask exclude region", "y", DAT, "<", NULL, ">", 1, 'b', &(*FilterSet).in_side, 0, NULL))							{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask exclude region", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[0], 0, NULL))						{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask exclude region", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[1], 0, NULL))						{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask include region"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_RegionInclude_Square;
		if (File_IO_GetValue("image-mask include region", "y", DAT, "<", NULL, ">", 1, 'b', &(*FilterSet).in_side, 0, NULL))							{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask include region", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[0], 0, NULL))						{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask include region", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[1], 0, NULL))						{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask expand"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_SquareExpand;
		if (File_IO_GetValue("image-mask expand", "0.0", DAT, "<", NULL, ">", 1, 'l', &(*FilterSet).Shell_Thicknes, 0, NULL))							{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask contract"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_SquareContract;
		if (File_IO_GetValue("image-mask contract", "0.0", DAT, "<", NULL, ">", 1, 'l', &(*FilterSet).Shell_Thicknes, 0, NULL))							{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask average"))
	{
		(*FilterSet).Tool = &ImageFilters_Mask_SquareAverage;
		if (File_IO_GetValue("image-mask average", "0.0", DAT, "<", NULL, ">", 1, 'l', &(*FilterSet).Shell_Thicknes, 0, NULL))							{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask select via RGB color"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_SelectColor;
		if (File_IO_GetValue("image-mask select via RGB color", "0.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb[0], 0, NULL))						{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask select via RGB color", "0.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb_delta[0], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask select via RGB color", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color mismatch"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_SelectColorRGBmismatch;
		if (File_IO_GetValue("image-mask via RGB color mismatch", "0.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb_mismatch[0], 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color mismatch", "0.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb_delta[0], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color mismatch", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-set ellipse"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_SelectColor_EllipsoidSet;
		//
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[0], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[1], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "\"", NULL, "\"", 1, 's', &(*FilterSet).Path[0], _Nch800, NULL))	{ _USER_IO_WAIT(); return 0; }
		
		ImageFilters_GetColorSet_Ellipsoid((*FilterSet).Path, (*FilterSet).Region[0][0], (*FilterSet).Region[0][1], (*FilterSet).Region[1][0], (*FilterSet).Region[1][1], &(*FilterSet).Ellipse, Mask_color);
	}
	else if (!strcmp(buffer, "image-mask via RGB color-set palette"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_SelectColor_Palette;
		//
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[0], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "<", NULL, ">", 2, 'd', &(*FilterSet).Region[1], 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "1", DAT, "<", NULL, ">", 1, 'l', &(*FilterSet).Shell_Thicknes, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
		
		if (File_IO_GetValue("image-mask via RGB color-set ellipse", "0.0", DAT, "\"", NULL, "\"", 1, 's', &(*FilterSet).Path[0], _Nch800, NULL))		{ _USER_IO_WAIT(); return 0; }
		
		ImageFilters_GetColorSet_Palette((*FilterSet).Path, (*FilterSet).Region[0][0], (*FilterSet).Region[0][1], (*FilterSet).Region[1][0], (*FilterSet).Region[1][1], &(*FilterSet).Palette, (*FilterSet).Shell_Thicknes, Mask_color);
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick red"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickRed;
		if (File_IO_GetValue("image-mask via RGB color-pick red", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-pick red", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick green"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickGreen;
		if (File_IO_GetValue("image-mask via RGB color-pick green", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-pick green", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick blue"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickBlue;
		if (File_IO_GetValue("image-mask via RGB color-pick blue", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-mask via RGB color-pick blue", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick gray"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickGray;
		if (File_IO_GetValue("image-mask via RGB color-pick gray", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick white"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickWhite;
		if (File_IO_GetValue("image-mask via RGB color-pick white", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-mask via RGB color-pick black"))
	{
		(*FilterSet).Tool = &ImageFilters_MaskUpdate_PickBlack;
		if (File_IO_GetValue("image-mask via RGB color-pick black", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))			{ _USER_IO_WAIT(); return 0; }
	}
	
	// Image
	
	else if (!strcmp(buffer, "image-filter grayscale"))
	{
		(*FilterSet).Tool = &ImageFilters_GrayScale;
	}
	else if (!strcmp(buffer, "image-filter color-gradient scale"))
	{
		(*FilterSet).Tool = &ImageFilters_ColorGradient_Scale;
		if (File_IO_GetValue("image-filter color-gradient scale", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter color-shift scale"))
	{
		(*FilterSet).Tool = &ImageFilters_ColorShift_Scale;
		if (File_IO_GetValue("image-filter color-shift scale scale", "1.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb_delta[0], 0, NULL))			{ _USER_IO_WAIT(); return 0; }
		if (File_IO_GetValue("image-filter color-shift scale scale", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter color-shift"))
	{
		(*FilterSet).Tool = &ImageFilters_ColorShift;
		if (File_IO_GetValue("image-filter color-shift", "0.0", DAT, "<", NULL, ">", 3, 'd', &(*FilterSet).rgb[0], 0, NULL))							{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter posterize"))
	{
		(*FilterSet).Tool = &ImageFilters_Posterize;
		if (File_IO_GetValue("image-filter posterize", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).rgb_distance, 0, NULL))						{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter border enhance (range 3)"))
	{
		(*FilterSet).Tool = &ImageFilters_BorderEnhance_3;
		if (File_IO_GetValue("image-filter border enhance (range 3)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter border enhance (range 5)"))
	{
		(*FilterSet).Tool = &ImageFilters_BorderEnhance_5;
		if (File_IO_GetValue("image-filter border enhance (range 5)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))				{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter identity"))
	{
		(*FilterSet).Tool = &ImageFilters_Identity;
		(*FilterSet).Scale = 1.0;
	}
	else if (!strcmp(buffer, "image-filter sharpen"))
	{
		(*FilterSet).Tool = &ImageFilters_Sharpen;
		if (File_IO_GetValue("image-filter sharpen", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))									{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter blure box (range 3)"))
	{
		(*FilterSet).Tool = &ImageFilters_Blure_Box_3;
		if (File_IO_GetValue("image-filter blure box (range 3)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))						{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter blure box (range 5)"))
	{
		(*FilterSet).Tool = &ImageFilters_Blure_Box_5;
		if (File_IO_GetValue("image-filter blure box (range 5)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))						{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter blure Gauss (range 3)"))
	{
		(*FilterSet).Tool = &ImageFilters_Blure_Gauss_3;
		if (File_IO_GetValue("image-filter blure Gauss (range 3)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter blure Gauss (range 5)"))
	{
		(*FilterSet).Tool = &ImageFilters_Blure_Gauss_5;
		if (File_IO_GetValue("image-filter blure Gauss (range 5)", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter edge-detection soft"))
	{
		(*FilterSet).Tool = &ImageFilters_Edge_3low;
		if (File_IO_GetValue("image-filter edge-detection soft", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter edge-detection midium"))
	{
		(*FilterSet).Tool = &ImageFilters_Edge_3mid;
		if (File_IO_GetValue("image-filter edge-detection midium", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else if (!strcmp(buffer, "image-filter edge-detection strong"))
	{
		(*FilterSet).Tool = &ImageFilters_Edge_3hig;
		if (File_IO_GetValue("image-filter edge-detection strong", "0.0", DAT, "<", NULL, ">", 1, 'd', &(*FilterSet).Scale, 0, NULL))					{ _USER_IO_WAIT(); return 0; }
	}
	else return 0;
	
	return 1;
}

int  SXD_Absorption_Input_RunSettings_Data_TestUpdate(struct SXD_Absorption_Data *InData)
{
	long i, j, k;
	long Count;
	
	bool* kk;
	
	char code[_Nch20];
	
	for (i = 0; i < _Nch300; i++) if ((*InData).App_Input_BaseFolder[i] == '\\') (*InData).App_Input_BaseFolder[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).App_Input_SubFolder[i] == '\\')  (*InData).App_Input_SubFolder[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).App_Input_Name[i] == '\\')       (*InData).App_Input_Name[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).App_LinearAbsorption[i] == '\\') (*InData).App_LinearAbsorption[i] = '/';
	
	for (i = 0; i < _Nch100; i++) if ((*InData).App_LinearAbsorption[i] == '\\') (*InData).App_LinearAbsorption[i] = '/';
	
	for (i = 0; i < _Nch300; i++) if ((*InData).Working_Path[i] == '\\') (*InData).Working_Path[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).Working_Name[i] == '\\') (*InData).Working_Name[i] = '/';
	
	for (i = 0; i < _Nch300; i++) if ((*InData).ImageSequence_Path[i] == '\\') (*InData).ImageSequence_Path[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).ImageSequence_Name[i] == '\\') (*InData).ImageSequence_Name[i] = '/';
	
	for (i = 0; i < _Nch300; i++) if ((*InData).PreFilteredData_Path[i] == '\\') (*InData).PreFilteredData_Path[i] = '/';
	for (i = 0; i < _Nch100; i++) if ((*InData).PreFilteredData_Name[i] == '\\') (*InData).PreFilteredData_Name[i] = '/';
	
	for (i = 0; i < 3; i++)
	{
		if ((*InData).RGB_Mask_Color[i] > 255) (*InData).RGB_Mask_Color[i] = 255;
		if ((*InData).RGB_Mask_Color[i] < 0)   (*InData).RGB_Mask_Color[i] = 0;
	}
	
	if ((*InData).Threads_Count < 1) (*InData).Threads_Count = 1;
	
#if _THREADING_EXCLUDE == 'Y'
	(*InData).Threads_Count = 1;
#endif
	
	if      ((*InData).Mask_Transparency < 0.0)   (*InData).Mask_Transparency = 0.0;
	else if ((*InData).Mask_Transparency > 100.0) (*InData).Mask_Transparency = 100.0;
	
	if      ((*InData).Mesh_Approximation_Threshold > 100.0) (*InData).Mesh_Approximation_Threshold = 100.0;
	else if ((*InData).Mesh_Approximation_Threshold < 0.0)   (*InData).Mesh_Approximation_Threshold = 0.0;
	
	if ((*InData).Skip_Count > 0)
	{
		Count = 0; for (i = 0; i < (*InData).Skip_Count; i++)
		{
			if ((*InData).Skip_Image[i][1] < (*InData).Skip_Image[i][0]) { j = (*InData).Skip_Image[i][0]; (*InData).Skip_Image[i][0] = (*InData).Skip_Image[i][1]; (*InData).Skip_Image[i][1] = j; }
			if ((*InData).Skip_Image[i][0] < 0) (*InData).Skip_Image[i][0] = -1;
			if ((*InData).Skip_Image[i][1] < 0) (*InData).Skip_Image[i][1] = -1;
			if ((*InData).Skip_Image[i][1] > Count) Count = (*InData).Skip_Image[i][1];
		}
		
		kk = (bool*)calloc(Count + 3, sizeof(bool));
		if (_APP_WARNING_MESSAGE(kk, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_CALLOC, "\"kk\" to", Count, 0)) { _USER_IO_WAIT(); return 1; }
		
		for (i = 0; i < (*InData).Skip_Count; i++) { for (j = (*InData).Skip_Image[i][0]; j <= (*InData).Skip_Image[i][1]; j++) kk[j + 1] = true; }
		
		i = 0;
		k = 0;
		for (j = 1; j < Count + 2; j++)
		{
			if (kk[j])
			{
				(*InData).Skip_Image[i][0] = k;
				while ((kk[j]) && (j < Count + 2)) { j++; k++; }
				(*InData).Skip_Image[i][1] = k - 1;
				i++;
			}
			k++;
		}
		
		if (i > (*InData).Skip_Count) { _APP_WARNING_MESSAGE(NULL, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_VALUE, "Inconstent number of ranges of snapshots to neglect", 0, 0); _USER_IO_WAIT(); return 1; }
		
		(*InData).Skip_Count = i;
		(*InData).Skip_Image = (long_2*)realloc((*InData).Skip_Image, (*InData).Skip_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Image\" to", (*InData).Skip_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	
	if ((*InData).Skip_Side_Up_Count > 0)
	{
		Count = 0; for (i = 0; i < (*InData).Skip_Side_Up_Count; i++)
		{
			if ((*InData).Skip_Side_Up[i][1] < (*InData).Skip_Side_Up[i][0]) { j = (*InData).Skip_Side_Up[i][0]; (*InData).Skip_Side_Up[i][0] = (*InData).Skip_Side_Up[i][1]; (*InData).Skip_Side_Up[i][1] = j; }
			if ((*InData).Skip_Side_Up[i][0] < 0) (*InData).Skip_Side_Up[i][0] = -1;
			if ((*InData).Skip_Side_Up[i][1] < 0) (*InData).Skip_Side_Up[i][1] = -1;
			if ((*InData).Skip_Side_Up[i][1] > Count) Count = (*InData).Skip_Side_Up[i][1];
		}
		
		kk = (bool*)calloc(Count + 3, sizeof(bool));
		if (_APP_WARNING_MESSAGE(kk, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_CALLOC, "\"kk\" to", Count, 0)) { _USER_IO_WAIT(); return 1; }
		
		for (i = 0; i < (*InData).Skip_Side_Up_Count; i++) { for (j = (*InData).Skip_Side_Up[i][0]; j <= (*InData).Skip_Side_Up[i][1]; j++) kk[j + 1] = true; }
		
		i = 0;
		k = 0;
		for (j = 1; j < Count + 2; j++)
		{
			if (kk[j])
			{
				(*InData).Skip_Side_Up[i][0] = k;
				while ((kk[j]) && (j < Count + 2)) { j++; k++; }
				(*InData).Skip_Side_Up[i][1] = k - 1;
				i++;
			}
			k++;
		}
		
		if (i > (*InData).Skip_Side_Up_Count) { _APP_WARNING_MESSAGE(NULL, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_VALUE, "Inconstent number of ranges of snapshots to neglect", 0, 0); _USER_IO_WAIT(); return 1; }
		
		(*InData).Skip_Side_Up_Count = i;
		(*InData).Skip_Side_Up = (long_2*)realloc((*InData).Skip_Side_Up, (*InData).Skip_Side_Up_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Up\" to", (*InData).Skip_Side_Up_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	if ((*InData).Skip_Side_Do_Count > 0)
	{
		Count = 0; for (i = 0; i < (*InData).Skip_Side_Do_Count; i++)
		{
			if ((*InData).Skip_Side_Do[i][1] < (*InData).Skip_Side_Do[i][0]) { j = (*InData).Skip_Side_Do[i][0]; (*InData).Skip_Side_Do[i][0] = (*InData).Skip_Side_Do[i][1]; (*InData).Skip_Side_Do[i][1] = j; }
			if ((*InData).Skip_Side_Do[i][0] < 0) (*InData).Skip_Side_Do[i][0] = -1;
			if ((*InData).Skip_Side_Do[i][1] < 0) (*InData).Skip_Side_Do[i][1] = -1;
			if ((*InData).Skip_Side_Do[i][1] > Count) Count = (*InData).Skip_Side_Do[i][1];
		}
		
		kk = (bool*)calloc(Count + 3, sizeof(bool));
		if (_APP_WARNING_MESSAGE(kk, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_CALLOC, "\"kk\" to", Count, 0)) { _USER_IO_WAIT(); return 1; }
		
		for (i = 0; i < (*InData).Skip_Side_Do_Count; i++) { for (j = (*InData).Skip_Side_Do[i][0]; j <= (*InData).Skip_Side_Do[i][1]; j++) kk[j + 1] = true; }
		
		i = 0;
		k = 0;
		for (j = 1; j < Count + 2; j++)
		{
			if (kk[j])
			{
				(*InData).Skip_Side_Do[i][0] = k;
				while ((kk[j]) && (j < Count + 2)) { j++; k++; }
				(*InData).Skip_Side_Do[i][1] = k - 1;
				i++;
			}
			k++;
		}
		
		if (i > (*InData).Skip_Side_Do_Count) { _APP_WARNING_MESSAGE(NULL, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_VALUE, "Inconstent number of ranges of snapshots to neglect", 0, 0); _USER_IO_WAIT(); return 1; }
		
		(*InData).Skip_Side_Do_Count = i;
		(*InData).Skip_Side_Do = (long_2*)realloc((*InData).Skip_Side_Do, (*InData).Skip_Side_Do_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Do\" to", (*InData).Skip_Side_Do_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	if ((*InData).Skip_Side_Lt_Count > 0)
	{
		Count = 0; for (i = 0; i < (*InData).Skip_Side_Lt_Count; i++)
		{
			if ((*InData).Skip_Side_Lt[i][1] < (*InData).Skip_Side_Lt[i][0]) { j = (*InData).Skip_Side_Lt[i][0]; (*InData).Skip_Side_Lt[i][0] = (*InData).Skip_Side_Lt[i][1]; (*InData).Skip_Side_Lt[i][1] = j; }
			if ((*InData).Skip_Side_Lt[i][0] < 0) (*InData).Skip_Side_Lt[i][0] = -1;
			if ((*InData).Skip_Side_Lt[i][1] < 0) (*InData).Skip_Side_Lt[i][1] = -1;
			if ((*InData).Skip_Side_Lt[i][1] > Count) Count = (*InData).Skip_Side_Lt[i][1];
		}
		
		kk = (bool*)calloc(Count + 3, sizeof(bool));
		if (_APP_WARNING_MESSAGE(kk, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_CALLOC, "\"kk\" to", Count, 0)) { _USER_IO_WAIT(); return 1; }
		
		for (i = 0; i < (*InData).Skip_Side_Lt_Count; i++) { for (j = (*InData).Skip_Side_Lt[i][0]; j <= (*InData).Skip_Side_Lt[i][1]; j++) kk[j + 1] = true; }
		
		i = 0;
		k = 0;
		for (j = 1; j < Count + 2; j++)
		{
			if (kk[j])
			{
				(*InData).Skip_Side_Lt[i][0] = k;
				while ((kk[j]) && (j < Count + 2)) { j++; k++; }
				(*InData).Skip_Side_Lt[i][1] = k - 1;
				i++;
			}
			k++;
		}
		
		if (i > (*InData).Skip_Side_Lt_Count) { _APP_WARNING_MESSAGE(NULL, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_VALUE, "Inconstent number of ranges of snapshots to neglect", 0, 0); _USER_IO_WAIT(); return 1; }
		
		(*InData).Skip_Side_Lt_Count = i;
		(*InData).Skip_Side_Lt = (long_2*)realloc((*InData).Skip_Side_Lt, (*InData).Skip_Side_Lt_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Lt\" to", (*InData).Skip_Side_Lt_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	if ((*InData).Skip_Side_Rt_Count > 0)
	{
		Count = 0; for (i = 0; i < (*InData).Skip_Side_Rt_Count; i++)
		{
			if ((*InData).Skip_Side_Rt[i][1] < (*InData).Skip_Side_Rt[i][0]) { j = (*InData).Skip_Side_Rt[i][0]; (*InData).Skip_Side_Rt[i][0] = (*InData).Skip_Side_Rt[i][1]; (*InData).Skip_Side_Rt[i][1] = j; }
			if ((*InData).Skip_Side_Rt[i][0] < 0) (*InData).Skip_Side_Rt[i][0] = -1;
			if ((*InData).Skip_Side_Rt[i][1] < 0) (*InData).Skip_Side_Rt[i][1] = -1;
			if ((*InData).Skip_Side_Rt[i][1] > Count) Count = (*InData).Skip_Side_Rt[i][1];
		}
		
		kk = (bool*)calloc(Count + 3, sizeof(bool));
		if (_APP_WARNING_MESSAGE(kk, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_CALLOC, "\"kk\" to", Count, 0)) { _USER_IO_WAIT(); return 1; }
		
		for (i = 0; i < (*InData).Skip_Side_Rt_Count; i++) { for (j = (*InData).Skip_Side_Rt[i][0]; j <= (*InData).Skip_Side_Rt[i][1]; j++) kk[j + 1] = true; }
		
		i = 0;
		k = 0;
		for (j = 1; j < Count + 2; j++)
		{
			if (kk[j])
			{
				(*InData).Skip_Side_Rt[i][0] = k;
				while ((kk[j]) && (j < Count + 2)) { j++; k++; }
				(*InData).Skip_Side_Rt[i][1] = k - 1;
				i++;
			}
			k++;
		}
		
		if (i > (*InData).Skip_Side_Rt_Count) { _APP_WARNING_MESSAGE(NULL, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_VALUE, "Inconstent number of ranges of snapshots to neglect", 0, 0); _USER_IO_WAIT(); return 1; }
		
		(*InData).Skip_Side_Rt_Count = i;
		(*InData).Skip_Side_Rt = (long_2*)realloc((*InData).Skip_Side_Rt, (*InData).Skip_Side_Rt_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_Data_Test", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Rt\" to", (*InData).Skip_Side_Rt_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "NONE");   if (strcmp(&code[0], &(*InData).STL_type[0])) {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "BINARY"); if (strcmp(&code[0], &(*InData).STL_type[0])) {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "ASCII");  if (strcmp(&code[0], &(*InData).STL_type[0])) {
	memset(&(*InData).STL_type[0], 0, _Nch10 * sizeof(char)); strcpy_s(&(*InData).STL_type[0], _Nch10, "ASCII");
	}}}
	
	for (i = 0; i < 3; i++)
	{
		if ((*InData).Shape_RGB_Mask_Color[i] > 255) (*InData).Shape_RGB_Mask_Color[i] = 255;
		if ((*InData).Shape_RGB_Mask_Color[i] < 0)   (*InData).Shape_RGB_Mask_Color[i] = 0;
	}
	
	if ((*InData).Pixel_mm < 1) (*InData).Pixel_mm = 1;
	(*InData).PixelSize = 1.0 / (double)(*InData).Pixel_mm;
	
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "None");			if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm = -1; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "Any");			if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm =  0; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "Analytic");		if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm =  1; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "Discrete");		if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm =  2; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "MonteCarlo");	if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm =  3; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "Beam_MC");		if (!strcmp(&code[0], &(*InData).Ab_Algorithm_Code[0])) (*InData).Ab_Algorithm =  4; else {
	memset(&(*InData).Ab_Algorithm_Code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&(*InData).Ab_Algorithm_Code[0], _Nch20, "None");                   (*InData).Ab_Algorithm = -1;
	}}}}}}
	
	if ((*InData).Ab_Resolution < FLT_EPSILON) (*InData).Ab_Resolution = FLT_EPSILON;
	
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "TXT");			if (!strcmp(&code[0], &(*InData).ReflectionList_FileType_Code[0])) (*InData).ReflectionList_FileType = 0; else {
	memset(&code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&code[0], _Nch20, "IDL");			if (!strcmp(&code[0], &(*InData).ReflectionList_FileType_Code[0])) (*InData).ReflectionList_FileType = 1; else {
	memset(&(*InData).ReflectionList_FileType_Code[0], 0, _Nch20 * sizeof(char)); strcpy_s(&(*InData).ReflectionList_FileType_Code[0], _Nch20, "none");        (*InData).ReflectionList_FileType = -1;
	}}
	
	(*InData).SampleOrientation[0] *= _CONST_DegToRad;
	(*InData).SampleOrientation[1] *= _CONST_DegToRad;
	(*InData).SampleOrientation[2] *= _CONST_DegToRad;
	
	return 0;
}
int  SXD_Absorption_Input_RunSettings_Data_Read(struct SXD_Absorption_Data *InData)
{
	long i;
	long Error_Id;
	
	long File_Length;
	long Filters_Line;
	long Filters_Count;
	
	char buffer[_Nch800];
	char* Default_token, * token;
	char* Default_subtoken, * subtoken;
	
	char* buffer_ptr[3];
	
	char InData_Pth[_Nch500];
	FILE *DAT;
	
	memset(&InData_Pth[0], 0, _Nch500*sizeof(char));
	sprintf_s(&InData_Pth[0], _Nch500, "%s/%s/%s", (*InData).App_Input_BaseFolder, (*InData).App_Input_SubFolder, (*InData).App_Input_Name);
	
	DAT = File_IO_Open(InData_Pth, "r");
	
	rewind(DAT);
	File_Length = File_IO_Get_FileLength(DAT);
	rewind(DAT);

	Error_Id = 0;
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "|||||||||| SOFTWARE RUN SETUP ||||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Random Generator Seed Numbers", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("User Seed", "0", DAT, "[", NULL, "]", 2, 'l', &(*InData).User_Seed[0], 0, NULL))														{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Number of CPU threads to use", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("User Seed", "0", DAT, "[", NULL, "]", 1, 'l', &(*InData).Threads_Count, 0, NULL))														{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "||||||||||  GENERAL SETTINGS  ||||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Output (working) base directory path", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Working Path", "./", DAT, "[", NULL, "]", 1, 's', &(*InData).Working_Path[0], _Nch300, NULL))											{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Output (working) base file root name", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Working Name", "SXD_Absorption", DAT, "[", NULL, "]", 1, 's', &(*InData).Working_Name[0], _Nch100, NULL))								{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Import crystal shape from STL file", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Shape from STL file", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).ShapeFromFile, 0, NULL))												{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																			  if (File_IO_GetValue("Rotation axes shift", "0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Image_RotationAxesShift, 0, NULL))									{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Ignore shape save and mesh match", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Ignore shape save and mesh match", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).DumpShape, 0, NULL))									{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Break after Step Frame-Sequence", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Break after Step", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).Break_StepSequence, 0, NULL))											{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Break after Polygon Frame-Sequence", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Break after Polygon", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).Break_PolygonSequence, 0, NULL))										{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "|||||||||| 360 FRAME SEQUENCE ||||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Use Preprocessed Sequence (y/n)", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Use preprocessed sequence", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).Use_Preprocessed_ImageSequence, 0, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence file directory path", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Preprocessed sequence path", "./", DAT, "[", NULL, "]", 1, 's', &(*InData).ImageSequence_Path, _Nch300, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence file name", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Preprocessed sequence name", "Sequence", DAT, "[", NULL, "]", 1, 's', &(*InData).ImageSequence_Name, _Nch100, NULL))					{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																			  if (File_IO_GetValue("Preprocessed sequence token", "_", DAT, "\"", NULL, "\"", 1, 'w', &(*InData).ImageSequence_Token, _Nch10, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																			  if (File_IO_GetValue("Preprocessed sequence 0 padding", "0", DAT, "[", NULL, "]", 1, 'W', &(*InData).ImageSequence_Digits_Count, 0, NULL))					{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence revolution step-id", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Revolution Image Sequence Step - Id", "0", DAT, "[", "-", "]", 2, 'l', &(*InData).ImageSequence_Step_Id[0], 0, NULL))					{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence revolution frame-id", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Revolution Image Sequence Frame - Id", "0", DAT, "[", "-", "]", 2, 'l', &(*InData).ImageSequence_Frame_Id[0], 0, NULL))					{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence revolution angle error", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Revolution Angle Error(actual - 360°)", "0", DAT, "[", NULL, "]", 1, 'd', &(*InData).ImageSequence_RevolutionAngleError, 0, NULL))		{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "||||||||||  IMAGE  FILTERING  ||||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Use Prefiltered Sequence", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Use prefiltered sequence", "n", DAT, "[", NULL, "]", 1, 'b', &(*InData).Use_Prefiltered_ImageSequence, 0, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																	  if((*InData).Use_Prefiltered_ImageSequence)
																																	  {
																																		  if (File_IO_GetValue("Prefiltered Path", "./", DAT, "[", NULL, "]", 1, 's', &(*InData).PreFilteredData_Path[0], _Nch300, NULL))									{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																		  if (File_IO_GetValue("Prefiltered Name", "SXD_Absorption", DAT, "[", NULL, "]", 1, 's', &(*InData).PreFilteredData_Name[0], _Nch100, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																	  } }

	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence RGB mask color", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("RGB mask color", "0", DAT, "[", ",", "]", 3, 'l', &(*InData).RGB_Mask_Color[0], 0, NULL))												{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																			  if (File_IO_GetValue("RGB mask color", "0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Mask_Transparency, 0, NULL))												{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence Mask & Filters overlap", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("RGB mask color", "0", DAT, "[", NULL, "]", 1, 'b', &(*InData).MaskFilterOverlap, 0, NULL))												{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	Filters_Line = File_Length - File_IO_Get_FileLength(DAT);
	rewind(DAT);
	for (i = 0; i < Filters_Line; i++) File_IO_JumpToNextLine(DAT);
	
	(*InData).Filter_Count = 0;
	Filters_Count = 0;
	(*InData).Filter = (struct ImageFilters_Setting*)calloc(1, sizeof(struct ImageFilters_Setting));
	if (_APP_WARNING_MESSAGE((*InData).Filter, "App_Input.cpp", "App_Input_RunSettings_Data_Read", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Filter\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
	
	ImageFilters_Reset_Settings(&(*InData).Filter[(*InData).Filter_Count]);
	
	while (!File_IO_Search_KeyWord(DAT, "@", "MSK:", false, true))
	{
		Filters_Count++;
		(*InData).Filter_Count += SXD_Absorption_Input_ImageFilters_Upload(DAT, &(*InData).Filter[(*InData).Filter_Count], &(*InData).RGB_Mask_Color[0]);
		
		(*InData).Filter = (struct ImageFilters_Setting*)realloc((*InData).Filter, ((*InData).Filter_Count + 1) * sizeof(struct ImageFilters_Setting));
		if (_APP_WARNING_MESSAGE((*InData).Filter, "App_Input.cpp", "App_Input_RunSettings_Data_Read", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Filter\" to", (*InData).Filter_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		ImageFilters_Reset_Settings(&(*InData).Filter[(*InData).Filter_Count]);
	}
	
	rewind(DAT);
	for (i = 0; i < Filters_Line; i++) File_IO_JumpToNextLine(DAT);
	for (i = 0; i < Filters_Count; i++) File_IO_Search_KeyWord(DAT, "@", "MSK:", false, true);
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "|||||||||| 3D SHAPE DETECTION ||||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence shape RGB mask color", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Shape RGB mask color", "0", DAT, "[", ",", "]", 3, 'l', &(*InData).Shape_RGB_Mask_Color[0], 0, NULL))									{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image pixel size (mm)", false, true))						{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Pixels per mm", "1", DAT, "[", NULL, "]", 1, 'l', &(*InData).Pixel_mm, _Nch10, NULL))													{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Camera focal distance (mm)", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Camera focal distance", "NONE", DAT, "[", NULL, "]", 1, 'd', &(*InData).Camera_FocalDistance, _Nch10, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image Rotation-Axes autodetection (pixels)", false, true))	{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Rotation-Axes Detection", "y", DAT, "[", NULL, "]", 1, 'b', &(*InData).Image_RotationAxesDetection, 0, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++;
																																			  if (File_IO_GetValue("Rotation-Axes Error", "0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Image_RotationAxesError, 0, NULL))									{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Shape-Mesh STL file type", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("STL file-format type", "NONE", DAT, "[", NULL, "]", 1, 'w', &(*InData).STL_type[0], _Nch10, NULL))										{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Shape-Mesh approximation", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Shape-Mesh approximation", "0.0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Mesh_Approximation_Threshold, 0, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	if (File_IO_Search_KeyPhrase(DAT, "@", "Image-Sequence to skip", false, true))						{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Image sequence range", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))															{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Skip_Count = 0;
	(*InData).Skip_Image = (long_2*)calloc((*InData).Skip_Count + 1, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Image\" to", ((*InData).Skip_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], ",", &token);
	while (Default_token != NULL)
	{
		i = 0; while ((Default_token[i] == ' ') || (Default_token[i] == '\t') || (Default_token[i] == '\v')) i++;
		if (Default_token[i] == '(')
		{
			Default_subtoken = strtok_s(Default_token, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Image[(*InData).Skip_Count][0]);
			Default_subtoken = strtok_s(NULL, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Image[(*InData).Skip_Count][1]);
		}
		else
		{
			sscanf_s(Default_token, "%ld", &(*InData).Skip_Image[(*InData).Skip_Count][0]);
			(*InData).Skip_Image[(*InData).Skip_Count][1] = (*InData).Skip_Image[(*InData).Skip_Count][0];
		}
		(*InData).Skip_Count++;
		
		(*InData).Skip_Image = (long_2*)realloc((*InData).Skip_Image, ((*InData).Skip_Count + 1) * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Image\" to", (*InData).Skip_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, ",", &token);
	}
	if ((*InData).Skip_Count > 0)
	{
		(*InData).Skip_Image = (long_2*)realloc((*InData).Skip_Image, (*InData).Skip_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Image, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Image\" to", (*InData).Skip_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Skip_Image); (*InData).Skip_Image = NULL; }
	
	//
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	if (File_IO_Search_KeyPhrase(DAT, "@", "Up-sides'    polygon exclude for images", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Image sequence range", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))																{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Skip_Side_Up_Count = 0;
	(*InData).Skip_Side_Up = (long_2*)calloc((*InData).Skip_Side_Up_Count + 1, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Up\" to", ((*InData).Skip_Side_Up_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], ",", &token);
	while (Default_token != NULL)
	{
		i = 0; while ((Default_token[i] == ' ') || (Default_token[i] == '\t') || (Default_token[i] == '\v')) i++;
		if (Default_token[i] == '(')
		{
			Default_subtoken = strtok_s(Default_token, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][0]);
			Default_subtoken = strtok_s(NULL, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][1]);
		}
		else
		{
			sscanf_s(Default_token, "%ld", &(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][0]);
			(*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][1] = (*InData).Skip_Side_Up[(*InData).Skip_Side_Up_Count][0];
		}
		(*InData).Skip_Side_Up_Count++;
		
		(*InData).Skip_Side_Up = (long_2*)realloc((*InData).Skip_Side_Up, ((*InData).Skip_Side_Up_Count + 1) * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Up\" to", (*InData).Skip_Side_Up_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, ",", &token);
	}
	if ((*InData).Skip_Side_Up_Count > 0)
	{
		(*InData).Skip_Side_Up = (long_2*)realloc((*InData).Skip_Side_Up, (*InData).Skip_Side_Up_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Up, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Up\" to", (*InData).Skip_Side_Up_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Skip_Side_Up); (*InData).Skip_Side_Up = NULL; }
	
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	if (File_IO_Search_KeyPhrase(DAT, "@", "Down-sides'  polygon exclude for images", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Image sequence range", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))																{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Skip_Side_Do_Count = 0;
	(*InData).Skip_Side_Do = (long_2*)calloc((*InData).Skip_Side_Do_Count + 1, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Do\" to", ((*InData).Skip_Side_Do_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], ",", &token);
	while (Default_token != NULL)
	{
		i = 0; while ((Default_token[i] == ' ') || (Default_token[i] == '\t') || (Default_token[i] == '\v')) i++;
		if (Default_token[i] == '(')
		{
			Default_subtoken = strtok_s(Default_token, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][0]);
			Default_subtoken = strtok_s(NULL, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][1]);
		}
		else
		{
			sscanf_s(Default_token, "%ld", &(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][0]);
			(*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][1] = (*InData).Skip_Side_Do[(*InData).Skip_Side_Do_Count][0];
		}
		(*InData).Skip_Side_Do_Count++;
		
		(*InData).Skip_Side_Do = (long_2*)realloc((*InData).Skip_Side_Do, ((*InData).Skip_Side_Do_Count + 1) * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Do\" to", (*InData).Skip_Side_Do_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, ",", &token);
	}
	if ((*InData).Skip_Side_Do_Count > 0)
	{
		(*InData).Skip_Side_Do = (long_2*)realloc((*InData).Skip_Side_Do, (*InData).Skip_Side_Do_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Do, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Do\" to", (*InData).Skip_Side_Do_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Skip_Side_Do); (*InData).Skip_Side_Do = NULL; }
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Left-sides'  polygon exclude for images", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Image sequence range", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))																{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Skip_Side_Lt_Count = 0;
	(*InData).Skip_Side_Lt = (long_2*)calloc((*InData).Skip_Side_Lt_Count + 1, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Lt\" to", ((*InData).Skip_Side_Lt_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], ",", &token);
	while (Default_token != NULL)
	{
		i = 0; while ((Default_token[i] == ' ') || (Default_token[i] == '\t') || (Default_token[i] == '\v')) i++;
		if (Default_token[i] == '(')
		{
			Default_subtoken = strtok_s(Default_token, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][0]);
			Default_subtoken = strtok_s(NULL, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][1]);
		}
		else
		{
			sscanf_s(Default_token, "%ld", &(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][0]);
			(*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][1] = (*InData).Skip_Side_Lt[(*InData).Skip_Side_Lt_Count][0];
		}
		(*InData).Skip_Side_Lt_Count++;
		
		(*InData).Skip_Side_Lt = (long_2*)realloc((*InData).Skip_Side_Lt, ((*InData).Skip_Side_Lt_Count + 1) * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Lt\" to", (*InData).Skip_Side_Lt_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, ",", &token);
	}
	if ((*InData).Skip_Side_Lt_Count > 0)
	{
		(*InData).Skip_Side_Lt = (long_2*)realloc((*InData).Skip_Side_Lt, (*InData).Skip_Side_Lt_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Lt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Lt\" to", (*InData).Skip_Side_Lt_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Skip_Side_Lt); (*InData).Skip_Side_Lt = NULL; }
	
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	if (File_IO_Search_KeyPhrase(DAT, "@", "Right-sides' polygon exclude for images", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Image sequence range", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))																{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Skip_Side_Rt_Count = 0;
	(*InData).Skip_Side_Rt = (long_2*)calloc((*InData).Skip_Side_Rt_Count + 1, sizeof(long_2));
	if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Skip_Side_Rt\" to", ((*InData).Skip_Side_Rt_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], ",", &token);
	while (Default_token != NULL)
	{
		i = 0; while ((Default_token[i] == ' ') || (Default_token[i] == '\t') || (Default_token[i] == '\v')) i++;
		if (Default_token[i] == '(')
		{
			Default_subtoken = strtok_s(Default_token, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][0]);
			Default_subtoken = strtok_s(NULL, " \t\v(:)", &subtoken);
			sscanf_s(Default_subtoken, "%ld", &(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][1]);
		}
		else
		{
			sscanf_s(Default_token, "%ld", &(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][0]);
			(*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][1] = (*InData).Skip_Side_Rt[(*InData).Skip_Side_Rt_Count][0];
		}
		(*InData).Skip_Side_Rt_Count++;
		
		(*InData).Skip_Side_Rt = (long_2*)realloc((*InData).Skip_Side_Rt, ((*InData).Skip_Side_Rt_Count + 1) * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Rt\" to", (*InData).Skip_Side_Rt_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, ",", &token);
	}
	if ((*InData).Skip_Side_Rt_Count > 0)
	{
		(*InData).Skip_Side_Rt = (long_2*)realloc((*InData).Skip_Side_Rt, (*InData).Skip_Side_Rt_Count * sizeof(long_2));
		if (_APP_WARNING_MESSAGE((*InData).Skip_Side_Rt, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Skip_Side_Rt\" to", (*InData).Skip_Side_Rt_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Skip_Side_Rt); (*InData).Skip_Side_Rt = NULL; }
	//
	
	if (File_IO_Search_KeyPhrase(DAT, NULL, "|||||||| ABSORPTION  CORRECTION ||||||||", true, true))	{ _USER_IO_WAIT(); return 1; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Estimation algorithm", false, true))						{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption algorithm", "None", DAT, "[", NULL, "]", 1, 'w', &(*InData).Ab_Algorithm_Code[0], _Nch20, NULL))								{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Analytic solution _ Resolution", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption resolution", "0.0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Ab_Resolution, 0, NULL))											{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Discrete solution _ Lattice Step Number", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption lattice step number", "0", DAT, "[", NULL, "]", 1, 'l', &(*InData).Ab_Lattice_StepNumber, 0, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Discrete solution _ Lattice Step Size", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption lattice step size", "0.0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Ab_Lattice_StepSize, 0, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Monte Carlo solution _ Max # Iterations", false, true))		{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption iterations", "0", DAT, "[", NULL, "]", 1, 'l', &(*InData).Ab_Iterations, 0, NULL))											{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Monte Carlo solution _ Convergence", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Absorption convergence", "0.0", DAT, "[", NULL, "]", 1, 'd', &(*InData).Ab_Convergence, 0, NULL))										{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Instrument vs. Sample Reference Axes", false, true))		{ _USER_IO_WAIT(); return 1; } else { buffer_ptr[0] = &(*InData).SampleReferenceAxes[0][0];
																																			  buffer_ptr[1] = &(*InData).SampleReferenceAxes[1][0];
																																			  buffer_ptr[2] = &(*InData).SampleReferenceAxes[2][0];
																																			  if (File_IO_GetValue("Instrument vs. Sample Orientation", "x", DAT, "[", ",", "]", 3, 'w', buffer_ptr, _Nch10, NULL))											{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Instrument vs. Sample Orientation", false, true))			{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Instrument vs. Sample Orientation", "x", DAT, "[", ",", "]", 3, 'd', &(*InData).SampleOrientation[0], NULL, NULL))						{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	if (File_IO_Search_KeyPhrase(DAT, "@", "Reflections set data file-type", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Reflections list file-type", "None", DAT, "[", NULL, "]", 1, 'w', &(*InData).ReflectionList_FileType_Code[0], _Nch20, NULL))			{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	if (File_IO_Search_KeyPhrase(DAT, "@", "Reflections set data files", false, true))					{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Reflections'list file count", "0", DAT, "[", NULL, "]", 1, 'l', &(*InData).ReflectionList_Count, NULL, NULL))							{ _USER_IO_WAIT(); return 1; } else Error_Id++;
		(*InData).ReflectionList_Path = (char**)calloc((*InData).ReflectionList_Count, sizeof(char*));
		if (_APP_WARNING_MESSAGE((*InData).ReflectionList_Path, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).ReflectionList_Path\" to", (*InData).ReflectionList_Count, 0)) { _USER_IO_WAIT(); return 1; }
		for (i = 0; i < (*InData).ReflectionList_Count; i++)
		{
			(*InData).ReflectionList_Path[i] = (char*)calloc(_Nch500, sizeof(char));
			if (_APP_WARNING_MESSAGE((*InData).ReflectionList_Path[i], "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).ReflectionList_Path[i]\" to", _Nch500, 0)) { _USER_IO_WAIT(); return 1; }
		}
		for (i = 0; i < (*InData).ReflectionList_Count; i++)                                                                                { if (File_IO_GetValue("Reflections'list file path", "./SXD_Reflections.txt", DAT, "[", NULL, "]", 1, 's', &(*InData).ReflectionList_Path[i][0], _Nch500, NULL)){ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	}
	
	memset(&buffer[0], 0, _Nch800 * sizeof(char));
	if (File_IO_Search_KeyPhrase(DAT, "@", "Unit-cell elemental composition", false, true))				{ _USER_IO_WAIT(); return 1; } else { if (File_IO_GetValue("Unit-cell elemental composition", "-1", DAT, "[", NULL, "]", 1, 's', &buffer[0], 0, NULL))												{ _USER_IO_WAIT(); return 1; } else Error_Id++; }
	
	(*InData).Elements_Count = 0;
	(*InData).Element = (struct LinearAbsorption_Element*)calloc((*InData).Elements_Count + 1, sizeof(struct LinearAbsorption_Element));
	if (_APP_WARNING_MESSAGE((*InData).Element, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_CALLOC, "\"(*InData).Element\" to", ((*InData).Elements_Count + 1) * 2, 0)) { _USER_IO_WAIT(); return 1; }
	
	Default_token = strtok_s(&buffer[0], "(", &token);
	while (Default_token != NULL)
	{
		Default_subtoken = strtok_s(Default_token, " \t\v:", &subtoken);
		strcpy_s(&(*InData).Element[(*InData).Elements_Count].Name[0], _Nch20, Default_subtoken);
		Default_subtoken = strtok_s(NULL, " \t\v:)", &subtoken);
		sscanf_s(Default_subtoken, "%lf", &(*InData).Element[(*InData).Elements_Count].Occupancy);
		
		(*InData).Elements_Count++;
		
		(*InData).Element = (struct LinearAbsorption_Element*)realloc((*InData).Element, ((*InData).Elements_Count + 1) * sizeof(struct LinearAbsorption_Element));
		if (_APP_WARNING_MESSAGE((*InData).Element, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Element\" to", (*InData).Elements_Count + 1, 0)) { _USER_IO_WAIT(); return 1; }
		
		Default_token = strtok_s(NULL, "(", &token);
	}
	if ((*InData).Elements_Count > 0)
	{
		(*InData).Element = (struct LinearAbsorption_Element*)realloc((*InData).Element, (*InData).Elements_Count * sizeof(struct LinearAbsorption_Element));
		if (_APP_WARNING_MESSAGE((*InData).Element, "App_Input.cpp", "App_Input_RunSettings_LineCommand", _WARNING_MESSAGE_KEY_REALLOC, "\"(*InData).Element\" to", (*InData).Elements_Count, 0)) { _USER_IO_WAIT(); return 1; }
	}
	else { free((*InData).Element); (*InData).Element = NULL; }
	
	fclose(DAT);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Output

int SXD_Absorption_Output_RunSettings(struct SXD_Absorption_Data *InData)
{
	time_t RunTime;
	
	long i;
	
	char  RunTimeDate[_Nch100];
	char  buffer[_Nch600], buffer_x[_Nch100];
	FILE *OUT = NULL;
	
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "%s/%s.in%s", (*InData).Working_Path, (*InData).Working_Name, _EXT_TXT);
	if((OUT = File_IO_Open(buffer, "w")) == NULL) return 1;
	
	time(&RunTime);
	ctime_s(&RunTimeDate[0], _Nch100, &RunTime);
	
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "               \"Single Crystal Absorption Tool\"\n");																				Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "          A. Leonardi, Ph.D. - V.%s / %s\n", _APP_VERSION_ID, _APP_VERSION_DATE);													Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "                   %s", RunTimeDate);																								Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Application Ghost Location\n");																									Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   . %s ...\n", (*InData).App_Input_BaseFolder);																					Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   . %s ...\n", (*InData).App_Input_SubFolder);																					Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   . %s\n", (*InData).App_Input_Name);																								Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_1);										  //"  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > User Seed Id.0 : %ld\n", (*InData).User_Seed[0]);																				Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > User Seed Id.1 : %ld\n", (*InData).User_Seed[1]);																				Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > CPU threads    : %ld\n", (*InData).Threads_Count);																				Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Working Location : %s\n", (*InData).Working_Path);																				Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Working File Name: %s\n", (*InData).Working_Name);																				Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Import crystal shape from STL file: %s\n", File_IO_BoolToString((*InData).ShapeFromFile));										Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Supposed rotation axes shift: %lf\n", (*InData).Image_RotationAxesShift);														Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Ignore shape save and mesh match: %s\n", File_IO_BoolToString((*InData).DumpShape));											Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Break after Step frame-sequence   : %s\n", File_IO_BoolToString((*InData).Break_StepSequence));									Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600*sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Break after Polygon frame-sequence: %s\n", File_IO_BoolToString((*InData).Break_PolygonSequence));								Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
	if (!(*InData).ShapeFromFile)
	{
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Use preprocessed image-sequence   : %s\n", File_IO_BoolToString((*InData).Use_Preprocessed_ImageSequence));					Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-sequence file directory path: %s\n", (*InData).ImageSequence_Path);														Log_Stream(OUT, buffer);
	memset(&buffer_x[0], 0, _Nch100 * sizeof(char)); for (i = 0; i < (*InData).ImageSequence_Digits_Count; i++) strcat_s(&buffer_x[0], _Nch100, "0");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-sequence file name          : %s%s%s\n", (*InData).ImageSequence_Name, (*InData).ImageSequence_Token, buffer_x);		Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence revolution step-id : %ld .. %ld\n", (*InData).ImageSequence_Step_Id[0], (*InData).ImageSequence_Step_Id[1]);	Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence revolution frame-id: %ld .. %ld\n", (*InData).ImageSequence_Frame_Id[0], (*InData).ImageSequence_Frame_Id[1]);	Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Working File Name: %lf\n", (*InData).ImageSequence_RevolutionAngleError);														Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Use prefiltered image-sequence   : %s\n", File_IO_BoolToString((*InData).Use_Prefiltered_ImageSequence));						Log_Stream(OUT, buffer);
	if ((*InData).Use_Prefiltered_ImageSequence)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   _ %s\n", (*InData).PreFilteredData_Path);																					Log_Stream(OUT, buffer);
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   _ %s\n", (*InData).PreFilteredData_Name);																					Log_Stream(OUT, buffer);
	}
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > RGB Mask color   : <%d,%ld,%ld>\n", (*InData).RGB_Mask_Color[0], (*InData).RGB_Mask_Color[1], (*InData).RGB_Mask_Color[2]);	Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Mask transparency: %.4lf %%\n", (*InData).Mask_Transparency);																	Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Mask & Filter overlap: %s\n", File_IO_BoolToString((*InData).MaskFilterOverlap));												Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_1);										  //"  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  "//
	Log_Stream(OUT, "\n");
	for (i = 0; i < (*InData).Filter_Count; i++)
	{
		// Mask
		
		if      ((*InData).Filter[i].Tool == &ImageFilters_Mask_Swap)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask swap\n");																									Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Mask_RegionExclude_Square)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask exclude regionn\n");																						Log_Stream(OUT, buffer);
			memset(&buffer_x[0], 0, _Nch100 * sizeof(char)); if((*InData).Filter[i].in_side) strcat_s(&buffer_x[0], _Nch100, "inside"); else strcat_s(&buffer_x[0], _Nch100, "outside");
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... %s region : width < %.4lf .. %.4lf > < %.4lf .. %.4lf > ", buffer_x,
																																			(*InData).Filter[i].Region[0][0], (*InData).Filter[i].Region[0][1],
																																			(*InData).Filter[i].Region[1][0], (*InData).Filter[i].Region[1][1]);	Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Mask_RegionInclude_Square)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask include regionn\n");																						Log_Stream(OUT, buffer);
			memset(&buffer_x[0], 0, _Nch100 * sizeof(char)); if((*InData).Filter[i].in_side) strcat_s(&buffer_x[0], _Nch100, "inside"); else strcat_s(&buffer_x[0], _Nch100, "outside");
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... %s region : < %.4lf .. %.4lf > < %.4lf .. %.4lf > ", buffer_x,
																																			(*InData).Filter[i].Region[0][0], (*InData).Filter[i].Region[0][1],
																																			(*InData).Filter[i].Region[1][0], (*InData).Filter[i].Region[1][1]);	Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Mask_SquareExpand)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask expand\n");																								Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Shell Thicknes (pixel) : %ld\n", (*InData).Filter[i].Shell_Thicknes);												Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Mask_SquareContract)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask contract\n");																								Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Shell Thicknes (pixel) : %ld\n", (*InData).Filter[i].Shell_Thicknes);												Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Mask_SquareAverage)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask average\n");																								Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Box size (pixel) : %ld\n", (*InData).Filter[i].Shell_Thicknes);													Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_SelectColor)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask select via RGB color\n");																					Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color          : %ld %ld %ld\n", (long)(*InData).Filter[i].rgb[0], (long)(*InData).Filter[i].rgb[1], (long)(*InData).Filter[i].rgb[2]);				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color delta    : %.4lf %.4lf %.4lf\n", (*InData).Filter[i].rgb_delta[0], (*InData).Filter[i].rgb_delta[1], (*InData).Filter[i].rgb_delta[2]);			Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_SelectColorRGBmismatch)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color mismatch\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color mismatch : %ld %ld %ld\n", (long)(*InData).Filter[i].rgb_mismatch[0], (long)(*InData).Filter[i].rgb_mismatch[1], (long)(*InData).Filter[i].rgb_mismatch[2]);	Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color delta    : %.4lf %.4lf %.4lf\n", (*InData).Filter[i].rgb_delta[0], (*InData).Filter[i].rgb_delta[1], (*InData).Filter[i].rgb_delta[2]);							Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_SelectColor_EllipsoidSet)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-set ellipse\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Ellipse Scale : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Region : < %.4lf .. %.4lf > < %.4lf .. %.4lf > ", (*InData).Filter[i].Region[0][0], (*InData).Filter[i].Region[0][1],
																																					(*InData).Filter[i].Region[1][0], (*InData).Filter[i].Region[1][1]);									Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Suorce image  : %s\n", (*InData).Filter[i].Path);																	Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_SelectColor_Palette)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-set palette\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Region : < %.4lf .. %.4lf > < %.4lf .. %.4lf > ", (*InData).Filter[i].Region[0][0], (*InData).Filter[i].Region[0][1],
																																					(*InData).Filter[i].Region[1][0], (*InData).Filter[i].Region[1][1]);									Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... max-size color palette : %ld\n", (*InData).Filter[i].Shell_Thicknes);												Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Suorce image  : %s\n", (*InData).Filter[i].Path);																	Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickRed)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick red\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... red color min      : %lf\n", (*InData).Filter[i].Scale);															Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickGreen)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick green\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... green color min    : %lf\n", (*InData).Filter[i].Scale);															Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickBlue)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick blue\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... blue color min     : %lf\n", (*InData).Filter[i].Scale);															Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickGray)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick gray\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickWhite)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick white\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_MaskUpdate_PickBlack)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-mask via RGB color-pick black\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color distance : %lf\n", (*InData).Filter[i].rgb_distance);													Log_Stream(OUT, buffer);
		}
		
		// Image
		
		else if ((*InData).Filter[i].Tool == &ImageFilters_GrayScale)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter grayscale\n");																							Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_ColorGradient_Scale)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter color-gradiant scale\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_ColorShift_Scale)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter color-shift scale\n");																					Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color delta : %.4lf %.4lf %.4lf\n", (*InData).Filter[i].rgb_delta[0], (*InData).Filter[i].rgb_delta[1], (*InData).Filter[i].rgb_delta[2]);							Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_ColorShift)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter color-shift\n");																						Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color shift : %lf %lf %lf\n", (*InData).Filter[i].rgb[0], (*InData).Filter[i].rgb[1], (*InData).Filter[i].rgb[2]);									Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Posterize)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter posterize\n");																							Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... RGB color chanels : %ld\n", (long)(*InData).Filter[i].rgb_distance);												Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_BorderEnhance_3)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter border enhance (range 3)\n");																			Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_BorderEnhance_5)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter border enhance (range 5)\n");																			Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Identity)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter identity\n");																							Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Sharpen)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter sharpen\n");																								Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Blure_Box_3)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter blure box (range 3)\n");																					Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Blure_Box_5)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter blure box (range 5)\n");																					Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Blure_Gauss_3)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter blure Gauss (range 3)\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Blure_Gauss_5)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter blure Gauss (range 5)\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Edge_3low)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter edge-detection soft\n");																					Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Edge_3mid)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter edge-detection midium\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
		else if ((*InData).Filter[i].Tool == &ImageFilters_Edge_3hig)
		{
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > image-filter edge-detection strong\n");																				Log_Stream(OUT, buffer);
			memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   ... Filter strength : %lf\n", (*InData).Filter[i].Scale);																Log_Stream(OUT, buffer);
		}
	}
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Shape RGB Mask color     : <%d,%ld,%ld>\n", (*InData).Shape_RGB_Mask_Color[0], (*InData).Shape_RGB_Mask_Color[1], (*InData).Shape_RGB_Mask_Color[2]);																Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Camera focal distance    : %lf\n", (*InData).Camera_FocalDistance);																																				Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image Rotation-Axes autodetection: %s\n", File_IO_BoolToString((*InData).Image_RotationAxesDetection));																											Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image Rotation-Axes assumed error: %lf\n", (*InData).Image_RotationAxesError);																																	Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Shape-Mesh file type     : %s\n", (*InData).STL_type);																																							Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Shape-Mesh approximation : %.2lf%%\n", (*InData).Mesh_Approximation_Threshold);																																	Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence ignore: %ld snaphots ranges\n", (*InData).Skip_Count/2);																																			Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Skip_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		if ((*InData).Skip_Image[i][0] != (*InData).Skip_Image[i][1]) { sprintf_s(&buffer[0], _Nch600, "   _ (%ld : %ld)\n", (*InData).Skip_Image[i][0], (*InData).Skip_Image[i][1]);																													Log_Stream(OUT, buffer); }
		else														  { sprintf_s(&buffer[0], _Nch600, "   _ %ld\n", (*InData).Skip_Image[i][0]);																																						Log_Stream(OUT, buffer); }
	}
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence polygon Up-side ignore: %ld snaphots ranges\n", (*InData).Skip_Side_Up_Count/2);																													Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Skip_Side_Up_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		if ((*InData).Skip_Side_Up[i][0] != (*InData).Skip_Side_Up[i][1]) { sprintf_s(&buffer[0], _Nch600, "   _ (%ld : %ld)\n", (*InData).Skip_Side_Up[i][0], (*InData).Skip_Side_Up[i][1]);																											Log_Stream(OUT, buffer); }
		else															  { sprintf_s(&buffer[0], _Nch600, "   _ %ld\n", (*InData).Skip_Side_Up[i][0]);																																					Log_Stream(OUT, buffer); }
	}
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence polygon Down-side ignore: %ld snaphots ranges\n", (*InData).Skip_Side_Do_Count/2);																													Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Skip_Side_Do_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		if ((*InData).Skip_Side_Do[i][0] != (*InData).Skip_Side_Do[i][1]) { sprintf_s(&buffer[0], _Nch600, "   _ (%ld : %ld)\n", (*InData).Skip_Side_Do[i][0], (*InData).Skip_Side_Do[i][1]);																											Log_Stream(OUT, buffer); }
		else															  { sprintf_s(&buffer[0], _Nch600, "   _ %ld\n", (*InData).Skip_Side_Do[i][0]);																																					Log_Stream(OUT, buffer); }
	}
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence polygon Left-side ignore: %ld snaphots ranges\n", (*InData).Skip_Side_Lt_Count/2);																													Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Skip_Side_Lt_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		if ((*InData).Skip_Side_Lt[i][0] != (*InData).Skip_Side_Lt[i][1]) { sprintf_s(&buffer[0], _Nch600, "   _ (%ld : %ld)\n", (*InData).Skip_Side_Lt[i][0], (*InData).Skip_Side_Lt[i][1]);																											Log_Stream(OUT, buffer); }
		else															  { sprintf_s(&buffer[0], _Nch600, "   _ %ld\n", (*InData).Skip_Side_Lt[i][0]);																																					Log_Stream(OUT, buffer); }
	}
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Image-Sequence polygon Right-side ignore: %ld snaphots ranges\n", (*InData).Skip_Side_Rt_Count/2);																												Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Skip_Side_Rt_Count; i++)
	{
		memset(&buffer[0], 0, _Nch600 * sizeof(char));
		if ((*InData).Skip_Side_Rt[i][0] != (*InData).Skip_Side_Rt[i][1]) { sprintf_s(&buffer[0], _Nch600, "   _ (%ld : %ld)\n", (*InData).Skip_Side_Rt[i][0], (*InData).Skip_Side_Rt[i][1]);																											Log_Stream(OUT, buffer); }
		else															  { sprintf_s(&buffer[0], _Nch600, "   _ %ld\n", (*InData).Skip_Side_Rt[i][0]);																																					Log_Stream(OUT, buffer); }
	}
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	}
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Absorption algorithm: %s\n", (*InData).Ab_Algorithm_Code);																																						Log_Stream(OUT, buffer);
	if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 1))
	{
		Log_Stream(OUT, "\n");
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Analytic solution _ Resolution: %lf\n", (*InData).Ab_Resolution);																																				Log_Stream(OUT, buffer);
	}
	if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 2))
	{
		Log_Stream(OUT, "\n");
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Discrete solution _ Lattice Step Number: %ld\n", (*InData).Ab_Lattice_StepNumber);																															Log_Stream(OUT, buffer);
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Discrete solution _ Lattice Step Size  : %lf\n", (*InData).Ab_Lattice_StepSize);																																Log_Stream(OUT, buffer);
	}
	if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 3) || ((*InData).Ab_Algorithm == 4))
	{
		Log_Stream(OUT, "\n");
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Monte Carlo solution _ Max # Iterations: %ld\n", (*InData).Ab_Iterations);																																	Log_Stream(OUT, buffer);
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Monte Carlo solution _ Convergence     : %lf\n", (*InData).Ab_Convergence);																																	Log_Stream(OUT, buffer);
	}
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Instrument vs. Sample Reference Axes : x,y,z <--> %s,%s,%s\n",(*InData).SampleReferenceAxes[0], (*InData).SampleReferenceAxes[1], (*InData).SampleReferenceAxes[2]);												Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Instrument vs. Sample Orientation    : %lf %lf %lf\n", (*InData).SampleOrientation[0] * _CONST_RadToDeg, (*InData).SampleOrientation[1] * _CONST_RadToDeg, (*InData).SampleOrientation[2] * _CONST_RadToDeg);		Log_Stream(OUT, buffer);
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Reflections'list file type : %s\n", (*InData).ReflectionList_FileType_Code);																																		Log_Stream(OUT, buffer);
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Reflections'list file paths :\n");																																												Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).ReflectionList_Count; i++) { memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   _ %s\n", (*InData).ReflectionList_Path[i]);																												Log_Stream(OUT, buffer); }
	Log_Stream(OUT, "\n");
	memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Unit-cell elemental composition : %ld elements\n", (*InData).Elements_Count);																																		Log_Stream(OUT, buffer);
	for (i = 0; i < (*InData).Elements_Count; i++) { memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, "   _ %s\t... %lf\n", (*InData).Element[i].Name, (*InData).Element[i].Occupancy);																					Log_Stream(OUT, buffer); }
	Log_Stream(OUT, "\n");
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, _OUTPUT_HEAD_0);										  //" *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** "//
	Log_Stream(OUT, "\n");
	
	fclose(OUT);
	
	return 0;
}