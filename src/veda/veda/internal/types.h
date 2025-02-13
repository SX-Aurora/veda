#pragma once

#include <stddef.h> // size_t

typedef struct VEDAdeviceptrInfo_struct {
	void*	ptr;
	size_t	size;
	size_t	flags;

#if __cplusplus
	inline VEDAdeviceptrInfo_struct(void) :
		ptr	(0),
		size	(0),
		flags	(0)
	{}

	inline VEDAdeviceptrInfo_struct(void* _ptr, const size_t _size, const size_t _flags = 0) :
		ptr	(_ptr),
		size	(_size),
		flags	(_flags)
	{}

	inline void addFlag(const size_t flag) {
		flags |= flag;
	}

	inline bool isDelayed	(void) const {	return flags & VEDA_IS_DELAYED;		}
	inline bool isReleased	(void) const {	return flags & VEDA_IS_RELEASED;	}
#endif
} VEDAdeviceptrInfo;

typedef struct VEDAdeviceptrInfoS_struct {
	void*	ptr;
	size_t	size;
} VEDAdeviceptrInfoS;

static_assert(sizeof(VEDAdeviceptrInfo)  == 24);
static_assert(sizeof(VEDAdeviceptrInfoS) == 16);
