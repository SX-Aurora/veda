#include "veda_internal.h"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaMemGetDevice(VEDAdevice* dev, VEDAdeviceptr ptr) {
	VEDAptr vptr(ptr);
	*dev = vptr.device();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAlloc(VEDAdeviceptr* ptr, size_t size) {
	CVEDA(vedaMemAllocAsync(ptr, size, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemAllocAsync(VEDAdeviceptr* ptr, size_t size, VEDAstream stream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(stream, &ctx));
		CVEDA(ctx->memAlloc(ptr, size));
	);
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
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(stream, &ctx));
		CVEDA(ctx->memAllocPitch(dptr, pPitch, WidthInBytes, Height, ElementSizeByte));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFree(VEDAdeviceptr ptr) {
	CVEDA(vedaMemFreeAsync(ptr, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFreeAsync(VEDAdeviceptr ptr, VEDAstream stream) {
	GUARDED(
		VEDAcontext ctx;
		VEDAptr vptr(ptr);
		CVEDA(vedaDevicePrimaryCtxRetain(&ctx, vptr.device()));
		if(stream)
			CVEDA(vedaStreamCompareCtx(stream, ctx));
		CVEDA(ctx->memFree(ptr));
		CVEDA(vedaDevicePrimaryCtxRelease(vptr.device()));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemFreeHost(void* ptr) {
	if(ptr == 0)
		return VEDA_ERROR_INVALID_VALUE;
	free(ptr);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetAddressRange(VEDAdeviceptr* base, size_t* size, VEDAdeviceptr ptr) {
	GUARDED(
		VEDAcontext ctx;
		VEDAptr vptr(ptr);
		CVEDA(vedaDevicePrimaryCtxRetain(&ctx, vptr.device()));
		*base = vptr.base();
		veo_ptr _;
		auto ret = ctx->getPtr(&_, size, ptr);
		if(ret == VEDA_ERROR_UNKNOWN_PPTR)	*size = 0;
		else								CVEDA(ret);
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetInfo(size_t* free, size_t* total) {
	GUARDED(
		*free = 0; // TODO: is it possible to determine this value?
		VEDAproc proc;
		CVEDA(vedaProcGetCurrent(&proc));
		CVEDA(vedaDeviceTotalMem(total, proc->device()));
		CVEDA(proc->getFreeMem(free, *total));
	);
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
VEDAresult vedaMemcpyDtoDAsync(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t size, VEDAstream hStream) {
	VEDAptr dst(dstDevice);
	VEDAptr src(srcDevice);

	if(dst.device() == src.device()) {
		GUARDED(
			VEDAcontext ctx = 0;
			CVEDA(vedaStreamGetCtx(hStream, &ctx));
			if(ctx->proc()->device() != dst.device())
				return VEDA_ERROR_INVALID_STREAM;
			CVEDA(ctx->memcpyD2D(dstDevice, srcDevice, size));
		);
	} else {
		GUARDED(
			VEDAcontext ctx;
			void* host = malloc(size);

			VEDAproc srcProc;
			CVEDA(vedaProcGet(&srcProc, src.device()));
			CVEDA(vedaCtxPushCurrent(srcProc->ctxDefault()));
			CVEDA(vedaMemcpyDtoHAsync(host, srcDevice, size, 0));
			CVEDA(vedaCtxSynchronize());
			CVEDA(vedaCtxPopCurrent(&ctx));

			VEDAproc dstProc;
			CVEDA(vedaProcGet(&dstProc, dst.device()));
			CVEDA(vedaCtxPushCurrent(dstProc->ctxDefault()));
			CVEDA(vedaMemcpyHtoDAsync(dstDevice, host, size, 0));
			CVEDA(vedaCtxSynchronize());
			CVEDA(vedaCtxPopCurrent(&ctx));

			free(host);
		);
	}
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoH(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount) {
	CVEDA(vedaMemcpyDtoHAsync(dstHost, srcDevice, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyDtoHAsync(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memcpyD2H(dstHost, srcDevice, ByteCount));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyHtoD(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount) {
	CVEDA(vedaMemcpyHtoDAsync(dstDevice, srcHost, ByteCount, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemcpyHtoDAsync(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memcpyH2D(dstDevice, srcHost, ByteCount));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD16(VEDAdeviceptr dstDevice, uint16_t us, size_t N) {
	CVEDA(vedaMemsetD16Async(dstDevice, us, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD16Async(VEDAdeviceptr dstDevice, uint16_t us, size_t N, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset(dstDevice, us, N));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D16(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D16Async(dstDevice, dstPitch, us, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D16Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t us, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset2D(dstDevice, dstPitch, us, Width, Height));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D32(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D32Async(dstDevice, dstPitch, ui, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D32Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t ui, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset2D(dstDevice, dstPitch, ui, Width, Height));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D8(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height) {
	CVEDA(vedaMemsetD2D8Async(dstDevice, dstPitch, uc, Width, Height, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD2D8Async(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t uc, size_t Width, size_t Height, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset2D(dstDevice, dstPitch, uc, Width, Height));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD32(VEDAdeviceptr dstDevice, uint32_t ui, size_t N) {
	CVEDA(vedaMemsetD32Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD32Async(VEDAdeviceptr dstDevice, uint32_t ui, size_t N, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset(dstDevice, ui, N));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD64(VEDAdeviceptr dstDevice, uint64_t ui, size_t N) {
	CVEDA(vedaMemsetD64Async(dstDevice, ui, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD64Async(VEDAdeviceptr dstDevice, uint64_t ui, size_t N, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset(dstDevice, ui, N));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD8(VEDAdeviceptr dstDevice, uint8_t uc, size_t N) {
	CVEDA(vedaMemsetD8Async(dstDevice, uc, N, 0));
	return vedaCtxSynchronize();
}

//------------------------------------------------------------------------------
VEDAresult vedaMemsetD8Async(VEDAdeviceptr dstDevice, uint8_t uc, size_t N, VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->memset(dstDevice, uc, N));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemReport(void) {
	GUARDED(
		CVEDA(vedaProcMemReport());
	);
}

//------------------------------------------------------------------------------
} // extern "C"