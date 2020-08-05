#include "veda_internal.h"

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
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->streamDestroy(hStream));
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
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->query(hStream));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamSynchronize(VEDAstream hStream) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->sync(hStream));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamAddCallback(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		callback(stream, ctx->sync(stream), userData);
	);
}

//------------------------------------------------------------------------------
} // extern "C"
