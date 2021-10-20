// Utility Functions :: Reflection_IO
//
// ...

#ifndef _INCLUDE_UTILITY_REFLECTION_IO
#define _INCLUDE_UTILITY_REFLECTION_IO 1

#include <App_Environment.h>

#include <Signal_Warning.h>

#include <Utility_File_IO.h>
#include <Utility_CrystalAbsorption.h>

struct Reflection_List_IDL_info
{
	long Reflection_Count;
	long h_cd;
	long k_cd;
	long l_cd;
	long Fsqr_cd;
	long sFsqr_cd;
	long Lambda_cd;
	long Tbar_cd;
	long latitude_cd;
	long longitude_cd;

	double_3 UB[3];
	double_3 ABC[3];

	long_2 *Ab_id;
};

void Reflection_IDL_info_Free(struct Reflection_List_IDL_info* IDL_info);

void Reflection_IO_Free(struct Reflection_List_DataSet* Reflection_List);
int  Reflection_IO_Stream(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], long Id);

int  Reflection_IO_Dump_TXT(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], char Name_Extension[], long Id);
int  Reflection_IO_Dump_IDL(struct Reflection_List_DataSet* Reflection_List, char Working_Path[], char Working_Name[], char Name_Extension[], long Id, char IDL_Path[], struct Reflection_List_IDL_info* IDL_info);

struct Reflection_List_DataSet* Reflection_IO_Read_ReflectionList_TXT(char Path[]);
struct Reflection_List_DataSet* Reflection_IO_Read_ReflectionList_IDL(char Path[], struct Reflection_List_IDL_info** IDL_info);
struct Reflection_List_DataSet* Reflection_Get_EmptyReflectionSet();

#endif