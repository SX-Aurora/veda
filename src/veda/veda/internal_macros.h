#pragma once

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)
#define MAX_NUMA_NODES 2

//------------------------------------------------------------------------------
inline void veda_throw [[noreturn]] (VEDAresult err, const char* file, const int line) {
#if 1
	const char* msg;
	vedaGetErrorName(err, &msg);
	tungl_log(TUNGL_LEVEL_TRACE, L_MODULE, file, line, "%s", msg);
#endif
	throw err;
}

//------------------------------------------------------------------------------
#define VEDA_THROW(...) veda_throw(__VA_ARGS__, __FILE__, __LINE__)
#define VEDA_ASSERT(COND, ...) if(!(COND)) VEDA_THROW(__VA_ARGS__)
#define CVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	return veda::VEOtoVEDA(err);		}
#define TVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	VEDA_THROW(veda::VEOtoVEDA(err));	}
#define TVEDA(...)	{ VEDAresult err = __VA_ARGS__;	if(err != VEDA_SUCCESS)		VEDA_THROW(err);			}

#define TRY(...)\
	try {\
		__VA_ARGS__\
	} catch(VEDAresult res) {\
		return res;\
	}\
	return VEDA_SUCCESS;

#define GUARDED(...)\
	veda::Guard __guard;\
	TRY(__VA_ARGS__)

#define CREQ(REQ)	({ uint64_t _r = REQ; if(_r == VEO_REQUEST_ID_INVALID) VEDA_THROW(VEDA_ERROR_INVALID_REQID); _r; })
