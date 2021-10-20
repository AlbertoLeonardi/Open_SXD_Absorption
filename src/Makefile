# *****************************************************
# Variables to control Makefile operation

CXX = g++
#CXXFLAGS = -Ofast -W -lm -lpthread -I./	# -lcudart	## Release Mode
CXXFLAGS = -Wall -Wextra -g -lm -lpthread -I./	# -lcudart	## Debug Mode

#NVCC = nvcc
#CUDAFLAGS = -use_fast_math --machine 64 -arch=sm_30

# *****************************************************
# Significant groups of headers

OS_VS    = OS_VisualStudio.h Sys_SecureFunctionsReDefinition.h
OS_DEVC  = OS_DevCpp.h Sys_SecureFunctionsReDefinition.h
OS_LINUX = OS_Linux.h Sys_LinuxFunctionsReDefinition.h Sys_SecureFunctionsReDefinition.h

APP_OS_COMPILER = $(OS_LINUX) App_OS_Compiler.h
APP_ENVIRONMENT = $(APP_OS_COMPILER) App_Environment.h App_Setup_Compile.h App_Setup_Update.h App_System.h

SIGNALS_ALL = Signal_Warning.h Signal_Log.h

UTILITY_STATISTICS = Utility_Statistics.h External_pcg_basic.h

UTILITY_IO         = $(UTILITY_STATISTICS) $(SIGNALS_ALL) Utility_File_IO.h Utility_User_IO.h
UTILITY_IMAGE      = Utility_ImageFilters.h External_bitmap_image.hpp
UTILITY_ABSORPTION = Utility_LinearAbsorption.h Utility_CrystalAbsorption.h


#UTILITY_CUDA = Utility_CUDA_GPU.cuh

UTILITY_ALL = $(UTILITY_IO) $(UTILITY_IMAGE) $(UTILITY_ABSORPTION) $(UTILITY_CUDA) Utility_Math.h Utility_Mesh.h Utility_Shape3DScan.h Utility_ImageFilters.h Utility_Reflection_IO.h

SXD_ABSORPTION_DEPS_ALL = $(APP_ENVIRONMENT) $(SIGNALS_ALL) $(UTILITY_ALL) SXD_Absorption_InOut.h App_Log.h

OBJ = SXD_Absorption_InOut.o App_Log.o Signal_Log.o Signal_Warning.o External_pcg_basic.o Sys_SecureFunctionsReDefinition.o Sys_LinuxFunctionsReDefinition.o Utility_Statistics.o Utility_User_IO.o Utility_File_IO.o Utility_Math.o Utility_Mesh.o Utility_Shape3DScan.o Utility_ImageFilters.o Utility_LinearAbsorption.o Utility_CrystalAbsorption.o Utility_Reflection_IO.o Utility_Test.o

OBJ_CUDA = #Utility_CUDA_GPU.o

# ****************************************************
# Target

SXD_ABSORPTION: SXD_Absorption.o $(OBJ) $(OBJ_CUDA)
	$(CXX) -o SXD_Absorption SXD_Absorption.o $(OBJ) $(OBJ_CUDA) $(CXXFLAGS)

# ****************************************************
# Rules

SXD_Absorption.o: $(SXD_ABSORPTION_DEPS_ALL) SXD_Absorption.h
SXD_Absorption_InOut.o: $(APP_ENVIRONMENT) $(UTILITY_IO) $(UTILITY_IMAGE) SXD_Absorption_InOut.h App_Log.h Utility_LinearAbsorption.h Utility_Reflection_IO.h

App_Log.o: $(APP_ENVIRONMENT) $(UTILITY_IO) App_Log.h

Signal_Log.o: $(APP_ENVIRONMENT) Signal_Log.h
Signal_Warning.o: $(APP_ENVIRONMENT) Signal_Warning.h

External_pcg_basic.o: External_pcg_basic.h

Sys_SecureFunctionsReDefinition.o: $(APP_ENVIRONMENT)
Sys_LinuxFunctionsReDefinition.o: $(APP_ENVIRONMENT)

Utility_Statistics.o: $(APP_ENVIRONMENT) $(UTILITY_STATISTICS)

Utility_User_IO.o: $(APP_ENVIRONMENT) $(SIGNAL_ALL) Utility_User_IO.h
Utility_File_IO.o: $(APP_ENVIRONMENT) $(UTILITY_IO)

Utility_Math.o: $(APP_ENVIRONMENT) Utility_Math.h

Utility_Mesh.o: $(APP_ENVIRONMENT) $(UTILITY_IO) Utility_Mesh.h Utility_Math.h
Utility_Shape3DScan.o: $(APP_ENVIRONMENT) $(UTILITY_IO) $(UTILITY_IMAGE) Utility_Shape3DScan.h
Utility_ImageFilters.o: $(APP_ENVIRONMENT) $(UTILITY_IO) $(UTILITY_IMAGE)

Utility_LinearAbsorption.o: $(APP_ENVIRONMENT) $(UTILITY_IO) $(UTILITY_ABSORPTION)
Utility_CrystalAbsorption.o: $(APP_ENVIRONMENT) $(UTILITY_IO) Utility_CrystalAbsorption.h

Utility_Reflection_IO.o: $(APP_ENVIRONMENT) $(UTILITY_IO) Utility_Reflection_IO.h Utility_CrystalAbsorption.h

Utility_Test.o: $(APP_ENVIRONMENT) Utility_Test.h

#Utility_CUDA_GPU.o: $(APP_ENVIRONMENT) $(UTILITY_IO) Utility_CUDA_GPU.cuh
#	$(NVCC) -c Utility_CUDA_GPU.cu $(CUDAFLAGS)


