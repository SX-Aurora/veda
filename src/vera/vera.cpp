#include "vera.h"
#include <veda/internal/api.h>
#include <cstring> // memcpy

//------------------------------------------------------------------------------
// implementation of VERA API functions
/**
 * \defgroup veraapi VERA API
 *
 * To use VERA API functions, include "vera.h" header.
 */
/** @{ */
/**
 * @brief Initialize the VEDA driver API library.
 * @retval VEDA_SUCCESS on Success\n 
 *
 * Initializes the driver API and it is optional to call when you are 
 * using other VERA APIs because it is being called at the begining 
 * of other vera APIs, calling multiple times don't make any effect.
 */
veraError_t veraInit(void) {
	static bool vera_initialized = false;
	if(!vera_initialized) {
		vera_initialized = true;
		CVEDA(vedaInit(0));
		VEDAcontext ctx = 0;
		CVEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
		CVEDA(vedaCtxSetCurrent(ctx));
	}
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Returns information about the VEDA device.
 * @param prop Returned VEDA device attribute value.
 * @param device VEDA device handle.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.\n 
 *
 * Returns in *pi the integer value of the attribute attrib on device dev. The 
 * supported attributes are:
 * VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO: VEDA precision ratio.\n 
 * VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE: Clock chip clock frequency of the VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE: Clock Base clock frequency of the VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT: Number of VE cores corresponding to 
 * VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE: Clock memory clock frequency of the
 * VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE: Size of the L1 cache of the device of VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE: Size of L1 instruction cache of the VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE: Size of L2 data cache of the VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_LLC_CACHE_SIZE: Size of last level cache of the VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_MODEL: VE device model corresponding to VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_ABI_VERSION: VE device ABI version corresponding to VEDA device.\n 
 * VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION: VE device firmware version corresponding to 
 * VEDA device.\n 
 */
veraError_t veraGetDeviceProperties(veraDeviceProp* prop, int device) {
	CVEDA(veraInit());

	CVEDA(vedaDeviceGetName(prop->name, sizeof(veraDeviceProp::name), device));
	CVEDA(vedaDeviceGetAttribute(&prop->clockRate,				VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->clockBase,				VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->multiProcessorCount,		VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, device));
	CVEDA(vedaDeviceGetAttribute(&prop->memoryClockRate,			VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->l1DCacheSize,			VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->l1ICacheSize,			VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->l2CacheSize,			VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, device));
	CVEDA(vedaDeviceGetAttribute(&prop->abiVersion,				VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, device));
	CVEDA(vedaDeviceGetAttribute(&prop->singleToDoublePrecisionPerfRatio,	VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, device));
	CVEDA(vedaDeviceGetAttribute(&prop->firmwareVersion,			VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, device));
	size_t free = 0;
	CVEDA(vedaMemGetInfo(&free, &prop->totalGlobalMem));
	return VEDA_SUCCESS;
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
veraError_t veraStreamAddCallback(veraStream_t stream, veraStreamCallback_t callback, void* userData, unsigned int flags) {
	CVEDA(veraInit());
	return vedaStreamAddCallback(stream, callback, userData, flags);
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory from source to destination as per kind value.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param count Size of memory copy in bytes.
 * @param kind value can be veraMemcpyHostToHost, veraMemcpyHostToDevice, veraMemcpyDeviceToHost, veraMemcpyDeviceToDevice.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from source memory to destination memory. dst and src are the base
 * pointers of the destination and source, respectively. count specifies the
 * number of bytes to copy.
 */
veraError_t veraMemcpy(void* dst, const void* src, size_t count, veraMemcpyKind kind) {
	CVEDA(veraInit());
	
	switch(kind) {
		case veraMemcpyHostToHost:	memcpy(dst, src, count); return VEDA_SUCCESS;
		case veraMemcpyHostToDevice:	return vedaMemcpyHtoD(VERA2VEDA(dst), src, count);
		case veraMemcpyDeviceToHost:	return vedaMemcpyDtoH(dst, VERA2VEDA(src), count);
		case veraMemcpyDeviceToDevice:	return vedaMemcpyDtoD(VERA2VEDA(dst), VERA2VEDA(src), count);
		case veraMemcpyDefault:
			;
	}

	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief Copies Memory asynchronously from source to destination as per kind value.
 * @param dst Destination virtual address pointer.
 * @param src Source virtual address pointer.
 * @param count Size of memory copy in bytes.
 * @param kind value can be veraMemcpyHostToHost, veraMemcpyHostToDevice, veraMemcpyDeviceToHost, veraMemcpyDeviceToDevice.
 * @param stream Stream identifier
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n 
 *
 * Copies from source memory to destination memory. dst and src are the base
 * pointers of the destination and source, respectively. count specifies the
 * number of bytes to copy.
 */
veraError_t veraMemcpyAsync(void* dst, const void* src, size_t count, veraMemcpyKind kind, veraStream_t stream) {
	CVEDA(veraInit());

	switch(kind) {
		case veraMemcpyHostToHost:		memcpy(dst, src, count); return VEDA_SUCCESS;
		case veraMemcpyHostToDevice:		return vedaMemcpyHtoDAsync(VERA2VEDA(dst), src, count, stream);
		case veraMemcpyDeviceToHost:		return vedaMemcpyDtoHAsync(dst, VERA2VEDA(src), count, stream);
		case veraMemcpyDeviceToDevice:		return vedaMemcpyDtoDAsync(VERA2VEDA(dst), VERA2VEDA(src), count, stream);
		case veraMemcpyDefault:
			;
	};

	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented.
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraDeviceReset(void) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented.
 * @param pitchedDevPtr
 * @param extent
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMalloc3D(veraPitchedPtr* pitchedDevPtr, veraExtent extent) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented.
 * @param dst
 * @param dpitch
 * @param src
 * @param spitch
 * @param width
 * @param height
 * @param kind
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemcpy2D(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented.
 * @param dst
 * @param dpitch
 * @param src
 * @param spitch
 * @param width
 * @param height
 * @param kind
 * @param stream
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemcpy2DAsync(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented. 
 * @param devPtr
 * @param pitch
 * @param value
 * @param width
 * @param height
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemset2D(void* devPtr, size_t pitch, int value, size_t width, size_t height) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented. 
 * @param devPtr
 * @param pitch
 * @param value
 * @param width
 * @param height
 * @param stream
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemset2DAsync(void* devPtr, size_t pitch, int value, size_t width, size_t height, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented. 
 * @param pitchedDevPtr
 * @param value
 * @param extent
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemset3D(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented. 
 * @param pitchedDevPtr
 * @param value
 * @param extent
 * @param stream
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraMemset3DAsync(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief This function is not yet implemented. 
 * @param device_arr
 * @param len
 * @return VEDA_ERROR_NOT_IMPLEMENTED
 */
veraError_t veraSetValidDevices(int* device_arr, int len) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
/**
 * @brief Returns attributes about a specified pointer.
 * @param attributes Attributes for the specified pointer
 * @param ptr Pointer to get attributes for
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_INVALID_DEVICE If the provided VEDA device id is not valid.
 * @return VEDA_ERROR_INVALID_VALUE If the provided ptr is invalid.
 */
veraError_t veraPointerGetAttributes(veraPointerAttributes* attributes, const void* ptr) {
	CVEDA(veraInit());
	
	auto vptr = VERA2VEDA(ptr);
	attributes->device		= VEDA_GET_DEVICE(vptr);
	attributes->hostPointer		= 0;
	attributes->type		= veraMemoryTypeDevice;
	CVEDA(vedaMemPtr(&attributes->devicePointer, vptr));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Set device to be used for device executions.
 * @param device Device on which the active host thread should execute the device code. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_INVALID_DEVICE If the provided VEDA device id is not valid.
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.\n 
 *
 * Sets device as the current device for the calling host thread. 
 */
veraError_t veraSetDevice(int device) {
	CVEDA(veraInit());
	
	VEDAcontext ctx = 0;
	CVEDA(vedaDevicePrimaryCtxRetain(&ctx, device));
	CVEDA(vedaCtxSetCurrent(ctx));
	return VEDA_SUCCESS;
}
/** @} */
//------------------------------------------------------------------------------
