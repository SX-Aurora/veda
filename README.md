# VEDA (VE Driver API) and VERA (VE Runtime API)

VEDA and VERA are a CUDA Driver and Runtime API-like APIs for programming the
NEC SX-Aurora. It is based on [AVEO](https://github.com/sx-aurora/aveo). Most of
the functionality is identical to the [CUDA Driver
API](https://docs.nvidia.com/cuda/cuda-driver-api/index.html) and [CUDA Runtime
API](https://docs.nvidia.com/cuda/cuda-runtime-api/index.html).

[![Github](https://img.shields.io/github/v/tag/sx-aurora/veda?display_name=tag&sort=semver)](https://github.com/sx-aurora/veda)
[![PyPI](https://img.shields.io/pypi/v/veda)](https://pypi.org/project/veda)
[![License](https://img.shields.io/pypi/l/veda)](https://pypi.org/project/veda)
![Python Versions](https://img.shields.io/pypi/pyversions/veda)
![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)
![Maintenance](https://img.shields.io/pypi/dm/veda)

**Sitemap:**
- [Release Notes](#veda_1)
- [Differences between VEDA and CUDA Driver API](#veda_2)
- [Differences between VERA and CUDA Runtime API](#veda_3)
- [VEDA/VERA Unique Features](#veda_4)
	- [Delayed Memory Allocation](#veda_41)
	- [OMP Threads vs Streams (experimental)](#veda_42)
	- [Advanced VEDA C++ Ptr](#veda_43)
	- [VEDA-NEC MPI integration](#veda_44)
	- [NUMA Support](#veda_45)
	- [VEDA-smi](#veda_46)
	- [Profiling API](#veda_47)
	- [C++ API *(Experimental!)*](#veda_48)
		- [Error Handling](#veda_481)
		- [Fetching a Device handle](#veda_482)
		- [Loading Modules](#veda_483)
		- [Memory Buffer Objects](#veda_484)
		- [Fetching Functions](#veda_485)
- [SX-Aurora VE3](#veda_5)
- [Limitations/Known Problems](#veda_6)
- [How to build](#veda_7)
- [How to use](#veda_8)
	- [VEDA Hybrid Offloading](#veda_81)
	- [VE Native applications](#veda_82)
	- [VE Native Injection](#veda_83)

---

<a name="veda_1"></a>
## Release Notes
<table>
<tr><th>Version</th><th>Comment</th></tr>

<tr><td>v2.2.1</td><td>
<p>
	Significantly reducted VEDA overhead in applications that make use of
	many delayed tensors.
</p>

<p>
	Minor change: when using <code>vedaMemRelease</code> additionally
	<code>vedaMemFree</code> needs to be called on the host. The reasoning
	is to have a consistent behavior, e.g., when opaque libraries such as
	VEDA-PyTorch will always call <code>vedaMemFree</code>.
</p>
</td></tr>

<tr><td>v2.2</td><td>
<p>
	Added experimental feature to free VEDAdeviceptr within a
	kernel. This mechanism has some limitations!
</p>

<p>
	The VEDAdeviceptr needs to be allocated using delayed malloc.
	First call <code>vedaMemAlloc(&vptr, 0)</code> on the host, call
	your kernel and execute <code>vedaMemAlloc(vptr, size)</code>.
</p>

<p>
	To free the VEDAdeviceptr, first call
	<code>vedaMemRelease(vptr)</code> on the host to take the
	ownership of the VEDAdeviceptr. Then you can use
	<code>vedaMemFree</code> within your kernel.
</p>

<p>
	This mechanism doesn't work with non-delayed mallocs as they
	might be registered to NEC MPI, which cannot be deregistered
	from within the device.
</p>
</td></tr>

<tr><td>v2.1.1</td><td>
<ul>
<li>Shutdown Bugfix</li>
</ul>
</td></tr>

<tr><td>v2.1.0</td><td>
<ul>
<li>VE3 support</li>
<li>NCC 5 Bugfixes</li>
</ul>
</td></tr>

<tr><td>v2.0.2</td><td>
<ul>
<li>Fixed detection of i64 cblas library.</li>
</ul>
</td></tr>

<tr><td>v2.0.2</td><td>
<ul>
<li>Fixed CMake to correct find 64Bit BLAS libraries</li>
</ul>
</td></tr>

<tr><td>v2.0.1</td><td>
<ul>
<li>Bugfixes.</li>
<li>Merged changes of VEOS VEDA.</li>
<li>Extended Profiler API.</li>
</ul>
</td></tr>

<tr><td>v2.0.0</td><td>
<ul>
<li>Breaking C-ABI changes</li>
<li><a href="#veda_48">Added Experimental C++ API</a></li>
<li><a href="#veda_47">Added VEDA profiling API</a></li>
</ul>
</td></tr>

<tr><td>v1.4.0</td><td>
<ul>
<li>Overhauled HMEM API</li>
<li>Switched to new AVEO async malloc</li>
<li>Fixed bug in CMake setting correct C++ standard flags</li>
</ul>
</td></tr>

<tr><td>v1.3.5</td><td>
<ul>
<li>Bugfix for <code>vedaArgsSet(...)</code> to again accept <code>VEDAptr<T></code></li>
</ul>
</td></tr>

<tr><td>v1.3.4</td><td>
<ul>
<li>Fixed RPM and LOCAL dist types.</li>
<li>Automatically downloading Illyrian and Tungl in VEOS and LOCAL builds.</li>
<li>Fixed DType issues with in cpp <code>vedaLaunchKernel(...)</code></li>
</ul>
</td></tr>

<tr><td>v1.3.3</td><td>
<ul>
<li>Removed polluting <code>VEDA_ERROR_UNKNOWN_CONTEXT</code> log message.</li>
<li>Fixed possible memleak when VPTR is already allocated.</li>
<li>Fixed possible memleak when using <code>vedaMemAllocOverrideOnce</code>.</li>
<li>Synchronizing TUNGL timer with VH.</li>
</ul>
</td></tr>

<tr><td>v1.3.2</td><td>
<ul>
<li>Added <code>vedaMemAllocOverrideOnce</code> to prevent <code>vedaMemAlloc</code> to allocate new memory and instead return the override value once. This is not meant to be used except when you want to circumvent unncessary memory allocations in opaque data structures that you don't have access to.</li>
<li>Fixed RPATH for veda-smi in Python releases</li>
</ul>
</td></tr>

<tr><td>v1.3.1</td><td>
<ul>
<li>Added compile guard to prevent <code>vedaArgsSet&lt;bool&gt;</code> because <code>bool</code> is defined as 1B on VH and 4B on VE.</li>
<li>Merged changes from <a href="https://github.com/veos-sxarr-NEC/veda_source/tree/release-2.11.1" target="_BLANK">VEOS 2.11.1 release</a>.</li>
<li>Compatibility Bugfix for <code>veda/omp.h</code> NCC &ge; v3.4</li>
</ul>
</td></tr>

<tr><td>v1.3.0</td><td>
<ul>
<li>changed definition of <code>VEDAdeviceptr</code> to prevent segfaults when passing them to <code>std::ostream</code></li>
<li>added <code>__global__</code> for device code</li>
<li>improved <code>veda_device_omp.h</code> implementations</li>
<li>renamed <code>FIND_PACKAGE(VE ...)</code> to <code>FIND_PACKAGE(VEDA ...)</code></li>
<li>added checks for <code>REQUIRED</code> and min versions to <code>FIND_PACKAGE(VEDA ...)</code></li>
<li>renamed <code>VEDA_INCLUDES</code> to <code>VEDA_INCLUDE_DIRS</code> to comply with CMake standard</li>
<li>moved <code>CMakeLists.txt</code> into project root</li>
<li>VEDA_CXX will now obey <code>CMAKE_CXX_STANDARD</code> when <code>CMAKE_CXX_STANDARD_REQUIRED</code> is set.</li>
<li>added device-side support for <a href="https://github.com/nec-research/tungl" target="_BLANK">Tungl</a></li>
</ul>
</td></tr>

<tr><td>v1.2.0</td><td>
<ul>
<li>replaced <code>bool checkResult</code> with <code>int64_t* result</code> in <code>vedaLaunchKernelEx</code> to receive return value of kernel</li>
<li>added C++ version of <code>vedaLaunchKernelEx</code></li>
<li>added <code>vedaLaunchHostFuncEx</code> that can return the return value of the function</li>
</ul>
</td></tr>

<tr><td>v1.1.2</td><td>
<ul>
<li>changed behavior of <code>VE_NODE_NUMBER</code> to be only used when <code>_VENODELIST</code> AND <code>VEDA_VISIBLE_DEVICES</code> are not set.</li>
</ul>
</td></tr>

<tr><td>v1.1.1</td><td>
<ul>
<li>Added support for AVEO's <code>_VENODELIST</code> env to ensure correct behavior in cluster environments.</li>
<li>Changed behavior of <code>VEDA_VISIBLE_DEVICES</code> in case of NUMA nodes. It now accepts the direct hardware id in the format of <code>[AVEO_ID].[NUMA_ID]</code></li>
</ul>
</td></tr>

<tr><td>v1.1.0</td><td>
<ul>
<li>added <code>vedaMemSwap</code> function to swap the underlying memory buffer between two <code>VEDAdeviceptr</code>.</li>
</ul>
</td></tr>

<tr><td>v1.0.0</td><td>
First stable release.
<ul>
<li>Improved memset performance, especially for D8 and D16 (up to 150x faster now!).</li>
<li>Added <code>vedaMemsetD128</code> and <code>vedaMemsetD2D128</code> API.</li>
<li>Added <code>ASL_FFTW_LIBRARIES</code> to ASL CMake.</li>
<li>Added device code <code>vedaMemset</code>. Enabled to use <code>vedaMemsetD*</code> in device code.</li>
<li>Added C++ wrapper to allow directly signed integer and floating point values for <code>vedaMemsetD*</code> functions.</li>
</ul>
</td></tr>

<tr><td>v0.10.6</td><td>Maintenance release that fixes SegFaults when context has been destroyed before freeing memory. <code>vedaMemFree</code> ignores calls if the context for the particular pointer has already been freed. BugFix for <code>VEDA_CONTEXT_MODE_SCALAR</code> if <code>VE_OMP_NUM_THREADS</code> is not set.</td></tr>

<tr><td>v0.10.5</td><td>added <code>veda_omp_simd_reduce</code>. MemTrace only get printed when env var<code>VEDA_MEM_TRACE=1</code> is set. VEDA no longer overrides VEORUN_BIN if already been set by user. Added LICENSE to installation target.</td></tr>
<tr><td>v0.10.4</td><td>Fixed Identification of VE model.</td></tr>
<tr><td>v0.10.3</td><td>Filtering negative values from <code>VEDA_VISIBLE_DEVICES</code>.</td></tr>
<tr><td>v0.10.2</td><td>Correct veda-smi RPATH to work without setting LD_LIBRARY_PATH.</td></tr>
<tr><td>v0.10.1</td><td>Added <code>aveorun-ftrace</code>. Can be activated using <code>VEDA_FTRACE=1</code> env var. Renamed RPM packages to only include major version in package name, i.e. <code>veda-0.10</code>.</td></tr>
<tr><td>v0.10.0</td><td>Renamed and improved <code>VEDAmpiptr</code> to <code>VEDAptr<typename></code>. Removed <code>VEDAdeviceptr->X</code> functions, as they are now part of <code>VEDAptr</code>. Added <code>veda-smi</code> executable.</td></tr>
<tr><td>v0.10.0rc5</td><td>Added boundary checks for Memcopy and MemSet. Added <code>vedaArgsSetHMEM</code>. Added <code>veda_device_omp.h</code> parallelization primitives for C++. Added experimental <code>VEDAmpiptr</code> for easier usage with VE-MPI. Added/corrected some of the sensor readings, i.e. LLC Cache, Total Device Memory, ...</td></tr>
<tr><td>v0.10.0rc4</td><td>Increased VEDA offset limit to 128GB. Added <code>VEDAdeviceptr->X</code> functions in C++. Renamed <code>vedaArgsSetPtr</code> to <code>vedaArgsSetVPtr</code>. Added <code>vedaArgsSetPtr</code> to automatically translate <code>VEDAdeviceptr</code> to <code>void*</code>. Fixed VEDA_VISIBLE_DEVICES to obey NUMA mode.</td></tr>
<tr><td>v0.10.0rc3</td><td>Added AVEO symlinks. Fixed wrong include.</td></tr>
<tr><td>v0.10.0rc2</td><td>Fixed problem in veda_types.h when compiling with C. Linking against shared AVEO instead of static.</td></tr>
<tr><td>v0.10.0rc1</td><td>Fixed 0°C core temperatures. Added NUMA support. Each NUMA node becomes a separate VEDAdevice. Added <code>vedaDeviceDistance(float**, VEDAdevice, VEDAdevice)</code> to determine the relationship between two VEDAdevices (0.0 == same device, 0.5 == same physical device but different NUMA node, 1.0 == different physical device). Added <code>vedaMemGetHMEMPointer(void**, VEDAdeviceptr)</code> to translate VEDA pointer to HMEM pointer.</td></tr>
<tr><td>v0.9.5.2</td><td>Bugfixes</td></tr>
<tr><td>v0.9.5.1</td><td>Bugfixes</td></tr>
<tr><td>v0.9.5</td><td>Bugfixes</td></tr>
<tr><td>v0.9.4</td><td>Bugfixes</td></tr>
<tr><td>v0.9.3</td><td>Bugfixes</td></tr>
<tr><td>v0.9.2</td><td>Added FindMPI. Set all CMake vars as advanced.</td></tr>
<tr><td>v0.9.1</td><td>Added FindBLAS, FindLAPACK, FindASL and FindNCL to CMake.</td></tr>
<tr><td>v0.9</td><td>Enhanced VEDA CMake Scripts, to also support native NCC compilation.</td></tr>
<tr><td>v0.8.1</td><td>updated AVEO. Using VE_NODE_NUMBER as fallback if VEDA_VISIBLE_DEVICES is not set.</td></tr>
<tr><td>v0.8</td><td>Implemented multi-stream support (experimental). Automatic setting of required env vars.</td></tr>
<tr><td>v0.7.1</td><td>Bugfix release</td></tr>
<tr><td>v0.7</td><td>initial VERA release</td></tr>
<tr><td>v0.6</td><td>initial VEDA release</td></tr>
</table>

---

<a name="veda_2"></a>
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
	vedaModuleGetFunction(&func, mod, "my_function");

	// Kernel Call Version 1: allows to reuse VEDAargs object
	VEDAstream stream = 0;
	vedaLaunchKernel(func, stream, args);

	// args are not allowed to be destroyed before synchronizing!
	vedaStreamSynchronize(stream);
	vedaArgsDestroy(&args);

	// Kernel Call Version 2: automatically destroys VEDAargs object after execution (can't be reused for other calls!)
	vedaLaunchKernelEx(func, stream, args, 1, 0);

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
1. VEDA uses the env var `VEDA_VISIBLE_DEVICES` in contrast to `CUDA_VISIBLE_DEVICES`. The behavior of `VEDA_VISIBLE_DEVICES` is slightly different:
	- `VEDA_VISIBLE_DEVICES=` enables all devices, `CUDA_VISIBLE_DEVICES=` disables all devices.
	- For enabling VE's in NUMA mode, use `{ID}.0` and `{ID}.1`.
	- `VEDA_VISIBLE_DEVICES` ids correspond to VE hardware ids, `CUDA_VISIBLE_DEVICES` corresponds to the CUDA specific ids.

---

<a name="veda_3"></a>
## Differences between VERA and CUDA Runtime API:
1. All function calls start with ```vera*``` instead of ```cuda*```
1. Objects start with ```vera*``` instead of ```cuda*```
1. VERA supports asynchronous malloc and free, see VEDA.
VEDA supports asynchronous ```vedaMemAllocAsync``` and ```vedaMemFreeAsync```. They can be used like the synchronous calls, but don't need to synchronize the execution between device and host.
1. ```vedaDeviceGetPower(float* power, VEDAdevice dev)``` and ```vedaDeviceGetTemp(float* tempC, const int coreIdx, VEDAdevice dev)``` allow to fetch the power consumption (in W) and temperature (in C).
1. As the programming model of the SX-Aurora differs from NVIDIA GPUs, launching kernels looks different.
1. Similar to CUDA Runtime API, calls from VEDA and VERA can be mixed!

---

<a name="veda_4"></a>
## VEDA/VERA Unique Features:

<a name="veda_41"></a>
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

---

<a name="veda_42"></a>
### OMP Threads vs Streams (experimental):
In CUDA streams can be used to create different execution queues, to overlap
compute with memcopy. VEDA supports two stream modes which differ from the CUDA
behavior. These can be defined by ```vedaCtxCreate(&ctx, MODE, device)```.

1. ```VEDA_CONTEXT_MODE_OMP``` (default): All cores will be assigned to the default stream (=0). This mode only supports a single stream.
1. ```VEDA_CONTEXT_MODE_SCALAR```: Every core gets assigned to a different
   stream. This mode allows to use each core independently with different
   streams. Use the function ```vedaCtxStreamCnt(&streamCnt)``` to determine how
   many streams are available.

Both methods use the env var ```VE_OMP_NUM_THREADS``` to determine the maximal
number of cores that get use for either mode. If the env var is not set, VEDA
uses all available cores of the hardware.

---

<a name="veda_43"></a>
### Advanced VEDA C++ Ptr
When you use C++, you can use the ```VEDAptr<typename>``` that gives you more
directly control over the ```VEDAdeviceptr```, i.e. you can use
```vptr.size()```, ```vptr.device()```, ... . The ```typename``` is used to
automatically determine the correct offsets when executing ```vptr +=
offset;```.

---

<a name="veda_44"></a>
### VEDA-NEC MPI integration
The VEO-aware NEC MPI ( https://www.hpc.nec/forums/topic?id=pgmcA8 ) enables to
much easier implement hybrid VE applications. For this, so called HMEM pointers
have been introduced in VEO. Starting with v1.4.0 VEDA introduced a new HMEM
API: ```vedaHMEM*```. See following example:

```cpp
VEDAhmemptr hmem;
vedaHMemAlloc(&hmem, size);
vedaHMemcpy(hmem, host_ptr, size);
mpi_send(hmem, ...);
```

<a name="veda_45"></a>
### NUMA Support
VEDA supports VE NUMA nodes since v0.10. To enable NUMA on your system you need
to execute (set ```-N ?``` to specific device index):
```bash
VCMD="sudo /opt/nec/ve/bin/vecmd -N ?"
$VCMD vconfig set partitioning_mode on
$VCMD state set off
$VCMD state set mnt
$VCMD reset card
```

VEDA then recognizes each NUMA node as a separate device, i.e. with 2 physical
devices in NUMA mode, VEDA would show 4 devices. You can use ```VEDAresult
vedaDeviceDistance(float* distance, VEDAdevice devA, VEDAdevice devB)``` to
determine the relationship of two VEDAdevices.

```
distance == 0.0; // same device
distance == 0.5; // same physical device, different NUMA node
distance == 1.0; // differeny physical device
```

---

<a name="veda_46"></a>
### VEDA-smi
The executable ```veda-smi``` displays available VEDA devices in your system. It
uses the ```VEDA_VISIBLE_DEVICES``` env var and therefore only shows the devices
that your VEDA application would be able to use. Use ```VEDA_VISIBLE_DEVICES=
veda-smi``` to ensure that you see all installed devices.

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

---

<a name="veda_47"></a>
### Profiling API
Since v1.5.0 VEDA supports to add a profiling callback using
`vedaProfilerSetCallback(...)`. The callback needs to have the signature `void
(*)(VEDAprofiler_data* data, int enter)`. If `enter` is non-zero, the callback
got called right before issuing the command. If it's zero, it just ended.

The data provides the following fields:
1. `type`: An enum that identifies which kind function got called (kernel, memcpy, ...)
2. `device_id`: VEDA device id
3. `stream_id`: VEDA stream id
4. `req_id`: ID of the request
5. `user_data`: `void*` that allows to store data between `enter` and `exit` of
   the event. This should be deleted by the user when `enter==0` to prevent
   memleaks.

Depending on the `type`, you can cast the `data` to one of the following data
types to get access to further information.

1. `type in [VEDA_PROFILER_MEM_ALLOC, VEDA_PROFILER_HMEM_ALLOC]`: `VEDAprofiler_vedaMemAlloc`
	1. `bytes`: number of bytes to be allocated
1. `type in [VEDA_PROFILER_MEM_FREE, VEDA_PROFILER_HMEM_FREE]`: `VEDAprofiler_vedaMemFree`
	1. `ptr`: pointer to be freed
1. `type in [VEDA_PROFILER_MEM_CPY_HTOD, VEDA_PROFILER_MEM_CPY_DTOH, VEDA_PROFILER_HMEM_CPY]`: `VEDAprofiler_vedaMemcpy`
	1. `dst`: destination pointer
	1. `src`: source pointer
	1. `bytes`: number of bytes transfered
1. `type == VEDA_PROFILER_LAUNCH_KERNEL`: `VEDAprofiler_vedaLaunchKernel`
	1. `func`: function pointer that gets called
	1. `kernel`: name of the kernel that gets called
1. `type == VEDA_PROFILER_LAUNCH_HOST`: `VEDAprofiler_vedaLaunchHostFunc`
	1. `func`: function pointer that gets called
1. `type == VEDA_PROFILER_SYNC`: `VEDAprofiler_data`

---

<a name="veda_48"></a>
### C++ API *(Experimental!)*
Starting with v1.5.0 we introduce a new experimental and lightweight C++ API.
This API aims for easier usage of VEDA, with much more comfort in C++
applications.

To include the new API just use `#include <veda/cpp/api.h>`.

<a name="veda_481"></a>
#### Error Handling
Instead of the C-API, the C++ API uses exceptions, which can be used like this:
```cpp
try {
	...
} catch(const veda::Exception& e) {
	std::cerr << e.what() << " @ " << e.file() << " (" << e.line() << ")";
}
```

<a name="veda_482"></a>
#### Fetching a Device handle
To get a handle to a device, just create an instance using:
```cpp
veda::Device device(0);
```

In contrast to the C-API, the `veda::Device` incorporates the `VEDAdevice` and
`VEDAcontext` into a single object. We use a lazy scheme, which will not boot up
the device context until you allocate memory, load a model, or similar.

The device provides the following attributes and metrics: `isActive, current,
currentEdge, distance, power, temp, voltage, voltageEdge, abi, aveoId, cacheL1d,
cacheL1i, cacheL2, cacheLLC, clockBase, clockMemory, clockRate, cores, firmware,
model, numaId, physicalId, singleToDoublePerfRatio, streamCnt, vedaId, totalMem,
usedMem`.

If your application requires to do the CUDA-style programming, where you bind
the device to a specific thread, you can use `device.pushCurrent()`,
`device.setCurrent()` and `auto device = Device::getCurrent()` or `auto device =
Device::popCurrent()`.

To synchronize the execution use `device.sync()` or `device.sync(stream)`.

<a name="veda_483"></a>
#### Loading Modules
Just do:
```cpp
auto mod = dev.load("libmymodule.vso");
```

<a name="veda_484"></a>
#### Memory Buffer Objects
The new C++ API uses buffer objects instead of raw pointers. These can be
allocated using `dev.alloc<float>(cnt)`, which will allocate `sizeof(T) * cnt`
bytes of memory.

If you want to use a different stream, just use `dev.alloc<float>(cnt, stream)`.

To allocate HMEM memory, use `dev.alloc<float, veda::HMEM>(size)`.

To copy data between different Buffers, or the host and the VE, just use:
```cpp
auto VE = dev.alloc<float>(cnt);
auto VH = malloc(sizeof(float) * cnt);

VE.to(VH);              // copies all items from VE to VH
VE.to(VH, 1);           // copies the first item from VE to VH
VE[4].to(VH + 4, 1);    // copies the 5th item from VE to VH
VE.from(VH);            // copies all items from VH to VE

auto V2 = dev.alloc<float>(cnt);
V2.to(VE);              // copies all items from V2 to VE
VE.from(V2);            // copies all items from V2 to VE
```

To memset data use:
```cpp
VE.memset(3.1415);      // set all items
VE[5].memset(3.1415);   // set all items starting the 6th
VE[5].memset(3.1415, 1);// set only the 6th item
```

To cast a buffer object to another type:
```cpp
auto Float = dev.alloc<float>(cnt);
auto Int32 = Float.cast<int32_t>(); // Float.cnt() == Int32.cnt()
auto Int16 = Float.cast<int16_t>(); // Float.cnt() == Int16.cnt()*2
```

All buffer objects use shared pointer semantics. When all objects using the same
source pointer are destroyed, it will be automatically freed.

To pass on pointers between methods just pass on the buffer object:

```cpp
veda::Ptr<VEDA, float> func(...) {
	...
	auto ptr = dev.alloc<float>(cnt);
	...
	return ptr;
}
```

<a name="veda_485"></a>
#### Fetching Functions
For fetching functions we provide three helper functions.

1. **C-style or `extern "C"` functions**:
	```cpp
	// VE
	extern "C" int name(int, float, VEDAdeviceptr);

	// VH
	using namespace veda;
	auto func = CFunction::Return<int>(mod, "name");
	auto result = func(0, 3.14f, ptr);
	printf("%i\n", int(result));
	```
	
	The `CFunction::Return<int>` returns you an executable object to an
	C-function on the VE. Whenever you call `func(...)` it issues a kernel
	call. By default we use the stream #0, but you can use
	`func[stream](...)` to define the stream yourself.

	`result` is a future object. When you call `result.wait()` or fetch the
	result using `(TYPE)result` or `result.get()`, it will synchronize the
	execution and provide the return value.

	The `::Return<...>` can be omitted when no return value is expected.

2. **C++-style functions**:
	```cpp
	// VE
	int name(int, float, VEDAdeviceptr);

	// VH
	using namespace veda;
	auto func = Function::Return<int>::Args<int, float, VEDAdeviceptr>(mod, "name");
	auto result = func(0, 3.14f, ptr);
	printf("%i\n", int(result));
	```

	For C++-style functions use `Function` instead of `CFunction`. In this
	case you also need to provide the types of all arguments using
	`Args<...>`.

	Again `::Return<...>` can be omitted when no return value is expected.

	Also struct types can be used as arguments:
	```cpp
	// VE + VH
	namespace whatever {
		template<typename T>
		struct complex {
			T x, y;
		};
	}

	// VE
	void name(VEDAdeviceptr, whatever::complex<float>);

	// VH
	auto func = Function::Args<VEDAdeviceptr, whatever::complex<float>>(mod, "name");
	whatever::complex<float> x = {3.0f, 4.0f};
	func(ptr, x);
	```

3. **Template functions**:
	```cpp
	// VE
	template<typename T, typename D>
	T name(T, float, D);

	template int name<int, VEDAdeviceptr>(int, float, VEDAdeviceptr);

	// VH
	using namespace veda;
	auto func = Template<int, VEDAdeviceptr>::Return<_0>::Args<_0, float, _1>(mod, "name");
	```

	Last, we also support to fetch templated functions. Here it is
	important, that in the VE code, the template gets explicitly
	instantiated using the `template ... name<...>(...);` syntax. Otherwise
	the compiler will not generate this specific templated function.

	On the VH, we first define the template parameters using
	`Template<...>`. Next, as before the return type. If it is
	`::Return<void>`, it can be omitted. And last the arguments, similar as
	before for the `Function`.

	In the code above you see `veda::_0` and `veda::_1`. These correspond to
	the template parameters, `_0` is the 0th, `_1` the 1st, and so on. It is
	necessary to use these template placeholders within `Return<...>` and
	`Args<...>` at the same locations as within the C++ code.

	If your template uses literals, such as:
	```cpp
	template<int i, typename T>
	T name(T a) { return a + i; }

	template float name<0>(float);
	template float name<5>(float);
	template int   name<5>(int);
	```

	You can to use the following code on VH:
	```cpp
	auto name_f0 = Template<Literal<0>, float>::Return<_1>::Args<_1>(...);
	auto name_f5 = Template<Literal<5>, float>::Return<_1>::Args<_1>(...);
	auto name_i0 = Template<Literal<5>, int>  ::Return<_1>::Args<_1>(...);
	```

	It's important that the data type you pass to `Literal<...>` matches the
	data type you use in your `template<...>`. I.e., if you use
	`template<char...>`, then you need to use `Literal('x')` or
	`Literal(char(15))`.

	Only integer-like types (char, short, ...) can be used as template
	literals.

For all function fetching methods it's important, that function arguments match
exactly the ones you use in your VE C++ code. Otherwise fetching the function
will fail at runtime!

---

<a name="veda_5"></a>
## SX-Aurora VE3 support
Since v2.1.0 VEDA supports the SX-Aurora VE3. It's important that your libraries
are compatible to the used architecture. Use these compile and linking flags:

| Architecture | Flags | File Extension
| --- | --- | --- |
| VE1+2 | `-march=ve1 -stdlib=libc++` | *.vso |
| VE3 | `-march=ve3 -stdlib=libc++` | *.vso3 |

To load the library you can just use `vedaModuleLoad(&mod, "libsomething.vso")`
and VEDA will automatically load `libsomething.vso` for VE1+2 or
`libsomething.vso3` for VE3.

By default VEDA determines automatically which architecture to use. You can
override this behavior by setting the env var `VEDA_ARCH=1` or `VEDA_ARCH=3`.
Be warned, you cannot run `VEDA_ARCH=3` on a VE1, but you can use `VEDA_ARCH=1`
on a VE3!

If you are unsure which architecture your library is for, you can use `nreadelf
-h libsomething.vso | grep 'Flags'`. Flags ending with 0 are for VE1, with 1 are
for VE3.

---

<a name="veda_6"></a>
## Limitations/Known Problems:
1. VEDA only supports one ```VEDAcontext``` per device.
1. No unified memory space (yet).
1. VEDA by default uses the current workdirectory for loading modules. This
   behavior can be changed by using the env var ```VE_LD_LIBRARY_PATH```.
1. Due to compiler incompatibilities it can be necessary to adjust the CMake
variable ```${AVEO_NFORT}``` to another compiler.
1. The C++ API can only return fundamental (void, int, short, ...) values.
1. The C++ API cannot compile `...::Args<void>`. Use `...:::Args<>` instead.

---

<a name="veda_7"></a>
## How to build:
```bash
git clone https://github.com/SX-Aurora/veda/
mkdir veda/build
cd veda/build

# Build Option 1: Local installation (default: /usr/local/ve (use -DCMAKE_INSTALL_PREFIX=... for other path))
cmake3 -DVEDA_DIST_TYPE=LOCAL ..
cmake3 --build . --target install 

# Build Option 2: VEOS installation
cmake3 -DVEDA_DIST_TYPE=VEOS ..
cmake3 --build . --target install 

# Build Option 3: Python package
pip3 install illyrian tungl
illyrian cmake3 -DVEDA_DIST_TYPE=PYTHON ..
cmake3 --build . --target dist
```

---

<a name="veda_8"></a>
## How to use:
VEDA has an own CMake find script. This supports 3 modes. The script uses the compilers installed in ```/opt/nec/ve/bin```. You can modify the ```CMAKE_[LANG]_COMPILER``` flags to change that behavior. See the Hello World examples in the [Examples Folder](example)

---

<a name="veda_81"></a>
### 1. VEDA Hybrid Offloading:
This mode is necessary for VEDA offloading applications. It enables to compile host and device code within the same CMake project. For this it is necessary to use different file extensions for the VE code. All ```*.vc``` files get compiled using NCC, ```*.vcpp``` using NC++ and ```*.vf``` with NFORT.

```cmake
SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake /opt/nec/ve/share/veda/cmake)
FIND_PACKAGE(VEDA)
ENABLE_LANGUAGE(VEDA_C VEDA_CXX)

INCLUDE_DIRECTORIES(${VEDA_INCLUDE_DIRS})
ADD_EXECUTABLE(myApp mycode.vc mycode.vcpp)
TARGET_LINK_LIBRARIES(myApp ${VEDA_LIBRARY})
```

---

<a name="veda_82"></a>
### 2. VE Native applications:
This mode enables to compile VE native applications.

```cmake
SET(CMAKE_MODULE_PATH /usr/local/ve/veda/cmake /opt/nec/ve/share/veda/cmake)
FIND_PACKAGE(VEDA)
ENABLE_LANGUAGE(VEDA_C VEDA_CXX)
ADD_EXECUTABLE(myApp mycode.c mycode.cpp)
```

---

<a name="veda_83"></a>
### 3. VE Native Injection:
If you have a CPU application and you don't want to modify the CMake script you can build your project using:
```
cmake -C /usr/local/ve/veda/cmake/InjectVE.cmake /path/to/your/source
```
It will replace the CPU ```C```, ```CXX``` and ```Fortran``` compilers with NCC.
