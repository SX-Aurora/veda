#pragma once

#include <stdint.h>
#include "veda_enums.h"

typedef int		VEDAdevice;
typedef uint64_t	VEDAdeviceptr;
typedef uint64_t	veo_ptr;

struct __VEDAmodule;
struct __VEDAcontext;
struct veo_args;

typedef uint64_t VEDAfunction;
typedef struct __VEDAcontext* VEDAcontext;
typedef struct __VEDAmodule* VEDAmodule;
typedef int VEDAstream;
typedef struct veo_args* VEDAargs;

typedef uint64_t (*VEDAhost_function)(void*);
typedef void (*VEDAstream_callback)(VEDAstream, VEDAresult, void*);

