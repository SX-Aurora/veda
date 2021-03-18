#pragma once

#include <stdint.h>
#include "veda_enums.h"

struct veo_args;
typedef struct veo_args*	VEDAargs;
typedef uint64_t		veo_ptr;

typedef int			VEDAdevice;
typedef int			VEDAstream;
typedef uint64_t		VEDAdeviceptr;
typedef uint64_t		VEDAfunction;
typedef uint64_t (*VEDAhost_function)(void*);
typedef void (*VEDAstream_callback)(VEDAstream, VEDAresult, void*);

#ifdef __cplusplus
	namespace veda {
		class Module;
		class Context;
		class Function;
	}

	typedef veda::Context*	VEDAcontext;
	typedef veda::Module*	VEDAmodule;
#else
	struct __VEDAfunction;
	struct __VEDAcontext;
	struct __VEDAmodule;
	typedef __VEDAcontext*	VEDAcontext;
	typedef __VEDAmodule*	VEDAmodule;
#endif



