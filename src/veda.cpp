#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
#include "veda_error.inc.cpp"

//------------------------------------------------------------------------------
VEDAresult vedaInit(uint32_t Flags) {
	// Flags needs to be 0
	if(Flags != 0)
		return VEDA_ERROR_INVALID_VALUE;

	TRY(
		veda::setInitialized(true);
		veda::Devices::init();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaExit(void) {
	TRY(
		veda::Semaphore::shutdown();
		veda::setInitialized(false);
		veda::Devices::shutdown();
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaDriverGetVersion(const char** str) {
	GUARDED(*str = veo_version_string();)
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchKernel(VEDAfunction f, VEDAstream stream, VEDAargs args) {
	return vedaLaunchKernelEx(f, stream, args, 0);
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchKernelEx(VEDAfunction f, VEDAstream stream, VEDAargs args, const int destroyArgs) {
	GUARDED(veda::Contexts::current()->call(f, stream, args, destroyArgs != 0);)
}

//------------------------------------------------------------------------------
VEDAresult vedaLaunchHostFunc(VEDAstream stream, VEDAhost_function fn, void* userData) {
	GUARDED(veda::Contexts::current()->call(fn, userData, stream);)
}

//------------------------------------------------------------------------------
} // extern "C"