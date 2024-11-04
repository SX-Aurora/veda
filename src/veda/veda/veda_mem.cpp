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
 * @brief Swaps the underlying buffer of two device pointers.
 * @param A first pointer
 * @param B second pointer
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 */
VEDAresult vedaMemSwap(VEDAdeviceptr A, VEDAdeviceptr B) {
	return vedaMemSwapAsync(A, B, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Swaps the underlying buffer of two device pointers.
 * @param A first pointer
 * @param B second pointer
 * @param stream stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 */
VEDAresult vedaMemSwapAsync(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream stream) {
	GUARDED(
		auto ctxA = &veda::internal::devices::get(A).ctx();
		auto ctxB = &veda::internal::devices::get(B).ctx();
		if(ctxA != ctxB)
			return VEDA_ERROR_INVALID_CONTEXT;
		L_TRACE("[ve:%i] vedaMemSwapAsync(%p, %p, %i)", ctxA->device().vedaId(), A, B, stream);
		ctxA->memSwap(A, B, stream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Get the VEDA device information correspoding to the Device pointer.
 * @param dev Pointer to hold the VEDA device handle.
 * @param ptr VEDA device pointer
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 */
VEDAresult vedaMemGetDevice(VEDAdevice* dev, VEDAdeviceptr ptr) {
	*dev = VEDAptr<>(ptr).device();
	L_TRACE("[ve:%i] vedaMemGetDevice(%i, %p)", *dev, *dev, ptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates VEDA device memory.
 * @param ptr Returned VEDA device pointer
 * @param size Requested allocation size in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY VEDA device memory exausted.\n 
 * 
 * Allocates size bytes of linear memory on the VEDA device and returns in *ptr a
 * pointer to the allocated memory.
 */
VEDAresult vedaMemAlloc(VEDAdeviceptr* ptr, size_t size) {
	return vedaMemAllocAsync(ptr, size, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Overrides the pointer returned by the next call to vedaMemAllocAsync.
 * @param size Requested allocation size in bytes.
 * @param stream The stream establishing the stream ordering contract and the
 * memory pool to allocate from
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OFFSET_NOT_ALLOWED Only non-offsetted pointers are allowed.
 * 
 * You probably don't need to use this method ever. It's a hack to override 
 * the allocation size of the next call to vedaMemAllocAsync. I.e. if you have
 * allocated memory on VE, but you need to load that data into an opaque data
 * structure that is not under your control, but that calls vedaMemAllocAsync,
 * you can use vedaMemAllocOverrideOnce to return this pointer instead.
 * 
 */
VEDAresult vedaMemAllocOverrideOnce(VEDAdeviceptr ptr) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		ctx->setMemOverride(ptr);
		L_TRACE("[ve:%i] vedaMemAllocOverrideOnce(%p)", ctx->device().vedaId(), ptr);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates memory with stream ordered semantics.
 * @param ptr Returned VEDA device pointer
 * @param size Requested allocation size in bytes.
 * @param stream The stream establishing the stream ordering contract and the
 * memory pool to allocate from
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY VEDA device memory exausted.\n 
 * 
 * Inserts an allocation operation into stream. A pointer to the allocated memory
 * is returned immediately in *ptr. The allocation must not be accessed until 
 * the the allocation operation completes.
 */
VEDAresult vedaMemAllocAsync(VEDAdeviceptr* ptr, size_t size, VEDAstream stream) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		*ptr = ctx->memAlloc(size, stream);
		L_TRACE("[ve:%i] vedaMemAllocAsync(%p, %llu, %i)", ctx->device().vedaId(), *ptr, size, stream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates host memory.
 * @param pp Returned host pointer
 * @param bytesize Requested allocation size in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.
 */
VEDAresult vedaMemAllocHost(void** pp, size_t bytesize) {
	*pp = malloc(bytesize);
	L_TRACE("[Host] vedaMemAllocHost(%p, %llu)", *pp, bytesize);
	return *pp == 0 ? VEDA_ERROR_OUT_OF_MEMORY : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates pitched device memory.
 * @param dptr Returned device pointer
 * @param pPitch Returned pitch of allocation in bytes.
 * @param WidthInBytes Requested allocation width in bytes
 * @param Height Requested allocation height in rows
 * @param ElementSizeByte Size of largest reads/writes for range
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.\n 
 *
 * Allocates at least WidthInBytes * Height bytes of linear memory on the device and
 * returns in *dptr a pointer to the allocated memory. The pitch returned in *pPitch
 * by vedaMemAllocPitch() is the width in bytes of the allocation.
 */
VEDAresult vedaMemAllocPitch(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte) {
	return vedaMemAllocPitchAsync(dptr, pPitch, WidthInBytes, Height, ElementSizeByte, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates pitched device memory with stream order symantics.
 * @param dptr Returned device pointer
 * @param pPitch Returned pitch of allocation in bytes.
 * @param WidthInBytes Requested allocation width in bytes
 * @param Height Requested allocation height in rows
 * @param ElementSizeByte Size of largest reads/writes for range
 * @param stream The stream establishing the stream ordering contract and the
 * memory pool to allocate from
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.\n 
 *
 * Allocates at least WidthInBytes * Height bytes of linear memory on the device and
 * returns in *dptr a pointer to the allocated memory. The pitch returned in *pPitch
 * by vedaMemAllocPitch() is the width in bytes of the allocation.
 */
VEDAresult vedaMemAllocPitchAsync(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte, VEDAstream stream) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		auto&& [ptr, pitch] = ctx->memAllocPitch(WidthInBytes, Height, ElementSizeByte, stream);
		*dptr	= ptr;
		*pPitch	= pitch;
		L_TRACE("[ve:%i] vedaMemAllocPitchAsync(%p, %llu, %llu, %llu, %u, %i)", ctx->device().vedaId(), *dptr, *pPitch, WidthInBytes, Height, ElementSizeByte, stream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Frees device memory.
 * @param ptr Pointer to memory to free.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaMemAlloc() or vedaMemAllocPitch().
 */
VEDAresult vedaMemFree(VEDAdeviceptr ptr) {
	return vedaMemFreeAsync(ptr, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Releases a VEDAdeviceptr, so it can be freed on the device using
 * vedaMemFree(ptr).
 * @param ptr Pointer to memory to be released.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaMemAlloc() or vedaMemAllocPitch().
 */
VEDAresult vedaMemRelease(VEDAdeviceptr ptr) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(ptr).ctx();
		L_TRACE("[ve:%i] vedaMemRelease(%p)", ctx.device().vedaId(), ptr);
		ctx.memFree(ptr, 0, false);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Frees device memory with stream ordered semantics.
 * @param ptr Pointer to memory to free.
 * @param stream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Inserts a free operation into stream. The allocation must not be accessed after
 * stream execution reaches the free. After this API returns, accessing the memory
 * from any subsequent work launched on the VEDA device or querying its pointer
 * attributes results in undefined behavior.
 */
VEDAresult vedaMemFreeAsync(VEDAdeviceptr ptr, VEDAstream stream) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(ptr).ctx();
		L_TRACE("[ve:%i] vedaMemFreeAsync(%p, %i)", ctx.device().vedaId(), ptr, stream);
		ctx.memFree(ptr, stream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Frees host memory.
 * @param ptr Pointer to memory to free.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaMemAllocHost().
 */
VEDAresult vedaMemFreeHost(void* ptr) {
	L_TRACE("[Host] vedaMemFreeHost(%p)", ptr);
	if(ptr == 0)
		return VEDA_ERROR_INVALID_VALUE;
	free(ptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Get information on memory allocations.
 * @param base Returned base address.
 * @param size Returned size of device memory allocation.
 * @param vptr  Device pointer to query.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Returns the base address in *base and size in *size of the allocation by
 * vedaMemAlloc() or vedaMemAllocPitch() that contains the input pointer vptr.
 */
VEDAresult vedaMemGetAddressRange(VEDAdeviceptr* base, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(vptr).ctx();
		auto res = ctx.getPtr(vptr);
		*base = VEDAptr<>(vptr).base();
		*size = res.size;
		L_TRACE("[ve:%i] vedaMemGetAddressRange(%p, %llu, %p)", ctx.device().vedaId(), *base, *size, vptr);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Gets free and total memory.
 * @param free Returned free memory in bytes.
 * @param total Returned total memory in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Returns in *total the total amount of memory available to the the current context.
 * Returns in *free the amount of memory on the VEDA device that is free.
 */
VEDAresult vedaMemGetInfo(size_t* free, size_t* total) {
	GUARDED(
		auto ctx = veda::internal::contexts::current();
		*total	= ctx->device().memorySize();
		*free	= *total - ctx->memUsed();
		L_TRACE("[ve:%i] vedaMemGetInfo(%llu, %llu)", ctx->device().vedaId(), *free, *total);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies data between two pointers. dst and src are base pointers of the
 * destination and source, respectively. ByteCount specifies the number of bytes
 * to copy.
 */
VEDAresult vedaMemcpy(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount) {
	return vedaMemcpyDtoD(dst, src, ByteCount);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory Asynchronously.
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
 * Copies data between two pointers. dst and src are base pointers of the
 * destination and source, respectively. ByteCount specifies the number of bytes
 * to copy.
 */
VEDAresult vedaMemcpyAsync(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount, VEDAstream hStream) {
	return vedaMemcpyDtoDAsync(dst, src, ByteCount, hStream);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA Device to VEDA Device.
 * @param dstDevice Destination device pointer.
 * @param srcDevice Source device pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from device memory to device memory. dstDevice and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoD(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t ByteCount) {
	return vedaMemcpyDtoDAsync(dstDevice, srcDevice, ByteCount, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA device to VEDA device Asynchronously.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param size Size of memory copy in bytes.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from device memory to device memory. dstDevice and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoDAsync(VEDAdeviceptr dst, VEDAdeviceptr src, size_t size, VEDAstream hStream) {
	auto ddst = VEDA_GET_DEVICE(dst);
	auto dsrc = VEDA_GET_DEVICE(src);
	L_TRACE("[ve:%i>%i] vedaMemcpyDtoDAsync(%p, %p, %llu, %i)", dsrc, ddst, dst, src, size, hStream);

	GUARDED(
		veda::internal::memcpy(dst, src, size, hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA Device to Host.
 * @param dstHost Destination host pointer.
 * @param srcDevice Source device pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from device memory to host. dstHost and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoH(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount) {
	return vedaMemcpyDtoHAsync(dstHost, srcDevice, ByteCount, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA device to Host Asynchronously.
 * @param dstHost Destination virtual address pointer.
 * @param srcDevice Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from device memory to host. dstHost and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoHAsync(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(srcDevice).ctx();
		L_TRACE("[ve:%i] vedaMemcpyDtoHAsync(%p, %p, %llu, %i)", ctx.device().vedaId(), dstHost, srcDevice, ByteCount, hStream);
		ctx.memcpyD2H(dstHost, srcDevice, ByteCount, hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from Host to VEDA Device.
 * @param dstDevice Destination device pointer.
 * @param srcHost Source host pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from host memory to device memory. dstDevice and srcHost are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyHtoD(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount) {
	return vedaMemcpyHtoDAsync(dstDevice, srcHost, ByteCount, -1);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from Host to VEDA device Asynchronously.
 * @param dstDevice Destination virtual address pointer.
 * @param srcHost Source Host virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from host memory to device memory. dstDevice and srcHost are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyHtoDAsync(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(dstDevice).ctx();
		L_TRACE("[ve:%i] vedaMemcpyHtoDAsync(%p, %p, %llu, %i)", ctx.device().vedaId(), dstDevice, srcHost, ByteCount, hStream);
		ctx.memcpyH2D(dstDevice, srcHost, ByteCount, hStream);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Prints the memory information of each Intialized VEDA device.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Prints the each allocation VEDA memory address and size of the every VEDA
 * device.
 */
VEDAresult vedaMemReport(void) {
	GUARDED(veda::internal::devices::memReport();)
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the VEDA device address for the given VEDA device virtual address.
 * @param ptr Pointer to hold the VEDA device address.
 * @param vptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaMemPtr(void** ptr, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(vptr).ctx();
		auto res = ctx.getPtr(vptr);
		*ptr = (void*)res.ptr;
		L_TRACE("[ve:%i] vedaMemPtr(%p, %p)", ctx.device().vedaId(), *ptr, vptr);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the VEDA device address and allocation size against the given VEDA
 * device virtual address.
 * @param ptr Pointer to hold the VEDA device address.
 * @param size total size of the allocation in bytes.
 * @param vptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaMemPtrSize(void** ptr, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(vptr).ctx();
		auto res = ctx.getPtr(vptr);
		*ptr	= (void*)res.ptr;
		*size	= res.size;
		L_TRACE("[ve:%i] vedaMemPtrSize(%p, %llu, %p)", ctx.device().vedaId(), *ptr, *size, vptr);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the VEDA memory size in bytes  for the given VEDA device
 * virtual address.
 * @param size total size of the allocation in bytes.
 * @param vptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaMemSize(size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx = veda::internal::devices::get(vptr).ctx();
		auto res = ctx.getPtr(vptr);
		*size	= res.size;
		L_TRACE("[ve:%i] vedaMemSize(%llu, %p)", ctx.device().vedaId(), *size, vptr);
	)
}

//------------------------------------------------------------------------------
} // extern "C"