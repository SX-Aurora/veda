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
 * @brief Query the flags of a given stream.
 * @param hStream Handle to the stream to be queried
 * @param flags Pointer to an unsigned integer in which the stream's flags are returned.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 */
VEDAresult vedaStreamGetFlags(VEDAstream hStream, uint32_t* flags) {
	GUARDED(
		*flags = 0;
		L_TRACE("[ve:%i] vedaStreamGetFlags(%i, %u)", hStream, *flags);
	);
}

//------------------------------------------------------------------------------
/**
 * @brief Determine status of a compute stream.
 * @param hStream Determine status of a compute stream.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Returns CUDA_SUCCESS if all operations in the stream specified by hStream 
 * have completed
 */
VEDAresult vedaStreamQuery(VEDAstream hStream) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		CVEDA(ctx->query(hStream));
		L_TRACE("[ve:%i] vedaStreamQuery(%i)", ctx->device().vedaId(), hStream);
	);
}

//------------------------------------------------------------------------------
/**
 * @brief Wait until a stream's tasks are completed.
 * @param hStream Determine status of a compute stream.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed. \n 
 *
 * Waits until the device has completed all operations in the stream specified by
 * hStream.
 */
VEDAresult vedaStreamSynchronize(VEDAstream hStream) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		L_TRACE("[ve:%i] vedaStreamSynchronize(%i)", ctx->device().vedaId(), hStream);
		ctx->sync(hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Add a callback to a compute stream.
 * @param stream Determine status of a compute stream.
 * @param callback The function to call once preceding stream operations are 
 * complete.
 * @param userData User specified data to be passed to the callback function.
 * @param flags Reserved for future use, must be 0.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 * 
 * Adds a callback to be called on the host after all currently enqueued items
 * in the stream have completed.
 */
VEDAresult vedaStreamAddCallback(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		L_TRACE("[ve:%i] vedaStreamAddCallback(%i, %p, %p, %i)", ctx->device().vedaId(), stream, callback, userData, flags);
		VEDAresult res = VEDA_SUCCESS;
		try {
			ctx->sync(stream);
		} catch(const veda::cpp::Exception& e) {
			res = e.error();
		}
		callback(stream, res, userData);
	);
}
/** @} */
//------------------------------------------------------------------------------
} // extern "C"
