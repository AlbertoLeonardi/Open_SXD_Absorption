// Utility Functions :: Shape 3D Scan
//
// Mash STL is reconstruct from a 360 deg image sequence 

#ifndef _INCLUDE_UTILITY_SHAPE_3D_SCAN
#define _INCLUDE_UTILITY_SHAPE_3D_SCAN 1

#include <App_Environment.h>

#include <External_bitmap_image.hpp> // this library uses a standard _WARNING_SIGNAL() function

#include <Signal_Warning.h>

#include <Utility_File_IO.h>
#include <Utility_ImageFilters.h>
#include <Utility_Mesh.h>

#define _SEQUENCE_BMP "/Step_Sequence"
#define _SEQUENCE_MSK "/Mask_Sequence"
#define _SEQUENCE_POL "/Poly_Sequence"
#define _SEQUENCE_USE "/Mesh_Sequence"

int Shape3DScan_Get_FrameSequence(bool Use_Preprocessed_Sequence, char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[], char Token[], long Digits_Count, long Step_Id_0, long Step_Id_1, long Frame_Id_0, long Frame_Id_1, double RevolutionAngleError);

int Shape3DScan_Get_MaskSequence(char Path[], char Name[], long_3 RGB_Mask_color, double Mask_Transparency, bool MaskFilterOverlap, long Filter_Count, struct ImageFilters_Setting* FilterTool, long Threads_Count);
int Shape3DScan_Get_PolygonSequence(char Path[], char Name[], long Skip_Side_Up_Count, long_2* Skip_Side_Up, long Skip_Side_Do_Count, long_2* Skip_Side_Do, long Skip_Side_Lt_Count, long_2* Skip_Side_Lt, long Skip_Side_Rt_Count, long_2* Skip_Side_Rt, long_3 RGB_Mask_color, long Threads_Count);

int Shape3DScan_Recover_PolygonSequence(char Path_In[], char Path_Ou[], char Name_In[], char Name_Ou[]);

int Shape3DScan_Get_RotationAxes(char Path[], char Name[], long_3 Mask_color, long Skip_Count, long_2* Skip_Id, bool CenterShift_Detection, double CenterShift_Error, double* CenterShift);

struct Mesh* Shape3DScan_Get_MeshFacets(char Path[], char Name[], long Skip_Count, long_2* Skip_Id, double Focal_Distance, double Rotation_Shift); // Focal_Distance <= 0.0 equal infinit

struct Mesh* Shape3DScan_Get_FakeMesh(double Size);

int Shape3DScan_MeshProjection(struct Mesh* Shape, char Path[], char Name[], double Pixel_mm, double Focal_Distance, double Rotation_Shift, long_3 RGB_Mask_color);

#endif