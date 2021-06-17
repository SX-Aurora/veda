# VEDA (VE Driver API) and VERA (VE Runtime API)

VEDA and VERA are a CUDA Driver and Runtime API-like APIs for programming the NEC SX-Aurora. It is based on [AVEO](https://github.com/sx-aurora/aveo). Most of the functionality is identical to the [CUDA Driver API](https://docs.nvidia.com/cuda/cuda-driver-api/index.html) and [CUDA Runtime API](https://docs.nvidia.com/cuda/cuda-runtime-api/index.html).

## Release Notes
| Version | Comment |
| --- | --- |
| v0.10.4 | Fixed Identification of VE model. |
| v0.10.3 | Filtering negative values from ```VEDA_VISIBLE_DEVICES```. |
| v0.10.2 | Correct veda-smi RPATH to work without setting LD_LIBRARY_PATH. |
| v0.10.1 | Added ```aveorun-ftrace```. Can be activated using ```VEDA_FTRACE=1``` env var. Renamed RPM packages to only include major version in package name, i.e. ```veda-0.10```. |
| v0.10.0 | Renamed and improved ```VEDAmpiptr``` to ```VEDAptr<typename>```. Removed ```VEDAdeviceptr->X``` functions, as they are now part of ```VEDAptr```. Added ```veda-smi``` executable. |
| v0.10.0rc5 | Added boundary checks for Memcopy and MemSet. Added ```vedaArgsSetHMEM```. Added ```veda_device_omp.h``` parallelization primitives for C++. Added experimental ```VEDAmpiptr``` for easier usage with VE-MPI. Added/corrected some of the sensor readings, i.e. LLC Cache, Total Device Memory, ... |
| v0.10.0rc4 | Increased VEDA offset limit to 128GB. Added ```VEDAdeviceptr->X``` functions in C++. Renamed ```vedaArgsSetPtr``` to ```vedaArgsSetVPtr```. Added ```vedaArgsSetPtr``` to automatically translate ```VEDAdeviceptr``` to ```void*```. Fixed VEDA_VISIBLE_DEVICES to obey NUMA mode. |
| v0.10.0rc3 | Added AVEO symlinks. Fixed wrong include. |
| v0.10.0rc2 | Fixed problem in veda_types.h when compiling with C. Linking against shared AVEO instead of static. |
| v0.10.0rc1 | Fixed 0°C core temperatures. Added NUMA support. Each NUMA node becomes a separate VEDAdevice. Added ```vedaDeviceDistance(float**, VEDAdevice, VEDAdevice)``` to determine the relationship between two VEDAdevices (0.0 == same device, 0.5 == same physical device but different NUMA node, 1.0 == different physical device). Added ```vedaMemGetHMEMPointer(void**, VEDAdeviceptr)``` to translate VEDA pointer to HMEM pointer. |
| v0.9.5.2 | Bugfixes |
| v0.9.5.1 | Bugfixes |
| v0.9.5 | Bugfixes |
| v0.9.4 | Bugfixes |
| v0.9.3 | Bugfixes |
| v0.9.2 | Added FindMPI. Set all CMake vars as advanced. |
| v0.9.1 | Added FindBLAS, FindLAPACK, FindASL and FindNCL to CMake. |
| v0.9 | Enhanced VEDA CMake Scripts, to also support native NCC compilation. |
| v0.8.1 | updated AVEO. Using VE_NODE_NUMBER as fallback if VEDA_VISIBLE_DEVICES is not set. |
| v0.8 | Implemented multi-stream support (experimental). Automatic setting of required env vars. |
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
1. VEDA streams differ from CUDA streams. See chapter "OMP Threads vs Streams" for more details.
1. VEDA uses the env var ```VEDA_VISIBLE_DEVICES``` in contrast to ```CUDA_VISIBLE_DEVICES```.

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
In CUDA streams can be used to create different execution queues, to overlap compute with memcopy. VEDA supports two stream modes which differ from the CUDA behavior. These can be defined by ```vedaCtxCreate(&ctx, MODE, device)```.

1. ```VEDA_CONTEXT_MODE_OMP``` (default): All cores will be assigned to the default stream (=0). This mode only supports a single stream.
1. ```VEDA_CONTEXT_MODE_SCALAR```: Every core gets assigned to a different stream. This mode allows to use each core independently with different streams. Use the function ```vedaCtxStreamCnt(&streamCnt)``` to determine how many streams are available.

Both methods use the env var ```VE_OMP_NUM_THREADS``` to determine the maximal number of cores that get use for either mode. If the env var is not set, VEDA uses all available cores of the hardware.

### Advanced VEDA C++ Ptr
When you use C++, you can use the ```VEDAptr<typename>``` that gives you more directly control over the ```VEDAdeviceptr```, i.e. you can use ```vptr.size()```, ```vptr.device()```, ... . The ```typename``` is used to automatically determine the correct offsets when executing ```vptr += offset;```.

### VEDA-NEC MPI integration
The VEO-aware NEC MPI ( https://www.hpc.nec/forums/topic?id=pgmcA8 ) enables to much easier implement hybrid VE applications. For this, so called HMEM pointers have been introduced in VEO. Starting with v0.10 VEDA also supports HMEM pointers via the functions ```vedaGetHMEM(void*, VEDAdeviceptr)``` or ```VEDAptr<> vptr; vptr.hmem()``` (C++ only). To make it more comfortable to use you can directly pass ```VEDAptr<typename>``` instances to the ```mpi_*``` method, as shown in this example:

```cpp
VEDAptr<float> vptr;
vedaMemAlloc(&vptr, size);
vedaMemcpyHtoD(vptr, hptr, size);
mpi_send(vptr, ...);
```

### NUMA Support
VEDA supports VE NUMA nodes since v0.10. To enable NUMA on your system you need to execute (set ```-N ?``` to specific device index):
```bash
VCMD="sudo /opt/nec/ve/bin/vecmd -N ?"
$VCMD vconfig set partitioning_mode on
$VCMD state set off
$VCMD state set mnt
$VCMD reset card
```

VEDA then recognizes each NUMA node as a separate device, i.e. with 2 physical devices in NUMA mode, VEDA would show 4 devices. You can use ```VEDAresult vedaDeviceDistance(float* distance, VEDAdevice devA, VEDAdevice devB)``` to determine the relationship of two VEDAdevices.

```
distance == 0.0; // same device
distance == 0.5; // same physical device, different NUMA node
distance == 1.0; // differeny physical device
```

### VEDA-smi
The executable ```veda-smi``` displays available VEDA devices in your system. It uses the ```VEDA_VISIBLE_DEVICES``` env var and therefore only shows the devices that your VEDA application would be able to use. Use ```VEDA_VISIBLE_DEVICES= veda-smi``` to ensure that you see all installed devices.

```
╔ veda-smi ═════════════════════════════════════════════════════════════════════╗
║ VEDA Version: 0.10.0     AVEO Version: 0.9.15                                 ║
╚═══════════════════════════════════════════════════════════════════════════════╝

┌── #0  NEC SX-Aurora Tsubasa VE10B ────────────────────────────────────────────┐
  ┌ Physical: 1.0
  ├ AVEO:     0.0
  ├ Clock:    current: 1400 MHz, base: 800 MHz, memory: 1600 MHz
  ├ Firmware: 5399
  ├ Memory:   49152 MiB
  ├ Cache:    LLC: 8192kB, L2: 256kB, L1d: 32kB, L1i: 32kB
  ├ Temp:     56.4°C 56.4°C 57.0°C 56.1°C
  └ Power:    18.0W (11.9V, 1.5A)
└───────────────────────────────────────────────────────────────────────────────┘

┌── #1  NEC SX-Aurora Tsubasa VE10B ────────────────────────────────────────────┐
  ┌ Physical: 1.1
  ├ AVEO:     0.1
  ├ Clock:    current: 1400 MHz, base: 800 MHz, memory: 1600 MHz
  ├ Firmware: 5399
  ├ Memory:   49152 MiB
  ├ Cache:    LLC: 8192kB, L2: 256kB, L1d: 32kB, L1i: 32kB
  ├ Temp:     56.1°C 56.4°C 55.9°C 56.0°C
  └ Power:    18.0W (11.9V, 1.5A)
└───────────────────────────────────────────────────────────────────────────────┘

┌── #2  NEC SX-Aurora Tsubasa VE10B ────────────────────────────────────────────┐
  ┌ Physical: 0.0
  ├ AVEO:     1.0
  ├ Clock:    current: 1400 MHz, base: 800 MHz, memory: 1600 MHz
  ├ Firmware: 5399
  ├ Memory:   49152 MiB
  ├ Cache:    LLC: 16384kB, L2: 256kB, L1d: 32kB, L1i: 32kB
  ├ Temp:     53.8°C 53.5°C 54.1°C 53.8°C 53.8°C 54.1°C 53.2°C 53.5°C
  └ Power:    36.3W (11.9V, 3.1A)
└───────────────────────────────────────────────────────────────────────────────┘
```

## Limitations/Known Problems:
1. VEDA only supports one ```VEDAcontext``` per device.
1. No unified memory space (yet).
1. VEDA by default uses the current workdirectory for loading modules. This behavior can be changed by using the env var ```VE_LD_LIBRARY_PATH```.
1. Due to compiler incompatibilities it can be necessary to adjust the CMake variable ```${AVEO_NFORT}``` to another compiler.

## How to build:
```bash
mkdir build
cd build
cmake3 ../
make
sudo make install
```

## How to use:
VEDA has an own CMake find script. This supports 3 modes. The script uses the compilers installed in ```/opt/nec/ve/bin```. You can modify the ```CMAKE_[LANG]_COMPILER``` flags to change that behavior. See the Hello World examples in the [Examples Folder](example)

### 1. VEDA Hybrid Offloading:
This mode is necessary for VEDA offloading applications. It enables to compile host and device code within the same CMake project. For this it is necessary to use different file extensions for the VE code. All ```*.vc``` files get compiled using NCC, ```*.vcpp``` using NC++ and ```*.vf``` with NFORT.

```cmake
SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake)
FIND_PACKAGE(VE)
ENABLE_LANGUAGE(VEDA_C VEDA_CXX)

INCLUDE_DIRECTORIES(${VEDA_INCLUDES})
ADD_EXECUTABLE(myApp mycode.vc mycode.vcpp)
TARGET_LINK_LIBRARIES(myApp ${VEDA_LIBRARY})
```

### 2. VE Native applications:
This mode enables to compile VE native applications.

```cmake
SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake)
FIND_PACKAGE(VE)
ENABLE_LANGUAGE(VE_C VE_CXX)
ADD_EXECUTABLE(myApp mycode.c mycode.cpp)
```

### 3. VE Native Injection:
If you have a CPU application and you don't want to modify the CMake script you can build your project using:
```
cmake -C /usr/local/ve/veda/cmake/InjectVE.cmake /path/to/your/source
```
It will replace the CPU ```C```, ```CXX``` and ```Fortran``` compilers with NCC.
