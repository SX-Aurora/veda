#pragma once

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
VEDAresult	vedaMemPtr			(void** ptr, VEDAdeviceptr vptr);
VEDAresult	vedaMemPtrSize		(void** ptr, size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaMemSize			(size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaGetErrorName	(VEDAresult error, const char** pStr);
VEDAresult	vedaGetErrorString	(VEDAresult error, const char** pStr);

#ifdef __cplusplus
}

template<typename T>
VEDAresult vedaMemPtr(T** ptr, VEDAdeviceptr vptr) {
	return vedaMemPtr((void**)ptr, vptr);
}

#endif
