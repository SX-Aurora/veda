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
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->call(f, stream, args, destroyArgs != 0));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchHostFunc(VEDAstream stream, VEDAhost_function fn, void* userData) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->call(fn, userData, stream));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaMemGetRawPointer(void** rawPtr, VEDAdeviceptr ptr) {
	GUARDED(
		VEDAptr vptr(ptr);
		VEDAcontext ctx;
		CVEDA(vedaCtxGet(&ctx, vptr.device()));
		size_t size;
		auto ret = ctx->getPtr((veo_ptr*)rawPtr, &size, ptr);
		if(ret != VEDA_ERROR_UNKNOWN_PPTR) CVEDA(ret);
	);
}

//------------------------------------------------------------------------------
} // extern "C"