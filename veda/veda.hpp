#pragma once
#include <ve_offload.h>
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
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <dlfcn.h>

#include "../veda.h"

namespace veda {
	class Module;
	class Context;
	class Device;
	class NUMA;
	struct Stream;
}

#include "macros.hpp"
#include "Semaphore.hpp"
#include "Kernel.hpp"
#include "Ptr.hpp"
#include "Module.hpp"
#include "Device.hpp"
#include "Devices.hpp"
#include "Stream.hpp"
#include "Context.hpp"
#include "Contexts.hpp"

//------------------------------------------------------------------------------
namespace veda {
	const char*	stdLib		(void);
	int		ompThreads	(void);
	VEDAresult	VEOtoVEDA	(const int err);
	void		checkInitialized(void);
	void		setInitialized	(const bool value);
inline	void		checkContext	(VEDAcontext ctx)	{	if(dynamic_cast<VEDAcontext>(ctx) == 0)	throw VEDA_ERROR_INVALID_CONTEXT;	}
inline	void		checkModule	(VEDAmodule mod) 	{	if(dynamic_cast<VEDAmodule> (mod) == 0) throw VEDA_ERROR_INVALID_MODULE;	}

//------------------------------------------------------------------------------
}

#include "Guard.hpp" // requires veda::functions
