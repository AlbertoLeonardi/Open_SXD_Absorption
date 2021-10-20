// Utility Functions :: Test
//
// ...

#ifndef _INCLUDE_UTILITY_LINEAR_ABSORPTION
#define _INCLUDE_UTILITY_LINEAR_ABSORPTION 1

#include <App_Environment.h>

#include <Signal_Warning.h>
#include <Signal_Log.h>

#include <Utility_File_IO.h>
#include <Utility_CrystalAbsorption.h>

struct LinearAbsorption_Element
{
	char Name[_Nch20];
	double Occupancy;
	double s_abs[2];
	double s_inc[4];
};

int    LinearAbsorption_GetElementCoefficients(long Elements_Count, struct LinearAbsorption_Element* Element, char DataBase_Path[]);
double LinearAbsorption_GetMu(long Elements_Count, struct LinearAbsorption_Element* Element, double CellVolume_A3, double Lambda);		// return Mu (1/cm)

int    LinearAbsorption_Reflections_Mu(struct Reflection_List_DataSet* Reflection_List, long Elements_Count, struct LinearAbsorption_Element* Element, double LenghtScale);

#endif