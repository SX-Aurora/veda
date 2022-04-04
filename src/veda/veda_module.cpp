#include "veda/internal.h"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaModuleGetFunction(VEDAfunction* hfunc, VEDAmodule hmod, const char* name) {
	GUARDED(
		*hfunc = hmod->getFunction(name);
		L_TRACE("[ve:%i] vedaModuleGetFunction(%p, %p, %s)", hmod->ctx()->device().vedaId(), *hfunc, hmod, name);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleLoad(VEDAmodule* module, const char* fname) {
	GUARDED(
		auto ctx = veda::Contexts::current();
		*module = ctx->moduleLoad(fname);
		L_TRACE("[ve:%i] vedaModuleLoad(%p, %s)", ctx->device().vedaId(), *module, fname);
	)
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleUnload(VEDAmodule hmod) {
	GUARDED(
		L_TRACE("[ve:%i] vedaModuleUnload(%p)", hmod->ctx()->device().vedaId(), hmod);
		hmod->ctx()->moduleUnload(hmod);
	)
}

//------------------------------------------------------------------------------
} // extern "C"
