#pragma once

#ifndef __ve__
#error Inclusion of veda_device.h in host code detected! veda_device.h is only meant for device code, use veda.h instead. If you want to use VEDA data types in a shared header file, use veda_types.h.
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <veda/enums.h>
#include <veda/types.h>
#include <veda/macros.h>

#ifdef __cplusplus
#define __global__ extern "C"
#else
#define __global__
#endif

__global__	VEDAresult	vedaGetErrorName	(VEDAresult error, const char** pStr);
__global__	VEDAresult	vedaGetErrorString	(VEDAresult error, const char** pStr);
__global__	VEDAresult	vedaMemAlloc		(VEDAdeviceptr vptr, const size_t size);
__global__	VEDAresult	vedaMemAllocPtr		(void** ptr, VEDAdeviceptr vptr, const size_t size);
__global__	VEDAresult	vedaMemPtr		(void** ptr, VEDAdeviceptr vptr);
__global__	VEDAresult	vedaMemPtrSize		(void** ptr, size_t* size, VEDAdeviceptr vptr);
__global__	VEDAresult	vedaMemSize		(size_t* size, VEDAdeviceptr vptr);
__global__	VEDAresult	vedaMemSwap		(VEDAdeviceptr A, VEDAdeviceptr B);
__global__	VEDAresult	vedaMemcpy		(void* dst, const void* src, const size_t bytes);
__global__	VEDAresult	vedaMemsetD128		(void* ptr, const uint64_t x, const uint64_t y, const size_t cnt);
__global__	VEDAresult	vedaMemsetD16		(void* ptr, const uint16_t value, const size_t cnt);
__global__	VEDAresult	vedaMemsetD2D128	(void* ptr, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h);
__global__	VEDAresult	vedaMemsetD2D16		(void* ptr, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
__global__	VEDAresult	vedaMemsetD2D32		(void* ptr, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
__global__	VEDAresult	vedaMemsetD2D64		(void* ptr, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
__global__	VEDAresult	vedaMemsetD2D8		(void* ptr, const size_t pitch, const uint8_t  value, const size_t w, const size_t h);
__global__	VEDAresult	vedaMemsetD32		(void* ptr, const uint32_t value, const size_t cnt);
__global__	VEDAresult	vedaMemsetD64		(void* ptr, const uint64_t value, const size_t cnt);
__global__	VEDAresult	vedaMemsetD8		(void* ptr, const uint8_t  value, const size_t cnt);

#include <veda/ptr.h>
