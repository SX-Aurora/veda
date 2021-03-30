#pragma once

#include <stdint.h>
#include "veda_enums.h"
#include "veda_macros.h"

struct veo_args;
typedef struct veo_args*	VEDAargs;
typedef uint64_t		veo_ptr;

typedef int			VEDAdevice;
typedef uint32_t		VEDAidx;
typedef uint64_t		VEDAoffset;
typedef int			VEDAstream;
typedef uint64_t		VEDAfunction;
typedef uint64_t (*VEDAhost_function)(void*);
typedef void (*VEDAstream_callback)(VEDAstream, VEDAresult, void*);

struct __VEDAdeviceptr;
typedef const struct __VEDAdeviceptr* VEDAdeviceptr;

struct __VEDAdeviceptr {
	char* _;

#if __cplusplus
	template<typename T>
	struct Tuple {
		T* const	ptr;
		const size_t	size;
		inline Tuple(T* _ptr, const size_t _size) : ptr(_ptr), size(_size) {}
	};

							__VEDAdeviceptr	(void) = delete;
					Tuple<void>	ptrSize		(void) const;
					VEDAdevice	device		(void) const;
					VEDAdeviceptr	base		(void) const;
					VEDAidx		idx		(void) const;
					size_t		offset		(void) const;
					size_t		size		(void) const;
					void*		ptr		(void) const;
		template<typename T>	Tuple<T>	ptrSize		(void) const;
		template<typename T>	operator	T		(void) const;
		
	#ifndef __NEC__
					void*		hmem		(void) const;
	#endif
#endif
};

#ifdef __cplusplus
	namespace veda {
		class Module;
		class Context;
	}

	typedef veda::Context*		VEDAcontext;
	typedef veda::Module*		VEDAmodule;
#else
	struct __VEDAcontext;
	struct __VEDAmodule;
	typedef struct __VEDAcontext*	VEDAcontext;
	typedef struct __VEDAmodule*	VEDAmodule;
#endif
