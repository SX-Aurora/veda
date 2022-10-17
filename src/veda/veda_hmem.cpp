#include <veda/internal.h>

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
 * @brief Allocates HMEM device memory.
 * @param ptr Returned HMEM device pointer
 * @param size Requested allocation size in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY VEDA device memory exausted.\n 
 * 
 * Allocates size bytes of linear memory on the HMEM device and returns in *ptr a
 * pointer to the allocated memory.
 */
VEDAresult vedaHMemAlloc(VEDAhmemptr* ptr, size_t size) {
	GUARDED(
		auto ctx = veda::contexts::current();
		*ptr = ctx->hmemAlloc(size);
		L_TRACE("[ve:%i] vedaHMemAlloc(%p, %llu)", ctx->device().vedaId(), ptr, size);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the VE device address for the given HMEM ptr.
 * @param ptr Pointer to hold the VEDA device address.
 * @param hptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 */
VEDAresult vedaHMemPtr(void** ptr, VEDAhmemptr hptr) {
	*ptr = veo_get_hmem_addr(hptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Frees HMEM memory.
 * @param ptr Pointer to memory to free.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_UNKNOWN_HMEM Unknown HMEM ptr.\n 
 *
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaHMemAlloc().
 */
VEDAresult vedaHMemFree(VEDAhmemptr ptr) {
	return veo_free_hmem(ptr) == 0 ? VEDA_SUCCESS : VEDA_ERROR_UNKNOWN_HMEM;
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory.
 * @param dst Destination address pointer.
 * @param src Source address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_INVALID_VALUE Invalid input ptrs or ByteCount.\n 
 *
 * Copies data between two pointers. dst and src are base pointers of the
 * destination and source, respectively. ByteCount specifies the number of bytes
 * to copy.
 */
VEDAresult vedaHMemcpy(void* dst, void* src, size_t ByteCount) {
	return veo_hmemcpy(dst, src, ByteCount) == 0 ? VEDA_SUCCESS : VEDA_ERROR_INVALID_VALUE;
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from HMEM to VEDA Device.
 * @param dst Destination device pointer.
 * @param src Source HMEM pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from HMEM memory to device. dst and src are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaHMemcpyXtoD(VEDAdeviceptr dst, void* src, size_t ByteCount) {
	CVEDA(vedaHMemcpyXtoDAsync(dst, src, ByteCount, 0));
	return vedaStreamSynchronize(0);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from HMEM to VEDA Device.
 * @param dst Destination device pointer.
 * @param src Source HMEM pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from HMEM memory to device. dst and src are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaHMemcpyDtoX(void* dst, VEDAdeviceptr src, size_t ByteCount) {
	CVEDA(vedaHMemcpyDtoXAsync(dst, src, ByteCount, 0));
	return vedaStreamSynchronize(0);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from HMEM to VEDAdevice Asynchronously.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from HMEM to device memory. dst and src are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaHMemcpyXtoDAsync(VEDAdeviceptr dst, void* src, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::devices::get(dst).ctx();
		L_TRACE("[ve:%i] vedaHMemcpyXtoDAsync(%p, %p, %llu, %i)", ctx.device().vedaId(), dst, src, ByteCount, hStream);
		if(veo_is_ve_addr(src))
			return veo_hmemcpy(ctx.getPtr(dst).ptr, src, ByteCount) == 0 ? VEDA_SUCCESS : VEDA_ERROR_INVALID_VALUE;
		return vedaMemcpyHtoDAsync(dst, src, ByteCount, hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA device to HMEM Asynchronously.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from device memory to HMEM. dst and src are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaHMemcpyDtoXAsync(void* dst, VEDAdeviceptr src, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::devices::get(src).ctx();
		L_TRACE("[ve:%i] vedaHMemcpyDtoXAsync(%p, %p, %llu, %i)", ctx.device().vedaId(), dst, src, ByteCount, hStream);
		if(veo_is_ve_addr(dst))
			return veo_hmemcpy(dst, ctx.getPtr(src).ptr, ByteCount) == 0 ? VEDA_SUCCESS : VEDA_ERROR_INVALID_VALUE;
		return vedaMemcpyDtoHAsync(dst, src, ByteCount, hStream);
	)
}

//------------------------------------------------------------------------------
} // extern "C"