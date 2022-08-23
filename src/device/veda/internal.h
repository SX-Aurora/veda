#pragma once

#include "device.h"
#include "omp.h"
#include <veda/internal_types.h>
#include <unordered_map>
#include <cstring>
#include <cstdlib>
#include <veo_ve.h>

#define L_MODULE "VEDA-VE"
#include <tungl/c.h>

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)

VEDAresult vedaAssignPtr(VEDAdeviceptr vptr, void* ptr, const size_t size);
VEDAresult vedaRemovePtr(VEDAdeviceptr vptr);

__global__	VEDAresult	veda_mem_free		(VEDAdeviceptr vptr);
__global__	VEDAresult	veda_mem_remove		(VEDAdeviceptr vptr);
__global__	VEDAresult	veda_mem_swap		(VEDAdeviceptr A, VEDAdeviceptr B);
__global__	size_t		veda_mem_size		(VEDAdeviceptr vptr);
__global__	void*		veda_mem_assign		(VEDAdeviceptr vptr, const size_t size);
__global__	void*		veda_mem_ptr		(VEDAdeviceptr vptr);

__global__	VEDAresult	veda_memcpy_d2d		(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size);
__global__	VEDAresult	veda_memset_u128	(VEDAdeviceptr dst, const uint64_t x, const uint64_t y, const size_t size);
__global__	VEDAresult	veda_memset_u128_2d	(VEDAdeviceptr dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h);
__global__	VEDAresult	veda_memset_u16		(VEDAdeviceptr dst, const uint16_t value, const size_t size);
__global__	VEDAresult	veda_memset_u16_2d	(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_memset_u32		(VEDAdeviceptr dst, const uint32_t value, const size_t size);
__global__	VEDAresult	veda_memset_u32_2d	(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_memset_u64		(VEDAdeviceptr dst, const uint64_t value, const size_t size);
__global__	VEDAresult	veda_memset_u64_2d	(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_memset_u8		(VEDAdeviceptr dst, const uint8_t value, const size_t size);
__global__	VEDAresult	veda_memset_u8_2d	(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h);

__global__	VEDAresult	veda_raw_memcpy_d2d	(void* dst, void* src, const size_t size);
__global__	VEDAresult	veda_raw_memset_u128	(void* dst, const uint64_t x, const uint64_t y, const size_t size);
__global__	VEDAresult	veda_raw_memset_u128_2d	(void* dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h);
__global__	VEDAresult	veda_raw_memset_u16	(void* dst, const uint16_t value, const size_t size);
__global__	VEDAresult	veda_raw_memset_u16_2d	(void* dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_raw_memset_u32	(void* dst, const uint32_t value, const size_t size);
__global__	VEDAresult	veda_raw_memset_u32_2d	(void* dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_raw_memset_u64	(void* dst, const uint64_t value, const size_t size);
__global__	VEDAresult	veda_raw_memset_u64_2d	(void* dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
__global__	VEDAresult	veda_raw_memset_u8	(void* dst, const uint8_t value, const size_t size);
__global__	VEDAresult	veda_raw_memset_u8_2d	(void* dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h);
