#include "veda/veda.hpp"

extern "C" {
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
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->query(hStream));
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Waits until the device has completed all operations in the stream specified by
 * hStream.
 */
VEDAresult vedaStreamSynchronize(VEDAstream hStream) {
	GUARDED(veda::Contexts::current()->sync(hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Add a callback to a compute stream.
 * @param hStream Determine status of a compute stream.
 * @param callback The function to call once preceding stream operations are 
 * complete.
 * @param userData User specified data to be passed to the callback function.
 * @param flags Reserved for future use, must be 0.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Adds a callback to be called on the host after all currently enqueued items
 * in the stream have completed.
 */
VEDAresult vedaStreamAddCallback(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags) {
	GUARDED(
		VEDAresult res = VEDA_SUCCESS;
		try {
			veda::Contexts::current()->sync(stream);
		} catch(VEDAresult r) {
			res = r;
		}
		callback(stream, res, userData);
	);
}

//------------------------------------------------------------------------------
} // extern "C"
