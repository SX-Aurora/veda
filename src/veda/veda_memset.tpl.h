#if	!defined(VGEN_B)	||\
	!defined(VGEN_F)	||\
	!defined(VGEN_P)	||\
	!defined(VGEN_T)	||\
	!defined(VGEN_L)	||\
	!defined(VGEN_C)
#error "INVALID INCLUDE"
#endif

#define VGEN_1D		VEDA_JOIN(VGEN_F, VGEN_B)
#define VGEN_1D_ASYNC	VEDA_JOIN(VGEN_1D, Async)
#define VGEN_2D		VEDA_JOIN(VEDA_JOIN(VGEN_F, D2), VGEN_B)
#define VGEN_2D_ASYNC	VEDA_JOIN(VGEN_2D, Async)

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N values to the specified value.
 */
VEDAresult VGEN_1D(VGEN_P dstDevice, VGEN_T value, size_t N) {
	CVEDA(VGEN_1D_ASYNC(dstDevice, value, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N values to the specified value.
 */
VEDAresult VGEN_1D_ASYNC(VGEN_P dstDevice, VGEN_T value, size_t N, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::devices::get(dstDevice).ctx();
		L_TRACE("[ve:%i] " VEDA_STR(VGEN_1D_ASYNC) "(%p, 0x%04X, %llu, " VGEN_L ")", ctx.device().vedaId(), dstDevice, VGEN_C value, N, hStream);
		ctx.memset(dstDevice, value, N, hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch memory pitch
 * @param value Value to set.
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N values to the specified value.
 */
VEDAresult VGEN_2D(VGEN_P dstDevice, size_t dstPitch, VGEN_T value, size_t Width, size_t Height) {
	CVEDA(VGEN_2D_ASYNC(dstDevice, dstPitch, value, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch memory pitch
 * @param value Value to set.
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @param hStream stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Sets the memory range of N 64-bit values to the specified value.
 */
VEDAresult VGEN_2D_ASYNC(VGEN_P dstDevice, size_t dstPitch, VGEN_T value, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::devices::get(dstDevice).ctx();
		L_TRACE("[ve:%i] " VEDA_STR(VGEN_2D_ASYNC) "(%p, %llu, " VGEN_L ", %llu, %llu, %i)", ctx.device().vedaId(), dstDevice, dstPitch, VGEN_C value, Width, Height, hStream);
		ctx.memset2D(dstDevice, dstPitch, value, Width, Height, hStream);
	)
}

//------------------------------------------------------------------------------

#undef VGEN_1D
#undef VGEN_1D_ASYNC
#undef VGEN_2D
#undef VGEN_2D_ASYNC

#undef VGEN_B
#undef VGEN_T
#undef VGEN_L
#undef VGEN_C

