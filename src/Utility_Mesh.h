// Utility Functions :: Test
//
// ...

#ifndef _INCLUDE_UTILITY_MESH
#define _INCLUDE_UTILITY_MESH 1

#include <App_Environment.h>

#include <Signal_Warning.h>

#include <Utility_File_IO.h>
#include <Utility_Math.h>

struct STL_Mesh_Triangle
{
    double_3 Normal[3];
    double_3 Corner[3];
    int16_t  Attribute;
};
struct STL_Mesh
{
    char Header[80];
    long Triangle_Count;
    
    struct STL_Mesh_Triangle* Triangle;
};

struct Mesh_Face
{
    double_4 Normal;    // nx, ny, nz, distance
    double Surface;
    
    long   Edge_Count;
    long   Node_Count;
    
    long*  Edge_Id;
    long*  Node_Id;
};
struct Mesh_Edge
{
    double_3 Direction;
    double_3 Center;
    double   Length;
    
    long_2   Node_Id;
};
struct Mesh_Node
{
    double_3 x;
};
struct Mesh
{
    double Volume;
    double Surface;
    
    long Face_Count;
    long Edge_Count;
    long Node_Count;
    
    struct Mesh_Face* Face;
    struct Mesh_Edge* Edge;
    struct Mesh_Node* Node;
};

// Data-structure Initialize

void Mesh_Free_Face(struct Mesh_Face* Face);
void Mesh_Free_Edge(struct Mesh_Edge* Edge);
void Mesh_Free_Node(struct Mesh_Node* Node);

void Mesh_Free_All(struct Mesh* Shape);

void Mesh_Reset(struct Mesh* Shape);

// Resolve Mesh

int Mesh_ResolveFromFacets(struct Mesh* Shape, long Threads_Count); // Assume only Facets normal are defined
int Mesh_Simplify(struct Mesh** Shape, double Threshold, long Threads_Count);

// TXT Mesh out

int Mesh_WriteMesh_TXT(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char UnitOfMeasure[]);

struct Mesh* Mesh_ReadMesh_TXT(char Path[], char Name[]);

// STL in-out Mesh

int Mesh_WriteSTL_ASCII(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[]);
int Mesh_WriteSTL_BINARY(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char Header[]); // The header is truncated at 80 bytes (i.e., 80 char)

struct Mesh* Mesh_ReadSTL(char Path[], char Name[]);

// Mesh Duplicate

struct Mesh* Mesh_Copy(struct Mesh* Shape);

// Processing Data

int Mesh_TestSite(struct Mesh* Mesh, double* Site); // return value: 0. site outside, 1 site inside
int Mesh_GetLeastPathLength(struct Mesh* Shape, double* Distance, double* Origin, double* Direction); // return value: 0. shortes distance toward a valid Face of the Mesh (note: the intersection could be outside the mesh), 1. no valid distance along Direction from Origin // The Distance is expressed in 'Direction' units

int Mesh_Size_Scale(struct Mesh* Mesh, double Scale);

int Mesh_Axes_Swap_and_Flip(struct Mesh* Mesh, char Xsf[], char Ysf[], char Zsf[]);
int Mesh_Rotation(struct Mesh* Mesh, double* R);
int Mesh_Rotation(struct Mesh* Mesh, double* R, struct Mesh* Mesh_Reference);

#endif