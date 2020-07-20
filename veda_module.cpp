#include "veda_internal.h"

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaModuleGetFunction(VEDAfunction* hfunc, VEDAmodule hmod, const char* name) {
	GUARDED(
		VEDAproc proc = 0;
		CVEDA(vedaProcGetCurrent(&proc));
		CVEDA(proc->moduleGetFunction(hfunc, hmod, name));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleLoad(VEDAmodule* module, const char* fname) {
	GUARDED(
		VEDAproc proc = 0;
		CVEDA(vedaProcGetCurrent(&proc));
		CVEDA(proc->moduleLoad(module, fname));
	);
}

//------------------------------------------------------------------------------
VEDAresult vedaModuleUnload(VEDAmodule hmod) {
	GUARDED(
		VEDAproc proc = 0;
		CVEDA(vedaProcGetCurrent(&proc));
		CVEDA(proc->moduleUnload(hmod));
	);
}

//------------------------------------------------------------------------------
} // extern "C"
