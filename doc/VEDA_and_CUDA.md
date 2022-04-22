# VEDA vs CUDA

## Overview
VEDA provides API inspired by the widely used CUDA Driver API. It builds upon AVEO and enables easy porting existing CUDA (and other hybrid) applications to VE. VEDA uses CUDA's design principles and maps these onto the execution model of AVEO. It supports multiple devices, NUMA nodes, asynchronous execution queues, and many more closely mirroring CUDA's best practices that have been tried and tested for over a decade.

Similar to CUDA, VEDA enumerates the physical devices and NUMA nodes starting from zero, whereby NUMA nodes have always adjacent indices. The environmental variable VEDA_VISIBLE_DEVICES determines which devices should be visible within the application. In contrast to CUDA, VEDA only supports a single device context at a time, which maintains all loaded libraries/modules and allocations.

@section vedadiffpoint Differences
VEDA have most of the similar APIs like CUDA however as the programming model of the SX-Aurora differs from NVIDIA GPUs, there are some differences.
##Differences between VEDA and CUDA Driver API:
**1.**      In VEDA, all function calls start with veda* instead of cu* and vera* instead of cuda* for VERA runtime APIs.

**2.**      Objects start with VEDA* instead of CU* and vera* instead of cuda*

**3.**      Similar to CUDA Runtime API, calls from VEDA and VERA can be mixed.

**4.**      VEDA uses the environment variable VEDA_VISIBLE_DEVICES in contrast to CUDA_VISIBLE_DEVICES.

**5.**      We required to call vedaExit() at the end of the application, to ensure that no dead device processes stay alive along with the call vedaInit(0) in the beginning like CUDA.

**6.**      VEDA/VERA supports asynchronous malloc and free, VEDA supports asynchronous vedaMemAllocAsync and vedaMemFreeAsync. They can be used like the synchronous calls, and don't need to synchronize the execution between device and host.

**7.**      VEDA streams differ from CUDA streams. As we have two modes to create Context in VEDA, See chapter "Two modes of VEDA Context" for more details.

**8.**      Due to programming model difference, launching kernels looks different:
~~~c
// Device Code -------------------------------------------------------------
extern "C" void my_function(float myFloat, uint8_t myUnsignedChar, float* array) {
        ...
}

// Host Code in C ------------------------------------------------------------
float myFloat;
uint8_t myUnsignedChar;
VEDAargs args;
vedaArgsCreate(&args);

// Scheme: vedaArgsSet[TYPE](&args, [PARAM_INDEX], [VARIABLE]);
vedaArgsSetF32(args, 0, myFloat);
vedaArgsSetU8(args, 1, myUnsignedChar);

// Copy entire arrays as function parameter
float array[32];
vedaArgsSetStack(args, 2, array, VEDA_ARGS_INTENT_INOUT, sizeof(array));

VEDAmodule mod;
VEDAfunction func;
vedaModuleLoad(&mod, "mylib.vso");
vedaModuleGetFunctions(&func, mod, "my_function");

// Kernel Call Version 1: allows to reuse VEDAargs object
VEDAstream stream = 0;
vedaLaunchKernel(func, stream, args);

// args are not allowed to be destroyed before synchronizing!
vedaStreamSynchronize(stream);
vedaArgsDestroy(&args);

// Kernel Call Version 2: vedaLaunchKernelEx(func, stream, args, 1, checkResult=0) automatically destroys VEDAargs object after execution, VEDAargs object can't be reused for other calls.

// CPP ---------------------------------------------------------------------
vedaLaunchKernel(func, stream, myFloat, myUnsignedChar, VEDAstack(array, VEDA_ARGS_INTENT_INOUT, sizeof(array)));
~~~

**9.**      VEDAdeviceptr needs to be dereferenced first on device side:
~~~c
// Host Code ---------------------------------------------------------------
VEDAdeviceptr ptr;
vedaMemAllocAsync(&ptr, sizeof(float) * cnt);
vedaLaunchKernel(func, 0, ptr, cnt);
vedaMemFreeAsync(ptr);

// Device Code -------------------------------------------------------------
void mykernel(VEDAdeviceptr vptr, size_t cnt) {
        float* ptr;
        vedaMemPtr(&ptr, vptr);

        for(size_t i = 0; i < cnt; i++)
                ptr[cnt] = ...;
}
~~~


@section vedaunique VEDA/VERA Unique Features
**1.	Delayed Memory Allocation:** In contrast to CUDA, the VE's memory allocator is operated by the device itself which gives VEDA the Opportunity of enabling asynchronous memory allocations from the host. This can reduce unnecessary synchronizations between the host and the device.

If VEDA pointers are allocated with a size of 0, only a virtual pointer will be created that can be allocated later using vedaMemAlloc(...) inside the device code. This enables to model execution graphs, without the need to know the required size before calling the device code. Virtual pointers behave identical to normal pointers, i.e. they can be dereferenced using offsets A = B + offset or A = &B[offset].
~~~c
// Host Code ---------------------------------------------------------------
VEDAdeviceptr vptr;
vedaMemAllocAsync(&vptr, 0, 0);
vedaLaunchKernel(func, 0, vptr, cnt);
vedaMemcpyDtoHAsync(host, vptr, sizeof(float) * cnt, 0);
vedaMemFreeAsync(vptr, 0);

// Device Code -------------------------------------------------------------
void mykernel(VEDAdeviceptr vptr, size_t cnt) {
        float* ptr;
        vedaMemAllocPtr((void**)&ptr, vptr, cnt * sizeof(float));

        for(size_t i = 0; i < cnt; i++)
                ptr[cnt] = ...;
}
~~~
VEDA does not need to allocate memory from the host, but can do that directly from the device. For this, the host only needs to create an empty VEDAdeviceptr.

**2.**	We can allow to fetch the power consumption (in W) and temperature (in C) from vedaDeviceGetPower(float* power, VEDAdevice dev) and vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev) functions. 

**3.**	Kernel api vedaLaunchKernelEx(func, stream, argsObject, 1, checkResult=0) can automatically destroy VEDAargs argObject i.e  VEDAargs argObject can't be reused for other calls.

**4.** Also vedaLaunchKernelEx api have fifth parameter as checkResult which is a optional parameter(default value is 0) and can be set as 1 when we want to have return value of device function, which we can get by calling vedaCtxSynchronize() or vedaStreamSynchronize() after it.
~~~c
vedaLaunchKernel(func, stream, args);
int return = vedaStreamSynchronize(stream);
~~~

### Two modes of VEDA context:
The context can be generated in two different modes: VEDA_CONTEXT_MODE_OMP (default) and VEDA_CONTEXT_MODE_SCALAR.
The first mode creates one stream (execution queue) that controls all threads through OpenMP.
The second mode creates one stream per core, allowing to directly address each core separately from within the host.

In CUDA streams can be used to create different execution queues, to overlap compute with memcopy while VEDA supports two stream modes which differ from the CUDA behavior. These modes can be defined by vedaCtxCreate(&ctx, MODE, device) API in VEDA.
1.      **VEDA_CONTEXT_MODE_OMP (default)**: All cores will be assigned to the default stream (=0). This mode only supports a single stream.
2.      **VEDA_CONTEXT_MODE_SCALAR**: Every core gets assigned to a different stream. This mode allows to use each core independently with different streams. Use the function vedaCtxStreamCnt(&streamCnt) to determine how many streams are available.
Both methods use the environment variable VE_OMP_NUM_THREADS to determine the maximal number of cores that get use for either mode. If the environment variable is not set, VEDA uses all available cores of the hardware.

### Advanced VEDA C++ Ptr
We have VEDAdeviceptr for pointer variable type in VEDA however we can also use VEDAptr<typename> while using C++ that gives us more directly control over the VEDAdeviceptr, i.e. we can use vptr.size(), vptr.device(), ... . The typename is used to automatically determine the correct offsets when executing vptr += offset;.

@section vedaapisamecuda VEDA APIs similar to CUDA
VEDA API			| CUDA API                        | Remarks                        |  
------------------------------- | ------------------------------- | ------------------------------- 
make_veraExtent()	|	make_cudaExtent()	|                         |
make_veraPitchedPtr()	|	make_cudaPitchedPtr()	|                         |
vedaArgsCreate()	|	NA	|                         |
vedaArgsDestroy()	|	NA	|                         |
vedaArgsSetF32()	|	NA	|                         |
vedaArgsSetF64()	|	NA	|                         |
vedaArgsSetHMEM()	|	NA	|                         |
vedaArgsSetI16()	|	NA	|                         |
vedaArgsSetI32()	|	NA	|                         |
vedaArgsSetI64()	|	NA	|                         |
vedaArgsSetI8()	|	NA	|                         |
vedaArgsSetPtr()	|	NA	|                         |
vedaArgsSetStack()	|	NA	|                         |
vedaArgsSetU16()	|	NA	|                         |
vedaArgsSetU32()	|	NA	|                         |
vedaArgsSetU64()	|	NA	|                         |
vedaArgsSetU8()	|	NA	|                         |
vedaArgsSetVPtr()	|	NA	|                         |
vedaCtxCreate()	|	cuCtxCreate()	|                         |
vedaCtxDestroy()	|	cuCtxDestroy()	|[VEDA] Multiple threads using same context, will not be destroyed by calling it until no other thread is using it.|
vedaCtxGet()	|	NA	|                         |
vedaCtxGetApiVersion()	|	cuCtxGetApiVersion()	|                         |
vedaCtxGetCurrent()	|	cuCtxGetCurrent()	|                         |
vedaCtxGetDevice()	|	cuCtxGetDevice()	|                         |
vedaCtxPopCurrent()	|	cuCtxPopCurrent()	|                         |
vedaCtxPushCurrent()	|	cuCtxPushCurrent()	|                         |
vedaCtxSetCurrent()	|	cuCtxSetCurrent()	|                         |
vedaCtxStreamCnt()	|	NA	|                         |
vedaCtxSynchronize()	|	cuCtxSynchronize()	|                         |
vedaDeviceDistance()	|	NA	|                         |
vedaDeviceGet()	|	cuDeviceGet()	|                         |
vedaDeviceGetAttribute()	|	cuDeviceGetAttribute()	|                         |
vedaDeviceGetAVEOId()	|	NA	|                         |
vedaDeviceGetCount()	|	cuDeviceGetCount()	|                         |
vedaDeviceGetCurrent()	|	NA	|                         |
vedaDeviceGetCurrentEdge()	|	NA	|                         |
vedaDeviceGetName()	|	cuDeviceGetName()	|                         |
vedaDeviceGetNUMAId()	|	NA	|                         |
vedaDeviceGetPhysicalId()	|	NA	|                         |
vedaDeviceGetPower()	|	NA	|                         |
vedaDeviceGetTemp()	|	NA	|                         |
vedaDeviceGetVoltage()	|	NA	|                         |
vedaDeviceGetVoltageEdge()	|	NA	|                         |
vedaDevicePrimaryCtxGetState()	|	cuDevicePrimaryCtxGetState()	|                         |
vedaDevicePrimaryCtxRelease()	|	cuDevicePrimaryCtxRelease()	|                         |
vedaDevicePrimaryCtxReset()	|	cuDevicePrimaryCtxReset()	|                         |
vedaDevicePrimaryCtxRetain()	|	cuDevicePrimaryCtxRetain()	|                         |
vedaDevicePrimaryCtxSetFlags()	|	cuDevicePrimaryCtxSetFlags()	|                         |
vedaDeviceTotalMem()	|	cuDeviceTotalMem()	|                         |
vedaDriverGetVersion()	|	cuDriverGetVersion()	|                         |
vedaExit()	|	NA	| [VEDA]Required to call at the end of the application, to ensure that no dead device processes stay alive.|
vedaInit()	|	cuInit()	|                         |
vedaLaunchHostFunc()	|	cuLaunchHostFunc()	|                         |
vedaLaunchKernel()	|	cuLaunchKernel()	|                         |
vedaLaunchKernelEx()	|	NA	|[VEDA] Automatically destroy VEDAargs argObject and optional fifth parameter can be set as 1 when we want to have return value of device function.|
vedaMemAlloc()	|	cuMemAlloc()	|                         |
vedaMemAllocAsync()	|	cuMemAllocAsync()	|                         |
vedaMemAllocHost()	|	cuMemAllocHost()	|                         |
vedaMemAllocPitch()	|	cuMemAllocPitch()	|                         |
vedaMemAllocPitchAsync()	|	NA	|                         |
vedaMemcpy()	|	cuMemcpy()	|                         |
vedaMemcpyAsync()	|	cuMemcpyAsync()	|                         |
vedaMemcpyDtoD()	|	cuMemcpyDtoD()	|                         |
vedaMemcpyDtoDAsync()	|	cuMemcpyDtoDAsync()	|                         |
vedaMemcpyDtoH()	|	cuMemcpyDtoH()	|                         |
vedaMemcpyDtoHAsync()	|	cuMemcpyDtoHAsync()	|                         |
vedaMemcpyHtoD()	|	cuMemcpyHtoD()	|                         |
vedaMemcpyHtoDAsync()	|	cuMemcpyHtoDAsync()	|                         |
vedaMemFree()	|	cuMemFree()	|                         |
vedaMemFreeAsync()	|	cuMemFreeAsync()	|                         |
vedaMemFreeHost()	|	cuMemFreeHost()	|                         |
vedaMemGetAddressRange()	|	cuMemGetAddressRange()	|                         |
vedaMemGetDevice()	|	NA	|                         |
vedaMemGetHMEMPointer()	|	NA	|                         |
vedaMemGetInfo()	|	cuMemGetInfo()	|                         |
vedaMemGetRawPointer()	|	NA	|                         |
vedaMemHMEM()	|	NA	|                         |
vedaMemHMEMSize()	|	NA	|                         |
vedaMemPtr()	|	NA	|                         |
vedaMemPtrSize()	|	NA	|                         |
vedaMemReport()	|	NA	|                         |
vedaMemsetD16()	|	cuMemsetD16()	|                         |
vedaMemsetD16Async()	|	cuMemsetD16Async()	|                         |
vedaMemsetD2D16()	|	cuMemsetD2D16()	|                         |
vedaMemsetD2D16Async()	|	cuMemsetD2D16Async()	|                         |
vedaMemsetD2D32()	|	cuMemsetD2D32()	|                         |
vedaMemsetD2D32Async()	|	cuMemsetD2D32Async()	|                         |
vedaMemsetD2D8()	|	cuMemsetD2D8()	|                         |
vedaMemsetD2D8Async()	|	cuMemsetD2D8Async()	|                         |
vedaMemsetD32()	|	cuMemsetD32()	|                         |
vedaMemsetD32Async()	|	cuMemsetD32Async()	|                         |
vedaMemsetD64()	|	NA	|                         |
vedaMemsetD64Async()	|	NA	|                         |
vedaMemsetD8()	|	cuMemsetD8()	|                         |
vedaMemsetD8Async()	|	cuMemsetD8Async()	|                         |
vedaMemSize()	|	NA	|                         |
vedaModuleGetFunction()	|	cuModuleGetFunction()	|                         |
vedaModuleLoad()	|	cuModuleLoad()	|                         |
vedaModuleUnload()	|	cuModuleUnload()	|                         |
vedaStreamAddCallback()	|	cuStreamAddCallback()	|                         |
vedaStreamGetFlags()	|	cuStreamGetFlags()	|                         |
vedaStreamQuery()	|	cuStreamQuery()	|                         |
vedaStreamSynchronize()	|	cuStreamSynchronize()	|                         |
veraDeviceGetAttribute()	|	cudaDeviceGetAttribute()	|                         |
veraDeviceGetPower()	|	NA	|                         |
veraDeviceGetTemp()	|	NA	|                         |
veraDeviceSynchronize()	|	cudaDeviceSynchronize()	|                         |
veraDriverGetVersion()	|	cudaDriverGetVersion()	|                         |
veraFree()	|	cudaFree()	|                         |
veraFreeAsync()	|	cudaFreeAsync()	|                         |
veraFreeHost()	|	cudaFreeHost()	|                         |
veraGetDevice()	|	cudaGetDevice()	|                         |
veraGetDeviceCount()	|	cudaGetDeviceCount()	|                         |
veraGetDeviceProperties()	|	cudaGetDeviceProperties()	|                         |
veraGetErrorName()	|	cudaGetErrorName()	|                         |
veraGetErrorString()	|	cudaGetErrorString()	|                         |
veraHostAlloc()	|	cudaHostAlloc()	|                         |
veraInit()	|	NA	|[VERA] Initializes the driver API and it is optional to call when you are using other VERA APIs.|
veraLaunchHostFunc()	|	cudaLaunchHostFunc()	|                         |
veraMalloc()	|	cudaMalloc()	|                         |
veraMallocAsync()	|	cudaMallocAsync()	|                         |
veraMallocHost()	|	cudaMallocHost()	|                         |
veraMallocPitch()	|	cudaMallocPitch()	|                         |
veraMemcpy()	|	cudaMemcpy()	|                         |
veraMemcpyAsync()	|	cudaMemcpyAsync()	|                         |
veraMemGetInfo()	|	cudaMemGetInfo()	|                         |
veraMemset()	|	cudaMemSet()	|                         |
veraMemsetAsync()	|	cudaMemsetAsync()	|                         |
veraModuleGetFunction()	|	NA	|                         |
veraModuleLoad()	|	NA	|                         |
veraModuleUnload()	|	NA	|                         |
veraPointerGetAttributes()	|	cudaPointerGetAttributes()	|                         |
veraRuntimeGetVersion()	|	cudaRuntimeGetVersion()	|                         |
veraSetDevice()	|	cudaSetDevice()	|                         |
veraStreamAddCallback()	|	cudaStreamAddCallback()	|                         |
veraStreamCnt()	|	NA	|                         |
veraStreamQuery()	|	cudaStreamQuery()	|                         |
veraStreamSynchronize()	|	cudaStreamSynchronize()	|                         |

NA: Not Available
