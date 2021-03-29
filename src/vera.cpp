#include "vera.h"
#include "veda/veda.hpp"
#include <cstring> // memcpy

//------------------------------------------------------------------------------
veraError_t veraInit(void) {
	static bool vera_initialized = false;
	if(!vera_initialized) {
		vera_initialized = true;
		CVEDA(vedaInit(0));
		VEDAcontext ctx;
		CVEDA(vedaDevicePrimaryCtxRetain(&ctx, 0));
		CVEDA(vedaCtxSetCurrent(ctx));
	}
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
veraError_t veraGetDeviceProperties(veraDeviceProp* prop, int device) {
	CVEDA(veraInit());

	CVEDA(vedaDeviceGetName 	(prop->name, sizeof(veraDeviceProp::name), device));
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
	size_t free;
	CVEDA(vedaMemGetInfo(&free, &prop->totalGlobalMem));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
veraError_t veraStreamAddCallback(veraStream_t stream, veraStreamCallback_t callback, void* userData, unsigned int flags) {
	CVEDA(veraInit());
	return vedaStreamAddCallback(stream, callback, userData, flags);
}

//------------------------------------------------------------------------------
veraError_t veraMemcpy(void* dst, const void* src, size_t count, veraMemcpyKind kind) {
	CVEDA(veraInit());
	
	switch(kind) {
		case veraMemcpyHostToHost:	memcpy(dst, src, count); return VEDA_SUCCESS;
		case veraMemcpyHostToDevice:	return vedaMemcpyHtoD((VEDAdeviceptr)dst, src, count);
		case veraMemcpyDeviceToHost:	return vedaMemcpyDtoH(dst, (VEDAdeviceptr)src, count);
		case veraMemcpyDeviceToDevice:	return vedaMemcpyDtoD((VEDAdeviceptr)dst, (VEDAdeviceptr)src, count);
		case veraMemcpyDefault:
			;
	}

	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemcpyAsync(void* dst, const void* src, size_t count, veraMemcpyKind kind, veraStream_t stream) {
	CVEDA(veraInit());

	switch(kind) {
		case veraMemcpyHostToHost:		memcpy(dst, src, count); return VEDA_SUCCESS;
		case veraMemcpyHostToDevice:		return vedaMemcpyHtoDAsync((VEDAdeviceptr)dst, src, count, stream);
		case veraMemcpyDeviceToHost:		return vedaMemcpyDtoHAsync(dst, (VEDAdeviceptr)src, count, stream);
		case veraMemcpyDeviceToDevice:		return vedaMemcpyDtoDAsync((VEDAdeviceptr)dst, (VEDAdeviceptr)src, count, stream);
		case veraMemcpyDefault:
			;
	};

	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraDeviceReset(void) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMalloc3D(veraPitchedPtr* pitchedDevPtr, veraExtent extent) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemcpy2D(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemcpy2DAsync(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemset2D(void* devPtr, size_t pitch, int value, size_t width, size_t height) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemset2DAsync(void* devPtr, size_t pitch, int value, size_t width, size_t height, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemset3D(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraMemset3DAsync(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent, veraStream_t stream) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraSetValidDevices(int* device_arr, int len) {
	return VEDA_ERROR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
veraError_t veraPointerGetAttributes(veraPointerAttributes* attributes, const void* ptr) {
	CVEDA(veraInit());
	
	auto vptr = (VEDAdeviceptr)ptr;
	attributes->device		= vptr->device();
	attributes->hostPointer		= 0;
	attributes->type		= veraMemoryTypeDevice;
	CVEDA(vedaMemGetRawPointer(&attributes->devicePointer, vptr));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
veraError_t veraSetDevice(int device) {
	CVEDA(veraInit());
	
	VEDAcontext ctx = 0;
	CVEDA(vedaDevicePrimaryCtxRetain(&ctx, device));
	CVEDA(vedaCtxSetCurrent(ctx));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
