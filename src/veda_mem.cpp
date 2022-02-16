#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaMemSwap(VEDAdeviceptr A, VEDAdeviceptr B) {
	CVEDA(vedaMemSwapAsync(A, B, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemSwapAsync(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream stream) {
	GUARDED(
		auto ctxA = &veda::Devices::get(A).ctx();
		auto ctxB = &veda::Devices::get(B).ctx();
		if(ctxA != ctxB)
			return VEDA_ERROR_INVALID_CONTEXT;
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
 * @retval VEDA_ERROR_OUT_OF_MEMORY VEDA device memory exausted.
 * Allocates size bytes of linear memory on the VEDA device and returns in *ptr a
 * pointer to the allocated memory.
 */
VEDAresult vedaMemAlloc(VEDAdeviceptr* ptr, size_t size) {
	CVEDA(vedaMemAllocAsync(ptr, size, 0));
	return vedaCtxSynchronize();
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
 * @retval VEDA_ERROR_OUT_OF_MEMORY VEDA device memory exausted.
 * Inserts an allocation operation into stream. A pointer to the allocated memory
 * is returned immediately in *ptr. The allocation must not be accessed until 
 * the the allocation operation completes.
 */
VEDAresult vedaMemAllocAsync(VEDAdeviceptr* ptr, size_t size, VEDAstream stream) {
	GUARDED(*ptr = veda::Contexts::current()->memAlloc(size, stream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Allocates host memory.
 * @param ptr Returned host pointer
 * @param size Requested allocation size in bytes.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.
 */
VEDAresult vedaMemAllocHost(void** pp, size_t bytesize) {
	return (*pp = malloc(bytesize)) == 0 ? VEDA_ERROR_OUT_OF_MEMORY : VEDA_SUCCESS;
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
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.
 * Allocates at least WidthInBytes * Height bytes of linear memory on the device and
 * returns in *dptr a pointer to the allocated memory. The pitch returned in *pPitch
 * by vedaMemAllocPitch() is the width in bytes of the allocation.
 */
VEDAresult vedaMemAllocPitch(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte) {
	CVEDA(vedaMemAllocPitchAsync(dptr, pPitch, WidthInBytes, Height, ElementSizeByte, 0));
	return vedaCtxSynchronize();
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
 * @retval VEDA_ERROR_OUT_OF_MEMORY Host memory exausted.
 * Allocates at least WidthInBytes * Height bytes of linear memory on the device and
 * returns in *dptr a pointer to the allocated memory. The pitch returned in *pPitch
 * by vedaMemAllocPitch() is the width in bytes of the allocation.
 */
VEDAresult vedaMemAllocPitchAsync(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte, VEDAstream stream) {
	GUARDED(
		auto&& [ptr, pitch]  = veda::Contexts::current()->memAllocPitch(WidthInBytes, Height, ElementSizeByte, stream);
		*dptr	= ptr;
		*pPitch	= pitch;
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaMemAlloc() or vedaMemAllocPitch().
 */
VEDAresult vedaMemFree(VEDAdeviceptr ptr) {
	CVEDA(vedaMemFreeAsync(ptr, 0));
	return vedaCtxSynchronize();
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Inserts a free operation into stream. The allocation must not be accessed after
 * stream execution reaches the free. After this API returns, accessing the memory
 * from any subsequent work launched on the VEDA device or querying its pointer
 * attributes results in undefined behavior.
 */
VEDAresult vedaMemFreeAsync(VEDAdeviceptr ptr, VEDAstream stream) {
	GUARDED(veda::Devices::get(ptr).ctx().memFree(ptr, stream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Frees host memory.
 * @param ptr Pointer to memory to free.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Frees the memory space pointed to by dptr, which must have been returned by a
 * previous call to vedaMemAllocHost().
 */
VEDAresult vedaMemFreeHost(void* ptr) {
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Returns the base address in *base and size in *size of the allocation by
 * vedaMemAlloc() or vedaMemAllocPitch() that contains the input pointer vptr.
 */
VEDAresult vedaMemGetAddressRange(VEDAdeviceptr* base, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto res = veda::Devices::get(vptr).ctx().getPtr(vptr);
		*base = VEDAptr<>(vptr).base();
		*size = res.size;
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Returns in *total the total amount of memory available to the the current context.
 * Returns in *free the amount of memory on the VEDA device that is free.
 */
VEDAresult vedaMemGetInfo(size_t* free, size_t* total) {
	GUARDED(
		auto ctx = veda::Contexts::current();
		*total	= ctx->device().memorySize();
		*free	= *total - ctx->memUsed();
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies data between two pointers. dst and src are base pointers of the
 * destination and source, respectively. ByteCount specifies the number of bytes
 * to copy.
 */
VEDAresult vedaMemcpy(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoD(dst, src, ByteCount));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory Asynchronously.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from device memory to device memory. dstDevice and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoD(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoDAsync(dstDevice, srcDevice, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA device to VEDA device Asynchronously.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from device memory to device memory. dstDevice and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoDAsync(VEDAdeviceptr dst, VEDAdeviceptr src, size_t size, VEDAstream hStream) {
	auto ddst = VEDA_GET_DEVICE(dst);
	auto dsrc = VEDA_GET_DEVICE(src);

	if(ddst == dsrc) {
		GUARDED(
			auto& ctx = veda::Devices::get(dst).ctx();
			ctx.memcpyD2D(dst, src, size, hStream);
		)
	} else {
		GUARDED(
			auto& sctx = veda::Devices::get(src).ctx();
			auto& dctx = veda::Devices::get(dst).ctx();

			void* host = malloc(size);
			if(!host)
				VEDA_THROW(VEDA_ERROR_OUT_OF_MEMORY);
			
			sctx.memcpyD2H(host, src, size, 0);
			sctx.sync(0);

			dctx.memcpyH2D(dst, host, size, 0);
			dctx.sync(0);
			
			free(host);
		)
	}
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from device memory to host. dstHost and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoH(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoHAsync(dstHost, srcDevice, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from VEDA device to Host Asynchronously.
 * @param dstHost Destination virtual address pointer.
 * @param srcDevice Source virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from device memory to host. dstHost and srcDevice are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyDtoHAsync(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream) {
	GUARDED(veda::Devices::get(srcDevice).ctx().memcpyD2H(dstHost, srcDevice, ByteCount, hStream);)
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
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from host memory to device memory. dstDevice and srcHost are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyHtoD(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount) {
	CVEDA(vedaMemcpyHtoDAsync(dstDevice, srcHost, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from Host to VEDA device Asynchronously.
 * @param dstDevice Destination virtual address pointer.
 * @param srcHost Source Host virtual address pointer.
 * @param ByteCount Size of memory copy in bytes.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Copies from host memory to device memory. dstDevice and srcHost are the base
 * pointers of the destination and source, respectively. ByteCount specifies the
 * number of bytes to copy.
 */
VEDAresult vedaMemcpyHtoDAsync(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memcpyH2D(dstDevice, srcHost, ByteCount, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 16-bit values to the specified value us.
 */
VEDAresult vedaMemsetD16(VEDAdeviceptr dstDevice, uint16_t us, size_t N) {
	CVEDA(vedaMemsetD16Async(dstDevice, us, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 16-bit values to the specified value us.
 */
VEDAresult vedaMemsetD16Async(VEDAdeviceptr dstDevice, uint16_t us, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset(dstDevice, us, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD128(VEDAdeviceptr dstDevice, uint64_t x, uint64_t y, size_t N) {
	CVEDA(vedaMemsetD128Async(dstDevice, x, y, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD128Async(VEDAdeviceptr dstDevice, uint64_t x, uint64_t y, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset(dstDevice, x, y, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D128(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D128Async(dstDevice, dstPitch, x, y, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D128Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset2D(dstDevice, dstPitch, x, y, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D64(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t il, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D64Async(dstDevice, dstPitch, il, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D64Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t ul, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset2D(dstDevice, dstPitch, ul, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 16-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D16(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D16Async(dstDevice, dstPitch, us, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 16-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D16Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset2D(dstDevice, dstPitch, us, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 32-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D32(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D32Async(dstDevice, dstPitch, ui, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 32-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D32Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset2D(dstDevice, dstPitch, ui, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 8-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D8(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D8Async(dstDevice, dstPitch, uc, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param us Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 8-bit values to the specified value uc.
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult vedaMemsetD2D8Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset2D(dstDevice, dstPitch, uc, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 32-bit values to the specified value us.
 */
VEDAresult vedaMemsetD32(VEDAdeviceptr dstDevice, uint32_t ui, size_t N) {
	CVEDA(vedaMemsetD32Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 32-bit values to the specified value us.
 */
VEDAresult vedaMemsetD32Async(VEDAdeviceptr dstDevice, uint32_t ui, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset(dstDevice, ui, N, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 64-bit values to the specified value us.
 */
VEDAresult vedaMemsetD64(VEDAdeviceptr dstDevice, uint64_t ui, size_t N) {
	CVEDA(vedaMemsetD64Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 64-bit values to the specified value us.
 */
VEDAresult vedaMemsetD64Async(VEDAdeviceptr dstDevice, uint64_t ui, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset(dstDevice, ui, N, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 8-bit values to the specified value us.
 */
VEDAresult vedaMemsetD8(VEDAdeviceptr dstDevice, uint8_t uc, size_t N) {
	CVEDA(vedaMemsetD8Async(dstDevice, uc, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param us Value to set.
 * @param N Number of Elements.
 * @param hstream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the memory range of N 8-bit values to the specified value us.
 */
VEDAresult vedaMemsetD8Async(VEDAdeviceptr dstDevice, uint8_t uc, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx().memset(dstDevice, uc, N, hStream);)
}

//------------------------------------------------------------------------------
/**
 * @brief Prints the memory information of each Intialized VEDA device.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Prints the each allocation VEDA memory address and size of the every VEDA
 * device.
 */
VEDAresult vedaMemReport(void) {
	GUARDED(veda::Devices::memReport();)
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
		auto res = veda::Devices::get(vptr).ctx().getPtr(vptr);
		*ptr = (void*)res.ptr;
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
		auto res = veda::Devices::get(vptr).ctx().getPtr(vptr);
		*ptr	= (void*)res.ptr;
		*size	= res.size;
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
		auto res = veda::Devices::get(vptr).ctx().getPtr(vptr);
		*size	= res.size;
	)
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the Hetrogenous Memory address for the given VEDA device virtual
 * address.
 * @param ptr Pointer to hold the HMEM address.
 * @param vptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaMemHMEM(void** ptr, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx	= veda::Devices::get(vptr).ctx();
		auto res	= ctx.getPtr(vptr);
		*ptr = (void*)((veo_ptr)res.ptr | ctx.hmemId());
	);
}

//------------------------------------------------------------------------------
/**
 * @brief  Gets the VEDA HMEM address and allocation size against the given VEDA
 * device virtual address.
 * @param ptr Pointer to hold the VEDA HMEM address.
 * @param size total size of the allocation in bytes.
 * @param vptr Pointer containing the VEDA device virtual address.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaMemHMEMSize(void** ptr, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto& ctx	= veda::Devices::get(vptr).ctx();
		auto res	= ctx.getPtr(vptr);
		*ptr		= (void*)((veo_ptr)res.ptr | ctx.hmemId());
		*size		= res.size;
	);
}

//------------------------------------------------------------------------------
} // extern "C"