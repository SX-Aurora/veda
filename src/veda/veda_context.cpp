#include "veda/internal.h"

//------------------------------------------------------------------------------
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
 * @brief Destroy a VEDA context.
 * @param ctx Context to destroy.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.\n 
 *
 * Destroys the VEDA context specified by ctx. The context ctx will be destroyed
 * regardless of how many threads it is current to. It is the responsibility of
 * the calling function to ensure that no API call issues using ctx while 
 * vedaCtxDestroy() is executing. Destroys and cleans up all resources associated
 * with the context. It is the caller's responsibility to ensure that the context
 * or its resources are not accessed or passed in subsequent API calls and doing
 * so will result in undefined behavior. These resources include VEDA types such
 * as VEDAmodule, VEDAfunction, VEDAstream. If ctx is current to the calling 
 * thread then ctx will also be popped from the current thread's context stack
 * (as though vedaCtxPopCurrent() were called). If ctx is current to other threads,
 * then ctx will remain current to those threads, and attempting to access ctx 
 * from those threads will result in the error VEDA_ERROR_CONTEXT_IS_DESTROYED.
 */
VEDAresult vedaCtxDestroy(VEDAcontext ctx) {
	if(!ctx)
		return VEDA_ERROR_UNKNOWN_CONTEXT;

	L_TRACE("[ve:%i] vedaCtxDestroy(%p)", ctx->device().vedaId(), ctx);
	GUARDED(
		veda::Contexts::remove(ctx);
		ctx->destroy();
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Gets the context's API version.
 * @param ctx Context to check.
 * @param version Pointer to hold the version.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 */
VEDAresult vedaCtxGetApiVersion(VEDAcontext ctx, uint32_t* version) {
	GUARDED(
		*version = (uint32_t)ctx->device().versionAbi();
		L_TRACE("[ve:%i] vedaCtxGetApiVersion(%p, %u)", ctx->device().vedaId(), ctx, *version);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Returns the VEDA context bound to the calling CPU thread.
 * @param pctx Returned context handle.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Returns in *pctx the VEDA context bound to the calling CPU thread. 
 * If no context is bound to the calling CPU thread then *pctx is set to NULL
 * and VEDA_SUCCESS is returned.
 */
VEDAresult vedaCtxGetCurrent(VEDAcontext* pctx) {
	GUARDED(
		*pctx = veda::Contexts::current();
		L_TRACE("[ve:%i] vedaCtxGetCurrent(%p, %u)", (*pctx)->device().vedaId(), *pctx);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Returns the device ID for the current context.
 * @param device Returned device ID for the current context.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Returns in *device the ordinal of the current context's device.
 */
VEDAresult vedaCtxGetDevice(VEDAdevice* device) {
	GUARDED(
		*device = -1; // initialize with some value in case we throw VEDA_ERROR_UNKNOWN_CONTEXT
		*device = veda::Contexts::current()->device().vedaId();
		L_TRACE("[ve:%i] vedaCtxGetDevice(%i)", *device, *device);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Pops the current VEDA context from the current CPU thread.
 * @param pctx Returned new context handle.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Pops the current VEDA context from the CPU thread and passes back the old
 * context handle in *pctx. That context may then be made current to a different
 * CPU thread by calling vedaCtxPushCurrent().
 *
 * If a context was current to the CPU thread before vedaCtxCreate() or 
 * vedaCtxPushCurrent() was called, this function makes that context current to
 * the CPU thread again.
 */
VEDAresult vedaCtxPopCurrent(VEDAcontext* pctx) {
	GUARDED(
		*pctx = veda::Contexts::pop();
		L_TRACE("[ve:%i] vedaCtxPopCurrent(%p)", (*pctx)->device().vedaId(), *pctx);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Pushes a context on the current CPU thread.
 * @param ctx Context to push.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.\n 
 *
 * Pushes the given context ctx onto the CPU thread's stack of current contexts.
 * The specified context becomes the CPU thread's current context, so all VEDA
 * functions that operate on the current context are affected. The previous
 * current context may be made current again by calling vedaCtxDestroy() or
 * vedaCtxPopCurrent().
 */
VEDAresult vedaCtxPushCurrent(VEDAcontext ctx) {
	if(!ctx)
		return VEDA_ERROR_INVALID_CONTEXT;
	GUARDED(
		L_TRACE("[ve:%i] vedaCtxPushCurrent(%p)", ctx->device().vedaId(), ctx);
		veda::Contexts::push(ctx);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Binds the specified VEDA context to the calling CPU thread.
 * @param ctx Context to bind to the calling CPU thread.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.\n 
 *
 * Binds the specified VEDA context to the calling CPU thread. If ctx is NULL 
 * then the VEDA context previously bound to the calling CPU thread is unbound
 * and VEDA_SUCCESS is returned.
 *
 * If there exists a VEDA context stack on the calling CPU thread, this will
 * replace the top of that stack with ctx. If ctx is NULL then this will be
 * equivalent to popping the top of the calling CPU thread's VEDA context stack
 * (or a no-op if the calling CPU thread's VEDA context stackis empty).
 */
VEDAresult vedaCtxSetCurrent(VEDAcontext ctx) {
	if(!ctx)
		return VEDA_ERROR_INVALID_CONTEXT;
	GUARDED(
		L_TRACE("[ve:%i] vedaCtxSetCurrent(%p)", ctx->device().vedaId(), ctx);
		veda::Contexts::set(ctx);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Gets the VEDA SM count.
 * @param cnt pointer to hold the VEDA SM count.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Returns the VEDA SM count of the VEDA context bound to the calling CPU thread.
 */
VEDAresult vedaCtxStreamCnt(int* cnt) {
	GUARDED(
		auto ctx = veda::Contexts::current();
		*cnt = ctx->streamCount();
		L_TRACE("[ve:%i] vedaCtxStreamCnt(%i)", ctx->device().vedaId(), *cnt);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Block for a context's tasks to complete.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Blocks until the device has completed all preceding requested tasks.
 * vedaCtxSynchronize() returns an error if one of the preceding tasks failed.
 */
VEDAresult vedaCtxSynchronize(void) {
	GUARDED(
		auto ctx = veda::Contexts::current();
		L_TRACE("[ve:%i] vedaCtxSynchronize()", ctx->device().vedaId());
		ctx->sync();
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Create a VEDA context.
 * @param pctx Returned context handle of the new context.
 * @param mode Context mode may contain below two values –\n 
 * VEDA_CONTEXT_MODE_OMP\n 
 * VEDA_CONTEXT_MODE_SCALAR
 * @param dev Device to create context on
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_CANNOT_CREATE_CONTEXT Error while creating VEDA context.
 * @retval VEDA_ERROR_CANNOT_CREATE_STREAM error while creating VEDA SM.\n 
 *
 * Creates a new VEDA context and associates it with the calling thread. The 
 * flags parameter is described below. The caller of vedaCtxCreate() must call
 * vedaCtxDestroy() or when done using the context. If any context is already
 * current to the thread, it is changed by the newly created context and it may
 * be restored by a subsequent call to vedaCtxPopCurrent().\n 
 * VEDA Context may be created with below two flags-\n 
 * VEDA_CONTEXT_MODE_OMP: In this mode, number of streaming multiprocessor(SM)
 * is always 1. However, in this mode the number of OPENMP threads in each 
 * VEDA SM is configured as the minimum value between the number of cores of 
 * the VEDA device and the user configured value for the number of OPENMP 
 * threads on VE device (set through environment variable).\n 
 * VEDA_CONTEXT_MODE_SCALAR: : In this mode number of streaming multiprocessor(SM)
 * are the minimum value between the number of cores of the VEDA device and the
 * user configured value for the number of OPENMP threads on VE device
 * (set through environment variable). However, in this mode number of 
 * OPENMP threads in each VEDA SM is configured as 1.
 */
VEDAresult vedaCtxCreate(VEDAcontext* pctx, int mode, VEDAdevice dev) {
	GUARDED(
		*pctx = &veda::Devices::get(dev).ctx();
		L_TRACE("[ve:%i] vedaCtxCreate(%p, %i, %i)", dev, *pctx, mode, dev);
		(*pctx)->init((VEDAcontext_mode)mode);
		veda::Contexts::push(*pctx);
	)
}
/** @} */
//------------------------------------------------------------------------------
} // extern "C"