#include "veda/veda.hpp"
#include <veo_hmem_macros.h>

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaMemGetDevice(VEDAdevice* dev, VEDAdeviceptr ptr) {
	*dev = ptr->device();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAlloc(VEDAdeviceptr* ptr, size_t size) {
	CVEDA(vedaMemAllocAsync(ptr, size, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAllocAsync(VEDAdeviceptr* ptr, size_t size, VEDAstream stream) {
	GUARDED(*ptr = veda::Contexts::current()->memAlloc(size, stream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAllocHost(void** pp, size_t bytesize) {
	return (*pp = malloc(bytesize)) == 0 ? VEDA_ERROR_OUT_OF_MEMORY : VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAllocPitch(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte) {
	CVEDA(vedaMemAllocPitchAsync(dptr, pPitch, WidthInBytes, Height, ElementSizeByte, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAllocPitchAsync(VEDAdeviceptr* dptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte, VEDAstream stream) {
	GUARDED(
		auto res = veda::Contexts::current()->memAllocPitch(WidthInBytes, Height, ElementSizeByte, stream);
		*dptr	= std::get<0>(res);
		*pPitch	= std::get<1>(res);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFree(VEDAdeviceptr ptr) {
	CVEDA(vedaMemFreeAsync(ptr, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFreeAsync(VEDAdeviceptr ptr, VEDAstream stream) {
	GUARDED(veda::Devices::get(ptr).ctx()->memFree(ptr, stream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFreeHost(void* ptr) {
	if(ptr == 0)
		return VEDA_ERROR_INVALID_VALUE;
	free(ptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetAddressRange(VEDAdeviceptr* base, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto res = veda::Devices::get(vptr->device()).ctx()->getPtr(vptr);
		*base = vptr->base();
		*size = std::get<1>(res);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetInfo(size_t* free, size_t* total) {
	GUARDED(
		auto ctx = veda::Contexts::current();
		*total	= ctx->device().memorySize();
		*free	= *total - ctx->memUsed();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpy(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoD(dst, src, ByteCount));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyAsync(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount, VEDAstream hStream) {
	return vedaMemcpyDtoDAsync(dst, src, ByteCount, hStream);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoD(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoDAsync(dstDevice, srcDevice, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoDAsync(VEDAdeviceptr dst, VEDAdeviceptr src, size_t size, VEDAstream hStream) {
	if(dst->device() == src->device()) {
		GUARDED(
			auto ctx = veda::Devices::get(dst->device()).ctx();
			ctx->memcpyD2D(dst, src, size, hStream);
		)
	} else {
		GUARDED(
			auto sctx = veda::Devices::get(src->device()).ctx();
			auto dctx = veda::Devices::get(dst->device()).ctx();

			void* host = malloc(size);
			if(!host)
				throw VEDA_ERROR_OUT_OF_MEMORY;
			
			
			sctx->memcpyD2H(host, src, size, 0);
			sctx->sync(0);

			dctx->memcpyH2D(dst, host, size, 0);
			dctx->sync(0);
			
			free(host);
		)
	}
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoH(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoHAsync(dstHost, srcDevice, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoHAsync(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream) {
	GUARDED(veda::Devices::get(srcDevice).ctx()->memcpyD2H(dstHost, srcDevice, ByteCount, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyHtoD(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount) {
	CVEDA(vedaMemcpyHtoDAsync(dstDevice, srcHost, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyHtoDAsync(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memcpyH2D(dstDevice, srcHost, ByteCount, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD16(VEDAdeviceptr dstDevice, uint16_t us, size_t N) {
	CVEDA(vedaMemsetD16Async(dstDevice, us, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD16Async(VEDAdeviceptr dstDevice, uint16_t us, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset(dstDevice, us, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D16(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D16Async(dstDevice, dstPitch, us, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D16Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset2D(dstDevice, dstPitch, us, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D32(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D32Async(dstDevice, dstPitch, ui, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D32Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset2D(dstDevice, dstPitch, ui, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D8(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D8Async(dstDevice, dstPitch, uc, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D8Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset2D(dstDevice, dstPitch, uc, Width, Height, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD32(VEDAdeviceptr dstDevice, uint32_t ui, size_t N) {
	CVEDA(vedaMemsetD32Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD32Async(VEDAdeviceptr dstDevice, uint32_t ui, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset(dstDevice, ui, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD64(VEDAdeviceptr dstDevice, uint64_t ui, size_t N) {
	CVEDA(vedaMemsetD64Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD64Async(VEDAdeviceptr dstDevice, uint64_t ui, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset(dstDevice, ui, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD8(VEDAdeviceptr dstDevice, uint8_t uc, size_t N) {
	CVEDA(vedaMemsetD8Async(dstDevice, uc, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD8Async(VEDAdeviceptr dstDevice, uint8_t uc, size_t N, VEDAstream hStream) {
	GUARDED(veda::Devices::get(dstDevice).ctx()->memset(dstDevice, uc, N, hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemReport(void) {
	GUARDED(veda::Devices::memReport();)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemPtr(void** ptr, VEDAdeviceptr vptr) {
	GUARDED(
		auto res = veda::Devices::get(vptr->device()).ctx()->getPtr(vptr);
		*ptr = (void*)std::get<0>(res);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemPtrSize(void** ptr, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto res = veda::Devices::get(vptr->device()).ctx()->getPtr(vptr);
		*ptr	= (void*)std::get<0>(res);
		*size	= std::get<1>(res);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemSize(size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto res = veda::Devices::get(vptr->device()).ctx()->getPtr(vptr);
		*size	= std::get<1>(res);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaMemHMEM(void** ptr, VEDAdeviceptr vptr) {
	GUARDED(
		auto ctx = veda::Devices::get(vptr->device()).ctx();
		auto res = ctx->getPtr(vptr);
		*ptr = (void*)(std::get<0>(res) | ctx->hmemId());
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemHMEMSize(void** ptr, size_t* size, VEDAdeviceptr vptr) {
	GUARDED(
		auto ctx = veda::Devices::get(vptr->device()).ctx();
		auto res = ctx->getPtr(vptr);
		*ptr	= (void*)(std::get<0>(res) | ctx->hmemId());
		*size	= std::get<1>(res);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetRawPointer(void** rawPtr, VEDAdeviceptr vptr) {
	return vedaMemPtr(rawPtr, vptr);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetHMEMPointer(void** hmemPtr, VEDAdeviceptr vptr) {
	return vedaMemHMEM(hmemPtr, vptr);
}

//------------------------------------------------------------------------------
} // extern "C"