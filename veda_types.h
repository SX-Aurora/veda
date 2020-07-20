#pragma once

typedef int VEDAdevice;
typedef uint64_t VEDAdeviceptr;

struct __VEDAcontext;
struct __VEDAmodule;
struct __VEDAstream;
struct veo_args;

typedef struct __VEDAcontext* VEDAcontext;
typedef uint64_t VEDAfunction;
typedef struct __VEDAmodule* VEDAmodule;
typedef struct __VEDAstream* VEDAstream;
typedef struct veo_args* VEDAargs;
