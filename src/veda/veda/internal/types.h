#pragma once

#include <stddef.h> // size_t

typedef struct VEDAdeviceptrInfo_struct {
	void*	ptr;
	size_t	size;
	size_t	isDelayed; // bool has different size on VE and x86!

#if __cplusplus
	inline VEDAdeviceptrInfo_struct(void) :
		ptr		(0),
		size		(0),
		isDelayed	(0)
	{}

	inline VEDAdeviceptrInfo_struct(void* _ptr, const size_t _size, const size_t isDelayed) :
		ptr		(_ptr),
		size		(_size),
		isDelayed	(isDelayed)
	{}
#endif
} VEDAdeviceptrInfo;

static_assert(sizeof(VEDAdeviceptrInfo) == 24);
