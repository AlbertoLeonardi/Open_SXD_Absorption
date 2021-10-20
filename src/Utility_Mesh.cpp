// Utility Functions :: Test
//

#include "Utility_Mesh.h"

#define _MESH_NODE_COUNT_STEP_ 20
#define _FACE_NODE_COUNT_STEP_ 10

#define _MESH_EDGE_COUNT_STEP_ 20
#define _FACE_EDGE_COUNT_STEP_ 10

#define _MESH_FACE_COUNT_STEP_ 20

// Data-structure Initialize

void Mesh_Free_Face(struct Mesh_Face* Face)
{
	if (Face->Edge_Id != NULL) free(Face->Edge_Id);
	if (Face->Node_Id != NULL) free(Face->Node_Id);
	
	return;
};
void Mesh_Free_Edge(struct Mesh_Edge* Edge)
{
	return;
};
void Mesh_Free_Node(struct Mesh_Node* Node)
{
	return;
};

void Mesh_Free_All(struct Mesh* Shape)
{
	long i;
	
	if (Shape != NULL)
	{
		if (Shape->Face != NULL) { for (i = 0; i < Shape->Face_Count; i++) { Mesh_Free_Face(&(Shape->Face[i])); } free(Shape->Face); }
		if (Shape->Edge != NULL) { for (i = 0; i < Shape->Edge_Count; i++) { Mesh_Free_Edge(&(Shape->Edge[i])); } free(Shape->Edge); }
		if (Shape->Node != NULL) { for (i = 0; i < Shape->Node_Count; i++) { Mesh_Free_Node(&(Shape->Node[i])); } free(Shape->Node); }
		
		free(Shape);
	}
	
	return;
}

void Mesh_Reset(struct Mesh* Shape)
{
	Shape->Volume = 0.0;
	
	Shape->Face_Count = 0;
	Shape->Edge_Count = 0;
	Shape->Node_Count = 0;
	
	Shape->Face = NULL;
	Shape->Edge = NULL;
	Shape->Node = NULL;
	
	return;
}

// Resolve Mesh

int Mesh_ResolveFromFacets_old(struct Mesh* Shape) // Assume only Facets normal are defined
{
	long i, j, k, l, m;
	long id;
	
	long* Edge_Node_Count;
	long_2* Edge_Vertex_id;
	
	bool Skip, Test;
	
	double A[3][3];
	double Norm;
	
	double_3 Center;
	double_3 xx, yy, vv;
	double_2 cc;
	
	long Angle_Count;
	double* Angle;
	
	// Free sub-structures of data
	
	if (Shape->Edge != NULL) { for (i = 0; i < Shape->Edge_Count; i++) { Mesh_Free_Edge(&(Shape->Edge[i])); } free(Shape->Edge); }
	if (Shape->Node != NULL) { for (i = 0; i < Shape->Node_Count; i++) { Mesh_Free_Node(&(Shape->Node[i])); } free(Shape->Node); }
	
	Shape->Edge_Count = 0;
	Shape->Node_Count = 0;
	Shape->Face_Count = 50;
	// Remove equivalent factes
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		for (j = 0; j < i; j++)
		{
			if (fabs((Shape->Face[i].Normal[0] * Shape->Face[j].Normal[0] +
					  Shape->Face[i].Normal[1] * Shape->Face[j].Normal[1] +
					  Shape->Face[i].Normal[2] * Shape->Face[j].Normal[2]) * Shape->Face[i].Normal[3] - 
					  Shape->Face[j].Normal[3]) < FLT_EPSILON)
			{
				if (i < Shape->Face_Count - 1)
				{
					Shape->Face[i].Normal[0] = Shape->Face[Shape->Face_Count - 1].Normal[0];
					Shape->Face[i].Normal[1] = Shape->Face[Shape->Face_Count - 1].Normal[1];
					Shape->Face[i].Normal[2] = Shape->Face[Shape->Face_Count - 1].Normal[2];
					Shape->Face[i].Normal[3] = Shape->Face[Shape->Face_Count - 1].Normal[3];
					i--;
				}
				
				Shape->Face_Count--;
			}
		}
	}
	
	Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face\" to", Shape->Face_Count, 0)) return 1;
	
	// Resolve facets nodes (if not valid remove)
	
	Shape->Node = (struct Mesh_Node*)calloc(_MESH_NODE_COUNT_STEP_, sizeof(struct Mesh_Node));
	Shape->Edge = (struct Mesh_Edge*)calloc(_MESH_EDGE_COUNT_STEP_, sizeof(struct Mesh_Edge));
	if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", _FACE_NODE_COUNT_STEP_, 1)) return 1;
	if (_APP_WARNING_MESSAGE(Shape->Edge, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Edge\" to", _FACE_EDGE_COUNT_STEP_, 2)) return 1;
	
	Edge_Node_Count = (long*)calloc(Shape->Face_Count, sizeof(long));
	Edge_Vertex_id = (long_2*)calloc(Shape->Face_Count, sizeof(long_2));
	if (_APP_WARNING_MESSAGE(Edge_Vertex_id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Edge_Vertex_id\" to", Shape->Face_Count, 3)) return 1;
	if (_APP_WARNING_MESSAGE(Edge_Node_Count, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Edge_Node_Count\" to", Shape->Face_Count, 4)) return 1;
	
	Shape->Node_Count = 0;
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		printf("%ld :: %ld       \r", i, Shape->Face_Count);
		
		Shape->Face[i].Node_Id = (long*)calloc(_FACE_NODE_COUNT_STEP_, sizeof(long));
		Shape->Face[i].Edge_Id = (long*)calloc(_FACE_EDGE_COUNT_STEP_, sizeof(long));
		if (_APP_WARNING_MESSAGE(Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face[i].Node_Id\" to", _FACE_NODE_COUNT_STEP_, 5)) return 1;
		if (_APP_WARNING_MESSAGE(Shape->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face[i].Edge_Id\" to", _FACE_EDGE_COUNT_STEP_, 6)) return 1;
		
		memset(Edge_Node_Count, 0, Shape->Face_Count * sizeof(long));
		
		Shape->Face[i].Node_Count = 0;
		Shape->Face[i].Edge_Count = 0;
		
		for (j = 0;     j < Shape->Face_Count; j++) { if(j!=i) {
		for (k = j + 1; k < Shape->Face_Count; k++) { if(k!=i) {
			Skip = false;
			for (l = 0; l < Shape->Node_Count; l++)
			{
				Test = false;
				for (m = 0; m < Shape->Face[i].Node_Count; m++) { if (Shape->Face[i].Node_Id[m] == l) { Test = true; m = Shape->Face[i].Node_Count; } } // The intersection can not have been already collected
				if (!Test)
				{
					if (fabs(Shape->Node[l].x[0] * Shape->Face[i].Normal[0] +
							 Shape->Node[l].x[1] * Shape->Face[i].Normal[1] +
							 Shape->Node[l].x[2] * Shape->Face[i].Normal[2] -
							 Shape->Face[i].Normal[3]) < FLT_EPSILON) {
					if (fabs(Shape->Node[l].x[0] * Shape->Face[j].Normal[0] +
							 Shape->Node[l].x[1] * Shape->Face[j].Normal[1] +
							 Shape->Node[l].x[2] * Shape->Face[j].Normal[2] -
							 Shape->Face[j].Normal[3]) < FLT_EPSILON) {
					if (fabs(Shape->Node[l].x[0] * Shape->Face[k].Normal[0] +
							 Shape->Node[l].x[1] * Shape->Face[k].Normal[1] +
							 Shape->Node[l].x[2] * Shape->Face[k].Normal[2] -
							 Shape->Face[k].Normal[3]) < FLT_EPSILON) {
						Test = true;											// The node l is the three-planes intersection and is shared with other (already detected) facets
					}}}
					if (Test)
					{
						Skip = true;
						
						Shape->Face[i].Node_Count++;
						
						if (Shape->Face[i].Node_Count % _FACE_NODE_COUNT_STEP_ == 0)
						{
							Shape->Face[i].Node_Id = (long*)realloc(Shape->Face[i].Node_Id, (Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_) * sizeof(long));
							if (_APP_WARNING_MESSAGE(Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Node_Id\" to", Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_, 7)) return 1;
						}
						
						Shape->Face[i].Node_Id[Shape->Face[i].Node_Count - 1] = l;
						Edge_Vertex_id[j][Edge_Node_Count[j]] = l;
						Edge_Vertex_id[k][Edge_Node_Count[k]] = l;
						Edge_Node_Count[j]++;
						Edge_Node_Count[k]++;
					}
				}
			}
			if (!Skip)
			{
				A[0][0] = Shape->Face[i].Normal[0]; A[0][1] = Shape->Face[i].Normal[1]; A[0][2] = Shape->Face[i].Normal[2];
				A[1][0] = Shape->Face[j].Normal[0]; A[1][1] = Shape->Face[j].Normal[1]; A[1][2] = Shape->Face[j].Normal[2];
				A[2][0] = Shape->Face[k].Normal[0]; A[2][1] = Shape->Face[k].Normal[1]; A[2][2] = Shape->Face[k].Normal[2];
				
				if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
				{
					id = Shape->Node_Count;
					Shape->Node_Count++;
					
					if (Shape->Node_Count % _MESH_NODE_COUNT_STEP_ == 0)
					{
						Shape->Node = (struct Mesh_Node*)realloc(Shape->Node, (Shape->Node_Count + _MESH_NODE_COUNT_STEP_) * sizeof(struct Mesh_Node));
						if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Node\" to", Shape->Node_Count + _MESH_NODE_COUNT_STEP_, 8)) return 1;
					}
					
					Shape->Node[id].x[0] = A[0][0] * Shape->Face[i].Normal[3] + A[0][1] * Shape->Face[j].Normal[3] + A[0][2] * Shape->Face[k].Normal[3];
					Shape->Node[id].x[1] = A[1][0] * Shape->Face[i].Normal[3] + A[1][1] * Shape->Face[j].Normal[3] + A[1][2] * Shape->Face[k].Normal[3];
					Shape->Node[id].x[2] = A[2][0] * Shape->Face[i].Normal[3] + A[2][1] * Shape->Face[j].Normal[3] + A[2][2] * Shape->Face[k].Normal[3];
					
					Test = true;
					for (l = 0; l < Shape->Face_Count; l++) {
					if ((l != i) && (l != j) && (l != k))   {
						if (Shape->Face[l].Normal[0] * Shape->Node[id].x[0] +
							Shape->Face[l].Normal[1] * Shape->Node[id].x[1] +
							Shape->Face[l].Normal[2] * Shape->Node[id].x[2] -
							Shape->Face[l].Normal[3] > FLT_EPSILON)
						{
							l = Shape->Face_Count;
							Shape->Node_Count--;
							Test = false;
					}}	}
					
					if (Test)
					{
						Shape->Face[i].Node_Count++;
						
						if (Shape->Face[i].Node_Count % _FACE_NODE_COUNT_STEP_ == 0)
						{
							Shape->Face[i].Node_Id = (long*)realloc(Shape->Face[i].Node_Id, (Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_) * sizeof(long));
							if (_APP_WARNING_MESSAGE(Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Node_Id\" to", Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_, 9)) return 1;
						}
						
						Shape->Face[i].Node_Id[Shape->Face[i].Node_Count - 1] = id;
						Edge_Vertex_id[j][Edge_Node_Count[j]] = id;
						Edge_Vertex_id[k][Edge_Node_Count[k]] = id;
						Edge_Node_Count[j]++;
						Edge_Node_Count[k]++;
					}
				}
			}
			
			if (Edge_Node_Count[j] > 1) // No more than two nodes (i.e., one edge) can be associated to a pair of planes (i,j)
			{
				k = Shape->Face_Count;
				
				if (Edge_Vertex_id[j][0] > Edge_Vertex_id[j][1]) { l = Edge_Vertex_id[j][0]; Edge_Vertex_id[j][0] = Edge_Vertex_id[j][1]; Edge_Vertex_id[j][1] = l; }
				
				Skip = false;
				for (l = 0; l < Shape->Edge_Count; l++)
				{
					if (Shape->Edge[l].Node_Id[0] == Edge_Vertex_id[j][0]) {
					if (Shape->Edge[l].Node_Id[1] == Edge_Vertex_id[j][1]) {
						Skip = true;
						
						Shape->Face[i].Edge_Count++;
						
						if (Shape->Face[i].Edge_Count % _FACE_EDGE_COUNT_STEP_ == 0)
						{
							Shape->Face[i].Edge_Id = (long*)realloc(Shape->Face[i].Edge_Id, (Shape->Face[i].Edge_Count + _FACE_EDGE_COUNT_STEP_) * sizeof(long));
							if (_APP_WARNING_MESSAGE(Shape->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Edge_Id\" to", Shape->Face[i].Edge_Count + _FACE_NODE_COUNT_STEP_, 10)) return 1;
						}
						
						Shape->Face[i].Edge_Id[Shape->Face[i].Edge_Count - 1] = l;
					}	}
				}
				if (!Skip)
				{
					id = Shape->Edge_Count;
					
					Shape->Edge_Count++;
					
					if (Shape->Edge_Count % _MESH_EDGE_COUNT_STEP_ == 0)
					{
						Shape->Edge = (struct Mesh_Edge*)realloc(Shape->Edge, (Shape->Edge_Count + _FACE_EDGE_COUNT_STEP_) * sizeof(struct Mesh_Edge));
						if (_APP_WARNING_MESSAGE(Shape->Edge, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Edge\" to", Shape->Edge_Count + _FACE_EDGE_COUNT_STEP_, 11)) return 1;
					}
					
					// Populate edge data structure
					Shape->Edge[id].Node_Id[0] = Edge_Vertex_id[j][0];
					Shape->Edge[id].Node_Id[1] = Edge_Vertex_id[j][1];
					
					Shape->Edge[id].Center[0] = 0.50 * (Shape->Node[Shape->Edge[id].Node_Id[0]].x[0] + Shape->Node[Shape->Edge[id].Node_Id[1]].x[0]);
					Shape->Edge[id].Center[1] = 0.50 * (Shape->Node[Shape->Edge[id].Node_Id[0]].x[1] + Shape->Node[Shape->Edge[id].Node_Id[1]].x[1]);
					Shape->Edge[id].Center[2] = 0.50 * (Shape->Node[Shape->Edge[id].Node_Id[0]].x[2] + Shape->Node[Shape->Edge[id].Node_Id[1]].x[2]);
					
					Shape->Edge[id].Direction[0] = Shape->Node[Shape->Edge[id].Node_Id[1]].x[0] - Shape->Node[Shape->Edge[id].Node_Id[0]].x[0];
					Shape->Edge[id].Direction[1] = Shape->Node[Shape->Edge[id].Node_Id[1]].x[1] - Shape->Node[Shape->Edge[id].Node_Id[0]].x[1];
					Shape->Edge[id].Direction[2] = Shape->Node[Shape->Edge[id].Node_Id[1]].x[2] - Shape->Node[Shape->Edge[id].Node_Id[0]].x[2];
					Shape->Edge[id].Length = sqrt(Shape->Edge[id].Direction[0] * Shape->Edge[id].Direction[0] + Shape->Edge[id].Direction[1] * Shape->Edge[id].Direction[1] + Shape->Edge[id].Direction[2] * Shape->Edge[id].Direction[2]);
					
					if (Shape->Edge[id].Length < FLT_EPSILON) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "Zero length edge detected", 0, 12); return 1; }
					else
					{
						Norm = 1.0 / Shape->Edge[id].Length;
						Shape->Edge[id].Direction[0] *= Norm;
						Shape->Edge[id].Direction[1] *= Norm;
						Shape->Edge[id].Direction[2] *= Norm;
					}
					// Populate edge data structure
					
					Shape->Face[i].Edge_Count++;
					
					if (Shape->Face[i].Edge_Count % _FACE_EDGE_COUNT_STEP_ == 0)
					{
						Shape->Face[i].Edge_Id = (long*)realloc(Shape->Face[i].Edge_Id, (Shape->Face[i].Edge_Count + _FACE_EDGE_COUNT_STEP_) * sizeof(long));
						if (_APP_WARNING_MESSAGE(Shape->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Edge_Id\" to", Shape->Face[i].Edge_Count + _FACE_NODE_COUNT_STEP_, 13)) return 1;
					}
					
					Shape->Face[i].Edge_Id[Shape->Face[i].Edge_Count - 1] = id;
				}
			}
		}}}}
		printf("_");
	}
	
	free(Edge_Vertex_id);
	free(Edge_Node_Count);
	
	// Sort facet nodes ids (i.e., clockwise) // This allows easy output in STL format
	
	Angle_Count = 0;
	for (i = 0; i < Shape->Face_Count; i++) { if (Angle_Count < Shape->Face[i].Node_Count) Angle_Count = Shape->Face[i].Node_Count; }

	Angle = (double*)calloc(Angle_Count, sizeof(double));
	if (_APP_WARNING_MESSAGE(Angle, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Angle\" to", Angle_Count, 14)) return 1;

	for (i = 0; i < Shape->Face_Count; i++)
	{
		if (Shape->Face[i].Node_Count < 3)
		{
			if (i < Shape->Face_Count - 1)
			{
				Shape->Face[i].Normal[0] = Shape->Face[Shape->Face_Count - 1].Normal[0];
				Shape->Face[i].Normal[1] = Shape->Face[Shape->Face_Count - 1].Normal[1];
				Shape->Face[i].Normal[2] = Shape->Face[Shape->Face_Count - 1].Normal[2];
				Shape->Face[i].Normal[3] = Shape->Face[Shape->Face_Count - 1].Normal[3];
				Shape->Face[i].Edge_Count = Shape->Face[Shape->Face_Count - 1].Edge_Count;
				Shape->Face[i].Node_Count = Shape->Face[Shape->Face_Count - 1].Node_Count;
				
				free(Shape->Face[i].Edge_Id);
				free(Shape->Face[i].Node_Id);
				
				Shape->Face[i].Edge_Id = Shape->Face[Shape->Face_Count - 1].Edge_Id;
				Shape->Face[i].Node_Id = Shape->Face[Shape->Face_Count - 1].Node_Id;
				
				i--;
			}
			else
			{
				free(Shape->Face[i].Edge_Id);
				free(Shape->Face[i].Node_Id);
			}
			
			Shape->Face_Count--;
		}
		else
		{
			Center[0] = Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
			Center[1] = Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
			Center[2] = Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
			
			for (j = 1; j < Shape->Face[i].Node_Count; j++)
			{
				Center[0] += Shape->Node[Shape->Face[i].Node_Id[j]].x[0];
				Center[1] += Shape->Node[Shape->Face[i].Node_Id[j]].x[1];
				Center[2] += Shape->Node[Shape->Face[i].Node_Id[j]].x[2];
			}
			
			Norm = 1.0 / (double)Shape->Face[i].Node_Count;
			
			Center[0] *= Norm;
			Center[1] *= Norm;
			Center[2] *= Norm;
			
			xx[0] = Shape->Node[Shape->Face[i].Node_Id[0]].x[0] - Center[0];
			xx[1] = Shape->Node[Shape->Face[i].Node_Id[0]].x[1] - Center[1];
			xx[2] = Shape->Node[Shape->Face[i].Node_Id[0]].x[2] - Center[2];
			
			Norm = 1.0 / sqrt(xx[0] * xx[0] + xx[1] * xx[1] + xx[2] * xx[2]);
			xx[0] *= Norm;
			xx[1] *= Norm;
			xx[2] *= Norm;
			
			yy[0] = xx[1] * Shape->Face[i].Normal[2] - xx[2] * Shape->Face[i].Normal[1];
			yy[1] = xx[2] * Shape->Face[i].Normal[0] - xx[0] * Shape->Face[i].Normal[2];
			yy[2] = xx[0] * Shape->Face[i].Normal[1] - xx[1] * Shape->Face[i].Normal[0];
			
			memset(&Angle[0], 0, Angle_Count * sizeof(double));
			Angle[0] = 0.0;
			
			for (j = 1; j < Shape->Face[i].Node_Count; j++)
			{
				vv[0] = Shape->Node[Shape->Face[i].Node_Id[j]].x[0] - Center[0];
				vv[1] = Shape->Node[Shape->Face[i].Node_Id[j]].x[1] - Center[1];
				vv[2] = Shape->Node[Shape->Face[i].Node_Id[j]].x[2] - Center[2];
				
				cc[0] = vv[0] * xx[0] + vv[1] * xx[1] + vv[2] * xx[2];
				cc[1] = vv[0] * yy[0] + vv[1] * yy[1] + vv[2] * yy[2];
				
				if (fabs(cc[0]) > fabs(cc[1]))
				{
					cc[1] /= cc[0];
					if (cc[0] > 0.0)
					{
						if (cc[1] > 0.0) Angle[j] = cc[1];
						else             Angle[j] = cc[1] + 8.0;
					}
					else                 Angle[j] = 4.0 - cc[1];
				}
				else
				{
					cc[0] /= cc[1];
					if (cc[1] > 0.0)     Angle[j] = 2.0 - cc[0];
					else                 Angle[j] = cc[0] + 6.0;
				}
			}
			
			for (j = 1; j < Shape->Face[i].Node_Count; j++)
			{
				l = j;
				for (k = j + 1; k < Shape->Face[i].Node_Count; k++) { if (Angle[k] < Angle[l]) l = k; }
				
				if (j != l)
				{
					m = Shape->Face[i].Node_Id[j];
					Shape->Face[i].Node_Id[j] = Shape->Face[i].Node_Id[l];
					Shape->Face[i].Node_Id[l] = m;
					
					// Angle[j] = Angle[l]; // the sorting is monotonous so Angle[j] will not anymore used. Hence, Angle[j] does not need to be updated
					Angle[l] = Angle[j];
				}
			}
		}
	}
	
	free(Angle);
	
	Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face\" to", Shape->Face_Count, 15)) return 1;
	
	// Compute facets surface
	
	Shape->Volume = 0.0;
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		Shape->Face[i].Surface = 0.0;
		
		xx[0] = Shape->Node[Shape->Face[i].Node_Id[1]].x[0] - Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
		xx[1] = Shape->Node[Shape->Face[i].Node_Id[1]].x[1] - Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
		xx[2] = Shape->Node[Shape->Face[i].Node_Id[1]].x[2] - Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
		for (k = 2; k < Shape->Face[i].Node_Count; k++)
		{
			yy[0] = Shape->Node[Shape->Face[i].Node_Id[k]].x[0] - Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
			yy[1] = Shape->Node[Shape->Face[i].Node_Id[k]].x[1] - Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
			yy[2] = Shape->Node[Shape->Face[i].Node_Id[k]].x[2] - Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
			
			vv[0] = xx[1] * yy[2] - xx[2] * yy[1];
			vv[1] = xx[2] * yy[0] - xx[0] * yy[2];
			vv[2] = xx[0] * yy[1] - xx[1] * yy[0];
			Shape->Face[i].Surface += 0.50 * sqrt(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);
			
			xx[0] = yy[0];
			xx[1] = yy[1];
			xx[2] = yy[2];
		}
		
		Shape->Volume += Shape->Face[i].Surface * Shape->Face[i].Normal[3] * _CONST_1_3;
	}
	
	return 0;
}

struct Threading_Mesh
{
	long Thread_id;
	long Thread_Count;
	
	struct Mesh* Shape;
	
	long* Valid_Facet;
	
	long Node_Count;
	double_3 *Node;
};
void* Mesh_ResolveFromFacets_Node_Thread(void* ptr)
{
	struct  Threading_Mesh* TH = (struct  Threading_Mesh*)ptr;
	
	long i, j, k, l;
	
	bool Skip;
	
	double A[3][3];
	
	TH->Node = (double_3*)calloc(_MESH_NODE_COUNT_STEP_, sizeof(double_3));
	if (_APP_WARNING_MESSAGE(TH->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"TH->Node\" to", _FACE_NODE_COUNT_STEP_, 1)) return NULL;
	
	TH->Valid_Facet = (long*)calloc(TH->Shape->Face_Count, sizeof(long));
	if (_APP_WARNING_MESSAGE(TH->Valid_Facet, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Valid_Facet\" to", TH->Shape->Face_Count, 1)) return NULL;
	
	memset(TH->Valid_Facet, 0, TH->Shape->Face_Count * sizeof(long));
	
	TH->Node_Count = 0;
	
	for (i = TH->Thread_id; i < TH->Shape->Face_Count; i+= TH->Thread_Count) { if(i%100==0) printf("%ld :: %ld      \r",i, TH->Shape->Face_Count);
	for (j = 0; j < i; j++) {
	for (k = 0; k < j; k++) {
		Skip = false;
		for (l = 0; l < TH->Node_Count; l++)
		{
			if (fabs(TH->Node[l][0] * TH->Shape->Face[i].Normal[0] +
					 TH->Node[l][1] * TH->Shape->Face[i].Normal[1] +
					 TH->Node[l][2] * TH->Shape->Face[i].Normal[2] -
					 TH->Shape->Face[i].Normal[3]) < FLT_EPSILON) {
			if (fabs(TH->Node[l][0] * TH->Shape->Face[j].Normal[0] +
					 TH->Node[l][1] * TH->Shape->Face[j].Normal[1] +
					 TH->Node[l][2] * TH->Shape->Face[j].Normal[2] -
					 TH->Shape->Face[j].Normal[3]) < FLT_EPSILON) {
			if (fabs(TH->Node[l][0] * TH->Shape->Face[k].Normal[0] +
					 TH->Node[l][1] * TH->Shape->Face[k].Normal[1] +
					 TH->Node[l][2] * TH->Shape->Face[k].Normal[2] -
					 TH->Shape->Face[k].Normal[3]) < FLT_EPSILON) {
				TH->Valid_Facet[i]++;
				TH->Valid_Facet[j]++;
				TH->Valid_Facet[k]++;
				Skip = true;
				l = TH->Node_Count;
			}}}
		}
		if (!Skip)
		{
			A[0][0] = TH->Shape->Face[i].Normal[0]; A[0][1] = TH->Shape->Face[i].Normal[1]; A[0][2] = TH->Shape->Face[i].Normal[2];
			A[1][0] = TH->Shape->Face[j].Normal[0]; A[1][1] = TH->Shape->Face[j].Normal[1]; A[1][2] = TH->Shape->Face[j].Normal[2];
			A[2][0] = TH->Shape->Face[k].Normal[0]; A[2][1] = TH->Shape->Face[k].Normal[1]; A[2][2] = TH->Shape->Face[k].Normal[2];
				
			if (!InvertMatrix_3x3(&A[0][0])) // otherwise no intersection is possible
			{
				TH->Node[TH->Node_Count][0] = A[0][0] * TH->Shape->Face[i].Normal[3] + A[0][1] * TH->Shape->Face[j].Normal[3] + A[0][2] * TH->Shape->Face[k].Normal[3];
				TH->Node[TH->Node_Count][1] = A[1][0] * TH->Shape->Face[i].Normal[3] + A[1][1] * TH->Shape->Face[j].Normal[3] + A[1][2] * TH->Shape->Face[k].Normal[3];
				TH->Node[TH->Node_Count][2] = A[2][0] * TH->Shape->Face[i].Normal[3] + A[2][1] * TH->Shape->Face[j].Normal[3] + A[2][2] * TH->Shape->Face[k].Normal[3];
				
				for (l = 0; l < TH->Shape->Face_Count; l++) {
				if ((l != i) && (l != j) && (l != k))   {
					if (TH->Shape->Face[l].Normal[0] * TH->Node[TH->Node_Count][0] +
						TH->Shape->Face[l].Normal[1] * TH->Node[TH->Node_Count][1] +
						TH->Shape->Face[l].Normal[2] * TH->Node[TH->Node_Count][2] -
						TH->Shape->Face[l].Normal[3] > FLT_EPSILON)
					{
						Skip = true;
						l = TH->Shape->Face_Count;
				}}	}
				
				if (!Skip)
				{
					TH->Node_Count++;
					
					TH->Valid_Facet[i]++;
					TH->Valid_Facet[j]++;
					TH->Valid_Facet[k]++;
					
					if (TH->Node_Count % _MESH_NODE_COUNT_STEP_ == 0)
					{
						TH->Node = (double_3*)realloc(TH->Node, (TH->Node_Count + _MESH_NODE_COUNT_STEP_) * sizeof(double_3));
						if (_APP_WARNING_MESSAGE(TH->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"TH->Node\" to", TH->Node_Count + _MESH_NODE_COUNT_STEP_, 0)) return NULL;
					}
				}
			}
		}
	}}}
	
	return NULL;
}
void* Mesh_ResolveFromFacets_FacetEdge_Thread(void* ptr)
{
	struct  Threading_Mesh* TH = (struct  Threading_Mesh*)ptr;
	
	long i, j, k, l, m;
	
	double Norm;
	double_2 cc, abs_cc;
	double_3 Center, xx, yy, vv;
	
	double* Angle;
	
	Angle = (double*)calloc(TH->Shape->Node_Count, sizeof(double));
	if (_APP_WARNING_MESSAGE(Angle, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets_FacetEdge_Thread", _WARNING_MESSAGE_KEY_CALLOC, "\"Angle\" to", TH->Shape->Node_Count, 0)) return NULL;
	
	for (i = TH->Thread_id; i < TH->Shape->Face_Count; i += TH->Thread_Count) { if(i%100==0) printf("%ld :: %ld      \r",i, TH->Shape->Face_Count);
		TH->Shape->Face[i].Node_Id = (long*)calloc(_FACE_NODE_COUNT_STEP_, sizeof(long));
		TH->Shape->Face[i].Edge_Id = (long*)calloc(_FACE_EDGE_COUNT_STEP_, sizeof(long));
		if (_APP_WARNING_MESSAGE(TH->Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets_FacetEdge_Thread", _WARNING_MESSAGE_KEY_CALLOC, "\"TH->Shape->Face[i].Node_Id\" to", _FACE_NODE_COUNT_STEP_, 0)) return NULL;
		if (_APP_WARNING_MESSAGE(TH->Shape->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets_FacetEdge_Thread", _WARNING_MESSAGE_KEY_CALLOC, "\"TH->Shape->Face[i].Edge_Id\" to", _FACE_EDGE_COUNT_STEP_, 0)) return NULL;
		
		// Detect nodes within the facet
		
		TH->Shape->Face[i].Node_Count = 0;
		
		for (j = 0; j < TH->Shape->Node_Count; j++)
		{
			if (fabs(TH->Shape->Face[i].Normal[0] * TH->Shape->Node[j].x[0] +
					 TH->Shape->Face[i].Normal[1] * TH->Shape->Node[j].x[1] +
					 TH->Shape->Face[i].Normal[2] * TH->Shape->Node[j].x[2] -
					 TH->Shape->Face[i].Normal[3]) < FLT_EPSILON)
			{
				TH->Shape->Face[i].Node_Id[TH->Shape->Face[i].Node_Count] = j;
				
				TH->Shape->Face[i].Node_Count++;
				
				if (TH->Shape->Face[i].Node_Count % _FACE_NODE_COUNT_STEP_ == 0)
				{
					TH->Shape->Face[i].Node_Id = (long*)realloc(TH->Shape->Face[i].Node_Id, (TH->Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_) * sizeof(long));
					if (_APP_WARNING_MESSAGE(TH->Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets_FacetEdge_Thread", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Node_Id\" to", TH->Shape->Face[i].Node_Count + _FACE_NODE_COUNT_STEP_, 0)) return NULL;
				}
			}
		}
		
		TH->Shape->Face[i].Node_Id = (long*)realloc(TH->Shape->Face[i].Node_Id, TH->Shape->Face[i].Node_Count * sizeof(long));
		if (_APP_WARNING_MESSAGE(TH->Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets_FacetEdge_Thread", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face[i].Node_Id\" to", TH->Shape->Face[i].Node_Count, 0)) return NULL;
		
		// Sort facet nodes ids (i.e., clockwise) // This allows easy edges definition and output in STL format
		
		Center[0] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[0];
		Center[1] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[1];
		Center[2] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[2];
		
		for (j = 1; j < TH->Shape->Face[i].Node_Count; j++)
		{
			Center[0] += TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[0];
			Center[1] += TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[1];
			Center[2] += TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[2];
		}
		
		Norm = 1.0 / (double)TH->Shape->Face[i].Node_Count;
		
		Center[0] *= Norm;
		Center[1] *= Norm;
		Center[2] *= Norm;
		
		xx[0] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[0] - Center[0];
		xx[1] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[1] - Center[1];
		xx[2] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[0]].x[2] - Center[2];
		
		Norm = 1.0 / sqrt(xx[0] * xx[0] + xx[1] * xx[1] + xx[2] * xx[2]);
		xx[0] *= Norm;
		xx[1] *= Norm;
		xx[2] *= Norm;
		
		yy[0] = xx[1] * TH->Shape->Face[i].Normal[2] - xx[2] * TH->Shape->Face[i].Normal[1];
		yy[1] = xx[2] * TH->Shape->Face[i].Normal[0] - xx[0] * TH->Shape->Face[i].Normal[2];
		yy[2] = xx[0] * TH->Shape->Face[i].Normal[1] - xx[1] * TH->Shape->Face[i].Normal[0];
		
		memset(&Angle[0], 0, TH->Shape->Face[i].Node_Count * sizeof(double));
		Angle[0] = 0.0;
		
		for (j = 1; j < TH->Shape->Face[i].Node_Count; j++)
		{
			vv[0] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[0] - Center[0];
			vv[1] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[1] - Center[1];
			vv[2] = TH->Shape->Node[TH->Shape->Face[i].Node_Id[j]].x[2] - Center[2];
			
			cc[0] = vv[0] * xx[0] + vv[1] * xx[1] + vv[2] * xx[2]; abs_cc[0] = fabs(cc[0]);
			cc[1] = vv[0] * yy[0] + vv[1] * yy[1] + vv[2] * yy[2]; abs_cc[1] = fabs(cc[1]);
			
			if (abs_cc[0] > abs_cc[1])
			{
				cc[1] /= abs_cc[0];
				if (cc[0] > 0.0)
				{
					if (cc[1] > 0.0) Angle[j] = cc[1];
					else             Angle[j] = cc[1] + 8.0;
				}
				else                 Angle[j] = 4.0 - cc[1];
			}
			else
			{
				cc[0] /= abs_cc[1];
				if (cc[1] > 0.0)     Angle[j] = 2.0 - cc[0];
				else                 Angle[j] = cc[0] + 6.0;
			}
		}
		
		for (j = 1; j < TH->Shape->Face[i].Node_Count; j++)
		{
			l = j;
			for (k = j + 1; k < TH->Shape->Face[i].Node_Count; k++) { if (Angle[k] < Angle[l]) l = k; }
				
			if (j != l)
			{
				m = TH->Shape->Face[i].Node_Id[j];
				TH->Shape->Face[i].Node_Id[j] = TH->Shape->Face[i].Node_Id[l];
				TH->Shape->Face[i].Node_Id[l] = m;
					
				// Angle[j] = Angle[l]; // the sorting is monotonous so Angle[j] will not anymore be used. Hence, Angle[j] does not need to be updated
				Angle[l] = Angle[j];
			}
		}
		
		// 
		
	}
	
	free(Angle);
	
	return NULL;
}
int   Mesh_ResolveFromFacets(struct Mesh* Shape, long Threads_Count) // Assume only Facets normal are defined
{
	long i, j, k;
	
	double Dvoid;
	
	bool Test;
	
	long   Vertex_Id;
	long_2 Edge_Vertex_id;
	double_3 xx, yy, vv;
	
	struct  Threading_Mesh* TH = NULL;
	pthread_t* Thread = NULL;
	
	// Threading Set-Up
	
	Thread = (pthread_t*)calloc(Threads_Count, sizeof(pthread_t));
	if (_APP_WARNING_MESSAGE(Thread, "Utility_Shape3DScan.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Thread_ST\" to", Threads_Count, 0)) return 1;
	
	TH = (struct Threading_Mesh*)calloc(Threads_Count, sizeof(struct Threading_Mesh));
	if (_APP_WARNING_MESSAGE(TH, "Utility_Shape3DScan.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"TH\" to", Threads_Count, 1)) { free(Thread); return 1; }
	
	for (i = 0; i < Threads_Count; i++)
	{
		TH[i].Thread_id = i;
		TH[i].Thread_Count = Threads_Count;
		
		TH[i].Shape = Shape;
	}
	
	// Free sub-structures of data
	
	if (Shape->Edge != NULL) { for (i = 0; i < Shape->Edge_Count; i++) { Mesh_Free_Edge(&(Shape->Edge[i])); } free(Shape->Edge); }
	if (Shape->Node != NULL) { for (i = 0; i < Shape->Node_Count; i++) { Mesh_Free_Node(&(Shape->Node[i])); } free(Shape->Node); }
	
	Shape->Edge_Count = 0;
	Shape->Node_Count = 0;
	
	// Remove equivalent factes
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		for (j = 0; j < i; j++)
		{
			if (fabs((Shape->Face[i].Normal[0] * Shape->Face[j].Normal[0] +
					  Shape->Face[i].Normal[1] * Shape->Face[j].Normal[1] +
					  Shape->Face[i].Normal[2] * Shape->Face[j].Normal[2]) * Shape->Face[i].Normal[3] - 
					  Shape->Face[j].Normal[3]) < FLT_EPSILON)
			{
				if (i < Shape->Face_Count - 1)
				{
					Shape->Face[i].Normal[0] = Shape->Face[Shape->Face_Count - 1].Normal[0];
					Shape->Face[i].Normal[1] = Shape->Face[Shape->Face_Count - 1].Normal[1];
					Shape->Face[i].Normal[2] = Shape->Face[Shape->Face_Count - 1].Normal[2];
					Shape->Face[i].Normal[3] = Shape->Face[Shape->Face_Count - 1].Normal[3];
					i--;
				}
				
				Shape->Face_Count--;
			}
		}
	}
	
	Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Face\" to", Shape->Face_Count, 0)) return 1;
	
	// Search valid Nodes
	
	Shape->Node = (struct Mesh_Node*)calloc(_MESH_NODE_COUNT_STEP_, sizeof(struct Mesh_Node));
	if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", _FACE_NODE_COUNT_STEP_, 1)) return 1;
	
	Shape->Node_Count = 0;
	
	// // Threading
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, Mesh_ResolveFromFacets_Node_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_THREAD_CREATE, "Mesh_ResolveFromFacets", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) Mesh_ResolveFromFacets_Node_Thread(&TH[i]);
#endif
	
	printf("                              \r");
	
	// // Collect Nodes
	
	for (i = 0; i < Threads_Count; i++)
	{
		for (j = 0; j < TH[i].Node_Count; j++)
		{
			Test = true;
			for (k = 0; k < Shape->Node_Count; k++)
			{
				xx[0] = Shape->Node[k].x[0] - TH[i].Node[j][0];
				xx[1] = Shape->Node[k].x[1] - TH[i].Node[j][1];
				xx[2] = Shape->Node[k].x[2] - TH[i].Node[j][2];
				if (xx[0] * xx[0] + xx[1] * xx[1] + xx[2] * xx[2] < FLT_EPSILON) { Test = false; k = Shape->Node_Count; }
			}
			if (Test)
			{
				Shape->Node[Shape->Node_Count].x[0] = TH[i].Node[j][0];
				Shape->Node[Shape->Node_Count].x[1] = TH[i].Node[j][1];
				Shape->Node[Shape->Node_Count].x[2] = TH[i].Node[j][2];
				Shape->Node_Count++;
				
				if (Shape->Node_Count % _MESH_NODE_COUNT_STEP_ == 0)
				{
					Shape->Node = (struct Mesh_Node*)realloc(Shape->Node, (Shape->Node_Count + _MESH_NODE_COUNT_STEP_) * sizeof(struct Mesh_Node));
					if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Node\" to", Shape->Node_Count + _MESH_NODE_COUNT_STEP_, 0)) return 1;
				}
			}
		}
	}
	
	if (Shape->Node_Count < 4) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "\"The Mesh is empty!\" Number of detected nodes is", Shape->Node_Count, 0); return 1; }
	
	Shape->Node = (struct Mesh_Node*)realloc(Shape->Node, Shape->Node_Count * sizeof(struct Mesh_Node));
	if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Node\" to", Shape->Node_Count + _MESH_NODE_COUNT_STEP_, 0)) return 1;
	
	// // Remove non-valid Facets
	
	for (i = 0; i < Shape->Face_Count; i++) { for (j = 1; j < Threads_Count; j++) TH[0].Valid_Facet[i] += TH[j].Valid_Facet[i]; }
	for (i = 0; i < Shape->Face_Count; i++)
	{
		if (TH[0].Valid_Facet[i] < 3)
		{
			if (i < Shape->Face_Count - 1)
			{
				Shape->Face[i].Normal[0] = Shape->Face[Shape->Face_Count - 1].Normal[0];
				Shape->Face[i].Normal[1] = Shape->Face[Shape->Face_Count - 1].Normal[1];
				Shape->Face[i].Normal[2] = Shape->Face[Shape->Face_Count - 1].Normal[2];
				Shape->Face[i].Normal[3] = Shape->Face[Shape->Face_Count - 1].Normal[3];
				
				TH[0].Valid_Facet[i] = TH[0].Valid_Facet[Shape->Face_Count - 1];
				i--;
			}
				
			Shape->Face_Count--;
		}
	}
	
	for (i = 0; i < Threads_Count; i++)
	{
		free(TH[i].Valid_Facet);
		free(TH[i].Node);
	}
	
	// Resolve Facets
	
	// // Threading
	
#if _THREADING_EXCLUDE == 'N'
	for (i = 0; i < Threads_Count; i++)
	{
		if (pthread_create(&Thread[i], NULL, Mesh_ResolveFromFacets_FacetEdge_Thread, &TH[i]))
		{
			_APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_THREAD_CREATE, "Mesh_ResolveFromFacets", i, 2);
			
			for (j = 0; j <= i; j++) pthread_cancel(Thread[j]);
			
			free(TH);
			free(Thread);
			
			return 1;
		}
	}
	for (i = 0; i < Threads_Count; i++) pthread_join(Thread[i], NULL);
#else
	for (i = 0; i < Threads_Count; i++) Mesh_ResolveFromFacets_FacetEdge_Thread(&TH[i]);
#endif
	
	printf("                              \r");
	
	// Resolve Edges
	
	Shape->Edge = (struct Mesh_Edge*)calloc(_MESH_EDGE_COUNT_STEP_, sizeof(struct Mesh_Edge));
	if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", _FACE_NODE_COUNT_STEP_, 1)) return 1;
	
	Shape->Edge_Count = 0;
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		Shape->Face[i].Edge_Id = (long*)calloc(Shape->Face[i].Node_Count, sizeof(long));
		if (_APP_WARNING_MESSAGE(Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face[i].Node_Id\" to", Shape->Face[i].Node_Count, 0)) return 1;
		
		Shape->Face[i].Edge_Count = 0;

		Vertex_Id = Shape->Face[i].Node_Id[Shape->Face[i].Node_Count - 1];
		for (j = 0; j < Shape->Face[i].Node_Count; j++)
		{
			if (Vertex_Id < Shape->Face[i].Node_Id[j])
			{
				Edge_Vertex_id[0] = Vertex_Id;
				Edge_Vertex_id[1] = Shape->Face[i].Node_Id[j];
			}
			else
			{
				Edge_Vertex_id[0] = Shape->Face[i].Node_Id[j];
				Edge_Vertex_id[1] = Vertex_Id;
			}
			Vertex_Id = Shape->Face[i].Node_Id[j];
			
			Test = true;
			for (k = 0; k < Shape->Edge_Count; k++)
			{
				if (Shape->Edge[k].Node_Id[0] == Edge_Vertex_id[0]) {
				if (Shape->Edge[k].Node_Id[1] == Edge_Vertex_id[1]) {
					Shape->Face[i].Edge_Id[Shape->Face[i].Edge_Count] = k;
					Shape->Face[i].Edge_Count++;
					if (Shape->Face[i].Edge_Count > Shape->Face[i].Node_Count) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "Number of edges higher than the number of nodes!", 0, 0); return 1; }
					Test = false;
					k = Shape->Edge_Count;
				}}
			}
			if (Test)
			{
				// Populate edge data structure
				
				Shape->Edge[Shape->Edge_Count].Node_Id[0] = Edge_Vertex_id[0];
				Shape->Edge[Shape->Edge_Count].Node_Id[1] = Edge_Vertex_id[1];
				
				Shape->Edge[Shape->Edge_Count].Center[0] = 0.50 * (Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[0] + Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[0]);
				Shape->Edge[Shape->Edge_Count].Center[1] = 0.50 * (Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[1] + Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[1]);
				Shape->Edge[Shape->Edge_Count].Center[2] = 0.50 * (Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[2] + Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[2]);
				
				Shape->Edge[Shape->Edge_Count].Direction[0] = Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[0] - Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[0];
				Shape->Edge[Shape->Edge_Count].Direction[1] = Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[1] - Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[1];
				Shape->Edge[Shape->Edge_Count].Direction[2] = Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[1]].x[2] - Shape->Node[Shape->Edge[Shape->Edge_Count].Node_Id[0]].x[2];
				Shape->Edge[Shape->Edge_Count].Length = sqrt(Shape->Edge[Shape->Edge_Count].Direction[0] * Shape->Edge[Shape->Edge_Count].Direction[0] + Shape->Edge[Shape->Edge_Count].Direction[1] * Shape->Edge[Shape->Edge_Count].Direction[1] + Shape->Edge[Shape->Edge_Count].Direction[2] * Shape->Edge[Shape->Edge_Count].Direction[2]);
				
				if (Shape->Edge[Shape->Edge_Count].Length < FLT_EPSILON) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "Zero length edge detected", 0, 0); return 1; }
				else
				{
					Dvoid = 1.0 / Shape->Edge[Shape->Edge_Count].Length;
					Shape->Edge[Shape->Edge_Count].Direction[0] *= Dvoid;
					Shape->Edge[Shape->Edge_Count].Direction[1] *= Dvoid;
					Shape->Edge[Shape->Edge_Count].Direction[2] *= Dvoid;
				}
				
				// Assign the new edge and upgrade data arrays
				
				Shape->Face[i].Edge_Id[Shape->Face[i].Edge_Count] = Shape->Edge_Count;
				
				Shape->Face[i].Edge_Count++;
				if (Shape->Face[i].Edge_Count > Shape->Face[i].Node_Count) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "Number of edges higher than the number of nodes!", 0, 0); return 1; }
				
				Shape->Edge_Count++;
				
				if (Shape->Edge_Count % _MESH_EDGE_COUNT_STEP_ == 0)
				{
					Shape->Edge = (struct Mesh_Edge*)realloc(Shape->Edge, (Shape->Edge_Count + _MESH_EDGE_COUNT_STEP_) * sizeof(struct Mesh_Edge));
					if (_APP_WARNING_MESSAGE(Shape->Edge, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Edge\" to", Shape->Edge_Count + _MESH_EDGE_COUNT_STEP_, 0)) return 1;
				}
			}
		}
		
		if (Shape->Face[i].Edge_Count != Shape->Face[i].Node_Count) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_VALUE, "Non-consistent number of edges and number of nodes!", 0, 0); return 1; }
	}
	
	Shape->Edge = (struct Mesh_Edge*)realloc(Shape->Edge, Shape->Edge_Count * sizeof(struct Mesh_Edge));
	if (_APP_WARNING_MESSAGE(Shape->Edge, "Utility_Mesh.cpp", "Mesh_ResolveFromFacets", _WARNING_MESSAGE_KEY_REALLOC, "\"Shape->Edge\" to", Shape->Edge_Count, 0)) return 1;
	
	// Compute facets surface
	
	Shape->Volume = 0.0;
	Shape->Surface = 0.0;
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		Shape->Face[i].Surface = 0.0;
		
		xx[0] = Shape->Node[Shape->Face[i].Node_Id[1]].x[0] - Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
		xx[1] = Shape->Node[Shape->Face[i].Node_Id[1]].x[1] - Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
		xx[2] = Shape->Node[Shape->Face[i].Node_Id[1]].x[2] - Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
		for (k = 2; k < Shape->Face[i].Node_Count; k++)
		{
			yy[0] = Shape->Node[Shape->Face[i].Node_Id[k]].x[0] - Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
			yy[1] = Shape->Node[Shape->Face[i].Node_Id[k]].x[1] - Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
			yy[2] = Shape->Node[Shape->Face[i].Node_Id[k]].x[2] - Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
			
			vv[0] = xx[1] * yy[2] - xx[2] * yy[1];
			vv[1] = xx[2] * yy[0] - xx[0] * yy[2];
			vv[2] = xx[0] * yy[1] - xx[1] * yy[0];
			Shape->Face[i].Surface += 0.50 * sqrt(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);
			
			xx[0] = yy[0];
			xx[1] = yy[1];
			xx[2] = yy[2];
		}
		
		Shape->Volume += Shape->Face[i].Surface * Shape->Face[i].Normal[3] * _CONST_1_3;
		Shape->Surface += Shape->Face[i].Surface;
	}
	
	// Threading Cleaning Set-Up
	
	free(TH);
	free(Thread);
	
	return 0;
}

int   Mesh_Simplify(struct Mesh** Shape, double Threshold, long Threads_Count)
{
	long i, j;
	
	long Id;
	double Surface, Volume_HD;
	
	struct Mesh* Shape_New;
	
	if (Threshold < FLT_EPSILON) return 0;
	
	Volume_HD = (*Shape)->Volume;

	do
	{
		Shape_New = (struct Mesh*)calloc(1, sizeof(struct Mesh));
		if (_APP_WARNING_MESSAGE(Shape_New, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape_New\" to", 1, 0)) return 1;
		
		Id = 0; Surface = (*Shape)->Face[Id].Surface;
		for (i = 1; i < (*Shape)->Face_Count; i++) { if (Surface > (*Shape)->Face[i].Surface) { Id = i; Surface = (*Shape)->Face[Id].Surface; } }
		
		if (fabs(Surface / (*Shape)->Surface) < Threshold)
		{
			Mesh_Reset(Shape_New);
			
			Shape_New->Face_Count = (*Shape)->Face_Count - 1;
			
			Shape_New->Face = (struct Mesh_Face*)calloc(Shape_New->Face_Count, sizeof(struct Mesh_Face));
			if (_APP_WARNING_MESSAGE(Shape_New->Face, "Utility_Shape3DScan.cpp", "Shape3DScan_Get_Mash", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape_New->Face\" to", 1, 0)) { free(Shape_New); return 1; }
			
			j = 0;
			for (i = 0; i < (*Shape)->Face_Count; i++)
			{
				if (i != Id)
				{
					Shape_New->Face[j].Normal[0] = (*Shape)->Face[i].Normal[0];
					Shape_New->Face[j].Normal[1] = (*Shape)->Face[i].Normal[1];
					Shape_New->Face[j].Normal[2] = (*Shape)->Face[i].Normal[2];
					Shape_New->Face[j].Normal[3] = (*Shape)->Face[i].Normal[3];
					j++;
				}
			}
			
			if (!Mesh_ResolveFromFacets(Shape_New, Threads_Count))
			{
				//if (fabs(Shape_New->Volume / (*Shape)->Volume - 1.0) < Threshold)
				if (fabs(Shape_New->Volume / Volume_HD - 1.0) < Threshold)
				{
					Mesh_Free_All((*Shape));
					(*Shape) = Shape_New;
				}
				else return 0;
			}
		}
		else return 0;
	} while (1);
	
	return 0;
}

// TXT in-out Mesh

int Mesh_WriteMesh_TXT(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char UnitOfMeasure[])
{
	long i, j;
	
	char Path_File[_Nch800];
	FILE* TXT;
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s.mesh%s", Path, Name, Name_Specification, _EXT_TXT);
	
	TXT = File_IO_Open(Path_File, "w");
	if (_APP_WARNING_MESSAGE(TXT, "Utility_Mesh.cpp", "Mesh_WriteMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
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
	fprintf(TXT, "ITEM: Face _ id normal_x normal_y normal_z surface vertex_count \"list of vertex_node_id\" edge_count \"list of edge_id\"\n");
	for (i = 0; i < Shape->Face_Count; i++)
	{
		fprintf(TXT, "%ld\t%lf\t%lf\t%lf\t%lf\t%ld", i + 1, Shape->Face[i].Normal[0], Shape->Face[i].Normal[1], Shape->Face[i].Normal[2], Shape->Face[i].Surface, Shape->Face[i].Node_Count);
		for (j = 0; j < Shape->Face[i].Node_Count; j++) fprintf(TXT, "\t%ld", Shape->Face[i].Node_Id[j] + 1);
		fprintf(TXT, "\t%ld", Shape->Face[i].Edge_Count);
		for (j = 0; j < Shape->Face[i].Edge_Count; j++) fprintf(TXT, "\t%ld", Shape->Face[i].Edge_Id[j] + 1);
		fprintf(TXT, "\n");
	}
	
	fclose(TXT);
	
	return 0;
}

struct Mesh* Mesh_ReadMesh_TXT(char Path[], char Name[])
{
	struct Mesh* Shape = NULL;
	
	long i, j;
	long Id;
	
	char Path_File[_Nch800];
	FILE* TXT;
	
	Shape = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(Shape, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape\" to", 1, 0)) return NULL;
	
	Mesh_Reset(Shape);
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s", Path, Name, _EXT_TXT);
	
	TXT = File_IO_Open(Path_File, "r");
	if (_APP_WARNING_MESSAGE(TXT, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); Mesh_Free_All(Shape); return NULL; }
	
	rewind(TXT);
	File_IO_JumpToNextLine(TXT);
	File_IO_JumpToNextLine(TXT);
	File_IO_JumpToNextLine(TXT);
	
	if (File_IO_Search_KeyPhrase(TXT, "#", "Solid-Mesh Volume:", false, false))     { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; } else fscanf_s(TXT, "%lf", &Shape->Volume);
	if (File_IO_Search_KeyPhrase(TXT, "#", "Solid-Mesh Surface:", false, false))    { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; } else fscanf_s(TXT, "%lf", &Shape->Surface);
	if (File_IO_Search_KeyPhrase(TXT, "#", "Solid-Mesh Node-Count:", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; } else fscanf_s(TXT, "%ld", &Shape->Node_Count);
	if (File_IO_Search_KeyPhrase(TXT, "#", "Solid-Mesh Edge-Count:", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; } else fscanf_s(TXT, "%ld", &Shape->Edge_Count);
	if (File_IO_Search_KeyPhrase(TXT, "#", "Solid-Mesh Face-Count:", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; } else fscanf_s(TXT, "%ld", &Shape->Face_Count);
	
	Shape->Node = (struct Mesh_Node*)calloc(Shape->Node_Count, sizeof(struct Mesh_Node)); if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", Shape->Node_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
	Shape->Edge = (struct Mesh_Edge*)calloc(Shape->Edge_Count, sizeof(struct Mesh_Edge)); if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", Shape->Edge_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
	Shape->Face = (struct Mesh_Face*)calloc(Shape->Face_Count, sizeof(struct Mesh_Face)); if (_APP_WARNING_MESSAGE(Shape->Node, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Node\" to", Shape->Face_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
	
	if (File_IO_Search_KeyPhrase(TXT, NULL, "ITEM: Node", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; }
	File_IO_JumpToNextLine(TXT);
	for (i = 0; i < Shape->Node_Count; i++)
	{
		fscanf_s(TXT, "%ld", &Id);
		if (Id - 1 != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Node Id mismatch\"", Id, i+1); Mesh_Free_All(Shape); return NULL; }
		fscanf_s(TXT, "%lf", &Shape->Node[i].x[0]);
		fscanf_s(TXT, "%lf", &Shape->Node[i].x[1]);
		fscanf_s(TXT, "%lf", &Shape->Node[i].x[2]);
	}
	
	if (File_IO_Search_KeyPhrase(TXT, NULL, "ITEM: Edge", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; }
	File_IO_JumpToNextLine(TXT);
	for (i = 0; i < Shape->Edge_Count; i++)
	{
		fscanf_s(TXT, "%ld", &Id);
		if (Id - 1 != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Edge Id mismatch\"", Id, i + 1); Mesh_Free_All(Shape); return NULL; }
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Center[0]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Center[1]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Center[2]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Direction[0]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Direction[1]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Direction[2]);
		fscanf_s(TXT, "%lf", &Shape->Edge[i].Length);
		fscanf_s(TXT, "%ld", &Shape->Edge[i].Node_Id[0]); Shape->Edge[i].Node_Id[0]--;
		fscanf_s(TXT, "%ld", &Shape->Edge[i].Node_Id[1]); Shape->Edge[i].Node_Id[1]--;
	}
	
	if (File_IO_Search_KeyPhrase(TXT, NULL, "ITEM: Face", false, false)) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Solid-Mesh Volume\"", 0, 0); Mesh_Free_All(Shape); return NULL; }
	File_IO_JumpToNextLine(TXT);
	for (i = 0; i < Shape->Face_Count; i++)
	{
		fscanf_s(TXT, "%ld", &Id);
		if (Id - 1 != i) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_VALUE, "\"Face Id mismatch\"", Id, i + 1); Mesh_Free_All(Shape); return NULL; }
		fscanf_s(TXT, "%lf", &Shape->Face[i].Normal[0]);
		fscanf_s(TXT, "%lf", &Shape->Face[i].Normal[1]);
		fscanf_s(TXT, "%lf", &Shape->Face[i].Normal[2]);
		fscanf_s(TXT, "%lf", &Shape->Face[i].Surface);
		fscanf_s(TXT, "%ld", &Shape->Face[i].Node_Count);
		Shape->Face[i].Node_Id = (long*)calloc(Shape->Face[i].Node_Count, sizeof(long)); if (_APP_WARNING_MESSAGE(Shape->Face[i].Node_Id, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face[i].Node_Id\" to", Shape->Face[i].Node_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
		for (j = 0; j < Shape->Face[i].Node_Count; j++) { fscanf_s(TXT, "%ld", &Shape->Face[i].Node_Id[j]); Shape->Face[i].Node_Id[j]--; }
		fscanf_s(TXT, "%ld", &Shape->Face[i].Edge_Count);
		Shape->Face[i].Edge_Id = (long*)calloc(Shape->Face[i].Edge_Count, sizeof(long)); if (_APP_WARNING_MESSAGE(Shape->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_ReadMesh_TXT", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face[i].Edge_Id\" to", Shape->Face[i].Edge_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
		for (j = 0; j < Shape->Face[i].Edge_Count; j++) { fscanf_s(TXT, "%ld", &Shape->Face[i].Edge_Id[j]); Shape->Face[i].Edge_Id[j]--; }
	}
	
	fclose(TXT);
	
	return 0;
}

// STL in-out Mesh

int Mesh_WriteSTL_ASCII(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[])
{
	long i, j;
	
	double_3 vv[3];
	
	char Path_File[_Nch800];
	FILE* STL;
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s%s", Path, Name, Name_Specification, _EXT_STL);
	
	STL = File_IO_Open(Path_File, "w");
	if (_APP_WARNING_MESSAGE(STL, "Utility_Mesh.cpp", "Mesh_WriteSTL_ASCI", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	fprintf(STL, "solid %s\n",Name);
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		vv[0][0] = Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
		vv[0][1] = Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
		vv[0][2] = Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
		
		vv[1][0] = Shape->Node[Shape->Face[i].Node_Id[1]].x[0];
		vv[1][1] = Shape->Node[Shape->Face[i].Node_Id[1]].x[1];
		vv[1][2] = Shape->Node[Shape->Face[i].Node_Id[1]].x[2];
		
		for (j = 2; j < Shape->Face[i].Node_Count; j++)
		{
			vv[2][0] = Shape->Node[Shape->Face[i].Node_Id[j]].x[0];
			vv[2][1] = Shape->Node[Shape->Face[i].Node_Id[j]].x[1];
			vv[2][2] = Shape->Node[Shape->Face[i].Node_Id[j]].x[2];
			
			fprintf(STL, "facet normal %e %e %e\n", Shape->Face[i].Normal[0], Shape->Face[i].Normal[1], Shape->Face[i].Normal[2]);
			fprintf(STL, "  outer loop\n");
			fprintf(STL, "    vertex  %e %e %e\n", vv[0][0], vv[0][1], vv[0][2]);
			fprintf(STL, "    vertex  %e %e %e\n", vv[1][0], vv[1][1], vv[1][2]);
			fprintf(STL, "    vertex  %e %e %e\n", vv[2][0], vv[2][1], vv[2][2]);
			fprintf(STL, "  endloop\n");
			fprintf(STL, "endfacet\n");
			
			vv[1][0] = vv[2][0];
			vv[1][1] = vv[2][1];
			vv[1][2] = vv[2][2];
		}
	}
	
	fprintf(STL, "endsolid %s\n", Name);
	
	fclose(STL);
	
	return 0;
}
int Mesh_WriteSTL_BINARY(struct Mesh* Shape, char Path[], char Name[], char Name_Specification[], char Header[]) // The header is truncated at 80 bytes (i.e., 80 char)
{
	long i, j;
	
	uint32_t Count_Triangle;
	float vv[3][3], nn[3];
	uint16_t att = 0;
	
	char Path_File[_Nch800];
	FILE* STL;
	
	if (sizeof(float) != 4)
	{
		_APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_WriteSTL_BINARY", _WARNING_MESSAGE_KEY_VALUE, "sizeof(float) != 4 byte (switch to ASCII STL)", 0, 0);
		Mesh_WriteSTL_ASCII(Shape, Path, Name, Name_Specification);
	}
	
	Count_Triangle = 0; for (i = 0; i < Shape->Face_Count; i++) Count_Triangle += Shape->Face[i].Node_Count - 2;
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s%s", Path, Name, Name_Specification, _EXT_STL);
	
	STL = File_IO_Open(Path_File, "wb");
	if (_APP_WARNING_MESSAGE(STL, "Utility_Mesh.cpp", "Mesh_WriteSTL_BINARY", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return 1; }
	
	fwrite(&Header[0], 1, 80, STL);
	fwrite(&Count_Triangle, sizeof(uint32_t), 1, STL);
	
	for (i = 0; i < Shape->Face_Count; i++)
	{
		nn[0] = (float)Shape->Face[i].Normal[0];
		nn[1] = (float)Shape->Face[i].Normal[1];
		nn[2] = (float)Shape->Face[i].Normal[2];
		
		vv[0][0] = (float)Shape->Node[Shape->Face[i].Node_Id[0]].x[0];
		vv[0][1] = (float)Shape->Node[Shape->Face[i].Node_Id[0]].x[1];
		vv[0][2] = (float)Shape->Node[Shape->Face[i].Node_Id[0]].x[2];
		
		vv[1][0] = (float)Shape->Node[Shape->Face[i].Node_Id[1]].x[0];
		vv[1][1] = (float)Shape->Node[Shape->Face[i].Node_Id[1]].x[1];
		vv[1][2] = (float)Shape->Node[Shape->Face[i].Node_Id[1]].x[2];
		
		for (j = 2; j < Shape->Face[i].Node_Count; j++)
		{
			vv[2][0] = (float)Shape->Node[Shape->Face[i].Node_Id[j]].x[0];
			vv[2][1] = (float)Shape->Node[Shape->Face[i].Node_Id[j]].x[1];
			vv[2][2] = (float)Shape->Node[Shape->Face[i].Node_Id[j]].x[2];
			
			fwrite(&nn[0], sizeof(float), 3, STL);
			fwrite(&vv[0][0], sizeof(float), 9, STL);
			fwrite(&att, sizeof(uint16_t), 1, STL);
			
			vv[1][0] = vv[2][0];
			vv[1][1] = vv[2][1];
			vv[1][2] = vv[2][2];
		}
	}
	
	fclose(STL);
	
	return 0;
}

struct Mesh* Mesh_ReadSTL_ASCII(char Path[], char Name[]) // Read / Set-up of the Facets ONLY !!!
{
	struct Mesh* Shape = NULL;
	
	double vv[3], qq[4];
	double Norm;
	
	char Path_File[_Nch800];
	FILE* STL;
	
	if (sizeof(float) != 4) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_WriteSTL_BINARY", _WARNING_MESSAGE_KEY_VALUE, "sizeof(float) != 4 byte (switch to ASCII STL)", 0, 0); return NULL; }
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s", Path, Name, _EXT_STL);
	
	STL = File_IO_Open(Path_File, "r");
	if (_APP_WARNING_MESSAGE(STL, "Utility_Mesh.cpp", "Mesh_ReadSTL_ASCII", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return NULL; }
	
	Shape = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(Shape, "Utility_Mesh.cpp", "Mesh_ReadSTL_ASCII", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape\" to", 1, 0)) return NULL;
	
	Mesh_Reset(Shape);
	
	Shape->Face = (struct Mesh_Face*)calloc(_MESH_FACE_COUNT_STEP_, sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ReadSTL_BINARY", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", _MESH_FACE_COUNT_STEP_, 0)) { Mesh_Free_All(Shape); return NULL; }
	
	qq[0] = 0.0;
	qq[1] = 0.0;
	qq[2] = 0.0;
	qq[3] = 0.0;
	
	Shape->Face_Count = 0;
	while (!feof(STL))
	{
		if (!File_IO_Search_KeyPhrase(STL, NULL, "facet normal", false, false))
		{
			fscanf_s(STL, "%lf", &Shape->Face[Shape->Face_Count].Normal[0]);
			fscanf_s(STL, "%lf", &Shape->Face[Shape->Face_Count].Normal[1]);
			fscanf_s(STL, "%lf", &Shape->Face[Shape->Face_Count].Normal[2]);
			
			if (!File_IO_Search_KeyPhrase(STL, NULL, "vertex", false, true))
			{
				fscanf_s(STL, "%lf", &vv[0]);
				fscanf_s(STL, "%lf", &vv[1]);
				fscanf_s(STL, "%lf", &vv[2]);
				
				Norm = 1.0 / sqrt(Shape->Face[Shape->Face_Count].Normal[0] * Shape->Face[Shape->Face_Count].Normal[0] + Shape->Face[Shape->Face_Count].Normal[1] * Shape->Face[Shape->Face_Count].Normal[1] + Shape->Face[Shape->Face_Count].Normal[2] * Shape->Face[Shape->Face_Count].Normal[2]);
				
				Shape->Face[Shape->Face_Count].Normal[0] *= Norm;
				Shape->Face[Shape->Face_Count].Normal[1] *= Norm;
				Shape->Face[Shape->Face_Count].Normal[2] *= Norm;
				Shape->Face[Shape->Face_Count].Normal[3] = Shape->Face[Shape->Face_Count].Normal[0] * vv[0] + Shape->Face[Shape->Face_Count].Normal[1] * vv[1] + Shape->Face[Shape->Face_Count].Normal[2] * vv[2];
				
				if ((fabs(Shape->Face[Shape->Face_Count].Normal[0] * qq[0] + Shape->Face[Shape->Face_Count].Normal[1] * qq[1] + Shape->Face[Shape->Face_Count].Normal[2] * qq[2] - 1.0) > FLT_EPSILON) || (fabs(Shape->Face[Shape->Face_Count].Normal[3] - qq[3]) > FLT_EPSILON))
				{
					qq[0] = Shape->Face[Shape->Face_Count].Normal[0];
					qq[1] = Shape->Face[Shape->Face_Count].Normal[1];
					qq[2] = Shape->Face[Shape->Face_Count].Normal[2];
					qq[3] = Shape->Face[Shape->Face_Count].Normal[3];
					Shape->Face_Count++;
					
					if (Shape->Face_Count % _MESH_FACE_COUNT_STEP_ == 0)
					{
						Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, (Shape->Face_Count + _MESH_FACE_COUNT_STEP_) * sizeof(struct Mesh_Face));
						if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ReadSTL_ASCII", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", (Shape->Face_Count + _MESH_FACE_COUNT_STEP_), 0)) { Mesh_Free_All(Shape); return NULL; }
					}
				}
			}
		}
	}
	
	Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ReadSTL_ASCII", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", Shape->Face_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
	
	fclose(STL);
	
	return Shape;
}
struct Mesh* Mesh_ReadSTL_BINARY(char Path[], char Name[]) // Read / Set-up of the Facets ONLY !!!
{
	struct Mesh* Shape = NULL;
	
	long i, ss;
	
	uint32_t Count_Triangle;
	float nn[3], vv[3];
	double qq[4];
	
	double Norm;
	
	char Path_File[_Nch800];
	FILE* STL;
	
	if (sizeof(float) != 4) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_WriteSTL_BINARY", _WARNING_MESSAGE_KEY_VALUE, "sizeof(float) != 4 byte (switch to ASCII STL)", 0, 0); return NULL; }
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s", Path, Name, _EXT_STL);
	
	STL = File_IO_Open(Path_File, "rb");
	if (_APP_WARNING_MESSAGE(STL, "Utility_Mesh.cpp", "Mesh_ReadSTL_BINARY", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return NULL; }
	
	fseek(STL, 80, SEEK_SET);
	
	fread(&Count_Triangle, sizeof(uint32_t), 1, STL);
	
	if (Count_Triangle < 4) return NULL;
	
	Shape = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(Shape, "Utility_Mesh.cpp", "Mesh_ReadSTL_BINARY", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape\" to", 1, 0)) return NULL;
	
	Mesh_Reset(Shape);
	
	Shape->Face = (struct Mesh_Face*)calloc(Count_Triangle, sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ReadSTL_BINARY", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", 1, 0)) { Mesh_Free_All(Shape); return NULL; }
	
	qq[0] = 0.0;
	qq[1] = 0.0;
	qq[2] = 0.0;
	qq[3] = 0.0;
	
	ss = 0;
	for (i = 0; i < (long)Count_Triangle; i++)
	{
		fread(&nn[0], sizeof(float), 3, STL);
		fread(&vv[0], sizeof(float), 3, STL);
		fseek(STL, 6 * sizeof(float) + sizeof(uint16_t), SEEK_CUR);
		
		Shape->Face[ss].Normal[0] = (double)nn[0];
		Shape->Face[ss].Normal[1] = (double)nn[1];
		Shape->Face[ss].Normal[2] = (double)nn[2];
		
		Norm = 1.0 / sqrt(Shape->Face[ss].Normal[0] * Shape->Face[ss].Normal[0] + Shape->Face[ss].Normal[1] * Shape->Face[ss].Normal[1] + Shape->Face[ss].Normal[2] * Shape->Face[ss].Normal[2]);
		
		Shape->Face[ss].Normal[0] *= Norm;
		Shape->Face[ss].Normal[1] *= Norm;
		Shape->Face[ss].Normal[2] *= Norm;
		Shape->Face[ss].Normal[3] = Shape->Face[ss].Normal[0] * (double)vv[0] + Shape->Face[ss].Normal[1] * (double)vv[1] + Shape->Face[ss].Normal[2] * (double)vv[2];
		
		if ((fabs(Shape->Face[ss].Normal[0] * qq[0] + Shape->Face[ss].Normal[1] * qq[1] + Shape->Face[ss].Normal[2] * qq[2] - 1.0) > FLT_EPSILON) || (fabs(Shape->Face[ss].Normal[3] - qq[3]) > FLT_EPSILON))
		{
			qq[0] = Shape->Face[ss].Normal[0];
			qq[1] = Shape->Face[ss].Normal[1];
			qq[2] = Shape->Face[ss].Normal[2];
			qq[3] = Shape->Face[ss].Normal[3];
			ss++;
		}
	}
	Shape->Face_Count = ss;
	
	Shape->Face = (struct Mesh_Face*)realloc(Shape->Face, Shape->Face_Count * sizeof(struct Mesh_Face));
	if (_APP_WARNING_MESSAGE(Shape->Face, "Utility_Mesh.cpp", "Mesh_ReadSTL_BINARY", _WARNING_MESSAGE_KEY_CALLOC, "\"Shape->Face\" to", Shape->Face_Count, 0)) { Mesh_Free_All(Shape); return NULL; }
	
	fclose(STL);
	
	return Shape;
}
struct Mesh* Mesh_ReadSTL(char Path[], char Name[])
{
	char Cvoid_t;
	
	char Path_File[_Nch800];
	FILE* STL;
	
	memset(&Path_File[0], 0, _Nch800 * sizeof(char));
	sprintf_s(&Path_File[0], _Nch800, "%s/%s%s", Path, Name, _EXT_STL);
	
	STL = File_IO_Open(Path_File, "r");
	if (_APP_WARNING_MESSAGE(STL, "Utility_Mesh.cpp", "Mesh_ReadSTL", _WARNING_MESSAGE_KEY_VALUE, Path_File, 0, 0)) { _USER_IO_WAIT(); return NULL; }
	
	Cvoid_t = 0; Cvoid_t = getc(STL); if (Cvoid_t == 's') {
	Cvoid_t = 0; Cvoid_t = getc(STL); if (Cvoid_t == 'o') {
	Cvoid_t = 0; Cvoid_t = getc(STL); if (Cvoid_t == 'l') {
	Cvoid_t = 0; Cvoid_t = getc(STL); if (Cvoid_t == 'i') {
	Cvoid_t = 0; Cvoid_t = getc(STL); if (Cvoid_t == 'd') {
		fclose(STL);
		return Mesh_ReadSTL_ASCII(Path, Name);
	}}}}}
	
	fclose(STL);
	return Mesh_ReadSTL_BINARY(Path, Name);
}

// Mesh Duplicate

struct Mesh* Mesh_Copy(struct Mesh* Shape)
{
	struct Mesh* ShapeCopy = NULL;
	
	long i, j;
	
	ShapeCopy = (struct Mesh*)calloc(1, sizeof(struct Mesh));
	if (_APP_WARNING_MESSAGE(ShapeCopy, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy\" to", 1, 0)) return NULL;
	
	ShapeCopy->Node_Count = Shape->Node_Count;
	ShapeCopy->Edge_Count = Shape->Edge_Count;
	ShapeCopy->Face_Count = Shape->Face_Count;

	ShapeCopy->Node = (struct Mesh_Node*)calloc(ShapeCopy->Node_Count, sizeof(struct Mesh_Node)); if (_APP_WARNING_MESSAGE(ShapeCopy->Node, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy->Node\" to", ShapeCopy->Node_Count, 0)) { Mesh_Free_All(ShapeCopy); return NULL; }
	ShapeCopy->Edge = (struct Mesh_Edge*)calloc(ShapeCopy->Edge_Count, sizeof(struct Mesh_Edge)); if (_APP_WARNING_MESSAGE(ShapeCopy->Node, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy->Node\" to", ShapeCopy->Edge_Count, 0)) { Mesh_Free_All(ShapeCopy); return NULL; }
	ShapeCopy->Face = (struct Mesh_Face*)calloc(ShapeCopy->Face_Count, sizeof(struct Mesh_Face)); if (_APP_WARNING_MESSAGE(ShapeCopy->Node, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy->Node\" to", ShapeCopy->Face_Count, 0)) { Mesh_Free_All(ShapeCopy); return NULL; }
	
	for (i = 0; i < ShapeCopy->Node_Count; i++)
	{
		ShapeCopy->Node[i].x[0] = Shape->Node[i].x[0];
		ShapeCopy->Node[i].x[1] = Shape->Node[i].x[1];
		ShapeCopy->Node[i].x[2] = Shape->Node[i].x[2];
	}
	for (i = 0; i < ShapeCopy->Edge_Count; i++)
	{
		ShapeCopy->Edge[i].Center[0] = Shape->Edge[i].Center[0];
		ShapeCopy->Edge[i].Center[1] = Shape->Edge[i].Center[1];
		ShapeCopy->Edge[i].Center[2] = Shape->Edge[i].Center[2];
		
		ShapeCopy->Edge[i].Direction[0] = Shape->Edge[i].Direction[0];
		ShapeCopy->Edge[i].Direction[1] = Shape->Edge[i].Direction[1];
		ShapeCopy->Edge[i].Direction[2] = Shape->Edge[i].Direction[2];
		
		ShapeCopy->Edge[i].Length = Shape->Edge[i].Length;

		ShapeCopy->Edge[i].Node_Id[0] = Shape->Edge[i].Node_Id[0];
		ShapeCopy->Edge[i].Node_Id[1] = Shape->Edge[i].Node_Id[1];
	}
	for (i = 0; i < ShapeCopy->Face_Count; i++)
	{
		ShapeCopy->Face[i].Normal[0] = Shape->Face[i].Normal[0];
		ShapeCopy->Face[i].Normal[1] = Shape->Face[i].Normal[1];
		ShapeCopy->Face[i].Normal[2] = Shape->Face[i].Normal[2];
		ShapeCopy->Face[i].Normal[3] = Shape->Face[i].Normal[3];
		
		ShapeCopy->Face[i].Surface = Shape->Face[i].Surface;
		
		ShapeCopy->Face[i].Edge_Count = Shape->Face[i].Edge_Count;
		ShapeCopy->Face[i].Node_Count = Shape->Face[i].Node_Count;
		
		ShapeCopy->Face[i].Edge_Id = (long*)calloc(ShapeCopy->Face[i].Edge_Count, sizeof(long)); if (_APP_WARNING_MESSAGE(ShapeCopy->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy->Face[i].Edge_Id\" to", ShapeCopy->Face[i].Edge_Count, 0)) { Mesh_Free_All(ShapeCopy); return NULL; }
		ShapeCopy->Face[i].Node_Id = (long*)calloc(ShapeCopy->Face[i].Node_Count, sizeof(long)); if (_APP_WARNING_MESSAGE(ShapeCopy->Face[i].Edge_Id, "Utility_Mesh.cpp", "Mesh_Copy", _WARNING_MESSAGE_KEY_CALLOC, "\"ShapeCopy->Face[i].Edge_Id\" to", ShapeCopy->Face[i].Node_Count, 0)) { Mesh_Free_All(ShapeCopy); return NULL; }
		
		for (j = 0; j < ShapeCopy->Face[i].Edge_Count; j++)ShapeCopy->Face[i].Edge_Id[j] = Shape->Face[i].Edge_Id[j];
		for (j = 0; j < ShapeCopy->Face[i].Node_Count; j++)ShapeCopy->Face[i].Node_Id[j] = Shape->Face[i].Node_Id[j];
	}

	return Shape;
}

// Processing Data

int Mesh_TestSite(struct Mesh* Mesh, double* Site)
{
    long i;
    
    for (i = 0; i < Mesh->Face_Count; i++)
    {
        if (Site[0] * Mesh->Face[i].Normal[0] +
            Site[1] * Mesh->Face[i].Normal[1] +
            Site[2] * Mesh->Face[i].Normal[2] -
            Mesh->Face[i].Normal[3] > FLT_EPSILON) return 0;
    }
    
    return 1;
} // return value: 0. site outside, 1 site inside
int Mesh_GetLeastPathLength(struct Mesh* Shape, double* Distance, double* Origin, double* Direction) // The Distance is expressed in 'Direction' units
{
    long i, i0;
    
	double rh, qh;
	
	double rr = 1.0;
	double qq = 1.0;
	double nn = 1.0;
    
    i0 = Shape->Face_Count;
    (*Distance) = 0.0;
    
    for (i = 0; i < Shape->Face_Count; i++)
    {
        rh = Shape->Face[i].Normal[3] - (Shape->Face[i].Normal[0] * Origin[0] + Shape->Face[i].Normal[1] * Origin[1] + Shape->Face[i].Normal[2] * Origin[2]);
        qh = Shape->Face[i].Normal[0] * Direction[0] + Shape->Face[i].Normal[1] * Direction[1] + Shape->Face[i].Normal[2] * Direction[2];
        if (rh > FLT_EPSILON)
        {
            if (qh > FLT_EPSILON)
            {
				rr = rh;
				qq = qh;
                i0 = i;
                break;
            }
        }
        else
        {
            if (qh > FLT_EPSILON) return 1;
        }
    }
    
    for (i = i0 + 1; i < Shape->Face_Count; i++)
    {
        rh = Shape->Face[i].Normal[3] - (Shape->Face[i].Normal[0] * Origin[0] + Shape->Face[i].Normal[1] * Origin[1] + Shape->Face[i].Normal[2] * Origin[2]);
        qh = Shape->Face[i].Normal[0] * Direction[0] + Shape->Face[i].Normal[1] * Direction[1] + Shape->Face[i].Normal[2] * Direction[2];
        if (rh > FLT_EPSILON)
        {
            if (qh > FLT_EPSILON)
            {
                if (rh * qq < rr * qh)
                {
					rr = rh;
					qq = qh;
                }
            }
        }
        else
        {
            if (qh > FLT_EPSILON) return 1;
        }
    }
    
	//rh = Direction[0] * Direction[0] + Direction[1] * Direction[1] + Direction[2] * Direction[2];
	//if (fabs(rh - 1.0) > FLT_EPSILON) (*Distance) = (rr * sqrt(rh)) / qq;
	//else                              (*Distance) = rr / qq;
	
	(*Distance) = rr / qq;
	
    return 0;
}

int Mesh_Size_Scale(struct Mesh* Mesh, double Scale)
{
	long i;
	
	double Scale_2 = Scale * Scale;
	double Scale_3 = Scale * Scale_2;
	
	Mesh->Volume  *= Scale_3;
	Mesh->Surface *= Scale_2;
	
	for (i = 0; i < Mesh->Node_Count; i++) { Mesh->Node[i].x[0]      *= Scale; Mesh->Node[i].x[1]      *= Scale; Mesh->Node[i].x[2]      *= Scale; }
	for (i = 0; i < Mesh->Edge_Count; i++) { Mesh->Edge[i].Center[0] *= Scale; Mesh->Edge[i].Center[1] *= Scale; Mesh->Edge[i].Center[2] *= Scale; Mesh->Edge[i].Length *= Scale; }
	for (i = 0; i < Mesh->Face_Count; i++) { Mesh->Face[i].Normal[3] *= Scale; Mesh->Face[i].Surface   *= Scale_2; }
	
	return 0;
}

int Mesh_Axes_Swap_and_Flip(struct Mesh* Mesh, char Xsf[], char Ysf[], char Zsf[])
{
	long i;
	
	long_3   id;
	double_3 sn;
	double_3 xx;
	
	if      ((Xsf[0] == 'x') || (Xsf[1] == 'x')) { id[0] = 0; if (Xsf[0] == '-') sn[0] = -1.0; else sn[0] = +1.0; }
	else if ((Xsf[0] == 'y') || (Xsf[1] == 'y')) { id[0] = 1; if (Xsf[0] == '-') sn[0] = -1.0; else sn[0] = +1.0; }
	else if ((Xsf[0] == 'z') || (Xsf[1] == 'z')) { id[0] = 2; if (Xsf[0] == '-') sn[0] = -1.0; else sn[0] = +1.0; }
	else { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Axes_Swap_and_Flip", _WARNING_MESSAGE_KEY_VALUE, "X axes match not found!", 0, 0); return 1; }
	
	if      ((Ysf[0] == 'x') || (Ysf[1] == 'x')) { id[1] = 0; if (Ysf[0] == '-') sn[1] = -1.0; else sn[1] = +1.0; }
	else if ((Ysf[0] == 'y') || (Ysf[1] == 'y')) { id[1] = 1; if (Ysf[0] == '-') sn[1] = -1.0; else sn[1] = +1.0; }
	else if ((Ysf[0] == 'z') || (Ysf[1] == 'z')) { id[1] = 2; if (Ysf[0] == '-') sn[1] = -1.0; else sn[1] = +1.0; }
	else { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Axes_Swap_and_Flip", _WARNING_MESSAGE_KEY_VALUE, "Y axes match not found!", 0, 0); return 1; }
	
	if      ((Zsf[0] == 'x') || (Zsf[1] == 'x')) { id[2] = 0; if (Zsf[0] == '-') sn[2] = -1.0; else sn[2] = +1.0; }
	else if ((Zsf[0] == 'y') || (Zsf[1] == 'y')) { id[2] = 1; if (Zsf[0] == '-') sn[2] = -1.0; else sn[2] = +1.0; }
	else if ((Zsf[0] == 'z') || (Zsf[1] == 'z')) { id[2] = 2; if (Zsf[0] == '-') sn[2] = -1.0; else sn[2] = +1.0; }
	else { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Axes_Swap_and_Flip", _WARNING_MESSAGE_KEY_VALUE, "Z axes match not found!", 0, 0); return 1; }
	
	if ((id[0] == id[1]) || (id[0] == id[2]) || (id[1] == id[2])) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Axes_Swap_and_Flip", _WARNING_MESSAGE_KEY_VALUE, "Axes match overflow!", 0, 0); return 1; }
	
	for (i = 0; i < Mesh->Node_Count; i++)
	{
		xx[0] = Mesh->Node[i].x[0];
		xx[1] = Mesh->Node[i].x[1];
		xx[2] = Mesh->Node[i].x[2];
		Mesh->Node[i].x[0] = sn[0] * xx[id[0]];
		Mesh->Node[i].x[1] = sn[1] * xx[id[1]];
		Mesh->Node[i].x[2] = sn[2] * xx[id[2]];
	}
	for (i = 0; i < Mesh->Edge_Count; i++)
	{
		xx[0] = Mesh->Edge[i].Center[0];
		xx[1] = Mesh->Edge[i].Center[1];
		xx[2] = Mesh->Edge[i].Center[2];
		Mesh->Edge[i].Center[0] = sn[0] * xx[id[0]];
		Mesh->Edge[i].Center[1] = sn[1] * xx[id[1]];
		Mesh->Edge[i].Center[2] = sn[2] * xx[id[2]];
		
		xx[0] = Mesh->Edge[i].Direction[0];
		xx[1] = Mesh->Edge[i].Direction[1];
		xx[2] = Mesh->Edge[i].Direction[2];
		Mesh->Edge[i].Direction[0] = sn[0] * xx[id[0]];
		Mesh->Edge[i].Direction[1] = sn[1] * xx[id[1]];
		Mesh->Edge[i].Direction[2] = sn[2] * xx[id[2]];
	}
	for (i = 0; i < Mesh->Face_Count; i++)
	{
		xx[0] = Mesh->Face[i].Normal[0];
		xx[1] = Mesh->Face[i].Normal[1];
		xx[2] = Mesh->Face[i].Normal[2];
		Mesh->Face[i].Normal[0] = sn[0] * xx[id[0]];
		Mesh->Face[i].Normal[1] = sn[1] * xx[id[1]];
		Mesh->Face[i].Normal[2] = sn[2] * xx[id[2]];
	}
	
	return 0;
}
int Mesh_Rotation(struct Mesh* Mesh, double* R)
{
	long i;
	
	double_3 xx;
	
	if (fabs(DeterminantMatrix_3x3(R) - 1.0) > FLT_EPSILON) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Rotation", _WARNING_MESSAGE_KEY_VALUE, "Improper rotation!", 0, 0); return 1; }
	
	for (i = 0; i < Mesh->Node_Count; i++)
	{
		SolveRotation_3D(&xx[0], &Mesh->Node[i].x[0], R);
		
		Mesh->Node[i].x[0] = xx[0];
		Mesh->Node[i].x[1] = xx[1];
		Mesh->Node[i].x[2] = xx[2];
	}
	for (i = 0; i < Mesh->Edge_Count; i++)
	{
		SolveRotation_3D(&xx[0], &Mesh->Edge[i].Center[0], R);
		
		Mesh->Edge[i].Center[0] = xx[0];
		Mesh->Edge[i].Center[1] = xx[1];
		Mesh->Edge[i].Center[2] = xx[2];
		
		SolveRotation_3D(&xx[0], &Mesh->Edge[i].Direction[0], R);
		
		Mesh->Edge[i].Direction[0] = xx[0];
		Mesh->Edge[i].Direction[1] = xx[1];
		Mesh->Edge[i].Direction[2] = xx[2];
	}
	for (i = 0; i < Mesh->Face_Count; i++)
	{
		SolveRotation_3D(&xx[0], &Mesh->Face[i].Normal[0], R);
		
		Mesh->Face[i].Normal[0] = xx[0];
		Mesh->Face[i].Normal[1] = xx[1];
		Mesh->Face[i].Normal[2] = xx[2];
	}
	
	return 0;
}
int Mesh_Rotation(struct Mesh* Mesh, double* R, struct Mesh* Mesh_Reference)
{
	long i;
	
	if (fabs(DeterminantMatrix_3x3(R) - 1.0) > FLT_EPSILON) { _APP_WARNING_MESSAGE(NULL, "Utility_Mesh.cpp", "Mesh_Rotation", _WARNING_MESSAGE_KEY_VALUE, "Improper rotation!", 0, 0); return 1; }
	
	for (i = 0; i < Mesh->Node_Count; i++) { SolveRotation_3D(&Mesh->Node[i].x[0], &Mesh_Reference->Node[i].x[0], R); }
	for (i = 0; i < Mesh->Edge_Count; i++) { SolveRotation_3D(&Mesh->Edge[i].Center[0], &Mesh_Reference->Edge[i].Center[0], R);
											 SolveRotation_3D(&Mesh->Edge[i].Direction[0], &Mesh_Reference->Edge[i].Direction[0], R); }
	for (i = 0; i < Mesh->Face_Count; i++) { SolveRotation_3D(&Mesh->Face[i].Normal[0], &Mesh_Reference->Face[i].Normal[0], R); }
	
	return 0;
}