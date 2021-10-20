// CUDA: Utility Functions :: Global GPU functionalities
//
// 

#ifndef _INCLUDE_UTILITY_CUDA_GPU
#define _INCLUDE_UTILITY_CUDA_GPU

#include <App_Environment.h>

#include <Signal_Warning.h>
#include <Utility_User_IO.h>

#ifdef _NVIDIA_CUDA_SUPPORT

struct GPU_Device
{
	bool   Lock;									// GPU lock (true --> GPU device taken) 
	char   Name[_Nch100];							// GPU device Name
	long   Id;										// GPU device Id
};

__device__ double atomicAdd_double(double* address, double val);

__host__ int  CUDA_GPU_GetDevice(struct GPU_Device* GPU);
__host__ void CUDA_GPU_FreeDevice(struct GPU_Device *GPU);
__host__ void CUDA_GPU_SyncronizeWorkFlow();

__host__ int CUDA_GPU_GetThreadsSetUp(int Device_Id, long *nBlocks, long *nThreadsPerBlock);

#endif

#endif