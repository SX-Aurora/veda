#include "veda_internal.h"

//------------------------------------------------------------------------------
VEDAresult vedaStreamGetProc(VEDAproc* proc, VEDAstream stream) {
	VEDAcontext ctx;
	CVEDA(vedaStreamGetCtx(stream, &ctx));
	*proc = ctx->proc();
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamCompareCtx(VEDAstream stream, VEDAcontext ctx) {
	VEDAcontext sctx;
	CVEDA(vedaStreamGetCtx(stream, &sctx));
	return sctx == ctx ? VEDA_SUCCESS : VEDA_ERROR_INVALID_STREAM;
}

//------------------------------------------------------------------------------

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaStreamCreate(VEDAstream* phStream, uint32_t Flags) {
	GUARDED(
		if(Flags != 0)	return VEDA_ERROR_INVALID_VALUE;
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->streamCreate(phStream));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamDestroy(VEDAstream hStream) {
	GUARDED(
		if(hStream == 0) return VEDA_ERROR_INVALID_STREAM;
		CVEDA(hStream->ctx->streamDestroy(hStream));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamGetCtx(VEDAstream hStream, VEDAcontext* pctx) {
	if(hStream == 0)
		return vedaCtxGetCurrent(pctx);

	GUARDED(
		*pctx = hStream->ctx;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamGetFlags(VEDAstream hStream, uint32_t* flags) {
	GUARDED(
		*flags = 0;
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamQuery(VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->query());
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamSynchronize(VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(hStream, &ctx));
		CVEDA(ctx->sync());
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamAddCallback(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaStreamGetCtx(stream, &ctx));
		callback(stream, ctx->sync(), userData);
	);
}

//------------------------------------------------------------------------------
} // extern "C"
