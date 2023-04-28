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
#include <memory>
#include <future>

#define L_MODULE "VEDA-VH"
#include <tungl/c.h>

#include "../api.h"
#include "../cpp/Exception.h"
#include "types.h"

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
class Module;
class Context;
class Device;
class NUMA;
struct Stream;
class StreamGuard;

template<typename T>
using is_function_ptr = std::is_function<typename std::remove_pointer<T>::type>;
using ResultPtr = std::shared_ptr<uint64_t>;
using ReqId = uint64_t;

class Result final : public ResultPtr {
	struct Deleter {
		inline void operator()(uint64_t* const p) {}
	};

public:
	template<typename T>
	inline Result(T* result = 0) :
		ResultPtr((uint64_t*)result, std::move(Deleter()))
	{
		static_assert(sizeof(T) == sizeof(uint64_t));
	}
};

// Stolen from: https://stackoverflow.com/questions/18695564/deducing-a-function-pointer-return-type
template <class F>
struct return_type;

template <class R, class... A>
struct return_type<R (*)(A...)> {
	typedef R type;
};


//------------------------------------------------------------------------------
	}
}

#include "macros.h"
#include "Semaphore.h"
#include "Kernel.h"
#include "Module.h"
#include "Context.h"
#include "contexts/contexts.h"
#include "Device.h"
#include "devices/devices.h"
#include "profiler/profiler.h"
#include "Stream.h"
#include "helper.h"

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
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
}

#include "Guard.hpp"
