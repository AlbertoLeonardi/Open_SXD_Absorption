// Utility Functions :: Crystal Absorption
//
// ...

#ifndef _INCLUDE_UTILITY_CRYSTAL_ABSORPTION
#define _INCLUDE_UTILITY_CRYSTAL_ABSORPTION 1

#include <App_Environment.h>

#include <Signal_Warning.h>

#include <Utility_Math.h>
#include <Utility_Mesh.h>

struct Face_Index
{
	long_3 hkl;
	double cosine;
};

struct Absorption_DataSet
{
	double_2 Intensity;
	double Mu;
	double Transmission;
};
struct Reflection_DataSet
{
	double_3 Beam_In;
	double_3 Beam_Ou;
	//
	long_3   hkl;
	double_3 PlaneNorm;
	//
	long Absorption_Count;
	struct Absorption_DataSet* Absorption;
};
struct Reflection_List_DataSet
{
	double SampleRotation[3];								// Phi, Chi, Omega
	double SampleRotation_Matrix[9];
	//
	double UnitCell[6];
	//
	long Reflection_Count;
	struct Reflection_DataSet* Reflection;
};

// Shape Mesh Indexing

void               CrystalAbsorption_Free_FaceIndex(struct Face_Index* FaceIndex);
struct Face_Index* CrystalAbsorption_GetMashFaceIndex(struct Face_Index* FaceIndex_In, struct Mesh* Shape, struct Reflection_List_DataSet* Reflection_List);
int                CrystalAbsorption_WriteMeshFaceIndex(struct Face_Index* FaceIndex, struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char UnitOfMeasure[]);

// Single Solution

int CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Max_Iteration_Count, double Convergence);

int CrystalAbsorption_GetTransmission_MonteCarloIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Max_Iteration_Count, double Convergence);
int CrystalAbsorption_GetTransmission_DiscreteIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Step_Count);
int CrystalAbsorption_GetTransmission_DiscreteIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, double Step_Size);
int CrystalAbsorption_GetTransmission_LatticeIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, double_3* Lattice, long Lattice_Count);

int  CrystalAbsorption_GetTransmission_AnalyticIntegration(double* Transmission, struct Mesh* Shape, double Range, double Mu, double* Beam_In, double* Beam_Ou);

// Multiple Solution

int   CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_DataSet* Reflection);
int   CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_List_DataSet* Reflection_List, long Threads_Count);

int   CrystalAbsorption_GetTransmission_MonteCarloIntegration(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_DataSet* Reflection);
int   CrystalAbsorption_GetTransmission_MonteCarloIntegration(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_List_DataSet* Reflection_List, long Threads_Count);

int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(struct Mesh* Shape, double Step_Size, struct Reflection_DataSet* Reflection);
int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(struct Mesh* Shape, double Step_Size, struct Reflection_List_DataSet* Reflection_List, long Threads_Count);

int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(struct Mesh* Shape, long Step_Count, struct Reflection_DataSet* Reflection);
int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(struct Mesh* Shape, long Step_Count, struct Reflection_List_DataSet* Reflection_List, long Threads_Count);

int  CrystalAbsorption_GetTransmission_AnalyticIntegration(struct Mesh* Shape, double Range, struct Reflection_DataSet* Reflection);
int  CrystalAbsorption_GetTransmission_AnalyticIntegration(struct Mesh* Shape, double Range, struct Reflection_List_DataSet* Reflection_List, long Threads_Count);

#endif