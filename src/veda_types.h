#pragma once

#include <stdint.h>
#include "veda_enums.h"
#include "veda_macros.h"
#if __cplusplus
#include <type_traits>
#endif

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
	public:
		__VEDAdeviceptr(void) = delete;

		inline	VEDAdevice	device	(void) const	{	return VEDA_GET_DEVICE((uint64_t)this);						}
		inline	VEDAidx		idx	(void) const	{	return VEDA_GET_IDX((uint64_t)this);						}
		inline	size_t		offset	(void) const	{	return VEDA_GET_OFFSET((uint64_t)this);						}
		inline	VEDAdeviceptr	base	(void) const	{	return (VEDAdeviceptr)VEDA_SET_DEVICE(device(), VEDA_SET_IDX(0, idx()));	}
		
	#ifndef __NEC__
		void*	raw	(void) const;
		void*	hmem	(void) const;
		template<typename T> operator T (void) const;
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
