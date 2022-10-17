#pragma once

enum VEDAresult_enum {
	VEDA_SUCCESS = 0,
	VEDA_ERROR_ALREADY_INITIALIZED,
	VEDA_ERROR_CANNOT_CREATE_CONTEXT,
	VEDA_ERROR_CANNOT_CREATE_DEVICE,
	VEDA_ERROR_CANNOT_CREATE_STREAM,
	VEDA_ERROR_CANNOT_POP_CONTEXT,
	VEDA_ERROR_CANT_READ_SENSOR_FILE,
	VEDA_ERROR_FUNCTION_NOT_FOUND,
	VEDA_ERROR_INITIALIZING_DEVICE,
	VEDA_ERROR_INVALID_ARGS,
	VEDA_ERROR_INVALID_CONTEXT,
	VEDA_ERROR_INVALID_COREIDX,
	VEDA_ERROR_INVALID_DEVICE,
	VEDA_ERROR_INVALID_MODULE,
	VEDA_ERROR_INVALID_REQID,
	VEDA_ERROR_INVALID_STREAM,
	VEDA_ERROR_INVALID_VALUE,
	VEDA_ERROR_MODULE_NOT_FOUND,
	VEDA_ERROR_NOT_IMPLEMENTED,
	VEDA_ERROR_NOT_INITIALIZED,
	VEDA_ERROR_NO_DEVICES_FOUND,
	VEDA_ERROR_NO_SENSOR_FILE,
	VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED,
	VEDA_ERROR_OUT_OF_BOUNDS,
	VEDA_ERROR_OUT_OF_MEMORY,
	VEDA_ERROR_SHUTTING_DOWN,
	VEDA_ERROR_UNABLE_TO_DETECT_DEVICES,
	VEDA_ERROR_UNKNOWN_CONTEXT,
	VEDA_ERROR_UNKNOWN_DEVICE,
	VEDA_ERROR_UNKNOWN_KERNEL,
	VEDA_ERROR_UNKNOWN_MODULE,
	VEDA_ERROR_UNKNOWN_PPTR,
	VEDA_ERROR_UNKNOWN_ATTRIBUTE,
	VEDA_ERROR_UNKNOWN_STREAM,
	VEDA_ERROR_UNKNOWN_VPTR,
	VEDA_ERROR_VEDA_LD_LIBRARY_PATH_NOT_DEFINED,
	VEDA_ERROR_VEO_COMMAND_ERROR,
	VEDA_ERROR_VEO_COMMAND_EXCEPTION,
	VEDA_ERROR_VEO_COMMAND_UNFINISHED,
	VEDA_ERROR_VEO_COMMAND_UNKNOWN_ERROR,
	VEDA_ERROR_VEO_STATE_BLOCKED,
	VEDA_ERROR_VEO_STATE_RUNNING,
	VEDA_ERROR_VEO_STATE_SYSCALL,
	VEDA_ERROR_VEO_STATE_UNKNOWN,
	VEDA_ERROR_VPTR_ALREADY_ALLOCATED,
	VEDA_ERROR_TODO,
	VEDA_ERROR_ASSERT,
	VEDA_ERROR_CONTEXT_IS_DESTROYED,
	VEDA_ERROR_INVALID_HANDLE,
	VEDA_ERROR_UNKNOWN,
	VEDA_ERROR_INVALID_DTYPE,
	VEDA_ERROR_OFFSET_NOT_ALLOWED,
	VEDA_ERROR_UNKNOWN_HMEM
};

enum VEDAdevice_attribute_enum {
	VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE,
	VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE,
	VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
	VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE,
	VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE,
	VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE,
	VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE,
	VEDA_DEVICE_ATTRIBUTE_LLC_CACHE_SIZE,
	VEDA_DEVICE_ATTRIBUTE_ABI_VERSION,
	VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO,
	VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION,
	VEDA_DEVICE_ATTRIBUTE_MODEL
};

enum VEDAargs_intent_enum {
	VEDA_ARGS_INTENT_IN	= 0,
	VEDA_ARGS_INTENT_INOUT	= 1,
	VEDA_ARGS_INTENT_OUT	= 2
};

enum VEDAcontext_mode_enum {
	VEDA_CONTEXT_MODE_OMP		= 0,
	VEDA_CONTEXT_MODE_SCALAR	= 1
};

typedef enum VEDAresult_enum		VEDAresult;
typedef enum VEDAdevice_attribute_enum	VEDAdevice_attribute;
typedef enum VEDAargs_intent_enum	VEDAargs_intent;
typedef enum VEDAcontext_mode_enum	VEDAcontext_mode;

