#pragma once

#include <veda/api.h>
#include <veda/enums.h>

typedef VEDAstream veraStream_t;
typedef VEDAresult veraError_t;

typedef struct  {
	int		device;
	void*		devicePointer;
	void*		hostPointer;
	veraMemoryType	type;
} veraPointerAttributes;

typedef struct {
	char	name[256];
	int	abiVersion;
	int	clockBase;
	int	clockRate;
	int	firmwareVersion;
	int	l1DCacheSize;
	int	l1ICacheSize;
	int	l2CacheSize;
	int	memoryClockRate;
	int	multiProcessorCount;
	int	singleToDoublePrecisionPerfRatio;
	size_t	totalGlobalMem;
} veraDeviceProp;

typedef struct {
	size_t	pitch;
	void*	ptr;
	size_t	xsize;
	size_t	ysize;
} veraPitchedPtr;

typedef struct {
	size_t	depth;
	size_t	height;
	size_t	width;
} veraExtent;

typedef VEDAargs		veraArgs_t;
typedef VEDAfunction		veraFunction_t;
typedef VEDAhost_function	veraHostFn_t;
typedef VEDAmodule		veraModule_t;
typedef VEDAstream_callback	veraStreamCallback_t;
typedef VEDAstack		veraStack_t;
