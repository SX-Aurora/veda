#include <veda/internal/api.h>

namespace veda {
	namespace internal {
		namespace profiler {
//------------------------------------------------------------------------------
static VEDAprofiler_callback	s_callback = 0;

//------------------------------------------------------------------------------
void			shutdown	(void)	{ s_callback = 0;	}
VEDAprofiler_callback	getCallback	(void)	{ return s_callback;	}

//------------------------------------------------------------------------------
VEDAprofiler_type funcType(void* func) {
	if(func == veo_async_read_mem)	return VEDA_PROFILER_MEM_CPY_DTOH;
	if(func == veo_async_write_mem)	return VEDA_PROFILER_MEM_CPY_HTOD;
	if(func == veo_free_mem_async)	return VEDA_PROFILER_MEM_FREE;
	if(func == veo_alloc_mem_async)	return VEDA_PROFILER_MEM_ALLOC;
	if(func == veo_call_async_vh)	return VEDA_PROFILER_LAUNCH_HOST;
	if(func == veo_call_async)	return VEDA_PROFILER_LAUNCH_KERNEL;
	if(func == veo_hmemcpy)		return VEDA_PROFILER_HMEM_CPY;
	if(func == veo_alloc_hmem)	return VEDA_PROFILER_HMEM_ALLOC;
	if(func == veo_free_hmem)	return VEDA_PROFILER_HMEM_FREE;
	VEDA_THROW(VEDA_ERROR_UNKNOWN);
}

//------------------------------------------------------------------------------
uint64_t callbackEnter(void* data) {
	if(s_callback)
		s_callback((VEDAprofiler_data*)data, 1);
	return 0;
}

//------------------------------------------------------------------------------
uint64_t callbackExit(void* data) {
	if(s_callback)
		s_callback((VEDAprofiler_data*)data, 0);
	free(data);
	return 0;
}

//------------------------------------------------------------------------------
void setCallback(VEDAprofiler_callback callback) {
	if( callback &&  s_callback)	VEDA_THROW(VEDA_ERROR_CALLBACK_ALREADY_REGISTERED);
	if(!callback && !s_callback)	VEDA_THROW(VEDA_ERROR_INVALID_ARGS);
	s_callback = callback;
}

//------------------------------------------------------------------------------
		}
	}
}