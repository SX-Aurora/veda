# VEDA (VE Driver API)

VEDA is a CUDA Driver API-like API for programming the NEC SX-Aurora. It is based on [AVEO](https://github.com/sx-aurora/aveo). Most of the functionality is identical to the [CUDA Driver API](https://docs.nvidia.com/cuda/cuda-driver-api/index.html).

## Differences between VEDA and CUDA Driver API:
1. Additionally to ```vedaInit(0)``` in the beginning, ```vedaExit()``` needs to be called at the end of the application, to ensure that no dead device processes stay alive.
1. All function calls start with ```veda*``` instead of ```cu*```
1. Objects start with ```VEDA*``` instead of ```CU*```
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
1. VEDA does not need to allocate memory from the host, but can do that directly from the device. For this, the host only needs to create an empty VEDAdeviceptr.
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
1. The SX-Aurora does not support the usage of multiple-streams, so VEDA maps all functions added to multiple streams onto the same work queue.

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