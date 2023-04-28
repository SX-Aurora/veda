#include <veda/internal/api.h>

extern "C" {
// implementation of VEDA API functions
/**
 * \defgroup vedaapi VEDA API
 *
 * To use VEDA API functions, include "veda.h" header.
 */
/** @{ */
//------------------------------------------------------------------------------
/**
 * @brief Sets profiler callback
 * @param callback profiler callback
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_CALLBACK_ALREADY_REGISTERED if callback is already registered
 */
VEDAresult vedaProfilerSetCallback(VEDAprofiler_callback callback) {
	TRY(
		L_TRACE("vedaProfilerSetCallback(%p)", callback);
		veda::internal::profiler::setCallback(callback);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Gets current profiler callback
 * @param callback profiler callback
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_INVALID_ARGS if callback is nullptr
 */
VEDAresult vedaProfilerGetCallback(VEDAprofiler_callback* callback) {
	if(!callback)
		return VEDA_ERROR_INVALID_ARGS;
		
	*callback = veda::internal::profiler::getCallback();
	L_TRACE("vedaProfilerGetCallback(%p)", *callback);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Returns name of a VEDAprofiler_type
 * @param type VEDA profiler type
 * @param VEDAstream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_INVALID_ARGS if name == 0 or unknown type
 */
VEDAresult vedaProfilerTypeName(VEDAprofiler_type type, const char** name, const VEDAstream stream) {
	if(!name)
		return VEDA_ERROR_INVALID_ARGS;

	switch(type) {
		case VEDA_PROFILER_LAUNCH_HOST:		*name = "vedaLaunchHostFunc";		return VEDA_SUCCESS;
		case VEDA_PROFILER_LAUNCH_KERNEL:	*name = "vedaLaunchKernel";		return VEDA_SUCCESS;
		case VEDA_PROFILER_MEM_CPY_HTOD:	*name = "vedaMemcpyHtoD";		return VEDA_SUCCESS;
		case VEDA_PROFILER_MEM_CPY_DTOH:	*name = "vedaMemcpyDtoH";		return VEDA_SUCCESS;
		case VEDA_PROFILER_MEM_FREE:		*name = "vedaMemFree";			return VEDA_SUCCESS;
		case VEDA_PROFILER_MEM_ALLOC:		*name = "vedaMemAlloc";			return VEDA_SUCCESS;
		case VEDA_PROFILER_HMEM_CPY:		*name = "vedaHMemcpy";			return VEDA_SUCCESS;
		case VEDA_PROFILER_HMEM_FREE:		*name = "vedaHMemFree";			return VEDA_SUCCESS;
		case VEDA_PROFILER_HMEM_ALLOC:		*name = "vedaHMemAlloc";		return VEDA_SUCCESS;
		case VEDA_PROFILER_SYNC:		*name = stream >= 0 ? "vedaStreamSynchronize" : "vedaCtxSynchronize";	return VEDA_SUCCESS;
	}

	*name = 0;
	return VEDA_ERROR_INVALID_ARGS;
}

/** @} */
//------------------------------------------------------------------------------
} // extern "C"
