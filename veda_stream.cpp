#include "veda/veda.hpp"

extern "C" {
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
	GUARDED(veda::Contexts::current()->sync(hStream);)
}

//------------------------------------------------------------------------------
VEDAresult vedaStreamAddCallback(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags) {
	GUARDED(
		VEDAresult res = VEDA_SUCCESS;
		try {
			veda::Contexts::current()->sync(stream);
		} catch(VEDAresult r) {
			res = r;
		}
		callback(stream, res, userData);
	);
}

//------------------------------------------------------------------------------
} // extern "C"
