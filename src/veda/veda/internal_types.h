#pragma once

#include <stdint.h>

typedef struct VEDAdeviceptrInfo_struct {
	void*	ptr;
	size_t	size;

#if __cplusplus
	inline VEDAdeviceptrInfo_struct(void) :
		ptr	(0),
		size	(0)
	{}

	inline VEDAdeviceptrInfo_struct(void* _ptr, const size_t _size) :
		ptr	(_ptr),
		size	(_size)
	{}
#endif
} VEDAdeviceptrInfo;

static_assert(sizeof(VEDAdeviceptrInfo) == 16);
