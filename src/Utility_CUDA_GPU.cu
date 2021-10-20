// CUDA: Utility Functions :: Solve Debye Scattering Equation From PDF
//
// 
// CUDA: Utility Functions :: Global GPU functionalities
//
// 

#include "Utility_CUDA_GPU.cuh"

#ifdef _NVIDIA_CUDA_SUPPORT

__device__ double atomicAdd_double(double* address, double val)
{
	unsigned long long int* address_as_ull = (unsigned long long int*)address;
	unsigned long long int old = *address_as_ull, assumed;
	do {
		assumed = old;
		old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val + __longlong_as_double(assumed)));
	} while (assumed != old);
	return __longlong_as_double(old);
}

__host__ int  CUDA_GPU_GetDevice(struct GPU_Device* GPU)
{
	int nDevices;
	cudaDeviceProp propDevice;
	
	(*GPU).Lock = false;
	(*GPU).Id = -1;

	if (!strcmp(&(*GPU).Name[0], "None")) return 1;
	
	cudaGetDeviceCount(&nDevices);
	
	if (!strcmp(&(*GPU).Name[0], "Any"))
	{
		for ((*GPU).Id = 0; (*GPU).Id < nDevices; (*GPU).Id++)
		{
			if (cudaSetDevice((*GPU).Id) == cudaSuccess) { (*GPU).Lock = true; return 0; }
		}
		
		_APP_WARNING_MESSAGE(NULL, "Utility_CUDA_GPU", "CUDA_GPU_GetDevice", _WARNING_MESSAGE_KEY_CUDA_DEVICE, "No GPU Available!", 0, 0);
		_USER_IO_WAIT();
		return 1;
	}
	else
	{
		for ((*GPU).Id = 0; (*GPU).Id < nDevices; (*GPU).Id++)
		{
			cudaGetDeviceProperties(&propDevice, (*GPU).Id);
			if (!strcmp(propDevice.name, (*GPU).Name))
			{
				if (cudaSetDevice((*GPU).Id) == cudaSuccess) { (*GPU).Lock = true; return 0; }
				else
				{
					_APP_WARNING_MESSAGE(NULL, "Utility_CUDA_GPU", "CUDA_GPU_GetDevice", _WARNING_MESSAGE_KEY_CUDA_DEVICE, "Requested GPU Not Available!", 0, 0);
					_USER_IO_WAIT();
					return 1;
				}
			}
		}
		
		_APP_WARNING_MESSAGE(NULL, "Utility_CUDA_GPU", "CUDA_GPU_GetDevice", _WARNING_MESSAGE_KEY_CUDA_DEVICE, "Requested GPU Not Found!", 0, 0);
		_USER_IO_WAIT();
		return 1;
	}
}
__host__ void CUDA_GPU_FreeDevice(struct GPU_Device* GPU)
{
	if((*GPU).Lock) cudaDeviceReset();
	return;
}
__host__ void CUDA_GPU_SyncronizeWorkFlow()
{
	cudaDeviceSynchronize();
	return;
}

__host__ int CUDA_GPU_GetThreadsSetUp(int Device_Id, long *nBlocks, long *nThreadsPerBlock)
{
	cudaDeviceProp propDevice;
	
	if(cudaGetDeviceProperties(&propDevice, Device_Id)!=cudaSuccess) return 1;
	
	*nBlocks = ((long)propDevice.multiProcessorCount * (long)propDevice.maxThreadsPerMultiProcessor) / (long)propDevice.maxThreadsPerBlock;
	*nThreadsPerBlock = (long)propDevice.maxThreadsPerBlock;
	
	return 0;
}

#endif