#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
// Memset
//------------------------------------------------------------------------------
#define MEMSET(T, C) template<> void memset<T, sizeof(C)>(T ptr, const void* value, const size_t cnt, const StreamId stream) {\
	auto cvalue = *(const C*)value;\
	if constexpr (sizeof(C) == 1)	L_TRACE("veda::memset::8(%p, 0x%02X, %llu, %i)",	ptr, (int)cvalue, cnt, stream);\
	if constexpr (sizeof(C) == 2)	L_TRACE("veda::memset::16(%p, 0x%04X, %llu, %i)",	ptr, (int)cvalue, cnt, stream);\
	if constexpr (sizeof(C) == 4)	L_TRACE("veda::memset::32(%p, 0x%08X, %llu, %i)",	ptr,      cvalue, cnt, stream);\
	if constexpr (sizeof(C) == 8)	L_TRACE("veda::memset::64(%p, 0x%016llX, %llu, %i)",	ptr,      cvalue, cnt, stream);\
	internal::devices::get(ptr).ctx().memset(ptr, cvalue, cnt, stream);\
}

MEMSET(VEDA,	uint8_t)
MEMSET(VEDA,	uint16_t)
MEMSET(VEDA,	uint32_t)
MEMSET(VEDA,	uint64_t)
MEMSET(HMEM,	uint8_t)
MEMSET(HMEM,	uint16_t)
MEMSET(HMEM,	uint32_t)
MEMSET(HMEM,	uint64_t)
#undef MEMSET

//------------------------------------------------------------------------------
#define MEMSET(T) template<> void memset<T, 16>(T ptr, const void* value, const size_t cnt, const StreamId stream) {\
	auto cvalue = (const uint64_t*)value;\
	L_TRACE("veda::memset::%i(%p, 0x%016X%016X, %llu, %i)", ptr, value, cnt, stream);\
	internal::devices::get(ptr).ctx().memset(ptr, cvalue[0], cvalue[1], cnt, stream);\
}

MEMSET(VEDA)
MEMSET(HMEM)
#undef MEMSET

//------------------------------------------------------------------------------
// Free
//------------------------------------------------------------------------------
template<> void Base<VEDA>::Deleter::operator()(const VEDA ptr) const {
	L_TRACE("veda::VEDA::free(%p, %i)", ptr, m_stream);
	internal::devices::get(ptr).ctx().memFree(ptr, m_stream);
}

//------------------------------------------------------------------------------
template<> void Base<HMEM>::Deleter::operator()(const HMEM ptr) const {
	L_TRACE("veda::HMEM::free(%p)", ptr);
	internal::hmemfree(ptr);
}

//------------------------------------------------------------------------------
// VEDA <> VEDA
//------------------------------------------------------------------------------
template<> void Copy::copy_<VEDA, VEDA>(VEDA dst, VEDA src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::d2d(%p > %p, %lli, %i)", src, dst, size, stream);
	internal::devices::get(dst).ctx().memcpyD2D(dst, src, size, stream);
}

//------------------------------------------------------------------------------
// void* <> VEDA
//------------------------------------------------------------------------------
template<> void Copy::copy_<VEDA, void*>(VEDA dst, void* src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::h2d(%p, %p, %lli, %i)", dst, src, size, stream);
	internal::devices::get(dst).ctx().memcpyH2D(dst, src, size, stream);
}

//------------------------------------------------------------------------------
template<> void Copy::copy_<void*, VEDA>(void* dst, VEDA src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::d2h(%p, %p, %lli, %i)", dst, src, size, stream);
	internal::devices::get(src).ctx().memcpyD2H(dst, src, size, stream);
}

//------------------------------------------------------------------------------
// HMEM <> HMEM
//------------------------------------------------------------------------------
template<> void Copy::copy_<HMEM, HMEM>(HMEM dst, HMEM src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::d2d(%p > %p, %lli, %i)", src, dst, size, stream);
	internal::hmemcpy(dst, src, size);
}

//------------------------------------------------------------------------------
// void* <> HMEM
//------------------------------------------------------------------------------
template<> void Copy::copy_<HMEM, void*>(HMEM dst, void* src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::h2d(%p, %p, %lli, %i)", dst, src, size, stream);
	internal::hmemcpy(dst, (HMEM)src, size);
}


//------------------------------------------------------------------------------
template<> void Copy::copy_<void*, HMEM>(void* dst, HMEM src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::d2h(%p, %p, %lli, %i)", dst, src, size, stream);
	internal::hmemcpy((HMEM)dst, src, size);
}

//------------------------------------------------------------------------------
// HMEM <> VEDA
//------------------------------------------------------------------------------
template<> void Copy::copy_<HMEM, VEDA>(HMEM dst, VEDA src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::d2d(%p > %p, %lli, %i)", src, dst, size, stream);
	auto info = internal::devices::get(src).ctx().getPtr(src, stream);
	internal::hmemcpy(dst, (HMEM)info.ptr, size);
}

//------------------------------------------------------------------------------
template<> void Copy::copy_<VEDA, HMEM>(VEDA dst, HMEM src, const size_t size, const StreamId stream) {
	L_TRACE("veda::copy::h2d(%p, %p, %lli, %i)", dst, src, size, stream);
	auto info = internal::devices::get(dst).ctx().getPtr(dst, stream);
	internal::hmemcpy((HMEM)info.ptr, src, size);
}

//------------------------------------------------------------------------------
	}
}