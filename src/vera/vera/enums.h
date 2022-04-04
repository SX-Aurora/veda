#pragma once

#include <veda/api.h>

enum veraMemoryType_enum {
	veraMemoryTypeUnregistered	= 0,
	veraMemoryTypeHost		= 1,
	veraMemoryTypeDevice		= 2,
	veraMemoryTypeManaged		= 3
};

typedef enum veraMemoryType_enum veraMemoryType;

enum veraMemcpyKind_enum {
	veraMemcpyHostToHost		= 0,
	veraMemcpyHostToDevice		= 1,
	veraMemcpyDeviceToHost		= 2,
	veraMemcpyDeviceToDevice	= 3,
	veraMemcpyDefault		= 4
};
typedef enum veraMemcpyKind_enum veraMemcpyKind;

enum veraDeviceAttr_enum {
	veraDevAttrClockRate				=	VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE,
	veraDevAttrClockBase				=	VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE,
	veraDevAttrMultiProcessorCount			=	VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT,
	veraDevAttrMemoryClockRate			=	VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE,
	veraDevAttrL1DCacheSize				=	VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE,
	veraDevAttrL1ICacheSize				=	VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE,
	veraDevAttrL2CacheSize				=	VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE,
	veraDevAttrABIVersion				=	VEDA_DEVICE_ATTRIBUTE_ABI_VERSION,
	veraDevAttrSingleToDoublePrecisionPerfRatio	=	VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO,
	veraDevAttrFirmwareVersion			=	VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION
};
typedef enum veraDeviceAttr_enum veraDeviceAttr;
