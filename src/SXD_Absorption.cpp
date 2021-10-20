/// SXD-Absorption Utility Tool
/// Created on: March 03 2021
/// Copyright : Dr. Alberto Leonardi (alberto.leonardi@stfc.ac.uk :: alberto.leonardi@fau.de :: leonaral@iu.edu :: alby_leo@yahoo.it)

#include "SXD_Absorption.h"

int main(int argc, char **argv)
{
	bool Bvoid;
	
	long ReflectionSet_Id;
	
	struct SXD_Absorption_Data *InData;
	struct Mesh* Shape;
	struct Face_Index* FaceIndex;
	
	char buffer_10[_Nch10];
	char buffer[_Nch600];
	
	time_t RunTime[2];
	
	double RotationMatrix[9];
	
	App_LogFile_open_default();
	
	InData = SXD_Absorption_Input_Get_RunSettings_DataStructure();
	if(InData==NULL)																																																												{ _USER_IO_WAIT(); return 1; }
	
	if (SXD_Absorption_Input_RunSettings_LineCommand_Preset(InData, argc, argv))																																													{ _USER_IO_WAIT(); return 0; }
	
	SXD_Absorption_Input_RunSettings_Data_Location(InData);
	
	if (SXD_Absorption_Input_RunSettings_Data_Read(InData))																																																			{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	
	if(SXD_Absorption_Input_RunSettings_LineCommand(InData, argc, argv))																																															{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	
	if (App_LogFile_move(InData->Working_Path, InData->Working_Name))																																																{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	if (SXD_Absorption_Input_RunSettings_Data_TestUpdate(InData))																																																	{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	if (SXD_Absorption_Output_RunSettings(InData))																																																					{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	
	memset(&buffer[0], 0, _Nch600 * sizeof(char));
	sprintf_s(&buffer[0], _Nch600, "%s/%s/%s", (*InData).App_Input_BaseFolder, (*InData).App_Input_SubFolder, (*InData).App_LinearAbsorption);
	
	if (LinearAbsorption_GetElementCoefficients((*InData).Elements_Count, (*InData).Element, buffer))																																								{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	
#ifdef _NVIDIA_CUDA_SUPPORT
	CUDA_GPU_GetDevice(&(*InData).GPU);
#endif
	// Application Routines // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
	
	// **** Shape Reconstruction

	if (!(*InData).ShapeFromFile)
	{
		/**/
		if(Shape3DScan_Get_FrameSequence(	(*InData).Use_Preprocessed_ImageSequence,
											(*InData).ImageSequence_Path, (*InData).Working_Path,
											(*InData).ImageSequence_Name, (*InData).Working_Name,
											(*InData).ImageSequence_Token, (*InData).ImageSequence_Digits_Count,
											(*InData).ImageSequence_Step_Id[0], (*InData).ImageSequence_Step_Id[1],
											(*InData).ImageSequence_Frame_Id[0], (*InData).ImageSequence_Frame_Id[1],
											(*InData).ImageSequence_RevolutionAngleError) )																																											{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if ((*InData).Break_StepSequence)																																																							{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 0; }
		
		if ((*InData).Use_Prefiltered_ImageSequence)
		{
			if(Shape3DScan_Recover_PolygonSequence((*InData).PreFilteredData_Path, (*InData).Working_Path, (*InData).PreFilteredData_Name, (*InData).Working_Name))																									{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		}
		else
		{
			if(Shape3DScan_Get_MaskSequence((*InData).Working_Path, (*InData).Working_Name, (*InData).RGB_Mask_Color, (*InData).Mask_Transparency * 0.010, (*InData).MaskFilterOverlap, (*InData).Filter_Count, (*InData).Filter, (*InData).Threads_Count))			{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
			if(Shape3DScan_Get_PolygonSequence( (*InData).Working_Path, (*InData).Working_Name,
												(*InData).Skip_Side_Up_Count, (*InData).Skip_Side_Up,
												(*InData).Skip_Side_Do_Count, (*InData).Skip_Side_Do,
												(*InData).Skip_Side_Lt_Count, (*InData).Skip_Side_Lt,
												(*InData).Skip_Side_Rt_Count, (*InData).Skip_Side_Rt,
												(*InData).RGB_Mask_Color, (*InData).Threads_Count))																																									{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		}
		
		if(Shape3DScan_Get_RotationAxes((*InData).Working_Path, (*InData).Working_Name, (*InData).RGB_Mask_Color, (*InData).Skip_Count, (*InData).Skip_Image, (*InData).Image_RotationAxesDetection, (*InData).Image_RotationAxesError, &(*InData).Image_RotationAxesShift))			{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Rotation axes shift from camera focus: %ld pixels\n", (long)(*InData).Image_RotationAxesShift);				Log_Stream(NULL, buffer);
		
		if ((*InData).Break_PolygonSequence) { SXD_Absorption_Input_RunSettings_Free(InData); _USER_IO_WAIT(); return 0; }

		Shape = Shape3DScan_Get_MeshFacets((*InData).Working_Path, (*InData).Working_Name, (*InData).Skip_Count, (*InData).Skip_Image, (*InData).Camera_FocalDistance * (double)(*InData).Pixel_mm, (*InData).Image_RotationAxesShift);
		if(Shape==NULL)																																																												{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		/*/
		Shape = Shape3DScan_Get_FakeMesh(100.0);
		/**/
		if (Mesh_ResolveFromFacets(Shape, (*InData).Threads_Count))																																																	{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "ASCII");  if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { if (Mesh_WriteSTL_ASCII(Shape, (*InData).Working_Path, (*InData).Working_Name, "FullResolution"))								{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; } } else {
		memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "BINARY"); if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { if (Mesh_WriteSTL_BINARY(Shape, (*InData).Working_Path, (*InData).Working_Name, "FullResolution", (*InData).Working_Name))	{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; } } else {
		memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "NONE");   if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { } else																														{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }}}
		
		if (Mesh_WriteMesh_TXT(Shape, (*InData).Working_Path, (*InData).Working_Name, "FullResolution", "mm"))																																						{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if (Mesh_Simplify(&Shape, (*InData).Mesh_Approximation_Threshold * 0.010, (*InData).Threads_Count))																																							{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if (Mesh_Size_Scale(Shape, (*InData).PixelSize))																																																			{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	}
	else
	{
		Shape = Mesh_ReadSTL((*InData).Working_Path, (*InData).Working_Name);
		if(Shape==NULL)																																																												{ _USER_IO_WAIT(); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if (Mesh_ResolveFromFacets(Shape, (*InData).Threads_Count))																																																	{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		if (Mesh_Simplify(&Shape, (*InData).Mesh_Approximation_Threshold * 0.010, (*InData).Threads_Count))																																							{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if (Mesh_Size_Scale(Shape, (*InData).PixelSize))																																																			{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	}
	
	if (!(*InData).DumpShape)
	{
		User_IO_AskValue("Do you want to save the optimized shape (y/n)? ", false); User_IO_GetValue(&Bvoid, 'b', 0);
		if (Bvoid)
		{
			memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "ASCII");  if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { if (Mesh_WriteSTL_ASCII(Shape, (*InData).Working_Path, (*InData).Working_Name, ""))							{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; } } else {
			memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "BINARY"); if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { if (Mesh_WriteSTL_BINARY(Shape, (*InData).Working_Path, (*InData).Working_Name, (*InData).Working_Name, ""))	{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; } } else {
			memset(&buffer_10[0], 0, _Nch10 * sizeof(char)); strcpy_s(&buffer_10[0], _Nch10, "NONE");   if (!strcmp(&buffer_10[0], &(*InData).STL_type[0])) { } else																										{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }}}
		}
		
		if (Mesh_WriteMesh_TXT(Shape, (*InData).Working_Path, (*InData).Working_Name, "", "mm"))																																										{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if(Shape3DScan_MeshProjection(Shape, (*InData).Working_Path, (*InData).Working_Name, (double)(*InData).Pixel_mm, (*InData).Camera_FocalDistance * (double)(*InData).Pixel_mm, (*InData).Image_RotationAxesShift, (*InData).Shape_RGB_Mask_Color))				{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	}
	
	FaceIndex = NULL;
	
	if (((*InData).Ab_Algorithm >= 0) && ((*InData).Ab_Algorithm < 5))
	{
		// **** Instrument vs. Sample Orientation & Local Rotation Shape Update
		
		if (Mesh_Axes_Swap_and_Flip(Shape, (*InData).SampleReferenceAxes[0], (*InData).SampleReferenceAxes[1], (*InData).SampleReferenceAxes[2]))																													{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		
		if ((fabs((*InData).SampleOrientation[0]) > FLT_EPSILON) || (fabs((*InData).SampleOrientation[1]) > FLT_EPSILON) || (fabs((*InData).SampleOrientation[2]) > FLT_EPSILON))
		{
			RotationMatrix_3D_Euler_ZXZ(-(*InData).SampleOrientation[0], -(*InData).SampleOrientation[1], -(*InData).SampleOrientation[2], &RotationMatrix[0]);
			if (Mesh_Rotation(Shape, &RotationMatrix[0]))																																																			{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
		}
		
		for (ReflectionSet_Id = 0; ReflectionSet_Id < (*InData).ReflectionList_Count; ReflectionSet_Id++)
		{
			switch ((*InData).ReflectionList_FileType)
			{
			case 0:
				(*InData).Reflection_List = Reflection_IO_Read_ReflectionList_TXT((*InData).ReflectionList_Path[ReflectionSet_Id]);
				if ((*InData).Reflection_List == NULL)																																																				{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
				break;
			case 1:
				(*InData).Reflection_List = Reflection_IO_Read_ReflectionList_IDL((*InData).ReflectionList_Path[ReflectionSet_Id], &(*InData).IDL_info);
				if ((*InData).Reflection_List == NULL)																																																				{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
				
				FaceIndex = CrystalAbsorption_GetMashFaceIndex(FaceIndex, Shape, (*InData).Reflection_List);
				if(FaceIndex==NULL)																																																									{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
				
				break;
			default:
				(*InData).Reflection_List = Reflection_Get_EmptyReflectionSet();
				break;
			}
			
			if (Reflection_IO_Stream((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, ReflectionSet_Id))																																	{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
			
			if ((fabs((*InData).Reflection_List->SampleRotation[0]) > FLT_EPSILON) || (fabs((*InData).Reflection_List->SampleRotation[1]) > FLT_EPSILON) || (fabs((*InData).Reflection_List->SampleRotation[2]) > FLT_EPSILON))
			{
				if (Mesh_Rotation(Shape, &(*InData).Reflection_List->SampleRotation_Matrix[0]))																																										{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
			}
			
			if(LinearAbsorption_Reflections_Mu((*InData).Reflection_List, (*InData).Elements_Count, (*InData).Element, 0.10))																																		{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
			
			// **** Absorption Correction Calculation
			
			if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 1))
			{
				time(&RunTime[0]);
			
				CrystalAbsorption_GetTransmission_AnalyticIntegration(Shape, (*InData).Ab_Resolution, (*InData).Reflection_List, (*InData).Threads_Count);
			
				time(&RunTime[1]);
				memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Analytic Integration Computing time: .................. %lds\n", (long)(RunTime[1] - RunTime[0]));			Log_Stream(NULL, buffer);
				switch ((*InData).ReflectionList_FileType)
				{
				case 0: Reflection_IO_Dump_TXT((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "Analytic", ReflectionSet_Id); break;
				case 1: Reflection_IO_Dump_IDL((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "Analytic", ReflectionSet_Id, (*InData).ReflectionList_Path[ReflectionSet_Id], (*InData).IDL_info); break;
				}
				//CrystalAbsorption_GetTransmission_AnalyticIntegration(&Transmission, Shape, (*InData).Ab_Resolution, Mu, &Beam_In[0], &Beam_Ou[0]);												printf("_ %lf (AnalyticIntegration)\n", Transmission);
			}
			
			if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 2))
			{
				if ((*InData).Ab_Lattice_StepNumber > 0)
				{
					time(&RunTime[0]);
					//
					CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(Shape, (*InData).Ab_Lattice_StepNumber, (*InData).Reflection_List, (*InData).Threads_Count);
					//
					time(&RunTime[1]);
					memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Discrete Integration (step number) Computing time: .... %lds\n", (long)(RunTime[1] - RunTime[0]));		Log_Stream(NULL, buffer);
					switch ((*InData).ReflectionList_FileType)
					{
					case 0: Reflection_IO_Dump_TXT((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "DiscreteStepCount", ReflectionSet_Id); break;
					case 1: Reflection_IO_Dump_IDL((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "DiscreteStepCount", ReflectionSet_Id, (*InData).ReflectionList_Path[ReflectionSet_Id], (*InData).IDL_info); break;
					}
					//CrystalAbsorption_GetTransmission_DiscreteIntegration(&Transmission, Shape, Mu, &Beam_In[0], &Beam_Ou[0], (*InData).Ab_Lattice_StepNumber);									printf("_ %lf (DiscreteIntegration / step number)\n", Transmission);
				}
				if ((*InData).Ab_Lattice_StepSize > FLT_EPSILON)
				{
					time(&RunTime[0]);
					//
					CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(Shape, (*InData).Ab_Lattice_StepSize, (*InData).Reflection_List, (*InData).Threads_Count);
					//
					time(&RunTime[1]);
					memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Discrete Integration (step size) Computing time: ...... %lds\n", (long)(RunTime[1] - RunTime[0]));		Log_Stream(NULL, buffer);
					switch ((*InData).ReflectionList_FileType)
					{
					case 0: Reflection_IO_Dump_TXT((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "DiscreteStepSize", ReflectionSet_Id); break;
					case 1: Reflection_IO_Dump_IDL((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "DiscreteStepSize", ReflectionSet_Id, (*InData).ReflectionList_Path[ReflectionSet_Id], (*InData).IDL_info); break;
					}
					//CrystalAbsorption_GetTransmission_DiscreteIntegration(&Transmission, Shape, Mu, &Beam_In[0], &Beam_Ou[0], (*InData).Ab_Lattice_StepSize);									printf("_ %lf (DiscreteIntegration / step size)\n", Transmission);
				}
			}
			
			if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 3))
			{
				time(&RunTime[0]);
				//
				CrystalAbsorption_GetTransmission_MonteCarloIntegration(Shape, (*InData).Ab_Iterations, (*InData).Ab_Convergence, (*InData).Reflection_List, (*InData).Threads_Count);
				//
				time(&RunTime[1]);
				memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Monte-Carlo Integration Computing time: ............... %lds\n", (long)(RunTime[1] - RunTime[0]));			Log_Stream(NULL, buffer);
				switch ((*InData).ReflectionList_FileType)
				{
				case 0: Reflection_IO_Dump_TXT((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "MonteCarlo", ReflectionSet_Id); break;
				case 1: Reflection_IO_Dump_IDL((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "MonteCarlo", ReflectionSet_Id, (*InData).ReflectionList_Path[ReflectionSet_Id], (*InData).IDL_info); break;
				}
				//CrystalAbsorption_GetTransmission_MonteCarloIntegration(&Transmission, Shape, Mu, &Beam_In[0], &Beam_Ou[0], (*InData).Ab_Iterations, (*InData).Ab_Convergence);					printf("_ %lf (MonteCarloIntegration)\n", Transmission);
			}
			
			if (((*InData).Ab_Algorithm == 0) || ((*InData).Ab_Algorithm == 4))
			{
				time(&RunTime[0]);
				//
				CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(Shape, (*InData).Ab_Iterations, (*InData).Ab_Convergence, (*InData).Reflection_List, (*InData).Threads_Count);
				//
				time(&RunTime[1]);
				memset(&buffer[0], 0, _Nch600 * sizeof(char)); sprintf_s(&buffer[0], _Nch600, " > Monte-Carlo In-comming Beam Integration Computing time: %lds\n", (long)(RunTime[1] - RunTime[0]));			Log_Stream(NULL, buffer);
				switch ((*InData).ReflectionList_FileType)
				{
				case 0: Reflection_IO_Dump_TXT((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "MC_Beam", ReflectionSet_Id); break;
				case 1: Reflection_IO_Dump_IDL((*InData).Reflection_List, (*InData).Working_Path, (*InData).Working_Name, "MC_Beam", ReflectionSet_Id, (*InData).ReflectionList_Path[ReflectionSet_Id], (*InData).IDL_info); break;
				}
				//CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(&Transmission, Shape, Mu, &Beam_In[0], &Beam_Ou[0], (*InData).Ab_Iterations, (*InData).Ab_Convergence);	printf("_ %lf (MonteCarloIntegration_BeamSampling)\n", Transmission);
			}
		}
		
		if ((*InData).ReflectionList_FileType == 1) if(CrystalAbsorption_WriteMeshFaceIndex(FaceIndex, Shape, (*InData).Working_Path, (*InData).Working_Name, "", "mm"))																							{ _USER_IO_WAIT(); CrystalAbsorption_Free_FaceIndex(FaceIndex); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	}
	
	// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
	
#ifdef _NVIDIA_CUDA_SUPPORT
	CUDA_GPU_FreeDevice(&(*InData).GPU);
#endif
	
	if (App_LogFile_close())																				{ _USER_IO_WAIT(); Mesh_Free_All(Shape); SXD_Absorption_Input_RunSettings_Free(InData); return 1; }
	
	Mesh_Free_All(Shape);
	SXD_Absorption_Input_RunSettings_Free(InData);
	
	_USER_IO_WAIT();
	
 	return 0;
} 
