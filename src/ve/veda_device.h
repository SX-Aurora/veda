#pragma once

#ifndef __ve__
#error Inclusion of veda_device.h in host code detected! veda_device.h is only meant for device code, use veda.h instead. If you want to use VEDA data types in a shared header file, use veda_types.h.
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <veda_enums.h>
#include <veda_types.h>
#include <veda_macros.h>

#ifdef __cplusplus
extern "C" {
#endif

VEDAresult	vedaGetErrorName	(VEDAresult error, const char** pStr);
VEDAresult	vedaGetErrorString	(VEDAresult error, const char** pStr);
VEDAresult	vedaMemAlloc		(VEDAdeviceptr vptr, const size_t size);
VEDAresult	vedaMemAllocPtr		(void** ptr, VEDAdeviceptr vptr, const size_t size);
VEDAresult	vedaMemPtr		(void** ptr, VEDAdeviceptr vptr);
VEDAresult	vedaMemPtrSize		(void** ptr, size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaMemsetD128		(void* ptr, const uint64_t x, const uint64_t y, const size_t cnt);
VEDAresult	vedaMemsetD16		(void* ptr, const uint16_t value, const size_t cnt);
VEDAresult	vedaMemsetD2D128	(void* ptr, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h);
VEDAresult	vedaMemsetD2D16		(void* ptr, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
VEDAresult	vedaMemsetD2D32		(void* ptr, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
VEDAresult	vedaMemsetD2D64		(void* ptr, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
VEDAresult	vedaMemsetD2D8		(void* ptr, const size_t pitch, const uint8_t  value, const size_t w, const size_t h);
VEDAresult	vedaMemsetD32		(void* ptr, const uint32_t value, const size_t cnt);
VEDAresult	vedaMemsetD64		(void* ptr, const uint64_t value, const size_t cnt);
VEDAresult	vedaMemsetD8		(void* ptr, const uint8_t  value, const size_t cnt);
VEDAresult	vedaMemSize		(size_t* size, VEDAdeviceptr vptr);

#ifdef __cplusplus
}

#include <veda_ptr.h>

#endif
