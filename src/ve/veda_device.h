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

VEDAresult	vedaMemAlloc		(VEDAdeviceptr vptr, const size_t size);
VEDAresult	vedaMemAllocPtr		(void** ptr, VEDAdeviceptr vptr, const size_t size);
VEDAresult	vedaMemPtr		(void** ptr, VEDAdeviceptr vptr);
VEDAresult	vedaMemPtrSize		(void** ptr, size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaMemSize		(size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaGetErrorName	(VEDAresult error, const char** pStr);
VEDAresult	vedaGetErrorString	(VEDAresult error, const char** pStr);

#ifdef __cplusplus
}

template<typename T>
VEDAresult vedaMemPtr(T** ptr, VEDAdeviceptr vptr) {
	return vedaMemPtr((void**)ptr, vptr);
}

#endif
