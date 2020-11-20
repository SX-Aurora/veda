#pragma once
#include <ve_offload.h>
typedef uint64_t veo_sym;
typedef uint64_t veo_lib;
typedef uint64_t veo_ptr;

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

#include "veda.h"
#include "VEDAkernel.h"
#include "VEDAptr.h"
#include "VEDAcontext.h"
#include "VEDAmodule.h"

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)

#define CVEO(ERR)	{ int err = ERR; if(err != VEO_COMMAND_OK) return vedaVEOtoVEDA(err); }
#if 0
#define TRACE(...)	printf("[VEDA Host  ] " __VA_ARGS__)
#else
#define TRACE(...)
#endif

//------------------------------------------------------------------------------
template<typename T>
inline VEDAresult vedaIs(T ptr, VEDAresult err) {
	return dynamic_cast<T>(ptr) != 0 ? VEDA_SUCCESS : err;
}

	const char*	vedaModuleStdLib	(void);
	VEDAresult	vedaCtxExit		(void);
	VEDAresult	vedaCtxMemReport	(void);
	VEDAresult	vedaDeviceExit		(void);
	VEDAresult	vedaDeviceGetInfo	(uint64_t* value, const char* file, const VEDAdevice device);
	VEDAresult	vedaDeviceGetPhysicalIdx(int* idx, const VEDAdevice device);
	VEDAresult	vedaDeviceGetSensorIdx	(int* idx, const VEDAdevice device);
	VEDAresult	vedaDeviceInitCount	(std::set<int>& devices);
	VEDAresult	vedaDeviceInitMapping	(const std::set<int>& devices);
	VEDAresult	vedaIsCTX		(VEDAcontext ctx);
	VEDAresult	vedaIsDevice		(const VEDAdevice device);
	VEDAresult	vedaIsInitialized	(void);
	VEDAresult	vedaKernelGetName	(const char** name, VEDAkernel kernel);
	VEDAresult	vedaSemAcquire		(void);
	VEDAresult	vedaSemShutdown		(void);
	VEDAresult	vedaSetInitialized	(const bool value);
	VEDAresult	vedaVEOtoVEDA		(const int err);
	int		vedaOmpThreads		(void);
	void		vedaSemRelease		(void);
inline	VEDAresult	vedaIsContext		(VEDAcontext ctx)	{	return vedaIs(ctx, VEDA_ERROR_INVALID_CONTEXT);		}
inline	VEDAresult	vedaIsModule		(VEDAmodule mod) 	{	return vedaIs(mod, VEDA_ERROR_INVALID_MODULE);		}

struct VEDAguard {
	VEDAresult acquire(void) {
		return vedaSemAcquire();
	}

	~VEDAguard(void) {
		vedaSemRelease();
	}
};

#define GUARDED(...)\
	CVEDA(vedaIsInitialized());\
	VEDAguard __guard;\
	CVEDA(__guard.acquire());\
	__VA_ARGS__\
	return VEDA_SUCCESS

#define CREQ(REQ)	({ uint64_t _r = REQ; if(_r == VEO_REQUEST_ID_INVALID) return VEDA_ERROR_INVALID_REQID; _r; })

//------------------------------------------------------------------------------
