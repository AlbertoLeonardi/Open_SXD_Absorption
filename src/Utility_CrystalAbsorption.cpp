// Utility Functions :: Crystal Absorption
//

#include "Utility_CrystalAbsorption.h"

// Shape Mesh Indexing

void               CrystalAbsorption_Free_FaceIndex(struct Face_Index* FaceIndex)
{
	if (FaceIndex != NULL) free(FaceIndex);
	
	return;
}
struct Face_Index* CrystalAbsorption_GetMashFaceIndex(struct Face_Index* FaceIndex_In, struct Mesh* Shape, struct Reflection_List_DataSet* Reflection_List)
{
    long i, j;
    
    double Cosine;
    
    struct Face_Index* FaceIndex;
    
    if (FaceIndex_In == NULL)
    {
        FaceIndex = (struct Face_Index*)calloc(Shape->Face_Count, sizeof(struct Face_Index));
        if (_APP_WARNING_MESSAGE(FaceIndex, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetMashFaceIndex", _WARNING_MESSAGE_KEY_CALLOC, "\"FaceIndex\" to", Shape->Face_Count, 0)) { _USER_IO_WAIT(); return NULL; }
    }
    else FaceIndex = FaceIndex_In;
    
    for (i = 0; i < Shape->Face_Count; i++)
    {
        for (j = 0; j < Reflection_List->Reflection_Count; j++)
        {
            Cosine = fabs( Shape->Face[i].Normal[0] * Reflection_List->Reflection[j].PlaneNorm[0] + 
                           Shape->Face[i].Normal[1] * Reflection_List->Reflection[j].PlaneNorm[1] +
                           Shape->Face[i].Normal[2] * Reflection_List->Reflection[j].PlaneNorm[2] );
            if (Cosine > FaceIndex[i].cosine)
            {
                FaceIndex[i].cosine = Cosine;
                FaceIndex[i].hkl[0] = Reflection_List->Reflection[j].hkl[0];
                FaceIndex[i].hkl[1] = Reflection_List->Reflection[j].hkl[1];
                FaceIndex[i].hkl[2] = Reflection_List->Reflection[j].hkl[2];
            }
        }
    }
    
    return FaceIndex;
}
int                CrystalAbsorption_WriteMeshFaceIndex(struct Face_Index* FaceIndex, struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char UnitOfMeasure[])
{
    long i, j;
	
	char Path_File[_Nch800];
	FILE* TXT;
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s.mesh_index%s", Path, Name, Name_Specification, _EXT_TXT);
	
	TXT = File_IO_Open(Path_File, "w");
	if (_APP_WARNING_MESSAGE(TXT, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_WriteMeshFaceIndex", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	fprintf(TXT, "# Solid-Mesh DataFile Version 0.0\n");
	fprintf(TXT, "# Solid-Mesh Name:\t%s\n", Name);
	fprintf(TXT, "# Solid-Mesh Unit-Size:\t%s\n", UnitOfMeasure);
	fprintf(TXT, "# Solid-Mesh Volume:\t%lf\n", Shape->Volume);
	fprintf(TXT, "# Solid-Mesh Surface:\t%lf\n", Shape->Surface);
	fprintf(TXT, "# Solid-Mesh Node-Count:\t%ld\n", Shape->Node_Count);
	fprintf(TXT, "# Solid-Mesh Edge-Count:\t%ld\n", Shape->Edge_Count);
	fprintf(TXT, "# Solid-Mesh Face-Count:\t%ld\n", Shape->Face_Count);
	fprintf(TXT, "ITEM: Node _ id x y z\n");
	for (i = 0; i < Shape->Node_Count; i++) fprintf(TXT, "%ld\t%lf\t%lf\t%lf\n", i + 1, Shape->Node[i].x[0], Shape->Node[i].x[1], Shape->Node[i].x[2]);
	fprintf(TXT, "ITEM: Edge _ id center_x center_y center_z direction_x direction_y direction_z length vertex_node_id[0] vertex_node_id[1]\n");
	for (i = 0; i < Shape->Edge_Count; i++) fprintf(TXT, "%ld\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%ld\t%ld\n", i + 1, Shape->Edge[i].Center[0], Shape->Edge[i].Center[1], Shape->Edge[i].Center[2], Shape->Edge[i].Direction[0], Shape->Edge[i].Direction[1], Shape->Edge[i].Direction[2], Shape->Edge[i].Length, Shape->Edge[i].Node_Id[0] + 1, Shape->Edge[i].Node_Id[1] + 1);
	fprintf(TXT, "ITEM: Face _ id index_h index_k index_l cosine normal_x normal_y normal_z surface vertex_count \"list of vertex_node_id\" edge_count \"list of edge_id\"\n");
	for (i = 0; i < Shape->Face_Count; i++)
	{
		fprintf(TXT, "%ld\t%ld\t%ld\t%ld\t%lf\t%lf\t%lf\t%lf\t%lf\t%ld", i + 1, FaceIndex[i].hkl[0], FaceIndex[i].hkl[1], FaceIndex[i].hkl[2], FaceIndex[i].cosine, Shape->Face[i].Normal[0], Shape->Face[i].Normal[1], Shape->Face[i].Normal[2], Shape->Face[i].Surface, Shape->Face[i].Node_Count);
		for (j = 0; j < Shape->Face[i].Node_Count; j++) fprintf(TXT, "\t%ld", Shape->Face[i].Node_Id[j] + 1);
		fprintf(TXT, "\t%ld", Shape->Face[i].Edge_Count);
		for (j = 0; j < Shape->Face[i].Edge_Count; j++) fprintf(TXT, "\t%ld", Shape->Face[i].Edge_Id[j] + 1);
		fprintf(TXT, "\n");
	}
	
	fclose(TXT);
	
	return 0;
}

// **** Single Solution ****

int CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Max_Iteration_Count, double Convergence)
{
    long i, j;
    
    long Count;
    
    double_3 ux, uy, uz, ls;
    double_2 xl, yl, zl;
    
    double dx, dy;
    double x_range, y_range;
    double Distance;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double Delta, Norm;
    double Transmission_2, Convergence_2;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    Sin[0] = Beam_In[0] * Norm;
    Sin[1] = Beam_In[1] * Norm;
    Sin[2] = Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    Sou[0] = Beam_Ou[0] * Norm;
    Sou[1] = Beam_Ou[1] * Norm;
    Sou[2] = Beam_Ou[2] * Norm;
    
    Transmission_2 = 0.0;
    Convergence_2 = Convergence * Convergence;
    
    uz[0] = -Sin[0];
    uz[1] = -Sin[1];
    uz[2] = -Sin[2];
    
    if      (fabs(Sin[0]) > FLT_EPSILON) { ux[0] = +Sin[1]; ux[1] = -Sin[0]; ux[2] = +0.0;        }
    else if (fabs(Sin[1]) > FLT_EPSILON) { ux[0] = +0.0;    ux[1] = +Sin[2]; ux[2] = -Sin[1]; }
    else if (fabs(Sin[2]) > FLT_EPSILON) { ux[0] = +Sin[2]; ux[1] = +0.0;    ux[2] = -Sin[0]; }
    else return 1;
    
    Norm = 1.0 / sqrt(ux[0] * ux[0] + ux[1] * ux[1] + ux[2] * ux[2]);
    for (i = 0; i < 3; i++) ux[i] *= Norm;
    
    uy[0] = ux[1] * Sin[2] - ux[2] * Sin[1];
    uy[1] = ux[2] * Sin[0] - ux[0] * Sin[2];
    uy[2] = ux[0] * Sin[1] - ux[1] * Sin[0];
    
    //Norm = 1.0 / sqrt(uy[0] * uy[0] + uy[1] * uy[1] + uy[2] * uy[2]); // |Sin| = |ux| = 1.0, Sin normal ux --> |uy| = |Sin X ux| = 1.0 !!!
    //for (i = 0; i < 3; i++) uy[i] *= Norm;
    
    xl[0] = 0.0;
    yl[0] = 0.0;
    zl[0] = 0.0;
    for (i = 0; i < 3; i++)
    {
        xl[0] += Shape->Node[0].x[i] * ux[i];
        yl[0] += Shape->Node[0].x[i] * uy[i];
        zl[0] += Shape->Node[0].x[i] * uz[i];
    }
    
    xl[1] = xl[0];
    yl[1] = yl[0];
    zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        ls[0] = 0.0;
        ls[1] = 0.0;
        ls[2] = 0.0;
        for (j = 0; j < 3; j++)
        {
            ls[0] += Shape->Node[i].x[j] * ux[j];
            ls[1] += Shape->Node[i].x[j] * uy[j];
            ls[2] += Shape->Node[i].x[j] * uz[j];
        }
        
        if (xl[0] > ls[0]) xl[0] = ls[0]; else if (xl[1] < ls[0]) xl[1] = ls[0];
        if (yl[0] > ls[1]) yl[0] = ls[1]; else if (yl[1] < ls[1]) yl[1] = ls[1];
        if (zl[0] > ls[2]) zl[0] = ls[2]; else if (zl[1] < ls[2]) zl[1] = ls[2];
    }
    
    x_range = xl[1] - xl[0];
    y_range = yl[1] - yl[0];
    
    (*Transmission) = 0.0;
    Count = 0;
    for (i = 0; i < Max_Iteration_Count; i++)
    {
        dx = xl[0] + (x_range * (double)rand()) / (double)RAND_MAX;
        dy = yl[0] + (y_range * (double)rand()) / (double)RAND_MAX;
        
        Origin[0] = dx * ux[0] + dy * uy[0] + zl[0] * uz[0] - uz[0];
        Origin[1] = dx * ux[1] + dy * uy[1] + zl[0] * uz[1] - uz[1];
        Origin[2] = dx * ux[2] + dy * uy[2] + zl[0] * uz[2] - uz[2];
        
        Mesh_GetLeastPathLength(Shape, &Distance, &Origin[0], &uz[0]);
        Origin[0] += Distance * uz[0];
        Origin[1] += Distance * uz[1];
        Origin[2] += Distance * uz[2];
        
        if (Mesh_TestSite(Shape, &Origin[0]))
        {
            Mesh_GetLeastPathLength(Shape, &Distance, &Origin[0], &Sin[0]);
            
            Path_In = (Distance * (double)rand()) / (double)RAND_MAX;
            Origin[0] += Path_In * Sin[0];
            Origin[1] += Path_In * Sin[1];
            Origin[2] += Path_In * Sin[2];
            
            Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
            
            Path_Length = Path_In + Path_Ou;
            
            if (Path_Length > FLT_EPSILON)
            {
                Delta = exp(-Path_Length * Mu);
                
                (*Transmission) += Delta;
                Transmission_2  += Delta * Delta;
                
                Count++;
                
                if (Convergence_2 > 0.0)
                {
                    Norm = (double)Count;
                    if ((*Transmission) * (*Transmission) - (Norm * (Transmission_2 - Norm * Convergence_2)) > FLT_EPSILON) i = Max_Iteration_Count;
                }
            }
        }
    }
    
    (*Transmission) /= (double)Count;
    
    return 0;
}

int CrystalAbsorption_GetTransmission_MonteCarloIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Max_Iteration_Count, double Convergence)
{
    long i;
    
    long Count;
    
    double x_range, y_range, z_range;
    double_2 xl, yl, zl;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double Delta, Norm;
    double Transmission_2, Convergence_2;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    Sin[0] = -Beam_In[0] * Norm;
    Sin[1] = -Beam_In[1] * Norm;
    Sin[2] = -Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    Sou[0] = Beam_Ou[0] * Norm;
    Sou[1] = Beam_Ou[1] * Norm;
    Sou[2] = Beam_Ou[2] * Norm;
    
    Transmission_2 = 0.0;
    Convergence_2  = Convergence * Convergence;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    x_range = xl[1] - xl[0];
    y_range = yl[1] - yl[0];
    z_range = zl[1] - zl[0];
    
    (*Transmission) = 0.0;
    Count = 0;
    for (i = 0; i < Max_Iteration_Count; i++)
    {
        Origin[0] = xl[0] + (x_range * (double)rand()) / (double)RAND_MAX;
        Origin[1] = yl[0] + (y_range * (double)rand()) / (double)RAND_MAX;
        Origin[2] = zl[0] + (z_range * (double)rand()) / (double)RAND_MAX;
        
        if (Mesh_TestSite(Shape, &Origin[0]))
        {
            Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
            Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
            
            Path_Length = Path_In + Path_Ou;
            
            if (Path_Length > FLT_EPSILON)
            {
                Delta = exp(-Path_Length * Mu);
                
                (*Transmission) += Delta;
                Transmission_2  += Delta * Delta;
                
                Count++;
                
                if (Convergence_2 > FLT_EPSILON)
                {
                    Norm = (double)Count;
                    if ((*Transmission) * (*Transmission) - (Norm * (Transmission_2 - Norm * Convergence_2)) > FLT_EPSILON) i = Max_Iteration_Count;
                }
            }
        }
    }
    
    (*Transmission) /= (double)Count;
    
    return 0;
}
int CrystalAbsorption_GetTransmission_DiscreteIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, double Step_Size)
{
    long i, j, k;
    
    long Count;
    
    long nx, ny, nz;
    double_2 xl, yl, zl;
    
    double Path_In, Path_Ou, Path_Length, Norm;
    double_3 Origin;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    Sin[0] = -Beam_In[0] * Norm;
    Sin[1] = -Beam_In[1] * Norm;
    Sin[2] = -Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    Sou[0] = Beam_Ou[0] * Norm;
    Sou[1] = Beam_Ou[1] * Norm;
    Sou[2] = Beam_Ou[2] * Norm;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    Norm = 1.0 / (double)Step_Size;
    nx = 1 + (long)ceil((xl[1] - xl[0]) * Norm);
    ny = 1 + (long)ceil((yl[1] - yl[0]) * Norm);
    nz = 1 + (long)ceil((zl[1] - zl[0]) * Norm);
    
    (*Transmission) = 0.0;
    Count = 0;
    for (i = 0; i < nx; i++) { Origin[0] = xl[0] + Step_Size * (double)i;
    for (j = 0; j < ny; j++) { Origin[1] = yl[0] + Step_Size * (double)j;
    for (k = 0; k < nz; k++) { Origin[2] = zl[0] + Step_Size * (double)k;
    if (Mesh_TestSite(Shape, &Origin[0]))
    {
        Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
        Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
        
        Path_Length = Path_In + Path_Ou;
        
        if (Path_Length > FLT_EPSILON)
        {
            (*Transmission) += exp(-Path_Length * Mu);
            Count++;
        }
    }}}}
    
    (*Transmission) /= (double)Count;
    
    return 0;
}
int CrystalAbsorption_GetTransmission_DiscreteIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, long Step_Count)
{
    long i, j, k;
    
    long Count;
    
    double_2 xl, yl, zl;
    double dx, dy, dz, Norm;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    Sin[0] = -Beam_In[0] * Norm;
    Sin[1] = -Beam_In[1] * Norm;
    Sin[2] = -Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    Sou[0] = Beam_Ou[0] * Norm;
    Sou[1] = Beam_Ou[1] * Norm;
    Sou[2] = Beam_Ou[2] * Norm;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    Norm = 1.0 / (double)Step_Count;
    dx = (xl[1] - xl[0]) * Norm;
    dy = (yl[1] - yl[0]) * Norm;
    dz = (zl[1] - zl[0]) * Norm;
    
    (*Transmission) = 0.0;
    Count = 0;
    for (i = 0; i <= Step_Count; i++) { Origin[0] = xl[0] + dx * (double)i;
    for (j = 0; j <= Step_Count; j++) { Origin[1] = yl[0] + dy * (double)j;
    for (k = 0; k <= Step_Count; k++) { Origin[2] = zl[0] + dz * (double)k;
    if (Mesh_TestSite(Shape, &Origin[0]))
    {
        Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
        Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
        
        Path_Length = Path_In + Path_Ou;
        
        if (Path_Length > FLT_EPSILON)
        {
            (*Transmission) += exp(-Path_Length * Mu);
            Count++;
        }
    }}}}
    
    (*Transmission) /= (double)Count;
    
    return 0;
}
int CrystalAbsorption_GetTransmission_LatticeIntegration(double* Transmission, struct Mesh* Shape, double Mu, double* Beam_In, double* Beam_Ou, double_3* Lattice, long Lattice_Count)
{
    long i;
    
    long Count;
    
    double Path_In, Path_Ou, Path_Length, Norm;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    Sin[0] = -Beam_In[0] * Norm;
    Sin[1] = -Beam_In[1] * Norm;
    Sin[2] = -Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    Sou[0] = Beam_Ou[0] * Norm;
    Sou[1] = Beam_Ou[1] * Norm;
    Sou[2] = Beam_Ou[2] * Norm;
    
    (*Transmission) = 0.0;
    Count = 0;
    for (i = 0; i < Lattice_Count; i++) // { if (Mesh_TestSite(Shape, &Lattice[i][0])) // the lattice nodes are assumed to be within the Shape
    {
        Mesh_GetLeastPathLength(Shape, &Path_In, &Lattice[i][0], &Sin[0]);
        Mesh_GetLeastPathLength(Shape, &Path_Ou, &Lattice[i][0], &Sou[0]);
        
        Path_Length = Path_In + Path_Ou;
        
        if (Path_Length > FLT_EPSILON)
        {
            (*Transmission) += exp(-Path_Length * Mu);
            Count++;
        }
    } // }
    
    (*Transmission) /= (double)Count;
    
    return 0;
}

// Analytic solution: Tompa 1969

#define _3D_SORTING_COUNT_ 20

struct SXD_3dSorting
{
    double_2 xl;
    double_2 yl;
    double_2 zl;
    //
    double Idx;
    double Idy;
    double Idz;
    //
    long_2 EntryId[_3D_SORTING_COUNT_][_3D_SORTING_COUNT_][_3D_SORTING_COUNT_];
    long*  NextId_Shape;
    long*  NextId_Ghost;
};

struct SXD_Tetrahedron
{
    long_4 Node_Id;
    double Volume;
    double Transmission;
};
struct SXD_SubHowellSet
{
    long Vertex_Count;
    double** Vertex_x;
    struct SXD_Scattering** Vertex_ab;
    //
    long Tetrahedron_Count;
    struct SXD_Tetrahedron* Tetrahedron;
};
struct SXD_Plane
{
    bool     Test_In;
    bool     Test_Ou;
    //
    double_4 Plane_In;
    double_4 Plane_Ou;
};
struct SXD_Scattering
{
    double_3 Path; // L[0] = l[1]+l[2]
    double   g;
};
struct SXD_Node
{
    double_3 x;
    struct SXD_Scattering ab;
};
struct SXD_Mesh
{
    struct Mesh* Shape;
    //
    double Volume;                  // Crystal Volume
    double Transmission;            // Crystal Transmission
    //
    double Mu;                      // Linear attenuation coefficient
    double_3 Beam_In;               // In-comming beam direction (normalized)
    double_3 Beam_II;               // In-comming beam opposit direction (normalized)
    double_3 Beam_Ou;               // Out-going  beam direction (normalized)
    //
    struct SXD_Plane* EdgePlane;    // In/Out-comming beam projection through the shape edges
    //
    long BeamPlane_Count;           // Valid Beam through Edge Planes
    double** BeamPlane;             // Pointer to call the Beam through EdgePlane
    //
    long GhostNode_Count;           // Count of the nodes from Edge-Planes
    struct SXD_Node* GhostNode;     // Nodes from Edge-Planes
    struct SXD_Scattering* Node_ab; // Scattering information for the shape nodes
    //
    long SubHowellSet_Count;
    struct SXD_SubHowellSet* SubHowellSet;
};


void CrystalAbsorption_SXD_Mesh_Reset(struct SXD_Mesh* SXD)
{
    SXD->EdgePlane = NULL;
    SXD->Node_ab   = NULL;
    SXD->GhostNode = NULL;
    SXD->BeamPlane = NULL;
    SXD->SubHowellSet = NULL;
    
    SXD->GhostNode_Count = 0;
    SXD->BeamPlane_Count = 0;
    SXD->SubHowellSet_Count = 0;
    
    SXD->Volume = 0.0;
    SXD->Transmission = 1.0;
    SXD->Mu = 0.0;
    
    SXD->Beam_In[0] = +1.0;
    SXD->Beam_In[1] = +0.0;
    SXD->Beam_In[2] = +0.0;
    
    SXD->Beam_Ou[0] = -1.0;
    SXD->Beam_Ou[1] = +0.0;
    SXD->Beam_Ou[2] = +0.0;
    
    return;
}
void CrystalAbsorption_SXD_Mesh_Free(struct SXD_Mesh* SXD)
{
    long i;
    
    if (SXD != NULL)
    {
        if (SXD->EdgePlane != NULL) free(SXD->EdgePlane);
        if (SXD->GhostNode != NULL) free(SXD->GhostNode);
        if (SXD->Node_ab   != NULL) free(SXD->Node_ab);
        if (SXD->SubHowellSet != NULL)
        {
            for (i = 0; i < SXD->SubHowellSet_Count; i++)
            {
                if (SXD->SubHowellSet[i].Vertex_x    != NULL) free(SXD->SubHowellSet[i].Vertex_x);
                if (SXD->SubHowellSet[i].Vertex_ab   != NULL) free(SXD->SubHowellSet[i].Vertex_ab);
                if (SXD->SubHowellSet[i].Tetrahedron != NULL) free(SXD->SubHowellSet[i].Tetrahedron);
            }
            free(SXD->SubHowellSet);
        }
        
        free(SXD);
    }
    
    return;
}


void CrystalAbsorption_SXD_Mesh_Exclude_Equivalent_EdgePlane(struct SXD_Mesh* SXD, long Edge_Id)
{
    long i;
    
    double Scalar;
    
    // EdgePlane Beam-In equivalent to Shape Face
    if (SXD->EdgePlane[Edge_Id].Test_In)
    {
        for (i = 0; i < SXD->Shape->Face_Count; i++)
        {
            Scalar = SXD->Shape->Face[i].Normal[0] * SXD->EdgePlane[Edge_Id].Plane_In[0] + SXD->Shape->Face[i].Normal[1] * SXD->EdgePlane[Edge_Id].Plane_In[1] + SXD->Shape->Face[i].Normal[2] * SXD->EdgePlane[Edge_Id].Plane_In[2];
            if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
            {
                if (fabs(SXD->Shape->Face[i].Normal[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_In[3]) < FLT_EPSILON)
                {
                    SXD->EdgePlane[Edge_Id].Test_In = false;
                    break;
                }
            }
        }
    }
    
    // EdgePlane Beam-Ou equivalent to Shape Face
    if (SXD->EdgePlane[Edge_Id].Test_Ou)
    {
        for (i = 0; i < SXD->Shape->Face_Count; i++)
        {
            Scalar = SXD->Shape->Face[i].Normal[0] * SXD->EdgePlane[Edge_Id].Plane_Ou[0] + SXD->Shape->Face[i].Normal[1] * SXD->EdgePlane[Edge_Id].Plane_Ou[1] + SXD->Shape->Face[i].Normal[2] * SXD->EdgePlane[Edge_Id].Plane_Ou[2];
            if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
            {
                if (fabs(SXD->Shape->Face[i].Normal[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_Ou[3]) < FLT_EPSILON)
                {
                    SXD->EdgePlane[Edge_Id].Test_Ou = false;
                    break;
                }
            }
        }
    }
    
    // EdgePlane Beam-In equivalent to already collected EdgePalne
    if (SXD->EdgePlane[Edge_Id].Test_In)
    {
        for (i = 0; i < Edge_Id; i++)
        {
            if (SXD->EdgePlane[i].Test_In)
            {
                Scalar = SXD->EdgePlane[i].Plane_In[0] * SXD->EdgePlane[Edge_Id].Plane_In[0] + SXD->EdgePlane[i].Plane_In[1] * SXD->EdgePlane[Edge_Id].Plane_In[1] + SXD->EdgePlane[i].Plane_In[2] * SXD->EdgePlane[Edge_Id].Plane_In[2];
                if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
                {
                    if (fabs(SXD->EdgePlane[i].Plane_In[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_In[3]) < FLT_EPSILON)
                    {
                        SXD->EdgePlane[Edge_Id].Test_In = false;
                        break;
                    }
                }
            }
            if (SXD->EdgePlane[i].Test_Ou)
            {
                Scalar = SXD->EdgePlane[i].Plane_Ou[0] * SXD->EdgePlane[Edge_Id].Plane_In[0] + SXD->EdgePlane[i].Plane_Ou[1] * SXD->EdgePlane[Edge_Id].Plane_In[1] + SXD->EdgePlane[i].Plane_Ou[2] * SXD->EdgePlane[Edge_Id].Plane_In[2];
                if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
                {
                    if (fabs(SXD->EdgePlane[i].Plane_Ou[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_In[3]) < FLT_EPSILON)
                    {
                        SXD->EdgePlane[Edge_Id].Test_In = false;
                        break;
                    }
                }
            }
        }
    }
    
    // EdgePlane Beam-Ou equivalent to already collected EdgePalne
    if (SXD->EdgePlane[Edge_Id].Test_Ou)
    {
        for (i = 0; i < Edge_Id; i++)
        {
            if (SXD->EdgePlane[i].Test_In)
            {
                Scalar = SXD->EdgePlane[i].Plane_In[0] * SXD->EdgePlane[Edge_Id].Plane_Ou[0] + SXD->EdgePlane[i].Plane_In[1] * SXD->EdgePlane[Edge_Id].Plane_Ou[1] + SXD->EdgePlane[i].Plane_In[2] * SXD->EdgePlane[Edge_Id].Plane_Ou[2];
                if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
                {
                    if (fabs(SXD->EdgePlane[i].Plane_In[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_Ou[3]) < FLT_EPSILON)
                    {
                        SXD->EdgePlane[Edge_Id].Test_Ou = false;
                        break;
                    }
                }
            }
            if (SXD->EdgePlane[i].Test_Ou)
            {
                Scalar = SXD->EdgePlane[i].Plane_Ou[0] * SXD->EdgePlane[Edge_Id].Plane_Ou[0] + SXD->EdgePlane[i].Plane_Ou[1] * SXD->EdgePlane[Edge_Id].Plane_Ou[1] + SXD->EdgePlane[i].Plane_Ou[2] * SXD->EdgePlane[Edge_Id].Plane_Ou[2];
                if (fabs(Scalar * Scalar - 1.0) < FLT_EPSILON)
                {
                    if (fabs(SXD->EdgePlane[i].Plane_Ou[3] - Scalar * SXD->EdgePlane[Edge_Id].Plane_Ou[3]) < FLT_EPSILON)
                    {
                        SXD->EdgePlane[Edge_Id].Test_Ou = false;
                        break;
                    }
                }
            }
        }
    }
    
    return;
}
void CrystalAbsorption_SXD_Mesh_Exclude_Tangent_EdgePlane(struct SXD_Mesh* SXD, long Edge_Id)
{
    long i, i0;
    
    double Side, Sign;
    bool   Test;
    
    // Test that EdgePane Beam-In splits the shape in two (if not the plane is tangent to the shape and should be ignored)
    if (SXD->EdgePlane[Edge_Id].Test_In)
    {
        Sign = +0.0;
        for (i = 0; i < SXD->Shape->Node_Count; i++)
        {
            Side = SXD->EdgePlane[Edge_Id].Plane_In[3] - (SXD->Shape->Node[i].x[0] * SXD->EdgePlane[Edge_Id].Plane_In[0] + SXD->Shape->Node[i].x[1] * SXD->EdgePlane[Edge_Id].Plane_In[1] + SXD->Shape->Node[i].x[2] * SXD->EdgePlane[Edge_Id].Plane_In[2]);
            
            if (fabs(Side) > FLT_EPSILON)
            {
                if (Side > 0.0) Sign = +1.0;
                else            Sign = -1.0;
                
                i0 = i + 1;
                i = SXD->Shape->Node_Count;
            }
        }
        
        Test = false;
        for (i = i0; i < SXD->Shape->Node_Count; i++)
        {
            Side = SXD->EdgePlane[Edge_Id].Plane_In[3] - (SXD->Shape->Node[i].x[0] * SXD->EdgePlane[Edge_Id].Plane_In[0] + SXD->Shape->Node[i].x[1] * SXD->EdgePlane[Edge_Id].Plane_In[1] + SXD->Shape->Node[i].x[2] * SXD->EdgePlane[Edge_Id].Plane_In[2]);
            
            if (Side * Sign < -FLT_EPSILON)
            {
                Test = true;
                break;
            }
        }
        
        SXD->EdgePlane[Edge_Id].Test_In = Test;
    }
    
    // Test that EdgePane Beam-Ou splits the shape in two (if not the plane is tangent to the shape and should be ignored)
    if (SXD->EdgePlane[Edge_Id].Test_Ou)
    {
        Sign = +0.0;
        for (i = 0; i < SXD->Shape->Node_Count; i++)
        {
            Side = SXD->EdgePlane[Edge_Id].Plane_Ou[3] - (SXD->Shape->Node[i].x[0] * SXD->EdgePlane[Edge_Id].Plane_Ou[0] + SXD->Shape->Node[i].x[1] * SXD->EdgePlane[Edge_Id].Plane_Ou[1] + SXD->Shape->Node[i].x[2] * SXD->EdgePlane[Edge_Id].Plane_Ou[2]);
            
            if (fabs(Side) > FLT_EPSILON)
            {
                if (Side > 0.0) Sign = +1.0;
                else            Sign = -1.0;
                
                i0 = i + 1;
                i = SXD->Shape->Node_Count;
            }
        }
        
        Test = false;
        for (i = i0; i < SXD->Shape->Node_Count; i++)
        {
            Side = SXD->EdgePlane[Edge_Id].Plane_Ou[3] - (SXD->Shape->Node[i].x[0] * SXD->EdgePlane[Edge_Id].Plane_Ou[0] + SXD->Shape->Node[i].x[1] * SXD->EdgePlane[Edge_Id].Plane_Ou[1] + SXD->Shape->Node[i].x[2] * SXD->EdgePlane[Edge_Id].Plane_Ou[2]);
            
            if (Side * Sign < -FLT_EPSILON)
            {
                Test = true;
                break;
            }
        }
        
        SXD->EdgePlane[Edge_Id].Test_Ou = Test;
    }
    
    return;
}
int  CrystalAbsorption_SXD_Mesh_Get_EdgePlane(struct SXD_Mesh* SXD)
{
    long i;
    
    double Norm2, Norm;
    
    SXD->EdgePlane = (struct SXD_Plane*)calloc(SXD->Shape->Edge_Count, sizeof(struct SXD_Plane));
    if (_APP_WARNING_MESSAGE(SXD->EdgePlane, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_GetEdgeBeamPlane", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->EdgePlane\" to", SXD->Shape->Edge_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->BeamPlane = (double**)calloc(SXD->Shape->Edge_Count * 2, sizeof(double*));
    if (_APP_WARNING_MESSAGE(SXD->BeamPlane, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_GetEdgeBeamPlane", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->BeamPlane\" to", SXD->Shape->Edge_Count * 2, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->BeamPlane_Count = 0;
    for (i = 0; i < SXD->Shape->Edge_Count; i++)
    {
        SXD->EdgePlane[i].Plane_In[0] = SXD->Shape->Edge[i].Direction[1] * SXD->Beam_In[2] - SXD->Shape->Edge[i].Direction[2] * SXD->Beam_In[1];
        SXD->EdgePlane[i].Plane_In[1] = SXD->Shape->Edge[i].Direction[2] * SXD->Beam_In[0] - SXD->Shape->Edge[i].Direction[0] * SXD->Beam_In[2];
        SXD->EdgePlane[i].Plane_In[2] = SXD->Shape->Edge[i].Direction[0] * SXD->Beam_In[1] - SXD->Shape->Edge[i].Direction[1] * SXD->Beam_In[0];
        Norm2 = SXD->EdgePlane[i].Plane_In[0] * SXD->EdgePlane[i].Plane_In[0] + SXD->EdgePlane[i].Plane_In[1] * SXD->EdgePlane[i].Plane_In[1] + SXD->EdgePlane[i].Plane_In[2] * SXD->EdgePlane[i].Plane_In[2];
        if (Norm2 > FLT_EPSILON)
        {
            Norm = 1.0 / sqrt(Norm2);
            
            SXD->EdgePlane[i].Test_In      = true;
            SXD->EdgePlane[i].Plane_In[0] *= Norm;
            SXD->EdgePlane[i].Plane_In[1] *= Norm;
            SXD->EdgePlane[i].Plane_In[2] *= Norm;
            SXD->EdgePlane[i].Plane_In[3]  = SXD->Shape->Edge[i].Center[0] * SXD->EdgePlane[i].Plane_In[0] +
                                             SXD->Shape->Edge[i].Center[1] * SXD->EdgePlane[i].Plane_In[1] +
                                             SXD->Shape->Edge[i].Center[2] * SXD->EdgePlane[i].Plane_In[2];
        }
        else SXD->EdgePlane[i].Test_In     = false;
        
        
        SXD->EdgePlane[i].Plane_Ou[0] = SXD->Shape->Edge[i].Direction[1] * SXD->Beam_Ou[2] - SXD->Shape->Edge[i].Direction[2] * SXD->Beam_Ou[1];
        SXD->EdgePlane[i].Plane_Ou[1] = SXD->Shape->Edge[i].Direction[2] * SXD->Beam_Ou[0] - SXD->Shape->Edge[i].Direction[0] * SXD->Beam_Ou[2];
        SXD->EdgePlane[i].Plane_Ou[2] = SXD->Shape->Edge[i].Direction[0] * SXD->Beam_Ou[1] - SXD->Shape->Edge[i].Direction[1] * SXD->Beam_Ou[0];
        Norm2 = SXD->EdgePlane[i].Plane_Ou[0] * SXD->EdgePlane[i].Plane_Ou[0] + SXD->EdgePlane[i].Plane_Ou[1] * SXD->EdgePlane[i].Plane_Ou[1] + SXD->EdgePlane[i].Plane_Ou[2] * SXD->EdgePlane[i].Plane_Ou[2];
        if (Norm2 > FLT_EPSILON)
        {
            Norm = 1.0 / sqrt(Norm2);
            
            SXD->EdgePlane[i].Test_Ou      = true;
            SXD->EdgePlane[i].Plane_Ou[0] *= Norm;
            SXD->EdgePlane[i].Plane_Ou[1] *= Norm;
            SXD->EdgePlane[i].Plane_Ou[2] *= Norm;
            SXD->EdgePlane[i].Plane_Ou[3]  = SXD->Shape->Edge[i].Center[0] * SXD->EdgePlane[i].Plane_Ou[0] +
                                             SXD->Shape->Edge[i].Center[1] * SXD->EdgePlane[i].Plane_Ou[1] +
                                             SXD->Shape->Edge[i].Center[2] * SXD->EdgePlane[i].Plane_Ou[2];
        }
        else SXD->EdgePlane[i].Test_Ou     = false;
        
        CrystalAbsorption_SXD_Mesh_Exclude_Equivalent_EdgePlane(SXD, i);
        CrystalAbsorption_SXD_Mesh_Exclude_Tangent_EdgePlane(SXD, i);
        
        if (SXD->EdgePlane[i].Test_In) { SXD->BeamPlane[SXD->BeamPlane_Count] = &SXD->EdgePlane[i].Plane_In[0]; SXD->BeamPlane_Count++; }
        if (SXD->EdgePlane[i].Test_Ou) { SXD->BeamPlane[SXD->BeamPlane_Count] = &SXD->EdgePlane[i].Plane_Ou[0]; SXD->BeamPlane_Count++; }
    }
    
    if (SXD->BeamPlane_Count > 0)
    {
        SXD->BeamPlane = (double**)realloc(SXD->BeamPlane, SXD->BeamPlane_Count * sizeof(double*));
        if (_APP_WARNING_MESSAGE(SXD->BeamPlane, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_GetEdgeBeamPlane", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->BeamPlane\" to", SXD->BeamPlane_Count, 0)) { _USER_IO_WAIT(); return 1; }
    }
    else
    {
        free(SXD->BeamPlane);
        SXD->BeamPlane = NULL;
    }
    
    return 0;
}

int  Get_IntersectionEdge_PlanePlane(double* Edge_direction, double* Edge_Origin, double* Plane_a, double* Plane_b)
{
    double rr, Norm;
    double A[3][3];
    
    Edge_direction[0] = Plane_a[1] * Plane_b[2] - Plane_a[2] * Plane_b[1];
    Edge_direction[1] = Plane_a[2] * Plane_b[0] - Plane_a[0] * Plane_b[2];
    Edge_direction[2] = Plane_a[0] * Plane_b[1] - Plane_a[1] * Plane_b[0];
    
    rr = Edge_direction[0] * Edge_direction[0] + Edge_direction[1] * Edge_direction[1] + Edge_direction[2] * Edge_direction[2];
    if (rr < FLT_EPSILON)             return 1; // parallel planes
    else if (fabs(rr - 1.0) > FLT_EPSILON)
    {
        Norm = 1.0 / sqrt(rr);
        Edge_direction[0] *= Norm;
        Edge_direction[1] *= Norm;
        Edge_direction[2] *= Norm;
    }
    
    A[0][0] = Plane_a[0]; A[0][1] = Plane_a[1]; A[0][2] = Plane_a[2];
    A[1][0] = Plane_b[0]; A[1][1] = Plane_b[1]; A[1][2] = Plane_b[2];
    A[2][0] = Edge_direction[0]; A[2][1] = Edge_direction[1]; A[2][2] = Edge_direction[2];
    
    if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
    {
        Edge_Origin[0] = A[0][0] * Plane_a[3] + A[0][1] * Plane_b[3];
        Edge_Origin[1] = A[1][0] * Plane_a[3] + A[1][1] * Plane_b[3];
        Edge_Origin[2] = A[2][0] * Plane_a[3] + A[2][1] * Plane_b[3];
    }
    
    return 0;
}
int  Get_IntersectionNode_PlaneEdge(double* Node, double* Plane, double* Edge_direction, double* Edge_Origin) // Plane and Edge direction are assumed to be normalized // return value: 0 intersection found, 1 if not-found.
{
    double rh, qh;
    
    qh = Plane[0] * Edge_direction[0] + Plane[1] * Edge_direction[1] + Plane[2] * Edge_direction[2];
    if (fabs(qh) > FLT_EPSILON)
    {
        rh = Plane[3] - (Plane[0] * Edge_Origin[0] + Plane[1] * Edge_Origin[1] + Plane[2] * Edge_Origin[2]);
        if (fabs(rh) > FLT_EPSILON)
        {
            rh /= qh;
            
            Node[0] = Edge_Origin[0] + rh * Edge_direction[0];
            Node[1] = Edge_Origin[1] + rh * Edge_direction[1];
            Node[2] = Edge_Origin[2] + rh * Edge_direction[2];
            
            return 0;
        }
        else
        {
            Node[0] = Edge_Origin[0];
            Node[1] = Edge_Origin[1];
            Node[2] = Edge_Origin[2];
            
            return 0;
        }
    }
    
    return 1;
}
int  Get_IntersectionNode_PlaneEdge(double* Node, double* Plane, double* Edge_direction, double* Edge_Origin, double *rh) // Plane and Edge direction are assumed to be normalized // return value: 0 intersection found, 1 if not-found.
{
    double qh;
    
    qh = Plane[0] * Edge_direction[0] + Plane[1] * Edge_direction[1] + Plane[2] * Edge_direction[2];
    if (fabs(qh) > FLT_EPSILON)
    {
        *rh = Plane[3] - (Plane[0] * Edge_Origin[0] + Plane[1] * Edge_Origin[1] + Plane[2] * Edge_Origin[2]);
        if (fabs(*rh) > FLT_EPSILON)
        {
            *rh /= qh;
            
            Node[0] = Edge_Origin[0] + *rh * Edge_direction[0];
            Node[1] = Edge_Origin[1] + *rh * Edge_direction[1];
            Node[2] = Edge_Origin[2] + *rh * Edge_direction[2];
            
            return 0;
        }
        else
        {
            Node[0] = Edge_Origin[0];
            Node[1] = Edge_Origin[1];
            Node[2] = Edge_Origin[2];
            
            return 0;
        }
    }
    
    return 1;
}
int  CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(struct SXD_Mesh* SXD, double Range)
{
    long i;
    
    double   xx;
    double_3 dd;
    
    for (i = 0; i < SXD->Shape->Node_Count; i++)
    {
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Node[i].x[0]; dd[0] = xx * xx; if (dd[0] > Range) continue;
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Node[i].x[1]; dd[1] = xx * xx; if (dd[1] > Range) continue;
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Node[i].x[2]; dd[2] = xx * xx; if (dd[2] > Range) continue;
        if (dd[0] + dd[1] + dd[2] > Range) continue;
        return 0;
    }
    
    for (i = 0; i < SXD->GhostNode_Count; i++)
    {
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->GhostNode[i].x[0]; dd[0] = xx * xx; if (dd[0] > Range) continue;
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->GhostNode[i].x[1]; dd[1] = xx * xx; if (dd[1] > Range) continue;
        xx = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->GhostNode[i].x[2]; dd[2] = xx * xx; if (dd[2] > Range) continue;
        if (dd[0] + dd[1] + dd[2] > Range) continue;
        return 0;
    }
    
    return 1;
}
int  CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(struct SXD_Mesh* SXD, double Range, struct SXD_3dSorting *Node_Sorting)
{
    long i, j, k, l;
    long ix, iy, iz;
    
    double   xx;
    double_3 dd;
    
    ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting->xl[0]) * Node_Sorting->Idx);
    iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting->yl[0]) * Node_Sorting->Idy);
    iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting->zl[0]) * Node_Sorting->Idz);
    
    for (i = ix - 1; i < ix + 2; i++) { if (i < 0) i = 0;
    for (j = iy - 1; j < iy + 2; j++) { if (j < 0) j = 0;
    for (k = iz - 1; k < iz + 2; k++) { if (k < 0) k = 0;
        if (Node_Sorting->EntryId[i][j][k][0] >= 0)
        {
            l = Node_Sorting->EntryId[i][j][k][0];
            while (l >= 0)
            {
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Node[l].x[0]; dd[0] = xx * xx; if (dd[0] > Range) { l = Node_Sorting->NextId_Shape[l]; continue; }
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Node[l].x[1]; dd[1] = xx * xx; if (dd[1] > Range) { l = Node_Sorting->NextId_Shape[l]; continue; }
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Node[l].x[2]; dd[2] = xx * xx; if (dd[2] > Range) { l = Node_Sorting->NextId_Shape[l]; continue; }
                if (dd[0] + dd[1] + dd[2] > Range) { l = Node_Sorting->NextId_Shape[l]; continue; }
                return 0;
            }
        }
        if (Node_Sorting->EntryId[i][j][k][1] >= 0)
        {
            l = Node_Sorting->EntryId[i][j][k][1];
            while (l >= 0)
            {
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->GhostNode[l].x[0]; dd[0] = xx * xx; if (dd[0] > Range) { l = Node_Sorting->NextId_Ghost[l]; continue; }
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->GhostNode[l].x[1]; dd[1] = xx * xx; if (dd[1] > Range) { l = Node_Sorting->NextId_Ghost[l]; continue; }
                xx = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->GhostNode[l].x[2]; dd[2] = xx * xx; if (dd[2] > Range) { l = Node_Sorting->NextId_Ghost[l]; continue; }
                if (dd[0] + dd[1] + dd[2] > Range) { l = Node_Sorting->NextId_Ghost[l]; continue; }
                return 0;
            }
        }
    if (k + 1 >= _3D_SORTING_COUNT_) k = iz + 2; }
    if (j + 1 >= _3D_SORTING_COUNT_) j = iy + 2; }
    if (i + 1 >= _3D_SORTING_COUNT_) i = ix + 2; }
    
    return 1;
}

int  CrystalAbsorption_SXD_Mesh_Get_GhostNode_Plane(struct SXD_Mesh* SXD, double Range)
{
    long i, j, k, l;
    long ix, iy, iz;
    
    double Margin;
    
    double_3 dx;
    double A[3][3];
    
    double_2 xl, yl, zl;
    
    struct SXD_3dSorting Node_Sorting;
    
    bool Skip;
    
    long   Ghost_Allocated = SXD->Shape->Edge_Count;
    
    if (Range < FLT_EPSILON) Range = FLT_EPSILON;
    
    SXD->GhostNode = (struct SXD_Node*)calloc(Ghost_Allocated, sizeof(struct SXD_Node));
    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->GhostNode_Count = 0;
    
    //clock_t begin, end;
    //begin = clock();
    
    xl[0] = SXD->Shape->Node[0].x[0]; xl[1] = SXD->Shape->Node[0].x[0];
    yl[0] = SXD->Shape->Node[0].x[1]; yl[1] = SXD->Shape->Node[0].x[1];
    zl[0] = SXD->Shape->Node[0].x[2]; zl[1] = SXD->Shape->Node[0].x[2];
    
    for (i = 1; i < SXD->Shape->Node_Count; i++)
    {
        if (xl[0] > SXD->Shape->Node[i].x[0]) { xl[0] = SXD->Shape->Node[i].x[0]; } if (xl[1] < SXD->Shape->Node[i].x[0]) { xl[1] = SXD->Shape->Node[i].x[0]; }
        if (yl[0] > SXD->Shape->Node[i].x[1]) { yl[0] = SXD->Shape->Node[i].x[1]; } if (yl[1] < SXD->Shape->Node[i].x[1]) { yl[1] = SXD->Shape->Node[i].x[1]; }
        if (zl[0] > SXD->Shape->Node[i].x[2]) { zl[0] = SXD->Shape->Node[i].x[2]; } if (zl[1] < SXD->Shape->Node[i].x[2]) { zl[1] = SXD->Shape->Node[i].x[2]; }
    }
    
    Node_Sorting.xl[0] = xl[0]; Node_Sorting.xl[1] = xl[1]; 
    Node_Sorting.yl[0] = yl[0]; Node_Sorting.yl[1] = yl[1];
    Node_Sorting.zl[0] = zl[0]; Node_Sorting.zl[1] = zl[1];

    Margin = 0.50 * (xl[1] - xl[0]) / (double)_3D_SORTING_COUNT_; Node_Sorting.xl[0] -= Margin; Node_Sorting.xl[1] += Margin;
    Margin = 0.50 * (yl[1] - yl[0]) / (double)_3D_SORTING_COUNT_; Node_Sorting.yl[0] -= Margin; Node_Sorting.yl[1] += Margin;
    Margin = 0.50 * (zl[1] - zl[0]) / (double)_3D_SORTING_COUNT_; Node_Sorting.zl[0] -= Margin; Node_Sorting.zl[1] += Margin;

    Node_Sorting.Idx = (double)_3D_SORTING_COUNT_ / (Node_Sorting.xl[1] - Node_Sorting.xl[0] + 2.0 * FLT_EPSILON);
    Node_Sorting.Idy = (double)_3D_SORTING_COUNT_ / (Node_Sorting.yl[1] - Node_Sorting.yl[0] + 2.0 * FLT_EPSILON);
    Node_Sorting.Idz = (double)_3D_SORTING_COUNT_ / (Node_Sorting.zl[1] - Node_Sorting.zl[0] + 2.0 * FLT_EPSILON);
    
    for (ix = 0; ix < _3D_SORTING_COUNT_; ix++) {
    for (iy = 0; iy < _3D_SORTING_COUNT_; iy++) {
    for (iz = 0; iz < _3D_SORTING_COUNT_; iz++) {
        Node_Sorting.EntryId[ix][iy][iz][0] = -1;
        Node_Sorting.EntryId[ix][iy][iz][1] = -1;
    }}}
    
    Node_Sorting.NextId_Shape = (long*)calloc(SXD->Shape->Node_Count, sizeof(long));
    if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Shape, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Shape\" to", SXD->Shape->Node_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    for (i = 0; i < SXD->Shape->Node_Count; i++)
    {
        ix = (long)floor((SXD->Shape->Node[i].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
        iy = (long)floor((SXD->Shape->Node[i].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
        iz = (long)floor((SXD->Shape->Node[i].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
        
        Node_Sorting.NextId_Shape[i] = Node_Sorting.EntryId[ix][iy][iz][0];
        Node_Sorting.EntryId[ix][iy][iz][0] = i;
    }
    
    Node_Sorting.NextId_Ghost = (long*)calloc(Ghost_Allocated, sizeof(long));
    if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
    
    // Shape->Edge with EdgePlane->Beam_In/Ou
    for (i = 0; i < SXD->Shape->Edge_Count; i++)
    {
        for (j = 0; j < SXD->Shape->Edge_Count; j++)
        {
            if (i != j)
            {
                if (SXD->EdgePlane[j].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_In[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                            {
                                ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                
                                Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                
                                SXD->GhostNode_Count++;
                            }
                            //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                
                                Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
                
                if (SXD->EdgePlane[j].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_Ou[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                            {
                                ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                
                                Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                
                                SXD->GhostNode_Count++;
                            }
                            //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                
                                Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
            }
            // else the Plane_In/Ou were generated from the edge itself 
        }
    }
    // Mesh->Plane with Beam_In/Ou and Beam_In/Ou
    for (i = 0; i < SXD->Shape->Face_Count; i++)
    {
        for (j = 0; j < SXD->Shape->Edge_Count; j++)
        {
            for (k = 0; k < SXD->Shape->Edge_Count; k++)
            {
                if (j != k) // exclude either same plane In-In / Ou-Ou and edge coincident to Mesh->Edge from In-Ou
                {
                    if (SXD->EdgePlane[j].Test_In && SXD->EdgePlane[k].Test_Ou)
                    {
                        A[0][0] = SXD->Shape->Face[i].Normal[0]; A[0][1] = SXD->Shape->Face[i].Normal[1]; A[0][2] = SXD->Shape->Face[i].Normal[2];
                        A[1][0] = SXD->EdgePlane[j].Plane_In[0]; A[1][1] = SXD->EdgePlane[j].Plane_In[1]; A[1][2] = SXD->EdgePlane[j].Plane_In[2];
                        A[2][0] = SXD->EdgePlane[k].Plane_Ou[0]; A[2][1] = SXD->EdgePlane[k].Plane_Ou[1]; A[2][2] = SXD->EdgePlane[k].Plane_Ou[2];
                        
                        if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
                        {
                            SXD->GhostNode[SXD->GhostNode_Count].x[0] = A[0][0] * SXD->Shape->Face[i].Normal[3] + A[0][1] * SXD->EdgePlane[j].Plane_In[3] + A[0][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[1] = A[1][0] * SXD->Shape->Face[i].Normal[3] + A[1][1] * SXD->EdgePlane[j].Plane_In[3] + A[1][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[2] = A[2][0] * SXD->Shape->Face[i].Normal[3] + A[2][1] * SXD->EdgePlane[j].Plane_In[3] + A[2][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            
                            Skip = false;
                            for (l = 0; l < SXD->Shape->Face_Count; l++) {
                                if (l != i) {
                                    if      (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->Shape->Face[l].Normal[0] * SXD->GhostNode[SXD->GhostNode_Count].x[0] +
                                             SXD->Shape->Face[l].Normal[1] * SXD->GhostNode[SXD->GhostNode_Count].x[1] +
                                             SXD->Shape->Face[l].Normal[2] * SXD->GhostNode[SXD->GhostNode_Count].x[2] -
                                             SXD->Shape->Face[l].Normal[3] > FLT_EPSILON)                      { Skip = true; break; }
                                }
                            }
                            if (!Skip)
                            {
                                if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                                {
                                    ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                    iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                    iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                    
                                    Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                    Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                    
                                    SXD->GhostNode_Count++;
                                }
                                //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                    
                                    Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                    if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }
                        }
                    }
                    
                    if (SXD->EdgePlane[j].Test_In && SXD->EdgePlane[k].Test_In)
                    {
                        A[0][0] = SXD->Shape->Face[i].Normal[0]; A[0][1] = SXD->Shape->Face[i].Normal[1]; A[0][2] = SXD->Shape->Face[i].Normal[2];
                        A[1][0] = SXD->EdgePlane[j].Plane_In[0]; A[1][1] = SXD->EdgePlane[j].Plane_In[1]; A[1][2] = SXD->EdgePlane[j].Plane_In[2];
                        A[2][0] = SXD->EdgePlane[k].Plane_In[0]; A[2][1] = SXD->EdgePlane[k].Plane_In[1]; A[2][2] = SXD->EdgePlane[k].Plane_In[2];
                        
                        if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
                        {
                            SXD->GhostNode[SXD->GhostNode_Count].x[0] = A[0][0] * SXD->Shape->Face[i].Normal[3] + A[0][1] * SXD->EdgePlane[j].Plane_In[3] + A[0][2] * SXD->EdgePlane[k].Plane_In[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[1] = A[1][0] * SXD->Shape->Face[i].Normal[3] + A[1][1] * SXD->EdgePlane[j].Plane_In[3] + A[1][2] * SXD->EdgePlane[k].Plane_In[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[2] = A[2][0] * SXD->Shape->Face[i].Normal[3] + A[2][1] * SXD->EdgePlane[j].Plane_In[3] + A[2][2] * SXD->EdgePlane[k].Plane_In[3];
                            
                            Skip = false;
                            for (l = 0; l < SXD->Shape->Face_Count; l++) {
                                if (l != i) {
                                    if      (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->Shape->Face[l].Normal[0] * SXD->GhostNode[SXD->GhostNode_Count].x[0] +
                                             SXD->Shape->Face[l].Normal[1] * SXD->GhostNode[SXD->GhostNode_Count].x[1] +
                                             SXD->Shape->Face[l].Normal[2] * SXD->GhostNode[SXD->GhostNode_Count].x[2] -
                                             SXD->Shape->Face[l].Normal[3] > FLT_EPSILON)                      { Skip = true; break; }
                                }
                            }
                            if (!Skip)
                            {
                                if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                                {
                                    ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                    iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                    iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                    
                                    Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                    Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                    
                                    SXD->GhostNode_Count++;
                                }
                                //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                    
                                    Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                    if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }
                        }
                    }
                    
                    if (SXD->EdgePlane[j].Test_Ou && SXD->EdgePlane[k].Test_Ou)
                    {
                        A[0][0] = SXD->Shape->Face[i].Normal[0]; A[0][1] = SXD->Shape->Face[i].Normal[1]; A[0][2] = SXD->Shape->Face[i].Normal[2];
                        A[1][0] = SXD->EdgePlane[j].Plane_Ou[0]; A[1][1] = SXD->EdgePlane[j].Plane_Ou[1]; A[1][2] = SXD->EdgePlane[j].Plane_Ou[2];
                        A[2][0] = SXD->EdgePlane[k].Plane_Ou[0]; A[2][1] = SXD->EdgePlane[k].Plane_Ou[1]; A[2][2] = SXD->EdgePlane[k].Plane_Ou[2];
                        
                        if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
                        {
                            SXD->GhostNode[SXD->GhostNode_Count].x[0] = A[0][0] * SXD->Shape->Face[i].Normal[3] + A[0][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[0][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[1] = A[1][0] * SXD->Shape->Face[i].Normal[3] + A[1][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[1][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            SXD->GhostNode[SXD->GhostNode_Count].x[2] = A[2][0] * SXD->Shape->Face[i].Normal[3] + A[2][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[2][2] * SXD->EdgePlane[k].Plane_Ou[3];
                            
                            Skip = false;
                            for (l = 0; l < SXD->Shape->Face_Count; l++) {
                                if (l != i) {
                                    if      (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->Shape->Face[l].Normal[0] * SXD->GhostNode[SXD->GhostNode_Count].x[0] +
                                             SXD->Shape->Face[l].Normal[1] * SXD->GhostNode[SXD->GhostNode_Count].x[1] +
                                             SXD->Shape->Face[l].Normal[2] * SXD->GhostNode[SXD->GhostNode_Count].x[2] -
                                             SXD->Shape->Face[l].Normal[3] > FLT_EPSILON)                      { Skip = true; break; }
                                }
                            }
                            if (!Skip)
                            {
                                if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                                {
                                    ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                    iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                    iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                    
                                    Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                    Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                    
                                    SXD->GhostNode_Count++;
                                }
                                //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                    
                                    Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                    if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // Beam_In/Ou and Beam_In/Ou and Beam_In/Ou
    for (i = 0; i < SXD->Shape->Edge_Count; i++)
    {
        for (j = 0; j < SXD->Shape->Edge_Count; j++)
        {
            if (i != j)
            {
                if (SXD->EdgePlane[i].Test_In && SXD->EdgePlane[j].Test_Ou)
                {
                    for (k = 0; k < SXD->Shape->Edge_Count; k++)
                    {
                        if ((i != k) && (SXD->EdgePlane[k].Test_In))
                        {
                            A[0][0] = SXD->EdgePlane[i].Plane_In[0]; A[0][1] = SXD->EdgePlane[i].Plane_In[1]; A[0][2] = SXD->EdgePlane[i].Plane_In[2];
                            A[1][0] = SXD->EdgePlane[j].Plane_Ou[0]; A[1][1] = SXD->EdgePlane[j].Plane_Ou[1]; A[1][2] = SXD->EdgePlane[j].Plane_Ou[2];
                            A[2][0] = SXD->EdgePlane[k].Plane_In[0]; A[2][1] = SXD->EdgePlane[k].Plane_In[1]; A[2][2] = SXD->EdgePlane[k].Plane_In[2];
                            
                            if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
                            {
                                SXD->GhostNode[SXD->GhostNode_Count].x[0] = A[0][0] * SXD->EdgePlane[i].Plane_In[3] + A[0][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[0][2] * SXD->EdgePlane[k].Plane_In[3];
                                SXD->GhostNode[SXD->GhostNode_Count].x[1] = A[1][0] * SXD->EdgePlane[i].Plane_In[3] + A[1][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[1][2] * SXD->EdgePlane[k].Plane_In[3];
                                SXD->GhostNode[SXD->GhostNode_Count].x[2] = A[2][0] * SXD->EdgePlane[i].Plane_In[3] + A[2][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[2][2] * SXD->EdgePlane[k].Plane_In[3];
                                
                                Skip = false;
                                for (l = 0; l < SXD->Shape->Face_Count; l++) {
                                    if      (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->Shape->Face[l].Normal[0] * SXD->GhostNode[SXD->GhostNode_Count].x[0] +
                                             SXD->Shape->Face[l].Normal[1] * SXD->GhostNode[SXD->GhostNode_Count].x[1] +
                                             SXD->Shape->Face[l].Normal[2] * SXD->GhostNode[SXD->GhostNode_Count].x[2] -
                                             SXD->Shape->Face[l].Normal[3] > FLT_EPSILON)                      { Skip = true; break; }
                                }
                                if (!Skip)
                                {
                                    if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                                    {
                                        ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                        iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                        iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                        
                                        Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                        Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                        
                                        SXD->GhostNode_Count++;
                                    }
                                    //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                    if (SXD->GhostNode_Count >= Ghost_Allocated)
                                    {
                                        Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                        
                                        Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                        if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                    }
                                }
                            }
                        }
                        if ((j != k) && (SXD->EdgePlane[k].Test_Ou))
                        {
                            A[0][0] = SXD->EdgePlane[i].Plane_In[0]; A[0][1] = SXD->EdgePlane[i].Plane_In[1]; A[0][2] = SXD->EdgePlane[i].Plane_In[2];
                            A[1][0] = SXD->EdgePlane[j].Plane_Ou[0]; A[1][1] = SXD->EdgePlane[j].Plane_Ou[1]; A[1][2] = SXD->EdgePlane[j].Plane_Ou[2];
                            A[2][0] = SXD->EdgePlane[k].Plane_Ou[0]; A[2][1] = SXD->EdgePlane[k].Plane_Ou[1]; A[2][2] = SXD->EdgePlane[k].Plane_Ou[2];
                            
                            if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
                            {
                                SXD->GhostNode[SXD->GhostNode_Count].x[0] = A[0][0] * SXD->EdgePlane[i].Plane_In[3] + A[0][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[0][2] * SXD->EdgePlane[k].Plane_Ou[3];
                                SXD->GhostNode[SXD->GhostNode_Count].x[1] = A[1][0] * SXD->EdgePlane[i].Plane_In[3] + A[1][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[1][2] * SXD->EdgePlane[k].Plane_Ou[3];
                                SXD->GhostNode[SXD->GhostNode_Count].x[2] = A[2][0] * SXD->EdgePlane[i].Plane_In[3] + A[2][1] * SXD->EdgePlane[j].Plane_Ou[3] + A[2][2] * SXD->EdgePlane[k].Plane_Ou[3];
                                
                                Skip = false;
                                for (l = 0; l < SXD->Shape->Face_Count; l++) {
                                    if      (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[0] - xl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[1] - yl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[0] < -FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->GhostNode[SXD->GhostNode_Count].x[2] - zl[1] > +FLT_EPSILON) { Skip = true; break; }
                                    else if (SXD->Shape->Face[l].Normal[0] * SXD->GhostNode[SXD->GhostNode_Count].x[0] +
                                             SXD->Shape->Face[l].Normal[1] * SXD->GhostNode[SXD->GhostNode_Count].x[1] +
                                             SXD->Shape->Face[l].Normal[2] * SXD->GhostNode[SXD->GhostNode_Count].x[2] -
                                             SXD->Shape->Face[l].Normal[3] > FLT_EPSILON)                      { Skip = true; break; }
                                }
                                if (!Skip)
                                {
                                    if (CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range, &Node_Sorting))
                                    {
                                        ix = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[0] - Node_Sorting.xl[0]) * Node_Sorting.Idx);
                                        iy = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[1] - Node_Sorting.yl[0]) * Node_Sorting.Idy);
                                        iz = (long)floor((SXD->GhostNode[SXD->GhostNode_Count].x[2] - Node_Sorting.zl[0]) * Node_Sorting.Idz);
                                        
                                        Node_Sorting.NextId_Ghost[SXD->GhostNode_Count] = Node_Sorting.EntryId[ix][iy][iz][1];
                                        Node_Sorting.EntryId[ix][iy][iz][1] = SXD->GhostNode_Count;
                                        
                                        SXD->GhostNode_Count++;
                                    }
                                    //SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                    if (SXD->GhostNode_Count >= Ghost_Allocated)
                                    {
                                        Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                        
                                        Node_Sorting.NextId_Ghost = (long*)realloc(Node_Sorting.NextId_Ghost, Ghost_Allocated* sizeof(long));
                                        if (_APP_WARNING_MESSAGE(Node_Sorting.NextId_Ghost, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"Node_Sorting.NextId_Ghost\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    free(Node_Sorting.NextId_Shape);
    free(Node_Sorting.NextId_Ghost);
    
    if (SXD->GhostNode_Count > 0)
    {
        //printf("__ %ld __\n\n", SXD->GhostNode_Count);
        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, SXD->GhostNode_Count * sizeof(struct SXD_Node));
        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", SXD->GhostNode_Count, 0)) { _USER_IO_WAIT(); return 1; }
    }
    else
    {
        free(SXD->GhostNode);
        SXD->GhostNode = NULL;
    }
    
    //end = clock();
    //printf("\n\nTime: %lf\n\n",(double)(end - begin));

    return 0;
}
int  CrystalAbsorption_SXD_Mesh_Get_GhostNode_Mixed(struct SXD_Mesh* SXD, double Range)
{
    long i, j, k;
    
    double_3 dx;
    
    double_3 Edge, Origin;
    
    long   Ghost_Allocated = SXD->Shape->Edge_Count;
    
    if (Range < FLT_EPSILON) Range = FLT_EPSILON;
    
    SXD->GhostNode = (struct SXD_Node*)calloc(Ghost_Allocated, sizeof(struct SXD_Node));
    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->GhostNode_Count = 0;
    
    //clock_t begin, end;
    //begin = clock();
    
    // Shape->Edge with EdgePlane->Beam_In/Ou
    for (i = 0; i < SXD->Shape->Edge_Count; i++)
    {
        for (j = 0; j < SXD->Shape->Edge_Count; j++)
        {
            if (i != j)
            {
                if (SXD->EdgePlane[j].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_In[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
                
                if (SXD->EdgePlane[j].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_Ou[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
            }
            // else the Plane_In/Ou were generated from the edge itself 
        }
    }
    
    // Edge from EdgePlane->Beam_In/Ou with Shape->Face and EdgePlane
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_In) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_In) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_In[0], &SXD->EdgePlane[j].Plane_In[0]))
        {
            for (k = 0; k < SXD->Shape->Face_Count; k++)
            {
                if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                {
                    SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                    if (SXD->GhostNode_Count >= Ghost_Allocated)
                    {
                        Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                    }
                }}
            }
            for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
            {
                if (SXD->EdgePlane[k].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }}
                }
                if (SXD->EdgePlane[k].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }}
                }
            }}
        }
    }}}}}
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_In) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_Ou) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_In[0], &SXD->EdgePlane[j].Plane_Ou[0]))
        {
            for (k = 0; k < SXD->Shape->Face_Count; k++)
            {
                if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                {
                    SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                    if (SXD->GhostNode_Count >= Ghost_Allocated)
                    {
                        Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                    }
                }}
            }
            for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
            {
                if (SXD->EdgePlane[k].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }}
                }
                if (SXD->EdgePlane[k].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }
                }}
            }}
        }
    }}}}}
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_Ou) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_Ou) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_Ou[0], &SXD->EdgePlane[j].Plane_Ou[0]))
        {
            for (k = 0; k < SXD->Shape->Face_Count; k++)
            {
                if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                {
                    SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                    if (SXD->GhostNode_Count >= Ghost_Allocated)
                    {
                        Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                    }
                }}
            }
            for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
            {
                if (SXD->EdgePlane[k].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }}
                }
                if (SXD->EdgePlane[k].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge[0], &Origin[0])) { if(Mesh_TestSite(SXD->Shape, &SXD->GhostNode[SXD->GhostNode_Count].x[0]))
                    {
                        SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                        if (SXD->GhostNode_Count >= Ghost_Allocated)
                        {
                            Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                            SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                            if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                        }
                    }}
                }
            }}
        }
    }}}}}
    
    if (SXD->GhostNode_Count > 0)
    {
        //printf("__ %ld __\n\n", SXD->GhostNode_Count);
        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, SXD->GhostNode_Count * sizeof(struct SXD_Node));
        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", SXD->GhostNode_Count, 0)) { _USER_IO_WAIT(); return 1; }
    }
    else
    {
        free(SXD->GhostNode);
        SXD->GhostNode = NULL;
    }
    
    //end = clock();
    //printf("\n\nTime: %lf\n\n",(double)(end - begin));

    return 0;
}
int  CrystalAbsorption_SXD_Mesh_Get_GhostNode_Edge(struct SXD_Mesh* SXD, double Range)
{
    long i, j, k;
    
    double_3 dx;
    
    double_3 Edge, Edge_I, Origin;
    double Distance, Shift;
    
    long   Ghost_Allocated = SXD->Shape->Edge_Count;
    
    if (Range < FLT_EPSILON) Range = FLT_EPSILON;
    
    SXD->GhostNode = (struct SXD_Node*)calloc(Ghost_Allocated, sizeof(struct SXD_Node));
    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->GhostNode_Count = 0;
    
    //clock_t begin, end;
    //begin = clock();
    
    // Shape->Edge with EdgePlane->Beam_In/Ou
    for (i = 0; i < SXD->Shape->Edge_Count; i++)
    {
        for (j = 0; j < SXD->Shape->Edge_Count; j++)
        {
            if (i != j)
            {
                if (SXD->EdgePlane[j].Test_In)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_In[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
                
                if (SXD->EdgePlane[j].Test_Ou)
                {
                    if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[j].Plane_Ou[0], &SXD->Shape->Edge[i].Direction[0], &SXD->Shape->Edge[i].Center[0]))
                    {
                        dx[0] = SXD->GhostNode[SXD->GhostNode_Count].x[0] - SXD->Shape->Edge[i].Center[0];
                        dx[1] = SXD->GhostNode[SXD->GhostNode_Count].x[1] - SXD->Shape->Edge[i].Center[1];
                        dx[2] = SXD->GhostNode[SXD->GhostNode_Count].x[2] - SXD->Shape->Edge[i].Center[2];
                    
                        if (4.0 * (dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2]) - SXD->Shape->Edge[i].Length * SXD->Shape->Edge[i].Length < -FLT_EPSILON) // Ghost node within the edge length, not coincident to edge extreme nodes
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // += SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }
                    }
                }
            }
            // else the Plane_In/Ou were generated from the edge itself 
        }
    }
    
    // Edge from EdgePlane->Beam_In/Ou with Shape->Face and EdgePlane
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_In) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_In) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_In[0], &SXD->EdgePlane[j].Plane_In[0]))
        {
            if (Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]))
            {
                Edge_I[0] = Edge[0]; Edge[0] = -Edge_I[0];
                Edge_I[1] = Edge[1]; Edge[1] = -Edge_I[1];
                Edge_I[2] = Edge[2]; Edge[2] = -Edge_I[2];
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]);
            }
            else
            {
                Edge_I[0] = -Edge[0];
                Edge_I[1] = -Edge[1];
                Edge_I[2] = -Edge[2];
            }
            Origin[0] += Distance * Edge[0];
            Origin[1] += Distance * Edge[1];
            Origin[2] += Distance * Edge[2];
            
            if (Mesh_TestSite(SXD->Shape, &Origin[0]))
            {
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge_I[0]);
                
                if (Distance > FLT_EPSILON)
                {
                    for (k = 0; k < SXD->Shape->Face_Count; k++)
                    {
                        if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }}
                    }
                    for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
                    {
                        if (SXD->EdgePlane[k].Test_In)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                        if (SXD->EdgePlane[k].Test_Ou)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                    }}
                }
            }
        }
    }}}}}
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_In) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_Ou) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_In[0], &SXD->EdgePlane[j].Plane_Ou[0]))
        {
            if (Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]))
            {
                Edge_I[0] = Edge[0]; Edge[0] = -Edge_I[0];
                Edge_I[1] = Edge[1]; Edge[1] = -Edge_I[1];
                Edge_I[2] = Edge[2]; Edge[2] = -Edge_I[2];
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]);
            }
            else
            {
                Edge_I[0] = -Edge[0];
                Edge_I[1] = -Edge[1];
                Edge_I[2] = -Edge[2];
            }
            Origin[0] += Distance * Edge[0];
            Origin[1] += Distance * Edge[1];
            Origin[2] += Distance * Edge[2];
            
            if (Mesh_TestSite(SXD->Shape, &Origin[0]))
            {
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge_I[0]);
                
                if (Distance > FLT_EPSILON)
                {
                    for (k = 0; k < SXD->Shape->Face_Count; k++)
                    {
                        if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }}
                    }
                    for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
                    {
                        if (SXD->EdgePlane[k].Test_In)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                        if (SXD->EdgePlane[k].Test_Ou)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                    }}
                }
            }
        }
    }}}}}
    for (i = 0; i < SXD->Shape->Edge_Count; i++) {               if (SXD->EdgePlane[i].Test_Ou) {
    for (j = 0; j < SXD->Shape->Edge_Count; j++) { if (j != i) { if (SXD->EdgePlane[j].Test_Ou) {
        if (!Get_IntersectionEdge_PlanePlane(&Edge[0], &Origin[0], &SXD->EdgePlane[i].Plane_Ou[0], &SXD->EdgePlane[j].Plane_Ou[0]))
        {
            if (Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]))
            {
                Edge_I[0] = Edge[0]; Edge[0] = -Edge_I[0];
                Edge_I[1] = Edge[1]; Edge[1] = -Edge_I[1];
                Edge_I[2] = Edge[2]; Edge[2] = -Edge_I[2];
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge[0]);
            }
            else
            {
                Edge_I[0] = -Edge[0];
                Edge_I[1] = -Edge[1];
                Edge_I[2] = -Edge[2];
            }
            Origin[0] += Distance * Edge[0];
            Origin[1] += Distance * Edge[1];
            Origin[2] += Distance * Edge[2];
            
            if (Mesh_TestSite(SXD->Shape, &Origin[0]))
            {
                Mesh_GetLeastPathLength(SXD->Shape, &Distance, &Origin[0], &Edge_I[0]);
                
                if (Distance > FLT_EPSILON)
                {
                    for (k = 0; k < SXD->Shape->Face_Count; k++)
                    {
                        if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->Shape->Face[k].Normal[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                        {
                            SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                            if (SXD->GhostNode_Count >= Ghost_Allocated)
                            {
                                Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                            }
                        }}
                    }
                    for (k = 0; k < SXD->Shape->Edge_Count; k++) { if ((k != i) && (k != j))
                    {
                        if (SXD->EdgePlane[k].Test_In)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_In[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                        if (SXD->EdgePlane[k].Test_Ou)
                        {
                            if (!Get_IntersectionNode_PlaneEdge(&SXD->GhostNode[SXD->GhostNode_Count].x[0], &SXD->EdgePlane[k].Plane_Ou[0], &Edge_I[0], &Origin[0], &Shift)) { if ((-FLT_EPSILON < Shift) && (Shift - Distance < FLT_EPSILON))
                            {
                                SXD->GhostNode_Count += CrystalAbsorption_SXD_Mesh_TestOverlap_LastGhostNode(SXD, Range);
                                if (SXD->GhostNode_Count >= Ghost_Allocated)
                                {
                                    Ghost_Allocated *= 2; // SXD->Shape->Edge_Count;
                                    SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, Ghost_Allocated * sizeof(struct SXD_Node));
                                    if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", Ghost_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
                                }
                            }}
                        }
                    }}
                }
            }
        }
    }}}}}
    
    if (SXD->GhostNode_Count > 0)
    {
        //printf("__ %ld __\n\n", SXD->GhostNode_Count);
        SXD->GhostNode = (struct SXD_Node*)realloc(SXD->GhostNode, SXD->GhostNode_Count * sizeof(struct SXD_Node));
        if (_APP_WARNING_MESSAGE(SXD->GhostNode, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_GhostNode", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->GhostNode\" to", SXD->GhostNode_Count, 0)) { _USER_IO_WAIT(); return 1; }
    }
    else
    {
        free(SXD->GhostNode);
        SXD->GhostNode = NULL;
    }
    
    //end = clock();
    //printf("\n\nTime: %lf\n\n",(double)(end - begin));

    return 0;
}

int  CrystalAbsorption_SXD_Mesh_Get_BeamPathG(struct SXD_Mesh* SXD)
{
    long i;
    
    SXD->Node_ab = (struct SXD_Scattering*)calloc(SXD->Shape->Node_Count, sizeof(struct SXD_Scattering));
    if (_APP_WARNING_MESSAGE(SXD->Node_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_BeamPath", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->Node_ab\" to", SXD->Shape->Node_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    for (i = 0; i < SXD->Shape->Node_Count; i++)
    {
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->Node_ab[i].Path[1], &SXD->Shape->Node[i].x[0], &SXD->Beam_II[0]);
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->Node_ab[i].Path[2], &SXD->Shape->Node[i].x[0], &SXD->Beam_Ou[0]);
        
        SXD->Node_ab[i].Path[0] = SXD->Node_ab[i].Path[1] + SXD->Node_ab[i].Path[2];
        SXD->Node_ab[i].g = SXD->Mu * SXD->Node_ab[i].Path[0];
    }
    
    for (i = 0; i < SXD->GhostNode_Count; i++)
    {
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->GhostNode[i].ab.Path[1], &SXD->GhostNode[i].x[0], &SXD->Beam_II[0]);
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->GhostNode[i].ab.Path[2], &SXD->GhostNode[i].x[0], &SXD->Beam_Ou[0]);
        
        SXD->GhostNode[i].ab.Path[0] = SXD->GhostNode[i].ab.Path[1] + SXD->GhostNode[i].ab.Path[2];
        SXD->GhostNode[i].ab.g = SXD->Mu * SXD->GhostNode[i].ab.Path[0];
    }
    
    return 0;
}


int  CrystalAbsorption_SXD_Mesh_Get_SubHowellSet(struct SXD_Mesh* SXD, double Range)
{
    long i, j, k, k0, l;
    
    long SubHowellSet_Allocated;
    
    long OutOfPlane;
    double Distance, Distance_2, Sign;
    
    bool Test;

    SubHowellSet_Allocated = 1 + SXD->BeamPlane_Count;
    
    SXD->SubHowellSet = (struct SXD_SubHowellSet*)calloc(SubHowellSet_Allocated, sizeof(struct SXD_SubHowellSet));
    if (_APP_WARNING_MESSAGE(SXD->SubHowellSet, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet\" to", SubHowellSet_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->SubHowellSet[0].Vertex_Count = SXD->Shape->Node_Count + SXD->GhostNode_Count;
    
    SXD->SubHowellSet[0].Vertex_x = (double**)calloc(SXD->SubHowellSet[0].Vertex_Count, sizeof(double*));
    SXD->SubHowellSet[0].Vertex_ab = (struct SXD_Scattering**)calloc(SXD->SubHowellSet[0].Vertex_Count, sizeof(struct SXD_Scattering*));
    if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[0].Vertex_x, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[0].Vertex_x\" to", SXD->SubHowellSet[0].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
    if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[0].Vertex_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[0].Vertex_ab\" to", SXD->SubHowellSet[0].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    SXD->SubHowellSet_Count = 1; j = 0;
    for (i = 0; i < SXD->Shape->Node_Count; i++) { SXD->SubHowellSet[0].Vertex_x[j] = &SXD->Shape->Node[i].x[0]; SXD->SubHowellSet[0].Vertex_ab[j] = &SXD->Node_ab[i];      j++; }
    for (i = 0; i < SXD->GhostNode_Count; i++)   { SXD->SubHowellSet[0].Vertex_x[j] = &SXD->GhostNode[i].x[0];   SXD->SubHowellSet[0].Vertex_ab[j] = &SXD->GhostNode[i].ab; j++; }
    
    for (i = 0; i < SXD->BeamPlane_Count; i++)
    {
        for (j = SXD->SubHowellSet_Count - 1; j >= 0; j--)
        {
            SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x = (double**)calloc(SXD->SubHowellSet[j].Vertex_Count, sizeof(double*));
            SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab = (struct SXD_Scattering**)calloc(SXD->SubHowellSet[j].Vertex_Count, sizeof(struct SXD_Scattering*));
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x\" to", SXD->SubHowellSet[j].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab\" to", SXD->SubHowellSet[j].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
            
            SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count = 0;
            
            k0 = SXD->SubHowellSet[j].Vertex_Count;
            Sign = +1.0;
            
            for (k = 0; k < SXD->SubHowellSet[j].Vertex_Count; k++)
            {
                Distance = SXD->BeamPlane[i][3] - (SXD->BeamPlane[i][0] * SXD->SubHowellSet[j].Vertex_x[k][0] +
                                                   SXD->BeamPlane[i][1] * SXD->SubHowellSet[j].Vertex_x[k][1] +
                                                   SXD->BeamPlane[i][2] * SXD->SubHowellSet[j].Vertex_x[k][2]);
                Distance_2 = Distance * Distance;
                if (Distance_2 > Range)
                {
                    if   (Distance > 0.0) { Sign = +1.0; k0 = k + 1; k = SXD->SubHowellSet[j].Vertex_Count; }
                    else                  { Sign = -1.0; k0 = k + 1; k = SXD->SubHowellSet[j].Vertex_Count; }
                }
                else
                {
                    Test = true; for (l = 0; l < SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count; l++) { if (SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x[l] == SXD->SubHowellSet[j].Vertex_x[k]) { Test = false; break; } }
                    if (Test)
                    {
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count] = SXD->SubHowellSet[j].Vertex_x[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count] = SXD->SubHowellSet[j].Vertex_ab[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count++;
                    }
                }
            }
            
            OutOfPlane = 0;
            for (k = k0; k < SXD->SubHowellSet[j].Vertex_Count; k++)
            {
                Distance = Sign * ( SXD->BeamPlane[i][3] - (SXD->BeamPlane[i][0] * SXD->SubHowellSet[j].Vertex_x[k][0] + 
                                                            SXD->BeamPlane[i][1] * SXD->SubHowellSet[j].Vertex_x[k][1] + 
                                                            SXD->BeamPlane[i][2] * SXD->SubHowellSet[j].Vertex_x[k][2]) );
                Distance_2 = Distance * Distance;
                if (Distance_2 > Range)
                {
                    if (Distance > 0.0) {}
                    else
                    {
                        OutOfPlane++;
                        
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count]  = SXD->SubHowellSet[j].Vertex_x[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count] = SXD->SubHowellSet[j].Vertex_ab[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count++;
                        
                        SXD->SubHowellSet[j].Vertex_Count--;
                        SXD->SubHowellSet[j].Vertex_x[k]  = SXD->SubHowellSet[j].Vertex_x[SXD->SubHowellSet[j].Vertex_Count];
                        SXD->SubHowellSet[j].Vertex_ab[k] = SXD->SubHowellSet[j].Vertex_ab[SXD->SubHowellSet[j].Vertex_Count];
                        k--;
                    }
                }
                else
                {
                    Test = true; for (l = 0; l < SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count; l++) { if (SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x[l] == SXD->SubHowellSet[j].Vertex_x[k]) { Test = false; break; } }
                    if (Test)
                    {
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count] = SXD->SubHowellSet[j].Vertex_x[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab[SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count] = SXD->SubHowellSet[j].Vertex_ab[k];
                        SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count++;
                    }
                }
            }
            
            if (OutOfPlane > 0)
            {
                SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x = (double**)realloc(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x, SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count * sizeof(double*));
                SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab = (struct SXD_Scattering**)realloc(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab, SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count * sizeof(struct SXD_Scattering*));
                if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x\" to", SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
                if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab\" to", SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
                
                SXD->SubHowellSet_Count++;
            }
            else
            {
                free(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_x);
                free(SXD->SubHowellSet[SXD->SubHowellSet_Count].Vertex_ab);
            }
            
            if (SXD->SubHowellSet_Count + 2 > SubHowellSet_Allocated)
            {
                SubHowellSet_Allocated += 1 + SXD->BeamPlane_Count;
                
                SXD->SubHowellSet = (struct SXD_SubHowellSet*)realloc(SXD->SubHowellSet, SubHowellSet_Allocated * sizeof(struct SXD_SubHowellSet));
                if (_APP_WARNING_MESSAGE(SXD->SubHowellSet, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->SubHowellSet\" to", SubHowellSet_Allocated, 0)) { _USER_IO_WAIT(); return 1; }
            }
        }
    }
    
    SXD->SubHowellSet = (struct SXD_SubHowellSet*)realloc(SXD->SubHowellSet, SXD->SubHowellSet_Count * sizeof(struct SXD_SubHowellSet));
    if (_APP_WARNING_MESSAGE(SXD->SubHowellSet, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->SubHowellSet\" to", SXD->SubHowellSet_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    for (i = 0; i < SXD->SubHowellSet_Count; i++)
    {
        SXD->SubHowellSet[i].Tetrahedron_Count = 0;
        SXD->SubHowellSet[i].Tetrahedron = NULL;
    }
    
    return 0;
}

int  CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet(struct SXD_Mesh* SXD, double Range)
{
    long i, j, k;
    long sh, bp, fp, vx;
    
    bool Remove_SubHowellSet;
    bool Remove_AllignedVertex;
    
    double_3 nn, xx, dd;
    double Distance, Distance_2;
    
    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++)
    {
        Remove_SubHowellSet = false;
        
        if (SXD->SubHowellSet[sh].Vertex_Count < 4) Remove_SubHowellSet = true;
        
        if(!Remove_SubHowellSet)// exclude vertex sets that belong to the same Beam or face plane
        {
            for (bp = 0; bp < SXD->BeamPlane_Count; bp++)
            {
                Remove_SubHowellSet = true;
                for (vx = 0; vx < SXD->SubHowellSet[sh].Vertex_Count; vx++)
                {
                    Distance = SXD->BeamPlane[bp][3] - (SXD->BeamPlane[bp][0] * SXD->SubHowellSet[sh].Vertex_x[vx][0] +
                                                        SXD->BeamPlane[bp][1] * SXD->SubHowellSet[sh].Vertex_x[vx][1] +
                                                        SXD->BeamPlane[bp][2] * SXD->SubHowellSet[sh].Vertex_x[vx][2]);
                    Distance_2 = Distance * Distance;
                    if (Distance_2 > Range) { Remove_SubHowellSet = false; break; }
                }
                if (Remove_SubHowellSet) break;
            }

            if (!Remove_SubHowellSet)
            {
                for (fp = 0; fp < SXD->Shape->Face_Count; fp++)
                {
                    Remove_SubHowellSet = true;
                    for (vx = 0; vx < SXD->SubHowellSet[sh].Vertex_Count; vx++)
                    {
                        Distance = SXD->Shape->Face[fp].Normal[3] - (SXD->Shape->Face[fp].Normal[0] * SXD->SubHowellSet[sh].Vertex_x[vx][0] +
                                                                     SXD->Shape->Face[fp].Normal[1] * SXD->SubHowellSet[sh].Vertex_x[vx][1] +
                                                                     SXD->Shape->Face[fp].Normal[2] * SXD->SubHowellSet[sh].Vertex_x[vx][2]);
                        Distance_2 = Distance * Distance;
                        if (Distance_2 > Range) { Remove_SubHowellSet = false; break; }
                    }
                    if (Remove_SubHowellSet) break;
                }
            }
        }
        
        if (!Remove_SubHowellSet)   // Exclude alligned triplet // remove mid vertex
        {
            Remove_AllignedVertex = false;
            
            for (i = 0; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
            {
                for (j = i + 1; j < SXD->SubHowellSet[sh].Vertex_Count; j++)
                {
                    nn[0] = SXD->SubHowellSet[sh].Vertex_x[j][0] - SXD->SubHowellSet[sh].Vertex_x[i][0];
                    nn[1] = SXD->SubHowellSet[sh].Vertex_x[j][1] - SXD->SubHowellSet[sh].Vertex_x[i][1];
                    nn[2] = SXD->SubHowellSet[sh].Vertex_x[j][2] - SXD->SubHowellSet[sh].Vertex_x[i][2];
                    
                    for (k = j + 1; k < SXD->SubHowellSet[sh].Vertex_Count; k++)
                    {
                        xx[0] = SXD->SubHowellSet[sh].Vertex_x[k][0] - SXD->SubHowellSet[sh].Vertex_x[i][0];
                        xx[1] = SXD->SubHowellSet[sh].Vertex_x[k][1] - SXD->SubHowellSet[sh].Vertex_x[i][1];
                        xx[2] = SXD->SubHowellSet[sh].Vertex_x[k][2] - SXD->SubHowellSet[sh].Vertex_x[i][2];

                        if(fabs(nn[0]*xx[1] - nn[1]*xx[0])<FLT_EPSILON) {
                        if(fabs(nn[0]*xx[2] - nn[2]*xx[0])<FLT_EPSILON) {
                        if(fabs(nn[1]*xx[2] - nn[2]*xx[1])<FLT_EPSILON) {
                            Remove_AllignedVertex = true;
                            
                            if (nn[0] * xx[0] + nn[1] * xx[1] + nn[2] * xx[2] < 0.0)
                            {
                                SXD->SubHowellSet[sh].Vertex_Count--;
                                
                                SXD->SubHowellSet[sh].Vertex_x[i] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Vertex_Count];
                                SXD->SubHowellSet[sh].Vertex_ab[i] = SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Vertex_Count];
                                
                                k = SXD->SubHowellSet[sh].Vertex_Count;
                                j = SXD->SubHowellSet[sh].Vertex_Count;
                                i--;
                            } 
                            else
                            {
                                dd[0] = nn[0] - xx[0];
                                dd[1] = nn[1] - xx[1];
                                dd[2] = nn[2] - xx[2];
                                if (nn[0] * dd[0] + nn[1] * dd[1] + nn[2] * dd[2] < 0.0)
                                {
                                    SXD->SubHowellSet[sh].Vertex_Count--;
                                    
                                    SXD->SubHowellSet[sh].Vertex_x[j] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Vertex_Count];
                                    SXD->SubHowellSet[sh].Vertex_ab[j] = SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Vertex_Count];
                                    
                                    k = SXD->SubHowellSet[sh].Vertex_Count;
                                    j--;
                                }
                                else
                                {
                                    SXD->SubHowellSet[sh].Vertex_Count--;
                                    
                                    SXD->SubHowellSet[sh].Vertex_x[k] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Vertex_Count];
                                    SXD->SubHowellSet[sh].Vertex_ab[k] = SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Vertex_Count];
                                    
                                    k--;
                                }
                            }
                        }}}
                    }
                }
            }
            if (Remove_AllignedVertex)
            {
                SXD->SubHowellSet[sh].Vertex_x = (double**)realloc(SXD->SubHowellSet[sh].Vertex_x, SXD->SubHowellSet[sh].Vertex_Count * sizeof(double*));
                SXD->SubHowellSet[sh].Vertex_ab = (struct SXD_Scattering**)realloc(SXD->SubHowellSet[sh].Vertex_ab, SXD->SubHowellSet[sh].Vertex_Count * sizeof(struct SXD_Scattering*));
                if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Vertex_x, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[sh].Vertex_x\" to", SXD->SubHowellSet[sh].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
                if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Vertex_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[sh].Vertex_ab\" to", SXD->SubHowellSet[sh].Vertex_Count, 0)) { _USER_IO_WAIT(); return 1; }
            }
        }
        
        if (!Remove_SubHowellSet)
        {
            xx[0] = SXD->SubHowellSet[sh].Vertex_x[1][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            xx[1] = SXD->SubHowellSet[sh].Vertex_x[1][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            xx[2] = SXD->SubHowellSet[sh].Vertex_x[1][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            dd[0] = SXD->SubHowellSet[sh].Vertex_x[2][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dd[1] = SXD->SubHowellSet[sh].Vertex_x[2][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dd[2] = SXD->SubHowellSet[sh].Vertex_x[2][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            nn[0] = xx[1] * dd[2] - xx[2] * dd[1];
            nn[1] = xx[2] * dd[0] - xx[0] * dd[2];
            nn[2] = xx[0] * dd[1] - xx[1] * dd[0];
            
            Remove_SubHowellSet = true;
            
            for (i = 3; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
            {
                xx[0] = SXD->SubHowellSet[sh].Vertex_x[i][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                xx[1] = SXD->SubHowellSet[sh].Vertex_x[i][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                xx[2] = SXD->SubHowellSet[sh].Vertex_x[i][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                
                if (sqrt(fabs(nn[0] * xx[0] + nn[1] * xx[1] + nn[2] * xx[2])) > FLT_EPSILON) { Remove_SubHowellSet = false; break; }
            }
        }
        
        if (Remove_SubHowellSet)
        {
            SXD->SubHowellSet_Count--;
            
            if (SXD->SubHowellSet[sh].Vertex_x != NULL)    free(SXD->SubHowellSet[sh].Vertex_x);
            if (SXD->SubHowellSet[sh].Vertex_ab != NULL)   free(SXD->SubHowellSet[sh].Vertex_ab);
            if (SXD->SubHowellSet[sh].Tetrahedron != NULL) free(SXD->SubHowellSet[sh].Tetrahedron);
            
            SXD->SubHowellSet[sh] = SXD->SubHowellSet[SXD->SubHowellSet_Count];
            
            SXD->SubHowellSet = (struct SXD_SubHowellSet*)realloc(SXD->SubHowellSet, SXD->SubHowellSet_Count * sizeof(struct SXD_SubHowellSet));
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->SubHowellSet\" to", SXD->SubHowellSet_Count, 0)) { _USER_IO_WAIT(); return 1; }
            
            sh--;
        }
    }
    
    return 0;
}

int  CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron(struct SXD_Mesh* SXD)
{
    long i, j, k;
    
    long sh, Tetrahedron_Count, Triangle_Count, Triangle_N, Triangle_Id, Triangle_Jd;
    long n_id[4];
    
    double Sign, Scalar, Norm, Norm_I;
    double dx[3], dy[3], dz[3], aa[3], bb[3], nn[4][4];
    double Distance, Distance_1, Distance_2;
    
    bool Exclude;
    
    long_4 *Tetrahedron_Face;
    
    struct Triangle
    {
        long   Id[3];
        double Side[4];
        bool   Skip;
    } *Triangle_List;
    
    double *swap_x;
    struct SXD_Scattering* swap_ab;
    
    Triangle_Count = 4;
    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++) { if(Triangle_Count < SXD->SubHowellSet[sh].Vertex_Count) Triangle_Count = SXD->SubHowellSet[sh].Vertex_Count; }
    
    Triangle_List = (struct Triangle*)calloc(Triangle_Count, sizeof(struct Triangle));
    if (_APP_WARNING_MESSAGE(Triangle_List, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_CALLOC, "\"Triangle_List\" to", Triangle_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    //Tetrahedron_Face_Count = Triangle_Count;
    
    Tetrahedron_Face = (long_4*)calloc(1, sizeof(long_4));
    if (_APP_WARNING_MESSAGE(Tetrahedron_Face, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_CALLOC, "\"Tetrahedron_Face\" to", 1, 0)) { _USER_IO_WAIT(); free(Triangle_List); return 1; }
    
    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++) { SXD->SubHowellSet[sh].Tetrahedron_Count = 0; SXD->SubHowellSet[sh].Tetrahedron = NULL;
    if (SXD->SubHowellSet[sh].Vertex_Count > 3)
    {
        dx[0] = SXD->SubHowellSet[sh].Vertex_x[1][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
        dx[1] = SXD->SubHowellSet[sh].Vertex_x[1][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
        dx[2] = SXD->SubHowellSet[sh].Vertex_x[1][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
        
        Exclude = false;
        
        Norm_I = 0.0;
        i = 1;
        while ((Norm_I < FLT_EPSILON) && (!Exclude))
        {
            i++;
            if (i < SXD->SubHowellSet[sh].Vertex_Count)
            {
                dy[0] = SXD->SubHowellSet[sh].Vertex_x[i][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                dy[1] = SXD->SubHowellSet[sh].Vertex_x[i][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                dy[2] = SXD->SubHowellSet[sh].Vertex_x[i][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                
                nn[0][0] = dx[1] * dy[2] - dx[2] * dy[1];
                nn[0][1] = dx[2] * dy[0] - dx[0] * dy[2];
                nn[0][2] = dx[0] * dy[1] - dx[1] * dy[0];

                Norm = 1.0 / (fabs(nn[0][0]) + fabs(nn[0][1]) + fabs(nn[0][2]));
                nn[0][0] *= Norm;
                nn[0][1] *= Norm;
                nn[0][2] *= Norm;

                for (j = 2; j < SXD->SubHowellSet[sh].Vertex_Count; j++) { if (j != i)
                {
                    dz[0] = SXD->SubHowellSet[sh].Vertex_x[j][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                    dz[1] = SXD->SubHowellSet[sh].Vertex_x[j][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                    dz[2] = SXD->SubHowellSet[sh].Vertex_x[j][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                    Norm_I = fabs(nn[0][0] * dz[0] + nn[0][1] * dz[1] + nn[0][2] * dz[2]);
                    if (Norm_I > FLT_EPSILON) break;
                }}
            }
            else Exclude = true;
        }
        
        if (!Exclude)
        {
            if (i > 2)
            {
                swap_x = SXD->SubHowellSet[sh].Vertex_x[2];
                swap_ab = SXD->SubHowellSet[sh].Vertex_ab[2];
                
                SXD->SubHowellSet[sh].Vertex_x[2] = SXD->SubHowellSet[sh].Vertex_x[i];
                SXD->SubHowellSet[sh].Vertex_ab[2] = SXD->SubHowellSet[sh].Vertex_ab[i];
                
                SXD->SubHowellSet[sh].Vertex_x[i] = swap_x;
                SXD->SubHowellSet[sh].Vertex_ab[i] = swap_ab;
            }
        }
        
        if (!Exclude)
        {
    // Search Tetrahedrons :: start
        if (SXD->SubHowellSet[sh].Vertex_Count == 4)
        {
            SXD->SubHowellSet[sh].Tetrahedron_Count = 1;
            
            SXD->SubHowellSet[sh].Tetrahedron = (struct SXD_Tetrahedron*)calloc(SXD->SubHowellSet[sh].Tetrahedron_Count, sizeof(struct SXD_Tetrahedron));
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Tetrahedron, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[sh].Tetrahedron\" to", SXD->SubHowellSet[sh].Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
            
            SXD->SubHowellSet[sh].Tetrahedron[0].Node_Id[0] = 0;
            SXD->SubHowellSet[sh].Tetrahedron[0].Node_Id[1] = 1;
            SXD->SubHowellSet[sh].Tetrahedron[0].Node_Id[2] = 2;
            SXD->SubHowellSet[sh].Tetrahedron[0].Node_Id[3] = 3;
            
            dx[0] = SXD->SubHowellSet[sh].Vertex_x[1][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dx[1] = SXD->SubHowellSet[sh].Vertex_x[1][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dx[2] = SXD->SubHowellSet[sh].Vertex_x[1][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            dy[0] = SXD->SubHowellSet[sh].Vertex_x[2][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dy[1] = SXD->SubHowellSet[sh].Vertex_x[2][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dy[2] = SXD->SubHowellSet[sh].Vertex_x[2][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            dz[0] = SXD->SubHowellSet[sh].Vertex_x[3][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dz[1] = SXD->SubHowellSet[sh].Vertex_x[3][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dz[2] = SXD->SubHowellSet[sh].Vertex_x[3][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            SXD->SubHowellSet[sh].Tetrahedron[0].Volume = fabs(dx[0] * (dy[1] * dz[2] - dy[2] * dz[1]) - dx[1] * (dy[0] * dz[2] - dy[2] * dz[0]) + dx[2] * (dy[0] * dz[1] - dy[1] * dz[0])) / 6.0;
        }
        else if (SXD->SubHowellSet[sh].Vertex_Count > 4)
        {
            Tetrahedron_Count = SXD->SubHowellSet[sh].Vertex_Count;
            
            SXD->SubHowellSet[sh].Tetrahedron = (struct SXD_Tetrahedron*)calloc(Tetrahedron_Count, sizeof(struct SXD_Tetrahedron));
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Tetrahedron, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->SubHowellSet[sh].Tetrahedron\" to", Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
            
            Tetrahedron_Face = (long_4*)realloc(Tetrahedron_Face, Tetrahedron_Count * sizeof(long_4));
            if (_APP_WARNING_MESSAGE(Tetrahedron_Face, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_REALLOC, "\"Tetrahedron_Face\" to", Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
            
            SXD->SubHowellSet[sh].Tetrahedron_Count = 0;
            
            // Search Tetrahedrons
            
            // Search first Triangle as first neighbors of first vertex
            
            dx[0] = SXD->SubHowellSet[sh].Vertex_x[1][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dx[1] = SXD->SubHowellSet[sh].Vertex_x[1][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dx[2] = SXD->SubHowellSet[sh].Vertex_x[1][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            Distance_1 = dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2];
            
            dz[0] = SXD->SubHowellSet[sh].Vertex_x[2][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
            dz[1] = SXD->SubHowellSet[sh].Vertex_x[2][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
            dz[2] = SXD->SubHowellSet[sh].Vertex_x[2][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
            Distance = dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2];
            
            SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0] = 0;
            if (Distance < Distance_1)
            {
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = 2;
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = 1;
                Distance_2 = Distance_1;
                Distance_1 = Distance;
                
                dy[0] = dx[0]; dx[0] = dz[0];
                dy[1] = dx[1]; dx[1] = dz[1];
                dy[2] = dx[2]; dx[2] = dz[2];
            }
            else
            {
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = 1;
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = 2;
                Distance_2 = Distance;
                
                dy[0] = dz[0];
                dy[1] = dz[1];
                dy[2] = dz[2];
            }
            
            for (i = 3; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
            {
                dz[0] = SXD->SubHowellSet[sh].Vertex_x[i][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                dz[1] = SXD->SubHowellSet[sh].Vertex_x[i][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                dz[2] = SXD->SubHowellSet[sh].Vertex_x[i][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                Distance = dz[0] * dz[0] + dz[1] * dz[1] + dz[2] * dz[2];
                
                if (Distance < Distance_1)
                {
                    SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
                    SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = i;
                    Distance_2 = Distance_1;
                    Distance_1 = Distance;
                    
                    dy[0] = dx[0]; dx[0] = dz[0];
                    dy[1] = dx[1]; dx[1] = dz[1];
                    dy[2] = dx[2]; dx[2] = dz[2];
                }
                else if (Distance < Distance_2)
                {
                    SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = i;
                    Distance_2 = Distance;
                    
                    dy[0] = dz[0];
                    dy[1] = dz[1];
                    dy[2] = dz[2];
                }
            }
            
            if ((fabs(dx[1] * dy[2] - dx[2] * dy[1]) < FLT_EPSILON) &&
                (fabs(dx[2] * dy[0] - dx[0] * dy[2]) < FLT_EPSILON) &&
                (fabs(dx[0] * dy[1] - dx[1] * dy[0]) < FLT_EPSILON))
            {
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = -1;
                for (i = 1; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
                {
                    if (i != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1])
                    {
                        dy[0] = SXD->SubHowellSet[sh].Vertex_x[i][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                        dy[1] = SXD->SubHowellSet[sh].Vertex_x[i][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                        dy[2] = SXD->SubHowellSet[sh].Vertex_x[i][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                        
                        if ((fabs(dx[1] * dy[2] - dx[2] * dy[1]) > FLT_EPSILON) ||
                            (fabs(dx[2] * dy[0] - dx[0] * dy[2]) > FLT_EPSILON) ||
                            (fabs(dx[0] * dy[1] - dx[1] * dy[0]) > FLT_EPSILON))
                        {
                            SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = i;
                            Distance_2 = dy[0] * dy[0] + dy[1] * dy[1] + dy[2] * dy[2];
                            break;
                        }
                    }
                }
                
                for (j = i+1; j < SXD->SubHowellSet[sh].Vertex_Count; j++)
                {
                    if (j != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1])
                    {
                        dz[0] = SXD->SubHowellSet[sh].Vertex_x[j][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                        dz[1] = SXD->SubHowellSet[sh].Vertex_x[j][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                        dz[2] = SXD->SubHowellSet[sh].Vertex_x[j][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                        
                        if ((fabs(dx[1] * dy[2] - dx[2] * dy[1]) > FLT_EPSILON) ||
                            (fabs(dx[2] * dy[0] - dx[0] * dy[2]) > FLT_EPSILON) ||
                            (fabs(dx[0] * dy[1] - dx[1] * dy[0]) > FLT_EPSILON))
                        {
                            Distance = dy[0] * dy[0] + dy[1] * dy[1] + dy[2] * dy[2];
                            if (Distance < Distance_2)
                            {
                                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = j;
                                Distance_2  = Distance;
                                
                                dy[0] = dz[0];
                                dy[1] = dz[1];
                                dy[2] = dz[2];
                            }
                        }
                    }
                }
            }
            
            if (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] < 0) {  _APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_VALUE, "Not alligned triplet not found!", 0, 0); _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
            
            // Search first tetrahedron
            
            nn[0][0] = dx[1] * dy[2] - dx[2] * dy[1];
            nn[0][1] = dx[2] * dy[0] - dx[0] * dy[2];
            nn[0][2] = dx[0] * dy[1] - dx[1] * dy[0];
            Norm = 1.0 / (fabs(nn[0][0]) + fabs(nn[0][1]) + fabs(nn[0][2])); nn[0][0] *= Norm; nn[0][1] *= Norm; nn[0][2] *= Norm;
            nn[0][3] = nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[0][0] + nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[0][1] + nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[0][2];
            
            bb[0] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][0] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][0];
            bb[1] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][1] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][1];
            bb[2] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][2] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][2];
            
            for (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = 1; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] < SXD->SubHowellSet[sh].Vertex_Count; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]++)
            {
                if ( // (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0]) && // implied by the loop starting from 1
                    (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]) &&
                    (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]))
                {
                    dz[0] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                    dz[1] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                    dz[2] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                    
                    //nn[0][0] = dx[1] * dy[2] - dx[2] * dy[1];
                    //nn[0][1] = dx[2] * dy[0] - dx[0] * dy[2];
                    //nn[0][2] = dx[0] * dy[1] - dx[1] * dy[0];
                    //if (nn[0][0] * dz[0] + nn[0][1] * dz[1] + nn[0][2] * dz[2] > 0.0) { nn[0][0] *= -1.0; nn[0][1] *= -1.0; nn[0][2] *= -1.0; nn[0][3] *= -1.0; }
                    //nn[0][3] = nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[0][0] + nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[0][1] + nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[0][2];
                    
                    Scalar = nn[0][0] * dz[0] + nn[0][1] * dz[1] + nn[0][2] * dz[2];
                    
                    if (fabs(Scalar) > FLT_EPSILON)
                    {
                        if (Scalar > 0.0) { nn[0][0] *= -1.0; nn[0][1] *= -1.0; nn[0][2] *= -1.0; nn[0][3] *= -1.0; }
                        
                        aa[0] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][0] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][0];
                        aa[1] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][1] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][1];
                        aa[2] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]][2] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][2];
                        
                        //bb[0] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][0] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][0];
                        //bb[1] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][1] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][1];
                        //bb[2] = SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]][2] - SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][2];
                        
                        nn[1][0] = dx[1] * dz[2] - dx[2] * dz[1];
                        nn[1][1] = dx[2] * dz[0] - dx[0] * dz[2];
                        nn[1][2] = dx[0] * dz[1] - dx[1] * dz[0];
                        Norm = 1.0 / (fabs(nn[1][0]) + fabs(nn[1][1]) + fabs(nn[1][2])); nn[1][0] *= Norm; nn[1][1] *= Norm; nn[1][2] *= Norm;
                        if (nn[1][0] * dy[0] + nn[1][1] * dy[1] + nn[1][2] * dy[2] > 0.0) { nn[1][0] *= -1.0; nn[1][1] *= -1.0; nn[1][2] *= -1.0; }
                        nn[1][3] = nn[1][0] * SXD->SubHowellSet[sh].Vertex_x[0][0] + nn[1][1] * SXD->SubHowellSet[sh].Vertex_x[0][1] + nn[1][2] * SXD->SubHowellSet[sh].Vertex_x[0][2];
                        
                        nn[2][0] = dy[1] * dz[2] - dy[2] * dz[1];
                        nn[2][1] = dy[2] * dz[0] - dy[0] * dz[2];
                        nn[2][2] = dy[0] * dz[1] - dy[1] * dz[0];
                        Norm = 1.0 / (fabs(nn[2][0]) + fabs(nn[2][1]) + fabs(nn[2][2])); nn[2][0] *= Norm; nn[2][1] *= Norm; nn[2][2] *= Norm;
                        if (nn[2][0] * dx[0] + nn[2][1] * dx[1] + nn[2][2] * dx[2] > 0.0) { nn[2][0] *= -1.0; nn[2][1] *= -1.0; nn[2][2] *= -1.0; }
                        nn[2][3] = nn[2][0] * SXD->SubHowellSet[sh].Vertex_x[0][0] + nn[2][1] * SXD->SubHowellSet[sh].Vertex_x[0][1] + nn[2][2] * SXD->SubHowellSet[sh].Vertex_x[0][2];
                        
                        nn[3][0] = aa[1] * bb[2] - aa[2] * bb[1];
                        nn[3][1] = aa[2] * bb[0] - aa[0] * bb[2];
                        nn[3][2] = aa[0] * bb[1] - aa[1] * bb[0];
                        Norm = 1.0 / (fabs(nn[3][0]) + fabs(nn[3][1]) + fabs(nn[3][2])); nn[3][0] *= Norm; nn[3][1] *= Norm; nn[3][2] *= Norm;
                        if (nn[3][0] * dx[0] + nn[3][1] * dx[1] + nn[3][2] * dx[2] < 0.0) { nn[3][0] *= -1.0; nn[3][1] *= -1.0; nn[3][2] *= -1.0; } // opposite disegual because dx is oriented toward the facet!
                        nn[3][3] = nn[3][0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][0] +
                                   nn[3][1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][1] +
                                   nn[3][2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]][2];
                        
                        Exclude = false;
                        for (i = 1; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
                        {
                            if ( // (i != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0]) && // implied by the loop starting from 1
                                (i != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]) &&
                                (i != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]) &&
                                (i != SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]))
                            {
                                //qq[0] = SXD->SubHowellSet[sh].Vertex_x[i][0] - SXD->SubHowellSet[sh].Vertex_x[0][0];
                                //qq[1] = SXD->SubHowellSet[sh].Vertex_x[i][1] - SXD->SubHowellSet[sh].Vertex_x[0][1];
                                //qq[2] = SXD->SubHowellSet[sh].Vertex_x[i][2] - SXD->SubHowellSet[sh].Vertex_x[0][2];
                                //
                                //if (nn[0][0] * qq[0] + nn[0][1] * qq[1] + nn[0][2] * qq[2] - nn[0][3] < -FLT_EPSILON) {
                                //if (nn[1][0] * qq[0] + nn[1][1] * qq[1] + nn[1][2] * qq[2] - nn[1][3] < -FLT_EPSILON) {
                                //if (nn[2][0] * qq[0] + nn[2][1] * qq[1] + nn[2][2] * qq[2] - nn[2][3] < -FLT_EPSILON) {
                                //if (nn[3][0] * qq[0] + nn[3][1] * qq[1] + nn[3][2] * qq[2] - nn[3][3] < -FLT_EPSILON) {
                                if (nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[i][2] - nn[0][3] < -FLT_EPSILON) {
                                if (nn[1][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[1][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[1][2] * SXD->SubHowellSet[sh].Vertex_x[i][2] - nn[1][3] < -FLT_EPSILON) {
                                if (nn[2][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[2][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[2][2] * SXD->SubHowellSet[sh].Vertex_x[i][2] - nn[2][3] < -FLT_EPSILON) {
                                if (nn[3][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[3][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[3][2] * SXD->SubHowellSet[sh].Vertex_x[i][2] - nn[3][3] < -FLT_EPSILON) {
                                    Exclude = true;
                                    break;
                                }}}}
                            }
                        }
                        
                        if (!Exclude) break;
                    }
                }
            }
            
            if (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] >= SXD->SubHowellSet[sh].Vertex_Count) { printf("__ %ld __", sh); _APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_VALUE, "First tetrahedron not found!", 0, 0); _USER_IO_WAIT(); continue; }
            
            SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Volume = fabs(dx[0] * (dy[1] * dz[2] - dy[2] * dz[1]) - dx[1] * (dy[0] * dz[2] - dy[2] * dz[0]) + dx[2] * (dy[0] * dz[1] - dy[1] * dz[0])) / 6.0;
            
            // The sorting of the Node_Id is consistent not equivalent to the sorting of the n_id !!!
            
            n_id[0] = 0;
            n_id[1] = 1;
            n_id[2] = 2;
            n_id[3] = 3;
            if (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] > SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2])
            {
                i = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = i;
                i = n_id[1];
                n_id[1] = n_id[2];
                n_id[2] = i;
            }
            if (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] > SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3])
            {
                i = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3];
                SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = i;
                i = n_id[0];
                n_id[0] = n_id[1];
                n_id[1] = i;
                if (SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] > SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2])
                {
                    i = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
                    SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
                    SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = i;
                    i = n_id[1];
                    n_id[1] = n_id[2];
                    n_id[2] = i;
                }
            }
            
            // Populate triangle list
            
            Triangle_N = 0;
            
            Triangle_List[Triangle_N].Id[0] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0];
            Triangle_List[Triangle_N].Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
            Triangle_List[Triangle_N].Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
            Triangle_List[Triangle_N].Side[0] = nn[n_id[0]][0];
            Triangle_List[Triangle_N].Side[1] = nn[n_id[0]][1];
            Triangle_List[Triangle_N].Side[2] = nn[n_id[0]][2];
            Triangle_List[Triangle_N].Side[3] = nn[n_id[0]][3];
            Triangle_List[Triangle_N].Skip = false;
            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][0] = Triangle_N;
            Triangle_N++;
            
            Triangle_List[Triangle_N].Id[0] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0];
            Triangle_List[Triangle_N].Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
            Triangle_List[Triangle_N].Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3];
            Triangle_List[Triangle_N].Side[0] = nn[n_id[1]][0];
            Triangle_List[Triangle_N].Side[1] = nn[n_id[1]][1];
            Triangle_List[Triangle_N].Side[2] = nn[n_id[1]][2];
            Triangle_List[Triangle_N].Side[3] = nn[n_id[1]][3];
            Triangle_List[Triangle_N].Skip = false;
            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][1] = Triangle_N;
            Triangle_N++;
            
            Triangle_List[Triangle_N].Id[0] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0];
            Triangle_List[Triangle_N].Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
            Triangle_List[Triangle_N].Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3];
            Triangle_List[Triangle_N].Side[0] = nn[n_id[2]][0];
            Triangle_List[Triangle_N].Side[1] = nn[n_id[2]][1];
            Triangle_List[Triangle_N].Side[2] = nn[n_id[2]][2];
            Triangle_List[Triangle_N].Side[3] = nn[n_id[2]][3];
            Triangle_List[Triangle_N].Skip = false;
            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][2] = Triangle_N;
            Triangle_N++;
            
            Triangle_List[Triangle_N].Id[0] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1];
            Triangle_List[Triangle_N].Id[1] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2];
            Triangle_List[Triangle_N].Id[2] = SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3];
            Triangle_List[Triangle_N].Side[0] = nn[n_id[3]][0];
            Triangle_List[Triangle_N].Side[1] = nn[n_id[3]][1];
            Triangle_List[Triangle_N].Side[2] = nn[n_id[3]][2];
            Triangle_List[Triangle_N].Side[3] = nn[n_id[3]][3];
            Triangle_List[Triangle_N].Skip = false;
            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][3] = Triangle_N;
            Triangle_N++;

            SXD->SubHowellSet[sh].Tetrahedron_Count++;
            
            // Systematic detection of other tetrahedrons
            
            for (Triangle_Id = 0; Triangle_Id < Triangle_N; Triangle_Id++)
            {
                if (!Triangle_List[Triangle_Id].Skip)
                {
                    Exclude = true;
                    
                    // Exclude non valid tetrahedron
                    for (i = 0; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
                    {
                        if ((i == Triangle_List[Triangle_Id].Id[0]) || (i == Triangle_List[Triangle_Id].Id[1]) || (i == Triangle_List[Triangle_Id].Id[2])) { } // exclude vertexes already picked by the base triangle
                        else if (Triangle_List[Triangle_Id].Side[0] * SXD->SubHowellSet[sh].Vertex_x[i][0] +
                                 Triangle_List[Triangle_Id].Side[1] * SXD->SubHowellSet[sh].Vertex_x[i][1] +
                                 Triangle_List[Triangle_Id].Side[2] * SXD->SubHowellSet[sh].Vertex_x[i][2] -
                                 Triangle_List[Triangle_Id].Side[3] > FLT_EPSILON) // consider only position forming a non null thetrahedron on the right side of the triangle facet
                        {
                            Exclude = false;
                            
                            if      (i < Triangle_List[Triangle_Id].Id[0]) { /* n_id[0] = 1; n_id[1] = 2; n_id[2] = 3; */ SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0] = i; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = Triangle_List[Triangle_Id].Id[0]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = Triangle_List[Triangle_Id].Id[1]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = Triangle_List[Triangle_Id].Id[2]; }
                            else if (i < Triangle_List[Triangle_Id].Id[1]) { /* n_id[0] = 0; n_id[1] = 2; n_id[2] = 3; */ SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0] = Triangle_List[Triangle_Id].Id[0]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = i; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = Triangle_List[Triangle_Id].Id[1]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = Triangle_List[Triangle_Id].Id[2]; }
                            else if (i < Triangle_List[Triangle_Id].Id[2]) { /* n_id[0] = 0; n_id[1] = 1; n_id[2] = 3; */ SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0] = Triangle_List[Triangle_Id].Id[0]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = Triangle_List[Triangle_Id].Id[1]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = i; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = Triangle_List[Triangle_Id].Id[2]; }
                            else                                           { /* n_id[0] = 0; n_id[1] = 1; n_id[2] = 2; */ SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0] = Triangle_List[Triangle_Id].Id[0]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1] = Triangle_List[Triangle_Id].Id[1]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2] = Triangle_List[Triangle_Id].Id[2]; SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3] = i; }
                            
                            // The same set of nodes should be excluded by Skip == false and the forth node on the open side of the base triangle
                            //for (j = 0; j < SXD->SubHowellSet[sh].Tetrahedron_Count; j++)
                            //{
                            //    if ((SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[0] == SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[0]) &&
                            //        (SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[1] == SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[1]) &&
                            //        (SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[2] == SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[2]) &&
                            //        (SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[3] == SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[3]))
                            //    {
                            //        Exclude = true;
                            //        break;
                            //    }
                            //}
                            
                            //if (!Exclude)
                            //{
                                dx[0] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[0]][0] - SXD->SubHowellSet[sh].Vertex_x[i][0];
                                dx[1] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[0]][1] - SXD->SubHowellSet[sh].Vertex_x[i][1];
                                dx[2] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[0]][2] - SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                dy[0] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[1]][0] - SXD->SubHowellSet[sh].Vertex_x[i][0];
                                dy[1] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[1]][1] - SXD->SubHowellSet[sh].Vertex_x[i][1];
                                dy[2] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[1]][2] - SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                dz[0] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[2]][0] - SXD->SubHowellSet[sh].Vertex_x[i][0];
                                dz[1] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[2]][1] - SXD->SubHowellSet[sh].Vertex_x[i][1];
                                dz[2] = SXD->SubHowellSet[sh].Vertex_x[Triangle_List[Triangle_Id].Id[2]][2] - SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                nn[0][0] = dx[1] * dy[2] - dx[2] * dy[1];
                                nn[0][1] = dx[2] * dy[0] - dx[0] * dy[2];
                                nn[0][2] = dx[0] * dy[1] - dx[1] * dy[0];
                                Norm = 1.0 / (fabs(nn[0][0]) + fabs(nn[0][1]) + fabs(nn[0][2])); nn[0][0] *= Norm; nn[0][1] *= Norm; nn[0][2] *= Norm;
                                if (nn[0][0] * dz[0] + nn[0][1] * dz[1] + nn[0][2] * dz[2] > 0.0) { nn[0][0] *= -1.0; nn[0][1] *= -1.0; nn[0][2] *= -1.0; }
                                nn[0][3] = nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                nn[1][0] = dx[1] * dz[2] - dx[2] * dz[1];
                                nn[1][1] = dx[2] * dz[0] - dx[0] * dz[2];
                                nn[1][2] = dx[0] * dz[1] - dx[1] * dz[0];
                                Norm = 1.0 / (fabs(nn[1][0]) + fabs(nn[1][1]) + fabs(nn[1][2])); nn[1][0] *= Norm; nn[1][1] *= Norm; nn[1][2] *= Norm;
                                if (nn[1][0] * dy[0] + nn[1][1] * dy[1] + nn[1][2] * dy[2] > 0.0) { nn[1][0] *= -1.0; nn[1][1] *= -1.0; nn[1][2] *= -1.0; }
                                nn[1][3] = nn[1][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[1][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[1][2] * SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                nn[2][0] = dy[1] * dz[2] - dy[2] * dz[1];
                                nn[2][1] = dy[2] * dz[0] - dy[0] * dz[2];
                                nn[2][2] = dy[0] * dz[1] - dy[1] * dz[0];
                                Norm = 1.0 / (fabs(nn[2][0]) + fabs(nn[2][1]) + fabs(nn[2][2])); nn[2][0] *= Norm; nn[2][1] *= Norm; nn[2][2] *= Norm;
                                if (nn[2][0] * dx[0] + nn[2][1] * dx[1] + nn[2][2] * dx[2] > 0.0) { nn[2][0] *= -1.0; nn[2][1] *= -1.0; nn[2][2] *= -1.0; }
                                nn[2][3] = nn[2][0] * SXD->SubHowellSet[sh].Vertex_x[i][0] + nn[2][1] * SXD->SubHowellSet[sh].Vertex_x[i][1] + nn[2][2] * SXD->SubHowellSet[sh].Vertex_x[i][2];
                                
                                // Exclude the tetrahedron if it contains any other SubHowells node
                                for (j = 0; j < SXD->SubHowellSet[sh].Vertex_Count; j++)
                                {
                                    if (((j != Triangle_List[Triangle_Id].Id[0]) || (j != Triangle_List[Triangle_Id].Id[1]) || (j != Triangle_List[Triangle_Id].Id[2])) && (j!=i)) {
                                    if (Triangle_List[Triangle_Id].Side[0] * SXD->SubHowellSet[sh].Vertex_x[j][0] + Triangle_List[Triangle_Id].Side[1] * SXD->SubHowellSet[sh].Vertex_x[j][1] + Triangle_List[Triangle_Id].Side[2] * SXD->SubHowellSet[sh].Vertex_x[j][2] - Triangle_List[Triangle_Id].Side[3] > FLT_EPSILON) {
                                    if (nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[j][0] + nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[j][1] + nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[j][2] - nn[0][3] < -FLT_EPSILON) {
                                    if (nn[1][0] * SXD->SubHowellSet[sh].Vertex_x[j][0] + nn[1][1] * SXD->SubHowellSet[sh].Vertex_x[j][1] + nn[1][2] * SXD->SubHowellSet[sh].Vertex_x[j][2] - nn[1][3] < -FLT_EPSILON) {
                                    if (nn[2][0] * SXD->SubHowellSet[sh].Vertex_x[j][0] + nn[2][1] * SXD->SubHowellSet[sh].Vertex_x[j][1] + nn[2][2] * SXD->SubHowellSet[sh].Vertex_x[j][2] - nn[2][3] < -FLT_EPSILON) {
                                        Exclude = true;
                                        break;
                                    }}}}}
                                }
                            //}
                            
                            // Exclude conpenetrating/intersecting tetrahedrons :: dijoint tetrahedrons require all the vertex of different polyhedrons being on opposite sides of one of the plane bounding any of the tetrahedrons
                            if (!Exclude)
                            {
                                for (j = 0; j < SXD->SubHowellSet[sh].Tetrahedron_Count; j++)
                                {
                                    // Exclude = false;
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (Triangle_List[Triangle_Id].Side[0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][0] +
                                            Triangle_List[Triangle_Id].Side[1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][1] +
                                            Triangle_List[Triangle_Id].Side[2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][2] -
                                            Triangle_List[Triangle_Id].Side[3] > FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (nn[0][0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][0] +
                                            nn[0][1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][1] +
                                            nn[0][2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][2] -
                                            nn[0][3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (nn[1][0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][0] +
                                            nn[1][1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][1] +
                                            nn[1][2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][2] -
                                            nn[1][3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (nn[2][0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][0] +
                                            nn[2][1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][1] +
                                            nn[2][2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[j].Node_Id[k]][2] -
                                            nn[2][3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    
                                    Exclude = false;
                                    if (Tetrahedron_Face[j][0] < 0) { Triangle_Jd = -Tetrahedron_Face[j][0] - 1; Sign = -1.0; }
                                    else                            { Triangle_Jd = +Tetrahedron_Face[j][0];     Sign = +1.0; }
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (Sign * Triangle_List[Triangle_Jd].Side[0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][0] +
                                            Sign * Triangle_List[Triangle_Jd].Side[1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][1] +
                                            Sign * Triangle_List[Triangle_Jd].Side[2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][2] -
                                            Sign * Triangle_List[Triangle_Jd].Side[3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    if (Tetrahedron_Face[j][1] < 0) { Triangle_Jd = -Tetrahedron_Face[j][1] - 1; Sign = -1.0; }
                                    else                            { Triangle_Jd = +Tetrahedron_Face[j][1];     Sign = +1.0; }
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (Sign * Triangle_List[Triangle_Jd].Side[0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][0] +
                                            Sign * Triangle_List[Triangle_Jd].Side[1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][1] +
                                            Sign * Triangle_List[Triangle_Jd].Side[2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][2] -
                                            Sign * Triangle_List[Triangle_Jd].Side[3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    if (Tetrahedron_Face[j][2] < 0) { Triangle_Jd = -Tetrahedron_Face[j][2] - 1; Sign = -1.0; }
                                    else                            { Triangle_Jd = +Tetrahedron_Face[j][2];     Sign = +1.0; }
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (Sign * Triangle_List[Triangle_Jd].Side[0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][0] +
                                            Sign * Triangle_List[Triangle_Jd].Side[1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][1] +
                                            Sign * Triangle_List[Triangle_Jd].Side[2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][2] -
                                            Sign * Triangle_List[Triangle_Jd].Side[3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (!Exclude) continue;
                                    Exclude = false;
                                    if (Tetrahedron_Face[j][3] < 0) { Triangle_Jd = -Tetrahedron_Face[j][3] - 1; Sign = -1.0; }
                                    else                            { Triangle_Jd = +Tetrahedron_Face[j][3];     Sign = +1.0; }
                                    for (k = 0; k < 4; k++)
                                    {
                                        if (Sign * Triangle_List[Triangle_Jd].Side[0] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][0] +
                                            Sign * Triangle_List[Triangle_Jd].Side[1] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][1] +
                                            Sign * Triangle_List[Triangle_Jd].Side[2] * SXD->SubHowellSet[sh].Vertex_x[SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Node_Id[k]][2] -
                                            Sign * Triangle_List[Triangle_Jd].Side[3] < -FLT_EPSILON)
                                        { Exclude=true; break; }
                                    }
                                    if (Exclude) break;
                                }
                            }
                            
                            if (!Exclude) break;
                        }
                    }
                    
                    // Store valid tetrahedron information
                    if (!Exclude)
                    {
                        Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][0] = -Triangle_Id -1;
                        Triangle_List[Triangle_Id].Skip = true;
                    
                        if (Triangle_N + 4 > Triangle_Count)
                        {
                            Triangle_Count += 4;
                            Triangle_List = (struct Triangle*)realloc(Triangle_List, Triangle_Count * sizeof(struct Triangle));
                            if (_APP_WARNING_MESSAGE(Triangle_List, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_REALLOC, "\"Triangle_List\" to", Triangle_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
                        }
                    
                        // Exclude = false;
                        if      (i < Triangle_List[Triangle_Id].Id[0]) { Triangle_List[Triangle_N].Id[0] = i; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[1]; }
                        else if (i < Triangle_List[Triangle_Id].Id[1]) { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[1] = i; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[1]; }
                        else                                           { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[1]; Triangle_List[Triangle_N].Id[2] = i; }
                        for (j = 0 ; j < Triangle_N; j++) { if (j != Triangle_Id) {
                            if ((Triangle_List[j].Id[0] == Triangle_List[Triangle_N].Id[0]) &&
                                (Triangle_List[j].Id[1] == Triangle_List[Triangle_N].Id[1]) &&
                                (Triangle_List[j].Id[2] == Triangle_List[Triangle_N].Id[2]))
                        { Triangle_List[j].Skip = true; Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][1] = -j -1; Exclude = true; break; } } }
                        if (!Exclude)
                        {
                            Triangle_List[Triangle_N].Side[0] = nn[0][0];
                            Triangle_List[Triangle_N].Side[1] = nn[0][1];
                            Triangle_List[Triangle_N].Side[2] = nn[0][2];
                            Triangle_List[Triangle_N].Side[3] = nn[0][3];
                            Triangle_List[Triangle_N].Skip = false;
                            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][1] = Triangle_N;
                            Triangle_N++;
                        }
                        Exclude = false;
                        if      (i < Triangle_List[Triangle_Id].Id[0]) { Triangle_List[Triangle_N].Id[0] = i; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[2]; }
                        else if (i < Triangle_List[Triangle_Id].Id[2]) { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[1] = i; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[2]; }
                        else                                           { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[0]; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[2]; Triangle_List[Triangle_N].Id[2] = i; }
                        for (j = 0 ; j < Triangle_N; j++) { if (j != Triangle_Id) {
                            if ((Triangle_List[j].Id[0] == Triangle_List[Triangle_N].Id[0]) &&
                                (Triangle_List[j].Id[1] == Triangle_List[Triangle_N].Id[1]) &&
                                (Triangle_List[j].Id[2] == Triangle_List[Triangle_N].Id[2]))
                        { Triangle_List[j].Skip = true; Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][2] = -j -1; Exclude = true; break; } } }
                        if (!Exclude)
                        {
                            Triangle_List[Triangle_N].Side[0] = nn[1][0];
                            Triangle_List[Triangle_N].Side[1] = nn[1][1];
                            Triangle_List[Triangle_N].Side[2] = nn[1][2];
                            Triangle_List[Triangle_N].Side[3] = nn[1][3];
                            Triangle_List[Triangle_N].Skip = false;
                            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][2] = Triangle_N;
                            Triangle_N++;
                        }
                        Exclude = false;
                        if      (i < Triangle_List[Triangle_Id].Id[1]) { Triangle_List[Triangle_N].Id[0] = i; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[1]; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[2]; }
                        else if (i < Triangle_List[Triangle_Id].Id[2]) { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[1]; Triangle_List[Triangle_N].Id[1] = i; Triangle_List[Triangle_N].Id[2] = Triangle_List[Triangle_Id].Id[2]; }
                        else                                           { Triangle_List[Triangle_N].Id[0] = Triangle_List[Triangle_Id].Id[1]; Triangle_List[Triangle_N].Id[1] = Triangle_List[Triangle_Id].Id[2]; Triangle_List[Triangle_N].Id[2] = i; }
                        for (j = 0 ; j < Triangle_N; j++) { if (j != Triangle_Id) {
                            if ((Triangle_List[j].Id[0] == Triangle_List[Triangle_N].Id[0]) &&
                                (Triangle_List[j].Id[1] == Triangle_List[Triangle_N].Id[1]) &&
                                (Triangle_List[j].Id[2] == Triangle_List[Triangle_N].Id[2]))
                        { Triangle_List[j].Skip = true; Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][3] = -j -1; Exclude = true; break; } } }
                        if (!Exclude)
                        {
                            Triangle_List[Triangle_N].Side[0] = nn[2][0];
                            Triangle_List[Triangle_N].Side[1] = nn[2][1];
                            Triangle_List[Triangle_N].Side[2] = nn[2][2];
                            Triangle_List[Triangle_N].Side[3] = nn[2][3];
                            Triangle_List[Triangle_N].Skip = false;
                            Tetrahedron_Face[SXD->SubHowellSet[sh].Tetrahedron_Count][3] = Triangle_N;
                            Triangle_N++;
                        }
                        
                        SXD->SubHowellSet[sh].Tetrahedron[SXD->SubHowellSet[sh].Tetrahedron_Count].Volume = fabs(dx[0] * (dy[1] * dz[2] - dy[2] * dz[1]) - dx[1] * (dy[0] * dz[2] - dy[2] * dz[0]) + dx[2] * (dy[0] * dz[1] - dy[1] * dz[0])) / 6.0;
                        
                        SXD->SubHowellSet[sh].Tetrahedron_Count++;
                        
                        if (SXD->SubHowellSet[sh].Tetrahedron_Count >= Tetrahedron_Count)
                        {
                            Tetrahedron_Count += 4;
                            SXD->SubHowellSet[sh].Tetrahedron = (struct SXD_Tetrahedron*)realloc(SXD->SubHowellSet[sh].Tetrahedron, Tetrahedron_Count * sizeof(struct SXD_Tetrahedron));
                            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Tetrahedron, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->SubHowellSet[sh].Tetrahedron\" to", Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
                            
                            Tetrahedron_Face = (long_4*)realloc(Tetrahedron_Face, Tetrahedron_Count * sizeof(long_4));
                            if (_APP_WARNING_MESSAGE(Tetrahedron_Face, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_REALLOC, "\"Tetrahedron_Face\" to", Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
                        }
                    }
                }
            }
            
            // End-Search Tetrahedrons
            
            SXD->SubHowellSet[sh].Tetrahedron = (struct SXD_Tetrahedron*)realloc(SXD->SubHowellSet[sh].Tetrahedron, SXD->SubHowellSet[sh].Tetrahedron_Count * sizeof(struct SXD_Tetrahedron));
            if (_APP_WARNING_MESSAGE(SXD->SubHowellSet[sh].Tetrahedron, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron", _WARNING_MESSAGE_KEY_REALLOC, "\"SXD->SubHowellSet[sh].Tetrahedron\" to", SXD->SubHowellSet[sh].Tetrahedron_Count, 0)) { _USER_IO_WAIT(); free(Triangle_List); free(Tetrahedron_Face); return 1; }
        }
    // Search Tetrahedrons :: end
        }
    }}
    
    free(Tetrahedron_Face);
    free(Triangle_List);
    
    return 0;
}

int  CrystalAbsorption_SXD_Mesh_Get_TransmissionContributions(struct SXD_Mesh* SXD, double Shape_Volume)
{
    long i, j;
    
    long sh, th;
    
    double gg[4], Dg[3], Hg[3], Dd[3], Fraction[2];
    
    double Dvoid;
    
    bool   EmptyTetrahedron = false;
    long   FreeVertex_Count;
    double FreeVertex_Volume;
    
    SXD->Volume = 0.0;
    SXD->Transmission = 0.0;
    
    FreeVertex_Count  = 0;
    FreeVertex_Volume = 0.0;
    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++)
    {
        if (SXD->SubHowellSet[sh].Tetrahedron_Count > 0)
        {
            for (th = 0; th < SXD->SubHowellSet[sh].Tetrahedron_Count; th++)
            {
                SXD->Volume += SXD->SubHowellSet[sh].Tetrahedron[th].Volume;
            }
        }
        else
        {
            FreeVertex_Count += SXD->SubHowellSet[sh].Vertex_Count;
        }
    }
    
    FreeVertex_Volume = (Shape_Volume - SXD->Volume) / (double)FreeVertex_Count;

    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++)
    {
        if (SXD->SubHowellSet[sh].Tetrahedron_Count > 0)
        {
            for (th = 0; th < SXD->SubHowellSet[sh].Tetrahedron_Count; th++)
            {
                // Sort the gi coefficients
                for (i = 0; i < 4; i++)
                {
                    j = i;
                    while (j > 0)
                    {
                        if (SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Tetrahedron[th].Node_Id[i]]->g < gg[j - 1]) gg[j] = gg[j - 1];
                        else break;
                        j--;
                    }
                    gg[j] = SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Tetrahedron[th].Node_Id[i]]->g;
                }
                
                // Compute the tetrahedron's absorption At
                Dvoid = gg[3] - gg[1];
                if (Dvoid*Dvoid > FLT_EPSILON) // --> Dd[0] != 0 !!!
                {
                    for (i = 0; i < 3; i++)
                    {
                        Dg[i] = gg[i + 1] - gg[0];
                        if (fabs(Dg[i]) > FLT_EPSILON) Hg[i] = (1.0 - exp(-Dg[i])) / Dg[i];
                        else                           Hg[i] = 1.0;
                    }
                    
                    Dd[0] = Dg[2] - Dg[0];
                    Dd[1] = Dg[1] - Dg[0];
                    Dd[2] = Dg[2] - Dg[1];
                    
                    if (fabs(Dd[1]) > FLT_EPSILON)        Fraction[0] = (Hg[0] - Hg[1]) / Dd[1];
                    else { if (fabs(Dg[1]) > FLT_EPSILON) Fraction[0] = Hg[1] - (1.0 - Hg[1]) / Dg[1]; // The sign has been corrected compared with Tompa 1964
                           else                           Fraction[0] = 0.50; }
                    
                    if (fabs(Dd[2]) > FLT_EPSILON)        Fraction[1] = (Hg[1] - Hg[2]) / Dd[2];
                    else { if (fabs(Dg[1]) > FLT_EPSILON) Fraction[1] = Hg[1] - (1.0 - Hg[1]) / Dg[1]; // The sign has been corrected compared with Tompa 1964
                           else                           Fraction[1] = 0.50; }
                    
                    SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = 6.0 * SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]) * ((Fraction[0] - Fraction[1]) / Dd[0]); // The operation between the fractions has been corrected compared with Tompa 1964
                }
                else
                {
                    Dg[1] = gg[1] - gg[0];
                    
                    if (Dg[1]* Dg[1] > FLT_EPSILON)
                    {
                        Dd[0] = 1.0 / Dg[1];
                        Dd[1] = Dd[0] / Dg[1];
                        Dd[2] = Dd[1] / Dg[1];
                        
                        SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = 6.0 * SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]) * (Dd[2] - exp(-Dg[1]) * (Dd[2] + Dd[1] + 0.50 * Dd[0]));
                    }
                    else SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]);
                }
                
                // update total attenuation and volume
                
                SXD->Transmission += SXD->SubHowellSet[sh].Tetrahedron[th].Transmission;
                SXD->Volume += SXD->SubHowellSet[sh].Tetrahedron[th].Volume;
            }
        }
        else
        {
            if (!EmptyTetrahedron) EmptyTetrahedron = true;
            for (i = 0; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
            {                                         
                SXD->Transmission += FreeVertex_Volume * exp(-SXD->SubHowellSet[sh].Vertex_ab[i]->g);
            }
        }
        
    }
    
    SXD->Transmission /= SXD->Volume;
    
    if (EmptyTetrahedron) _APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_TransmissionContributions", _WARNING_MESSAGE_KEY_VALUE, "None Tetrahedron per SubHowell Polyhedron!", 0, 0);
    
    return 0;
}

int  CrystalAbsorption_GetTransmission_AnalyticIntegration(double* Transmission, struct Mesh* Shape, double Range, double Mu, double* Beam_In, double* Beam_Ou)
{
    double Norm;
    
    struct SXD_Mesh* SXD = NULL;
    
    SXD = (struct SXD_Mesh*)calloc(1, sizeof(struct SXD_Mesh));
    if (_APP_WARNING_MESSAGE(SXD, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_GetNew", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
    
    CrystalAbsorption_SXD_Mesh_Reset(SXD);
    
    SXD->Shape = Shape;
    
    Norm = 1.0 / sqrt(Beam_In[0] * Beam_In[0] + Beam_In[1] * Beam_In[1] + Beam_In[2] * Beam_In[2]);
    SXD->Beam_In[0] = Beam_In[0] * Norm;
    SXD->Beam_In[1] = Beam_In[1] * Norm;
    SXD->Beam_In[2] = Beam_In[2] * Norm;
    
    SXD->Beam_II[0] = -SXD->Beam_In[0];
    SXD->Beam_II[1] = -SXD->Beam_In[1];
    SXD->Beam_II[2] = -SXD->Beam_In[2];
    
    Norm = 1.0 / sqrt(Beam_Ou[0] * Beam_Ou[0] + Beam_Ou[1] * Beam_Ou[1] + Beam_Ou[2] * Beam_Ou[2]);
    SXD->Beam_Ou[0] = Beam_Ou[0] * Norm;
    SXD->Beam_Ou[1] = Beam_Ou[1] * Norm;
    SXD->Beam_Ou[2] = Beam_Ou[2] * Norm;
    
    SXD->Mu = Mu;
    
    if (CrystalAbsorption_SXD_Mesh_Get_EdgePlane(SXD))                                 { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_GhostNode_Plane(SXD, Range))                    { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_BeamPathG(SXD))                                 { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_SubHowellSet(SXD, Range))                       { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet(SXD, Range))                     { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron(SXD))                  { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_TransmissionContributions(SXD, Shape->Volume))  { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    
    *Transmission = SXD->Transmission;
    
    CrystalAbsorption_SXD_Mesh_Free(SXD);
    
    return 0;
}


// **** Multiple Solution ****

struct Threading_ReflectionList
{
    long Thread_id;
    long Thread_Count;
    
    struct Mesh* Shape;
    struct Reflection_List_DataSet* Reflection_List;

    double  Range;
    long    Max_Iteration_Count;
    double  Convergence;
    double  Step_Size;
    long    Step_Count;
};

int   CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_DataSet* Reflection) // Assume |Beam_In| = |Beam_Ou| = 1.0 !!!
{
    long i, j, k;
    
    long Count;
    
    double_3 ux, uy, uz, ls;
    double_2 xl, yl, zl;
    
    double dx, dy;
    double x_range, y_range;
    double Distance;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double Delta, Norm;
    double Convergence_2;
    double* Transmission_2;
    
    double_3 Sin, Sou;
    
    bool Test;
    
    Transmission_2 = NULL;
    Transmission_2 = (double*)calloc(Reflection->Absorption_Count, sizeof(double));
    if (Transmission_2 == NULL) return 1;
    
    Norm = 1.0 / sqrt(Reflection->Beam_In[0] * Reflection->Beam_In[0] + Reflection->Beam_In[1] * Reflection->Beam_In[1] + Reflection->Beam_In[2] * Reflection->Beam_In[2]);
    Sin[0] = Reflection->Beam_In[0] * Norm;
    Sin[1] = Reflection->Beam_In[1] * Norm;
    Sin[2] = Reflection->Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Reflection->Beam_Ou[0] * Reflection->Beam_Ou[0] + Reflection->Beam_Ou[1] * Reflection->Beam_Ou[1] + Reflection->Beam_Ou[2] * Reflection->Beam_Ou[2]);
    Sou[0] = Reflection->Beam_Ou[0] * Norm;
    Sou[1] = Reflection->Beam_Ou[1] * Norm;
    Sou[2] = Reflection->Beam_Ou[2] * Norm;
    
    Convergence_2 = Convergence * Convergence;
    
    uz[0] = -Sin[0];
    uz[1] = -Sin[1];
    uz[2] = -Sin[2];
    
    if      (fabs(Sin[0]) > FLT_EPSILON) { ux[0] = +Sin[1]; ux[1] = -Sin[0]; ux[2] = +0.0;    }
    else if (fabs(Sin[1]) > FLT_EPSILON) { ux[0] = +0.0;    ux[1] = +Sin[2]; ux[2] = -Sin[1]; }
    else if (fabs(Sin[2]) > FLT_EPSILON) { ux[0] = +Sin[2]; ux[1] = +0.0;    ux[2] = -Sin[0]; }
    else return 1;
    
    Norm = 1.0 / sqrt(ux[0] * ux[0] + ux[1] * ux[1] + ux[2] * ux[2]);
    for (i = 0; i < 3; i++) ux[i] *= Norm;
    
    uy[0] = ux[1] * Sin[2] - ux[2] * Sin[1];
    uy[1] = ux[2] * Sin[0] - ux[0] * Sin[2];
    uy[2] = ux[0] * Sin[1] - ux[1] * Sin[0];
    
    //Norm = 1.0 / sqrt(uy[0] * uy[0] + uy[1] * uy[1] + uy[2] * uy[2]); // |Sin| = |ux| = 1.0, Sin normal ux --> |uy| = |Sin X ux| = 1.0 !!!
    //for (i = 0; i < 3; i++) uy[i] *= Norm;
    
    xl[0] = 0.0;
    yl[0] = 0.0;
    zl[0] = 0.0;
    for (i = 0; i < 3; i++)
    {
        xl[0] += Shape->Node[0].x[i] * ux[i];
        yl[0] += Shape->Node[0].x[i] * uy[i];
        zl[0] += Shape->Node[0].x[i] * uz[i];
    }
    
    xl[1] = xl[0];
    yl[1] = yl[0];
    zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        ls[0] = 0.0;
        ls[1] = 0.0;
        ls[2] = 0.0;
        for (j = 0; j < 3; j++)
        {
            ls[0] += Shape->Node[i].x[j] * ux[j];
            ls[1] += Shape->Node[i].x[j] * uy[j];
            ls[2] += Shape->Node[i].x[j] * uz[j];
        }
        
        if (xl[0] > ls[0]) xl[0] = ls[0]; else if (xl[1] < ls[0]) xl[1] = ls[0];
        if (yl[0] > ls[1]) yl[0] = ls[1]; else if (yl[1] < ls[1]) yl[1] = ls[1];
        if (zl[0] > ls[2]) zl[0] = ls[2]; else if (zl[1] < ls[2]) zl[1] = ls[2];
    }
    
    x_range = xl[1] - xl[0];
    y_range = yl[1] - yl[0];
    
    for (k = 0; k < Reflection->Absorption_Count; k++) { Reflection->Absorption[k].Transmission = 0.0; Transmission_2[k] = 0.0; }
    
    Count = 0;
    for (i = 0; i < Max_Iteration_Count; i++)
    {
        dx = xl[0] + (x_range * (double)rand()) / (double)RAND_MAX;
        dy = yl[0] + (y_range * (double)rand()) / (double)RAND_MAX;
        
        Origin[0] = dx * ux[0] + dy * uy[0] + zl[0] * uz[0] - uz[0];
        Origin[1] = dx * ux[1] + dy * uy[1] + zl[0] * uz[1] - uz[1];
        Origin[2] = dx * ux[2] + dy * uy[2] + zl[0] * uz[2] - uz[2];
        
        Mesh_GetLeastPathLength(Shape, &Distance, &Origin[0], &uz[0]);
        Origin[0] += Distance * uz[0];
        Origin[1] += Distance * uz[1];
        Origin[2] += Distance * uz[2];
        
        if (Mesh_TestSite(Shape, &Origin[0]))
        {
            Mesh_GetLeastPathLength(Shape, &Distance, &Origin[0], &Sin[0]);
            
            Path_In = (Distance * (double)rand()) / (double)RAND_MAX;
            Origin[0] += Path_In * Sin[0];
            Origin[1] += Path_In * Sin[1];
            Origin[2] += Path_In * Sin[2];
            
            Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
            
            Path_Length = Path_In + Path_Ou;
            
            if (Path_Length > FLT_EPSILON)
            {
                for (k = 0; k < Reflection->Absorption_Count; k++)
                {
                    Delta = exp(-Path_Length * Reflection->Absorption[k].Mu);
                    
                    Reflection->Absorption[k].Transmission += Delta;
                    Transmission_2[k]  += Delta * Delta;
                }
                
                Count++;
                
                if (Convergence_2 > FLT_EPSILON)
                {
                    Norm = (double)Count;
                    Test = true;
                    for (k = 0; k < Reflection->Absorption_Count; k++) { if (Reflection->Absorption[k].Transmission * Reflection->Absorption[k].Transmission - (Norm * (Transmission_2[k] - Norm * Convergence_2)) < FLT_EPSILON) { Test = false; break; } }
                    if (Test) i = Max_Iteration_Count;
                }
            }
        }
    }
    
    Norm = 1.0 / (double)Count;
    for (k = 0; k < Reflection->Absorption_Count; k++) { Reflection->Absorption[k].Transmission *= Norm; }
    
    return 0;
}
void* CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling_Thread(void* ptr)
{
    long i;
    
    struct  Threading_ReflectionList* TH = (struct  Threading_ReflectionList*)ptr;
    
    for (i = TH->Thread_id; i < TH->Reflection_List->Reflection_Count; i += TH->Thread_Count)
    {
        CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(TH->Shape, TH->Max_Iteration_Count, TH->Convergence, &TH->Reflection_List->Reflection[i]);
    }
    
    return NULL;
}
int   CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_List_DataSet* Reflection_List, long Threads_Count)
{
    long i, j;
    
    struct  Threading_ReflectionList* TH = NULL;
    pthread_t* Thread = NULL;
    
    Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_ReflectionList*)calloc(Threads_Count, sizeof(struct Threading_ReflectionList));
	if (_APP_WARNING_MESSAGE(TH, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
    if (Threads_Count > Reflection_List->Reflection_Count) Threads_Count = Reflection_List->Reflection_Count;
    
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
        TH[i].Shape = Shape;
        TH[i].Reflection_List = Reflection_List;
        
        TH[i].Max_Iteration_Count = Max_Iteration_Count;
        TH[i].Convergence = Convergence;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling", _WARNING_MESSAGE_KEY_THREAD_CREATE, "CrystalAbsorption_GetTransmission_Integration_Threading", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) CrystalAbsorption_GetTransmission_MonteCarloIntegration_BeamSampling_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);

    return 0;
}

int   CrystalAbsorption_GetTransmission_MonteCarloIntegration(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_DataSet* Reflection)
{
    long i, k;
    
    long Count;
    
    double x_range, y_range, z_range;
    double_2 xl, yl, zl;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double Delta, Norm;
    double Convergence_2;
    double* Transmission_2;
    
    double_3 Sin, Sou;
    
    bool Test;

    Transmission_2 = NULL;
    Transmission_2 = (double*)calloc(Reflection->Absorption_Count, sizeof(double));
    if (Transmission_2 == NULL) return 1;

    Norm = 1.0 / sqrt(Reflection->Beam_In[0] * Reflection->Beam_In[0] + Reflection->Beam_In[1] * Reflection->Beam_In[1] + Reflection->Beam_In[2] * Reflection->Beam_In[2]);
    Sin[0] = -Reflection->Beam_In[0] * Norm;
    Sin[1] = -Reflection->Beam_In[1] * Norm;
    Sin[2] = -Reflection->Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Reflection->Beam_Ou[0] * Reflection->Beam_Ou[0] + Reflection->Beam_Ou[1] * Reflection->Beam_Ou[1] + Reflection->Beam_Ou[2] * Reflection->Beam_Ou[2]);
    Sou[0] = Reflection->Beam_Ou[0] * Norm;
    Sou[1] = Reflection->Beam_Ou[1] * Norm;
    Sou[2] = Reflection->Beam_Ou[2] * Norm;
    
    Convergence_2  = Convergence * Convergence;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    x_range = xl[1] - xl[0];
    y_range = yl[1] - yl[0];
    z_range = zl[1] - zl[0];
    
    for (k = 0; k < Reflection->Absorption_Count; k++) { Reflection->Absorption[k].Transmission = 0.0; Transmission_2[k] = 0.0; }
    
    Count = 0;
    for (i = 0; i < Max_Iteration_Count; i++)
    {
        Origin[0] = xl[0] + (x_range * (double)rand()) / (double)RAND_MAX;
        Origin[1] = yl[0] + (y_range * (double)rand()) / (double)RAND_MAX;
        Origin[2] = zl[0] + (z_range * (double)rand()) / (double)RAND_MAX;
        
        if (Mesh_TestSite(Shape, &Origin[0]))
        {
            Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
            Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
            
            Path_Length = Path_In + Path_Ou;
            
            if (Path_Length > FLT_EPSILON)
            {
                for (k = 0; k < Reflection->Absorption_Count; k++)
                {
                    Delta = exp(-Path_Length * Reflection->Absorption[k].Mu);
                    
                    Reflection->Absorption[k].Transmission += Delta;
                    Transmission_2[k]  += Delta * Delta;
                }
                
                Count++;
                
                if (Convergence_2 > FLT_EPSILON)
                {
                    Norm = (double)Count;
                    Test = true;
                    for (k = 0; k < Reflection->Absorption_Count; k++) { if (Reflection->Absorption[k].Transmission * Reflection->Absorption[k].Transmission - (Norm * (Transmission_2[k] - Norm * Convergence_2)) < FLT_EPSILON) { Test = false; break; } }
                    if (Test) i = Max_Iteration_Count;
                }
            }
        }
    }
    
    Norm = 1.0 / (double)Count;
    for (k = 0; k < Reflection->Absorption_Count; k++) { Reflection->Absorption[k].Transmission *= Norm; }
    
    return 0;
}
void* CrystalAbsorption_GetTransmission_MonteCarloIntegration_Thread(void* ptr)
{
    long i;
    
    struct  Threading_ReflectionList* TH = (struct  Threading_ReflectionList*)ptr;
    
    for (i = TH->Thread_id; i < TH->Reflection_List->Reflection_Count; i += TH->Thread_Count)
    {
        CrystalAbsorption_GetTransmission_MonteCarloIntegration(TH->Shape, TH->Max_Iteration_Count, TH->Convergence, &TH->Reflection_List->Reflection[i]);
    }
    
    return NULL;
}
int   CrystalAbsorption_GetTransmission_MonteCarloIntegration(struct Mesh* Shape, long Max_Iteration_Count, double Convergence, struct Reflection_List_DataSet* Reflection_List, long Threads_Count)
{
    long i, j;
    
    struct  Threading_ReflectionList* TH = NULL;
    pthread_t* Thread = NULL;
    
    Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_ReflectionList*)calloc(Threads_Count, sizeof(struct Threading_ReflectionList));
	if (_APP_WARNING_MESSAGE(TH, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
    if (Threads_Count > Reflection_List->Reflection_Count) Threads_Count = Reflection_List->Reflection_Count;
    
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
        TH[i].Shape = Shape;
        TH[i].Reflection_List = Reflection_List;
        
        TH[i].Max_Iteration_Count = Max_Iteration_Count;
        TH[i].Convergence = Convergence;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, CrystalAbsorption_GetTransmission_MonteCarloIntegration_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_MonteCarloIntegration", _WARNING_MESSAGE_KEY_THREAD_CREATE, "CrystalAbsorption_GetTransmission_Integration_Threading", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) CrystalAbsorption_GetTransmission_MonteCarloIntegration_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);

    return 0;
}

int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(struct Mesh* Shape, double Step_Size, struct Reflection_DataSet* Reflection)
{
    long i, j, k, l;
    
    long Count;
    
    long nx, ny, nz;
    double_2 xl, yl, zl;
    
    double Path_In, Path_Ou, Path_Length, Norm;
    double_3 Origin;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Reflection->Beam_In[0] * Reflection->Beam_In[0] + Reflection->Beam_In[1] * Reflection->Beam_In[1] + Reflection->Beam_In[2] * Reflection->Beam_In[2]);
    Sin[0] = -Reflection->Beam_In[0] * Norm;
    Sin[1] = -Reflection->Beam_In[1] * Norm;
    Sin[2] = -Reflection->Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Reflection->Beam_Ou[0] * Reflection->Beam_Ou[0] + Reflection->Beam_Ou[1] * Reflection->Beam_Ou[1] + Reflection->Beam_Ou[2] * Reflection->Beam_Ou[2]);
    Sou[0] = Reflection->Beam_Ou[0] * Norm;
    Sou[1] = Reflection->Beam_Ou[1] * Norm;
    Sou[2] = Reflection->Beam_Ou[2] * Norm;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    Norm = 1.0 / (double)Step_Size;
    nx = 1 + (long)ceil((xl[1] - xl[0]) * Norm);
    ny = 1 + (long)ceil((yl[1] - yl[0]) * Norm);
    nz = 1 + (long)ceil((zl[1] - zl[0]) * Norm);
    
    for (l = 0; l < Reflection->Absorption_Count; l++) Reflection->Absorption[l].Transmission = 0.0;
    
    Count = 0;
    for (i = 0; i < nx; i++) { Origin[0] = xl[0] + Step_Size * (double)i;
    for (j = 0; j < ny; j++) { Origin[1] = yl[0] + Step_Size * (double)j;
    for (k = 0; k < nz; k++) { Origin[2] = zl[0] + Step_Size * (double)k;
    if (Mesh_TestSite(Shape, &Origin[0]))
    {
        Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
        Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
        
        Path_Length = Path_In + Path_Ou;
        
        if (Path_Length > FLT_EPSILON)
        {
            for (l = 0; l < Reflection->Absorption_Count; l++) Reflection->Absorption[l].Transmission += exp(-Path_Length * Reflection->Absorption[l].Mu);
            Count++;
        }
    }}}}
    
    Norm = 1.0 / (double)Count;
    for (l = 0; l < Reflection->Absorption_Count; l++) { Reflection->Absorption[l].Transmission *= Norm; }
    
    return 0;
}
void* CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize_Thread(void* ptr)
{
    long i;
    
    struct  Threading_ReflectionList* TH = (struct  Threading_ReflectionList*)ptr;
    
    for (i = TH->Thread_id; i < TH->Reflection_List->Reflection_Count; i += TH->Thread_Count)
    {
        CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(TH->Shape, TH->Step_Size, &TH->Reflection_List->Reflection[i]);
    }
    
    return NULL;
}
int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize(struct Mesh* Shape, double Step_Size, struct Reflection_List_DataSet* Reflection_List, long Threads_Count)
{
    long i, j;
    
    struct  Threading_ReflectionList* TH = NULL;
    pthread_t* Thread = NULL;
    
    Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_ReflectionList*)calloc(Threads_Count, sizeof(struct Threading_ReflectionList));
	if (_APP_WARNING_MESSAGE(TH, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
    if (Threads_Count > Reflection_List->Reflection_Count) Threads_Count = Reflection_List->Reflection_Count;
    
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
        TH[i].Shape = Shape;
        TH[i].Reflection_List = Reflection_List;
        
        TH[i].Step_Size = Step_Size;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize", _WARNING_MESSAGE_KEY_THREAD_CREATE, "CrystalAbsorption_GetTransmission_Integration_Threading", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);

    return 0;
}

int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(struct Mesh* Shape, long Step_Count, struct Reflection_DataSet* Reflection)
{
    long i, j, k, l;
    
    long Count;
    
    double_2 xl, yl, zl;
    double dx, dy, dz, Norm;
    
    double Path_In, Path_Ou, Path_Length;
    double_3 Origin;
    
    double_3 Sin, Sou;
    
    Norm = 1.0 / sqrt(Reflection->Beam_In[0] * Reflection->Beam_In[0] + Reflection->Beam_In[1] * Reflection->Beam_In[1] + Reflection->Beam_In[2] * Reflection->Beam_In[2]);
    Sin[0] = -Reflection->Beam_In[0] * Norm;
    Sin[1] = -Reflection->Beam_In[1] * Norm;
    Sin[2] = -Reflection->Beam_In[2] * Norm;
    
    Norm = 1.0 / sqrt(Reflection->Beam_Ou[0] * Reflection->Beam_Ou[0] + Reflection->Beam_Ou[1] * Reflection->Beam_Ou[1] + Reflection->Beam_Ou[2] * Reflection->Beam_Ou[2]);
    Sou[0] = Reflection->Beam_Ou[0] * Norm;
    Sou[1] = Reflection->Beam_Ou[1] * Norm;
    Sou[2] = Reflection->Beam_Ou[2] * Norm;
    
    xl[0] = Shape->Node[0].x[0]; xl[1] = xl[0];
    yl[0] = Shape->Node[0].x[1]; yl[1] = yl[0];
    zl[0] = Shape->Node[0].x[2]; zl[1] = zl[0];
    
    for (i = 1; i < Shape->Node_Count; i++)
    {
        if (xl[0] > Shape->Node[i].x[0]) xl[0] = Shape->Node[i].x[0]; else if (xl[1] < Shape->Node[i].x[0]) xl[1] = Shape->Node[i].x[0];
        if (yl[0] > Shape->Node[i].x[1]) yl[0] = Shape->Node[i].x[1]; else if (yl[1] < Shape->Node[i].x[1]) yl[1] = Shape->Node[i].x[1];
        if (zl[0] > Shape->Node[i].x[2]) zl[0] = Shape->Node[i].x[2]; else if (zl[1] < Shape->Node[i].x[2]) zl[1] = Shape->Node[i].x[2];
    }
    
    Norm = 1.0 / (double)Step_Count;
    dx = (xl[1] - xl[0]) * Norm;
    dy = (yl[1] - yl[0]) * Norm;
    dz = (zl[1] - zl[0]) * Norm;
    
    for (l = 0; l < Reflection->Absorption_Count; l++) Reflection->Absorption[l].Transmission = 0.0;
    
    Count = 0;
    for (i = 0; i <= Step_Count; i++) { Origin[0] = xl[0] + dx * (double)i;
    for (j = 0; j <= Step_Count; j++) { Origin[1] = yl[0] + dy * (double)j;
    for (k = 0; k <= Step_Count; k++) { Origin[2] = zl[0] + dz * (double)k;
    if (Mesh_TestSite(Shape, &Origin[0]))
    {
        Mesh_GetLeastPathLength(Shape, &Path_In, &Origin[0], &Sin[0]);
        Mesh_GetLeastPathLength(Shape, &Path_Ou, &Origin[0], &Sou[0]);
        
        Path_Length = Path_In + Path_Ou;
        
        if (Path_Length > FLT_EPSILON)
        {
            for (l = 0; l < Reflection->Absorption_Count; l++) Reflection->Absorption[l].Transmission += exp(-Path_Length * Reflection->Absorption[l].Mu);
            Count++;
        }
    }}}}
    
    Norm = 1.0 / (double)Count;
    for (l = 0; l < Reflection->Absorption_Count; l++) { Reflection->Absorption[l].Transmission *= Norm; }
    
    return 0;
}
void* CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount_Thread(void* ptr)
{
    long i;
    
    struct  Threading_ReflectionList* TH = (struct  Threading_ReflectionList*)ptr;
    
    for (i = TH->Thread_id; i < TH->Reflection_List->Reflection_Count; i += TH->Thread_Count)
    {
        CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(TH->Shape, TH->Step_Count, &TH->Reflection_List->Reflection[i]);
    }
    
    return NULL;
}
int   CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount(struct Mesh* Shape, long Step_Count, struct Reflection_List_DataSet* Reflection_List, long Threads_Count)
{
    long i, j;
    
    struct  Threading_ReflectionList* TH = NULL;
    pthread_t* Thread = NULL;
    
    Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_ReflectionList*)calloc(Threads_Count, sizeof(struct Threading_ReflectionList));
	if (_APP_WARNING_MESSAGE(TH, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
    if (Threads_Count > Reflection_List->Reflection_Count) Threads_Count = Reflection_List->Reflection_Count;
    
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
        TH[i].Shape = Shape;
        TH[i].Reflection_List = Reflection_List;
        
        TH[i].Step_Count = Step_Count;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_DiscreteIntegration_StepCount", _WARNING_MESSAGE_KEY_THREAD_CREATE, "CrystalAbsorption_GetTransmission_Integration_Threading", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) CrystalAbsorption_GetTransmission_DiscreteIntegration_StepSize_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);
    
    return 0;
}

// Discrete Integrations!!!!

// Analytic solution: Tompa 1969

// include warning about subhowels with none valid tetrahedron!!! Avoid repetition for as much as possible!!!

int   CrystalAbsorption_SXD_Mesh_Get_BeamPath(struct SXD_Mesh* SXD)
{
    long i;
    
    SXD->Node_ab = (struct SXD_Scattering*)calloc(SXD->Shape->Node_Count, sizeof(struct SXD_Scattering));
    if (_APP_WARNING_MESSAGE(SXD->Node_ab, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_BeamPath", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD->Node_ab\" to", SXD->Shape->Node_Count, 0)) { _USER_IO_WAIT(); return 1; }
    
    for (i = 0; i < SXD->Shape->Node_Count; i++)
    {
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->Node_ab[i].Path[1], &SXD->Shape->Node[i].x[0], &SXD->Beam_II[0]);
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->Node_ab[i].Path[2], &SXD->Shape->Node[i].x[0], &SXD->Beam_Ou[0]);
        
        SXD->Node_ab[i].Path[0] = SXD->Node_ab[i].Path[1] + SXD->Node_ab[i].Path[2];
    }
    
    for (i = 0; i < SXD->GhostNode_Count; i++)
    {
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->GhostNode[i].ab.Path[1], &SXD->GhostNode[i].x[0], &SXD->Beam_II[0]);
        Mesh_GetLeastPathLength(SXD->Shape, &SXD->GhostNode[i].ab.Path[2], &SXD->GhostNode[i].x[0], &SXD->Beam_Ou[0]);
        
        SXD->GhostNode[i].ab.Path[0] = SXD->GhostNode[i].ab.Path[1] + SXD->GhostNode[i].ab.Path[2];
    }
    
    return 0;
}
int   CrystalAbsorption_SXD_Mesh_Get_ListTransmissionContributions(struct SXD_Mesh* SXD, struct Reflection_DataSet* Reflection, double Shape_Volume)
{
    long i, j, k;
    
    long sh, th;
    
    double_2 Fraction;
    double_3 Hg, Dd, Dg;
    double_4 ll, gg;
    
    double Dvoid;
    
    bool   EmptyTetrahedron = false;
    long   FreeVertex_Count;
    double FreeVertex_Volume;
    
    SXD->Volume = 0.0;
    //SXD->Transmission = 0.0;
    
    for (k = 0; k < Reflection->Absorption_Count; k++) Reflection->Absorption[k].Transmission = 0.0;
    
    FreeVertex_Count  = 0;
    FreeVertex_Volume = 0.0;
    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++)
    {
        if (SXD->SubHowellSet[sh].Tetrahedron_Count > 0)
        {
            for (th = 0; th < SXD->SubHowellSet[sh].Tetrahedron_Count; th++)
            {
                SXD->Volume += SXD->SubHowellSet[sh].Tetrahedron[th].Volume;
            }
        }
        else
        {
            FreeVertex_Count += SXD->SubHowellSet[sh].Vertex_Count;
        }
    }
    
    if (FreeVertex_Count > 0) FreeVertex_Volume = (Shape_Volume - SXD->Volume) / (double)FreeVertex_Count;
    else                      FreeVertex_Volume = 0.0;

    for (sh = 0; sh < SXD->SubHowellSet_Count; sh++)
    {
        if (SXD->SubHowellSet[sh].Tetrahedron_Count > 0)
        {
            for (th = 0; th < SXD->SubHowellSet[sh].Tetrahedron_Count; th++)
            {
                // Sort the gi coefficients
                for (i = 0; i < 4; i++)
                {
                    j = i;
                    while (j > 0)
                    {
                        if (SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Tetrahedron[th].Node_Id[i]]->Path[0] < ll[j - 1]) ll[j] = ll[j - 1];
                        else break;
                        j--;
                    }
                    ll[j] = SXD->SubHowellSet[sh].Vertex_ab[SXD->SubHowellSet[sh].Tetrahedron[th].Node_Id[i]]->Path[0];
                }
                
                for (k = 0; k < Reflection->Absorption_Count; k++)
                {
                    gg[0] = ll[0] * Reflection->Absorption[k].Mu;
                    gg[1] = ll[1] * Reflection->Absorption[k].Mu;
                    gg[2] = ll[2] * Reflection->Absorption[k].Mu;
                    gg[3] = ll[3] * Reflection->Absorption[k].Mu;
                    
                    // Compute the tetrahedron's absorption At
                    Dvoid = gg[3] - gg[1];
                    if (Dvoid*Dvoid > FLT_EPSILON) // --> Dd[0] != 0 !!!
                    {
                        for (i = 0; i < 3; i++)
                        {
                            Dg[i] = gg[i + 1] - gg[0];
                            if (fabs(Dg[i]) > FLT_EPSILON) Hg[i] = (1.0 - exp(-Dg[i])) / Dg[i];
                            else                           Hg[i] = 1.0;
                        }
                        
                        Dd[0] = Dg[2] - Dg[0];
                        Dd[1] = Dg[1] - Dg[0];
                        Dd[2] = Dg[2] - Dg[1];
                        
                        if (fabs(Dd[1]) > FLT_EPSILON)        Fraction[0] = (Hg[0] - Hg[1]) / Dd[1];
                        else { if (fabs(Dg[1]) > FLT_EPSILON) Fraction[0] = Hg[1] - (1.0 - Hg[1]) / Dg[1]; // The sign has been corrected compared with Tompa 1964
                               else                           Fraction[0] = 0.50; }
                        
                        if (fabs(Dd[2]) > FLT_EPSILON)        Fraction[1] = (Hg[1] - Hg[2]) / Dd[2];
                        else { if (fabs(Dg[1]) > FLT_EPSILON) Fraction[1] = Hg[1] - (1.0 - Hg[1]) / Dg[1]; // The sign has been corrected compared with Tompa 1964
                               else                           Fraction[1] = 0.50; }
                        
                        SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = 6.0 * SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]) * ((Fraction[0] - Fraction[1]) / Dd[0]); // The operation between the fractions has been corrected compared with Tompa 1964
                    }
                    else
                    {
                        Dg[1] = gg[1] - gg[0];
                        
                        if (Dg[1]* Dg[1] > FLT_EPSILON)
                        {
                            Dd[0] = 1.0 / Dg[1];
                            Dd[1] = Dd[0] / Dg[1];
                            Dd[2] = Dd[1] / Dg[1];
                            
                            SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = 6.0 * SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]) * (Dd[2] - exp(-Dg[1]) * (Dd[2] + Dd[1] + 0.50 * Dd[0]));
                        }
                        else SXD->SubHowellSet[sh].Tetrahedron[th].Transmission = SXD->SubHowellSet[sh].Tetrahedron[th].Volume * exp(-gg[0]);
                    }
                    
                    // update total attenuation and volume
                    
                    Reflection->Absorption[k].Transmission += SXD->SubHowellSet[sh].Tetrahedron[th].Transmission;
                    //SXD->Transmission += SXD->SubHowellSet[sh].Tetrahedron[th].Transmission;
                }
                // SXD->Volume += SXD->SubHowellSet[sh].Tetrahedron[th].Volume;
            }
        }
        else
        {
            if (!EmptyTetrahedron) EmptyTetrahedron = true;
            for (i = 0; i < SXD->SubHowellSet[sh].Vertex_Count; i++)
            {
                for (k = 0; k < Reflection->Absorption_Count; k++)
                {
                    Reflection->Absorption[k].Transmission += FreeVertex_Volume * exp(-SXD->SubHowellSet[sh].Vertex_ab[i]->Path[0] * Reflection->Absorption[k].Mu);
                }
            }
        }
    }
    
    if (FreeVertex_Count > 0)
    {
        for (k = 0; k < Reflection->Absorption_Count; k++) Reflection->Absorption[k].Transmission /= (SXD->Volume + FreeVertex_Volume * (double)FreeVertex_Count);
    }
    else
    {
        for (k = 0; k < Reflection->Absorption_Count; k++) Reflection->Absorption[k].Transmission /= SXD->Volume;
    }
    
    if (EmptyTetrahedron) _APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_Get_ListTransmissionContributions", _WARNING_MESSAGE_KEY_VALUE, "None Tetrahedron per SubHowell Polyhedron!", 0, 0);
    
    return 0;
}

int   CrystalAbsorption_GetTransmission_AnalyticIntegration(struct Mesh* Shape, double Range, struct Reflection_DataSet* Reflection)
{
    double Norm;
    
    struct SXD_Mesh* SXD = NULL;
    
    SXD = (struct SXD_Mesh*)calloc(1, sizeof(struct SXD_Mesh));
    if (_APP_WARNING_MESSAGE(SXD, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_SXD_Mesh_GetNew", _WARNING_MESSAGE_KEY_CALLOC, "\"SXD\" to", 1, 0)) { _USER_IO_WAIT(); return 1; }
    
    CrystalAbsorption_SXD_Mesh_Reset(SXD);
    
    SXD->Shape = Shape;
    
    Norm = 1.0 / sqrt(Reflection->Beam_In[0] * Reflection->Beam_In[0] + Reflection->Beam_In[1] * Reflection->Beam_In[1] + Reflection->Beam_In[2] * Reflection->Beam_In[2]);
    SXD->Beam_In[0] = Reflection->Beam_In[0] * Norm;
    SXD->Beam_In[1] = Reflection->Beam_In[1] * Norm;
    SXD->Beam_In[2] = Reflection->Beam_In[2] * Norm;
    
    SXD->Beam_II[0] = -SXD->Beam_In[0];
    SXD->Beam_II[1] = -SXD->Beam_In[1];
    SXD->Beam_II[2] = -SXD->Beam_In[2];
    
    Norm = 1.0 / sqrt(Reflection->Beam_Ou[0] * Reflection->Beam_Ou[0] + Reflection->Beam_Ou[1] * Reflection->Beam_Ou[1] + Reflection->Beam_Ou[2] * Reflection->Beam_Ou[2]);
    SXD->Beam_Ou[0] = Reflection->Beam_Ou[0] * Norm;
    SXD->Beam_Ou[1] = Reflection->Beam_Ou[1] * Norm;
    SXD->Beam_Ou[2] = Reflection->Beam_Ou[2] * Norm;
    
    SXD->Mu = Reflection->Absorption[0].Mu;
    
    if (CrystalAbsorption_SXD_Mesh_Get_EdgePlane(SXD))                                                 { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_GhostNode_Plane(SXD, Range))                                    { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_BeamPath(SXD))                                                  { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_SubHowellSet(SXD, Range))                                       { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Clean_SubHowellSet(SXD, Range))                                     { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_SubHowellSet_Tetarhedron(SXD))                                  { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    if (CrystalAbsorption_SXD_Mesh_Get_ListTransmissionContributions(SXD, Reflection, Shape->Volume))  { CrystalAbsorption_SXD_Mesh_Free(SXD); return 1; }
    
    CrystalAbsorption_SXD_Mesh_Free(SXD);
    
    return 0;
}
void* CrystalAbsorption_GetTransmission_AnalyticIntegration_Thread(void* ptr)
{
    long i;
    
    struct  Threading_ReflectionList* TH = (struct  Threading_ReflectionList*)ptr;
    
    for (i = TH->Thread_id; i < TH->Reflection_List->Reflection_Count; i += TH->Thread_Count)
    {
        CrystalAbsorption_GetTransmission_AnalyticIntegration(TH->Shape, TH->Range, &TH->Reflection_List->Reflection[i]);
    }
    
    return NULL;
}
int   CrystalAbsorption_GetTransmission_AnalyticIntegration(struct Mesh* Shape, double Range, struct Reflection_List_DataSet* Reflection_List, long Threads_Count)
{
    long i, j;
    
    struct  Threading_ReflectionList* TH = NULL;
    pthread_t* Thread = NULL;
    
    Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_AnalyticIntegration", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_ReflectionList*)calloc(Threads_Count, sizeof(struct Threading_ReflectionList));
	if (_APP_WARNING_MESSAGE(TH, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_AnalyticIntegration", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
    if (Threads_Count > Reflection_List->Reflection_Count) Threads_Count = Reflection_List->Reflection_Count;
    
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
        TH[i].Shape = Shape;
        TH[i].Reflection_List = Reflection_List;
        
        TH[i].Range = Range;
	}
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, CrystalAbsorption_GetTransmission_AnalyticIntegration_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_CrystalAbsorption.cpp", "CrystalAbsorption_GetTransmission_AnalyticIntegration", _WARNING_MESSAGE_KEY_THREAD_CREATE, "CrystalAbsorption_GetTransmission_Integration_Threading", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) CrystalAbsorption_GetTransmission_AnalyticIntegration_Thread(&TH[i]);
#endif
	
	free(TH);
	free(Thread);

    return 0;
}