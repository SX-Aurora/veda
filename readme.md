# VEDA (VE Driver API) and VERA (VE Runtime API)

VEDA and VERA are a CUDA Driver and Runtime API-like APIs for programming the NEC SX-Aurora. It is based on [AVEO](https://github.com/sx-aurora/aveo). Most of the functionality is identical to the [CUDA Driver API](https://docs.nvidia.com/cuda/cuda-driver-api/index.html) and [CUDA Runtime API](https://docs.nvidia.com/cuda/cuda-runtime-api/index.html).

## Release Notes
| Version | Comment |
| --- | --- |
| v0.8 | implemented multi-stream support (experimental) |
| v0.7.1 | Bugfix release |
| v0.7 | initial VERA release |
| v0.6 | initial VEDA release |

## Differences between VEDA and CUDA Driver API:
1. [VEDA] Additionally to ```vedaInit(0)``` in the beginning, ```vedaExit()``` needs to be called at the end of the application, to ensure that no dead device processes stay alive.
1. All function calls start with: [VEDA] ```veda*``` instead of ```cu*``` and [VERA] ```vera*``` instead of ```cuda*```
1. Objects start with [VEDA] ```VEDA*``` instead of ```CU*``` and ```vera*``` instead of ```cuda*```
1. VEDA supports asynchronous malloc and free:
VEDA supports asynchronous ```vedaMemAllocAsync``` and ```vedaMemFreeAsync```. They can be used like the synchronous calls, but don't need to synchronize the execution between device and host.
1. ```vedaDeviceGetPower(float* power, VEDAdevice dev)``` and ```vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev)``` allow to fetch the power consumption (in W) and temperature (in C).
1. As the programming model of the SX-Aurora differs from NVIDIA GPUs, launching kernels looks different:
	```cpp
	// Device Code -------------------------------------------------------------
	extern "C" void my_function(float myFloat, uint8_t myUnsignedChar, float* array) {
		...
	}

	// C -----------------------------------------------------------------------
	float myFloat;
	uint8_t myUnsignedChar;
	VEDAargs args;
	vedaArgsCreate(&args);

	// Scheme: vedaArgsSet[TYPE](&args, [PARAM_INDEX], [VARIABLE]);
	vedaArgsSetF32(&args, 0, myFloat);
	vedaArgsSetU8(&args, 1, myUnsignedChar);

	// Copy entire arrays as function parameter
	float array[32];
	vedaArgsSetStack(&args, 2, array, VEDA_ARGS_INTENT_INOUT, sizeof(array));

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

	// Kernel Call Version 2: automatically destroys VEDAargs object after execution (can't be reused for other calls!)
	vedaLaunchKernelEx(func, stream, args, 1);

	// CPP ---------------------------------------------------------------------
	vedaLaunchKernel(func, stream, myFloat, myUnsignedChar, VEDAstack(array, VEDA_ARGS_INTENT_INOUT, sizeof(array)));
	```
1. VEDAdeviceptr need to be dereferenced first on device side:
	```cpp
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
	```

## Differences between VERA and CUDA Runtime API:
1. All function calls start with ```vera*``` instead of ```cuda*```
1. Objects start with ```vera*``` instead of ```cuda*```
1. VERA supports asynchronous malloc and free, see VEDA.
VEDA supports asynchronous ```vedaMemAllocAsync``` and ```vedaMemFreeAsync```. They can be used like the synchronous calls, but don't need to synchronize the execution between device and host.
1. ```vedaDeviceGetPower(float* power, VEDAdevice dev)``` and ```vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev)``` allow to fetch the power consumption (in W) and temperature (in C).
1. As the programming model of the SX-Aurora differs from NVIDIA GPUs, launching kernels looks different.
1. Similar to CUDA Runtime API, calls from VEDA and VERA can be mixed!

## VEDA/VERA Unique Features:
### Delayed Memory Allocation
VEDA does not need to allocate memory from the host, but can do that directly from the device. For this, the host only needs to create an empty VEDAdeviceptr.
```cpp
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
```

### OMP Threads vs Streams (experimental):
By default VEDA uses all available cores or the number defined by the env var ```VE_OMP_NUM_THREADS``` to parallelize across all cores. Since v0.8 it is possible to define these per device using ```vedaCtxCreate(&ctx, OMP_THREADS, device)```. This influences the number of available streams of the context. Available number of streams calculates as ```numStreams = numCores / numOmpThreads```. Odd values can lead to unused cores! All threads and streams share the same memory space on the VE.

If you use VERA, you can use ```vedaCtxCreate(...)``` to specify the behavior or set ```VE_OMP_NUM_THREADS``` if you want to set it globally for all devices.

## Limitations:
1. VEDA only supports one ```VEDAcontext``` per device.
1. VEDA does only support a limited number of streams per ```VEDAcontext```. Use ```vedaCtxGetMaxStreams()``` to query available number of threads. The default stream (== 0) is always created and cannot be destroyed.
1. No unified memory space (yet).

## How to build:
```bash
mkdir build
cd build
cmake3 ../
make
sudo make install
```

## How to use:
VEDA has an own CMAKE find script. All ```*.vc``` files get compiled using NCC and ```*.vcpp``` using NC++. The script uses the compilers installed in ```/opt/nec/ve/bin```. You can modify the ```CMAKE_[VC/VCPP]_COMPILER``` flags to change that behavior.

```cmake
SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake)
FIND_PACKAGE(VE)
ENABLE_LANGUAGE(VC VCPP)

INCLUDE_DIRECTORIES(${VEDA_INCLUDES})
ADD_EXECUTABLE(myApp mycode.vc mycode.vcpp)
TARGET_LINK_LIBRARIES(myApp ${VEDA_LIBRARY})
```

Further you need to set following entry in your ```.bashrc```:
```bash
export VE_LD_LIBRARY_PATH=/usr/local/ve/veda/lib
```