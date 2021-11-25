#pragma once

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)
#define MAX_NUMA_NODES 2

#ifndef NDEBUG
#define VEDA_THROW(err) do { printf("[EXCEPTION] %s (%i)\n", __FILE__, __LINE__); throw err; }while(false)
#else
#define VEDA_THROW(err) throw err
#endif

#define CVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	return veda::VEOtoVEDA(err);		}
#define TVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	VEDA_THROW(veda::VEOtoVEDA(err));	}
#define TVEDA(...)	{ VEDAresult err = __VA_ARGS__;	if(err != VEDA_SUCCESS)		VEDA_THROW(err);			}
#define ASSERT(...)	{ if(!(__VA_ARGS__))						VEDA_THROW(VEDA_ERROR_ASSERT);		}

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
