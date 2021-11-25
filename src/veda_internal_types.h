#pragma once

#include <stdint.h>
#include "veda_enums.h"
#include "veda_macros.h"

typedef struct VEDAdeviceptrInfo_struct {
	void*	ptr;
	size_t	size;
	int8_t	inFlight;

#if __cplusplus
	inline VEDAdeviceptrInfo_struct(void) :
		ptr	(0),
		size	(0),
		inFlight(0)
	{}

	inline VEDAdeviceptrInfo_struct(void* _ptr, const size_t _size) :
		ptr	(_ptr),
		size	(_size),
		inFlight(0)
	{}
#endif
} VEDAdeviceptrInfo;

static_assert(sizeof(VEDAdeviceptrInfo) == 24);
