#pragma once

extern "C" {
// Memcopy ---------------------------------------------------------------------
VEDAresult	veda_memcpy_d2d		(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size);
VEDAresult	veda_memset_u16		(VEDAdeviceptr dst, const uint16_t value, const size_t size);
VEDAresult	veda_memset_u16_2d	(VEDAdeviceptr dst, const size_t pitch, const uint16_t value, const size_t w, const size_t h);
VEDAresult	veda_memset_u32		(VEDAdeviceptr dst, const uint32_t value, const size_t size);
VEDAresult	veda_memset_u32_2d	(VEDAdeviceptr dst, const size_t pitch, const uint32_t value, const size_t w, const size_t h);
VEDAresult	veda_memset_u64		(VEDAdeviceptr dst, const uint64_t value, const size_t size);
VEDAresult	veda_memset_u64_2d	(VEDAdeviceptr dst, const size_t pitch, const uint64_t value, const size_t w, const size_t h);
VEDAresult	veda_memset_u128	(VEDAdeviceptr dst, const uint64_t x, const uint64_t y, const size_t size);
VEDAresult	veda_memset_u128_2d	(VEDAdeviceptr dst, const size_t pitch, const uint64_t x, const uint64_t y, const size_t w, const size_t h);
VEDAresult	veda_memset_u8		(VEDAdeviceptr dst, const uint8_t value, const size_t size);
VEDAresult	veda_memset_u8_2d	(VEDAdeviceptr dst, const size_t pitch, const uint8_t value, const size_t w, const size_t h);

// Malloc/Free -----------------------------------------------------------------
VEDAresult	veda_mem_alloc		(VEDAdeviceptr vptr, VEDAdeviceptrInfo* info);
VEDAresult	veda_mem_free		(VEDAdeviceptr vptr);
VEDAresult	veda_mem_ptr		(VEDAdeviceptr vptr, VEDAdeviceptrInfo* info);
VEDAresult	veda_mem_swap		(VEDAdeviceptr A, VEDAdeviceptrInfo* Ainfo, VEDAdeviceptr B, VEDAdeviceptrInfo* Binfo);

//------------------------------------------------------------------------------
}
