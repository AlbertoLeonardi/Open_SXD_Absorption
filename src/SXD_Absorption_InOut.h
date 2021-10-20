// Application System
//
// Collect imput parameters from external text file

#ifndef _INCLUDE_APP_INPUT
#define _INCLUDE_APP_INPUT 1

#include <App_Environment.h>
#include <App_Log.h>

#include <External_bitmap_image.hpp> // this library uses a standard _WARNING_SIGNAL() function

#include <Signal_Warning.h>
#include <Signal_Log.h>

#include <Utility_File_IO.h>
#include <Utility_ImageFilters.h>
#include <Utility_Reflection_IO.h>
#include <Utility_LinearAbsorption.h>

#include <Utility_CUDA_GPU.cuh>

struct SXD_Absorption_Data
{
	long   User_Seed[2];									// Seeds for random generator
	long   Threads_Count;
	//
	// ||||||||||  GENERAL SETTINGS  ||||||||||
	//
	char   App_Input_BaseFolder[_Nch300];					// Application exectuting directory path
	char   App_Input_SubFolder[_Nch100];					// Application set-up directory path
	char   App_Input_Name[_Nch100];							// Application set-up file name
	char   App_LinearAbsorption[_Nch100];					// Application linear absorption table
	//
	char   Working_Path[_Nch300];							// Output (working) directory path
	char   Working_Name[_Nch100];							// Output files' root-name
	//
	bool   ShapeFromFile;									// use existing STL file instead of image processing
	bool   DumpShape;										// skip post shape dump and mesh match
	bool   Break_StepSequence;								// exit after step frame-sequence
	bool   Break_PolygonSequence;							// exit after polygon frame-sequence
	//
#ifdef _NVIDIA_CUDA_SUPPORT
	struct GPU_Device GPU;									// GPU device Name
#endif
	//
	// |||||||||| 360 FRAME SEQUENCE ||||||||||
	//
	bool   Use_Preprocessed_ImageSequence;
	//
	char   ImageSequence_Path[_Nch300];
	char   ImageSequence_Name[_Nch100];
	//
	char   ImageSequence_Token[_Nch10];
	long   ImageSequence_Digits_Count;
	long   ImageSequence_Step_Id[2];
	long   ImageSequence_Frame_Id[2];
	double ImageSequence_RevolutionAngleError;
	//
	// ||||||||||  IMAGE  FILTERING  ||||||||||
	//
	bool   Use_Prefiltered_ImageSequence;
	//
	char   PreFilteredData_Path[_Nch300];
	char   PreFilteredData_Name[_Nch100];
	//
	long   Filter_Count;
	struct ImageFilters_Setting *Filter;
	//
	long   RGB_Mask_Color[3];
	double Mask_Transparency;
	bool   MaskFilterOverlap;
	//
	// |||||||||| 3D SHAPE DETECTION ||||||||||
	//
	long   Shape_RGB_Mask_Color[3];
	long   Pixel_mm;
	double PixelSize;
	double Camera_FocalDistance;
	bool   Image_RotationAxesDetection;
	double Image_RotationAxesError;
	double Image_RotationAxesShift;
	double Mesh_Approximation_Threshold;
	//
	char   STL_type[_Nch10];
	long   Skip_Count;
	long_2*  Skip_Image;									// pair of ids --> range

	long   Skip_Side_Up_Count;
	long   Skip_Side_Do_Count;
	long   Skip_Side_Lt_Count;
	long   Skip_Side_Rt_Count;
	long_2*  Skip_Side_Up;									// pair of ids --> range
	long_2*  Skip_Side_Do;									// pair of ids --> range
	long_2*  Skip_Side_Lt;									// pair of ids --> range
	long_2*  Skip_Side_Rt;									// pair of ids --> range
	//
	// |||||||||| ABSORPTION FACTORs ||||||||||
	//
	char   Ab_Algorithm_Code[_Nch20];
	long   Ab_Algorithm;									// -1. None, 0. Any, 1. Analytic, 2. Discrete, 3. MonteCarlo, 4. Beam_MC
	double Ab_Resolution;
	long   Ab_Iterations;
	double Ab_Convergence;
	long   Ab_Lattice_StepNumber;
	double Ab_Lattice_StepSize;
	//
	char   SampleReferenceAxes[3][_Nch10];
	double SampleOrientation[3];							// Assumed to be applied after Axes match
	//
	char   ReflectionList_FileType_Code[_Nch20];
	int    ReflectionList_FileType;							// -1. none, 0. txt, 1. idl
	long   ReflectionList_Count;
	char  **ReflectionList_Path;							// _Nch500
	struct Reflection_List_DataSet* Reflection_List;
	struct Reflection_List_IDL_info* IDL_info;
	//
	long Elements_Count;
	struct LinearAbsorption_Element* Element;
	//
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Input

struct SXD_Absorption_Data* SXD_Absorption_Input_Get_RunSettings_DataStructure();
void SXD_Absorption_Input_RunSettings_Reset(struct SXD_Absorption_Data *InData);
void SXD_Absorption_Input_RunSettings_Free(struct SXD_Absorption_Data *InData);

int SXD_Absorption_Input_RunSettings_LineCommand_Preset(struct SXD_Absorption_Data *InData, int argc, char **argv);

int SXD_Absorption_Input_RunSettings_LineCommand(struct SXD_Absorption_Data *InData, int argc, char **argv);

void SXD_Absorption_Input_RunSettings_Data_Location(struct SXD_Absorption_Data *InData);

int  SXD_Absorption_Input_RunSettings_Data_TestUpdate(struct SXD_Absorption_Data *InData);
int  SXD_Absorption_Input_RunSettings_Data_Read(struct SXD_Absorption_Data *InData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Output

int SXD_Absorption_Output_RunSettings(struct SXD_Absorption_Data* InData);

#endif
