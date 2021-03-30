#pragma once

inline	VEDAdevice		__VEDAdeviceptr::device		(void) const	{	return VEDA_GET_DEVICE((uint64_t)this);							}
inline	VEDAidx			__VEDAdeviceptr::idx		(void) const	{	return VEDA_GET_IDX((uint64_t)this);							}
inline	size_t			__VEDAdeviceptr::offset		(void) const	{	return VEDA_GET_OFFSET((uint64_t)this);							}
inline	VEDAdeviceptr		__VEDAdeviceptr::base		(void) const	{	return (VEDAdeviceptr)VEDA_SET_DEVICE(device(), VEDA_SET_IDX(0, idx()));		}
template<typename T> inline	__VEDAdeviceptr::operator T	(void) const	{	return static_cast<T>(ptr());								}
inline	void*			__VEDAdeviceptr::ptr		(void) const	{	void* ptr = 0; vedaMemPtr(&ptr, this); return ptr;					}
inline	size_t			__VEDAdeviceptr::size		(void) const	{	size_t size = 0; vedaMemSize(&size, this); return size;					}

inline __VEDAdeviceptr::Tuple<void> __VEDAdeviceptr::ptrSize(void) const {
	void* ptr = 0;
	size_t size = 0;
	vedaMemPtrSize(&ptr, &size, this);
	return {ptr, size};
}

template<typename T> inline __VEDAdeviceptr::Tuple<T> __VEDAdeviceptr::ptrSize(void) const {
	void* ptr = 0;
	size_t size = 0;
	vedaMemPtrSize(&ptr, &size, this);
	return {static_cast<T*>(ptr), size};
}
