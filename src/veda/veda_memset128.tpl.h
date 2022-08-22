#if !defined(VGEN_F) || !defined(VGEN_P)
#error "INVALID INCLUDE"
#endif

#define VGEN_1D		VEDA_JOIN(VGEN_F, D128)
#define VGEN_1D_ASYNC	VEDA_JOIN(VGEN_1D, Async)
#define VGEN_2D		VEDA_JOIN(VGEN_F, D2D128)
#define VGEN_2D_ASYNC	VEDA_JOIN(VGEN_2D, Async)

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult VGEN_1D(VGEN_P dstDevice, uint64_t x, uint64_t y, size_t N) {
	CVEDA(VGEN_1D_ASYNC(dstDevice, x, y, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @param stream Stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult VGEN_1D_ASYNC(VGEN_P dstDevice, uint64_t x, uint64_t y, size_t N, VEDAstream stream) {
	GUARDED(
		auto& ctx = veda::devices::get(dstDevice).ctx();
		L_TRACE("[ve:%i] " VEDA_STR(VGEN_1D_ASYNC) "(%p, 0x%016llX, 0x%016llX, %llu, %i)", ctx.device().vedaId(), dstDevice, x, y, N, stream);
		ctx.memset(dstDevice, x, y, N, stream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch memory pitch
 * @param x first 64bit value
 * @param y second 64bit value
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult VGEN_2D(VGEN_P dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height) {
	CVEDA(VGEN_2D_ASYNC(dstDevice, dstPitch, x, y, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch memory pitch
 * @param x first 64bit value
 * @param y second 64bit value
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @param stream stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult VGEN_2D_ASYNC(VGEN_P dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height, VEDAstream stream) {
	GUARDED(
		auto& ctx = veda::devices::get(dstDevice).ctx();
		L_TRACE("[ve:%i] " VEDA_STR(VGEN_2D_ASYNC) "(%p, %llu, 0x%016llX, 0x%016llX, %llu, %llu, %i)", ctx.device().vedaId(), dstDevice, dstPitch, x, y, Width, Height, stream);
		ctx.memset2D(dstDevice, dstPitch, x, y, Width, Height, stream);
	)
}

//------------------------------------------------------------------------------

#undef VGEN_1D
#undef VGEN_1D_ASYNC
#undef VGEN_2D
#undef VGEN_2D_ASYNC