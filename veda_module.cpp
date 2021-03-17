#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaModuleGetFunction(VEDAfunction* hfunc, VEDAmodule hmod, const char* name) {
	GUARDED(*hfunc = hmod->getFunction(name);)
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleLoad(VEDAmodule* module, const char* fname) {
	GUARDED(*module = veda::Contexts::current()->moduleLoad(fname);)
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleUnload(VEDAmodule hmod) {
	GUARDED(hmod->ctx()->moduleUnload(hmod);)
}

//------------------------------------------------------------------------------
} // extern "C"
