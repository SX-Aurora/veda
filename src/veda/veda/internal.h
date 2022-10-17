#pragma once
#include <ve_offload.h>

#if VEO_API_VERSION < 14
#error "VEO_API_VERSION >= 14 required"
#endif

typedef uint64_t veo_sym;
typedef uint64_t veo_lib;

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <libudev.h>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <deque>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <dlfcn.h>

#define L_MODULE "VEDA-VH"
#include <tungl/c.h>

#include "api.h"
#include "veda/internal_types.h"

namespace veda {
	class Module;
	class Context;
	class Device;
	class NUMA;
	struct Stream;
	class StreamGuard;
}

#include "internal_macros.h"
#include "Semaphore.h"
#include "Kernel.h"
#include "Module.h"
#include "Context.h"
#include "contexts/contexts.h"
#include "Device.h"
#include "devices/devices.h"
#include "Stream.h"

//------------------------------------------------------------------------------
namespace veda {
	const char*	stdLib		(void);
	int		ompThreads	(void);
	bool		isMemTrace	(void);
	VEDAresult	VEOtoVEDA	(const int err);
	void		checkInitialized(void);
	void		setInitialized	(const bool value);
inline	void		checkContext	(VEDAcontext ctx)	{	if(dynamic_cast<VEDAcontext>(ctx) == 0)	VEDA_THROW(VEDA_ERROR_INVALID_CONTEXT);	}
inline	void		checkModule	(VEDAmodule mod) 	{	if(dynamic_cast<VEDAmodule> (mod) == 0) VEDA_THROW(VEDA_ERROR_INVALID_MODULE);	}

//------------------------------------------------------------------------------
}

#include "Guard.hpp" // requires veda::functions
