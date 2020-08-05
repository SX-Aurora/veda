#include "veda_internal.h"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaModuleGetFunction(VEDAfunction* hfunc, VEDAmodule hmod, const char* name) {
	GUARDED(
		CVEDA(hmod->ctx()->moduleGetFunction(hfunc, hmod, name));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleLoad(VEDAmodule* module, const char* fname) {
	GUARDED(
		VEDAcontext ctx = 0;
		CVEDA(vedaCtxGetCurrent(&ctx));
		CVEDA(ctx->moduleLoad(module, fname));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleUnload(VEDAmodule hmod) {
	GUARDED(
		CVEDA(hmod->ctx()->moduleUnload(hmod));
	);
}

//------------------------------------------------------------------------------
} // extern "C"
