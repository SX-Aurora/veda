#pragma once

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)
#define MAX_NUMA_NODES 2

#define CVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	return veda::VEOtoVEDA(err);	}
#define TVEO(...)	{ int err = __VA_ARGS__;	if(err != VEO_COMMAND_OK)	throw veda::VEOtoVEDA(err);	}
#define TVEDA(...)	{ VEDAresult err = __VA_ARGS__;	if(err != VEDA_SUCCESS)		throw err;			}
#define ASSERT(...)	{ if(!(__VA_ARGS__))						throw VEDA_ERROR_ASSERT;	}

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

#define CREQ(REQ)	({ uint64_t _r = REQ; if(_r == VEO_REQUEST_ID_INVALID) throw VEDA_ERROR_INVALID_REQID; _r; })
