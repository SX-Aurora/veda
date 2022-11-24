#include <veda/internal/api.h>
#include <veda/cpp/api.h>

extern "C" {
//------------------------------------------------------------------------------
#include <veda/veda_error.inc.cpp>

//------------------------------------------------------------------------------
// implementation of VEDA API functions
/**
 * \defgroup vedaapi VEDA API
 *
 * To use VEDA API functions, include "veda.h" header.
 */

/** @{ */
/**
 * @brief Initialize the VEDA driver API library.
 * @param Flags Initialization flags for VEDA library.
 * @retval VEDA_SUCCESS on Success \n 
 *
 * Initializes the driver API and must be called before any other function of
 * the VEDA driver API. Currently, the Flags parameter must be 0. If vedaInit()
 * has not been called, any function from the driver API will return 
 * VEDA_ERROR_NOT_INITIALIZED.
 */
VEDAresult vedaInit(uint32_t Flags) {
	L_TRACE("vedaInit(%u)", Flags);
	TRY(veda::init(Flags);)
}

//------------------------------------------------------------------------------
VEDAresult vedaGetVersion(const char** str) {
	*str = veda::version().begin();
	L_TRACE("vedaGetVersion(%s)", *str);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Releases the VEDA driver API library gracefully.
 * @retval VEDA_SUCCESS on Success \n 
 *
 * This function should be called from the hybrid program in order to perform the
 * proper cleanup of the VEDA driver library.
 */
VEDAresult vedaExit(void) {
	L_TRACE("vedaExit()");
	TRY(veda::exit();)
}

//------------------------------------------------------------------------------
/**
 * @brief Return the latest AVEO version supported by VEDA.
 * @param str Pointer to hold the AVEO version.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 */
VEDAresult vedaDriverGetVersion(const char** str) {
	*str = veda::driverVersion().begin();
	L_TRACE("vedaDriverGetVersion(%s)", *str);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a VEDA device function f on the VEDA device.
 * @param f Handle to VEDA Device function to launch.
 * @param stream Stream Identifier.
 * @param args Handle to the VEDA device parameters
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed. \n 
 *
 * VEDA device function arguements can be allocated and set through the vedaArgsCreate
 * and vedaArgsSet* APIs.
 */ 
VEDAresult vedaLaunchKernel(VEDAfunction f, VEDAstream stream, VEDAargs args) {
	return vedaLaunchKernelEx(f, stream, args, 0, 0);
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a VEDA device function f on the VEDA device with flexibility.
 * @param f Handle to VEDA Device function to launch.
 * @param stream Stream Identifier.
 * @param args Handle to the VEDA device parameters.
 * @param destroyArgs Set 1 if the VEDA arguement needs to be destroyed after VEDA 
 * device function is called else 0.
 * @param result If set, the function return value will be copied to this pointer.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaLaunchKernelEx(VEDAfunction f, VEDAstream stream, VEDAargs args, const int destroyArgs, uint64_t* result) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		L_TRACE("[ve:%i] vedaLaunchKernelEx(%p, %i, ..., %i, %p)", ctx->device().vedaId(), f, stream, destroyArgs, result);
		ctx->call(f, stream, args, destroyArgs != 0, false, veda::internal::Result(result));
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a host function call in a stream.
 * @param stream Stream Identifier.
 * @param fn The function to call once preceding stream operations are complete.
 * @param userData User-specified data to be passed to the function.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed. \n 
 *
 * Enqueues a host function to run in a stream. The function will be called after
 * currently enqueued work and will block work added after it.
 */ 
VEDAresult vedaLaunchHostFunc(VEDAstream stream, VEDAhost_function fn, void* userData) {
	return vedaLaunchHostFuncEx(stream, fn, userData, 0);
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a host function call in a stream.
 * @param stream Stream Identifier.
 * @param fn The function to call once preceding stream operations are complete.
 * @param userData User-specified data to be passed to the function.
 * @param result If non-zero, the function return value gets written into the buffer.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Enqueues a host function to run in a stream. The function will be called after
 * currently enqueued work and will block work added after it.
 */ 
VEDAresult vedaLaunchHostFuncEx(VEDAstream stream, VEDAhost_function fn, void* userData, uint64_t* result) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		L_TRACE("[ve:%i] vedaLaunchHostFuncEx(%i, %p, %p, %p)", ctx->device().vedaId(), stream, fn, userData, result);
		ctx->call(fn, stream, userData, false, veda::internal::Result(result));
	)
}

/** @} */
//------------------------------------------------------------------------------
} // extern "C"
