#include "veda_internal.h"

extern "C" {
//------------------------------------------------------------------------------
#include "veda_error.inc.cpp"

//------------------------------------------------------------------------------
VEDAresult vedaInit(uint32_t Flags) {
	// Flags needs to be 0
	if(Flags != 0)
		return VEDA_ERROR_INVALID_VALUE;

	CVEDA(vedaSetInitialized(true));
	GUARDED(
		std::set<int> devices;
		CVEDA(vedaDeviceInitCount(devices));
		CVEDA(vedaDeviceInitMapping(devices));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaExit(void) {
	CVEDA(vedaSemShutdown());
	CVEDA(vedaSetInitialized(false));
	CVEDA(vedaCtxExit());
	CVEDA(vedaProcExit());
	CVEDA(vedaDeviceExit());
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaDriverGetVersion(const char** str) {
	GUARDED(
		*str = veo_version_string();
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchKernel(VEDAfunction f, VEDAstream stream, VEDAargs args) {
	return vedaLaunchKernelEx(f, stream, args, 0);
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchKernelEx(VEDAfunction f, VEDAstream stream, VEDAargs args, const int destroyArgs) {
	GUARDED(
		VEDAcontext ctx;
		CVEDA(vedaStreamGetCtx(stream, &ctx));
		CVEDA(ctx->call(f, args, destroyArgs != 0));
	);
}

//------------------------------------------------------------------------------
} // extern "C"