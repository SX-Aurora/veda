#include <veda/internal.h>

extern "C" {
// implementation of VEDA API functions
/**
 * \defgroup vedaapi VEDA API
 *
 * To use VEDA API functions, include "veda.h" header.
 */
/** @{ */
//------------------------------------------------------------------------------
/**
 * @brief Return the VE address of the VEDA device function located in VEDA module. 
 * @param hfunc Buffer to hold the VE address of the VEDA device function.
 * @param hmod handle to the VEDA module corresponding to the VE device code.
 * @param name Name of the VEDA device function for which the VE address is required.
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_VALUE In case the name of the VEDA device function is passed as NULL.
 * @return VEDA_ERROR_FUNCTION_NOT_FOUND Internal error from the AVEO library
 */
//------------------------------------------------------------------------------
VEDAresult vedaModuleGetFunction(VEDAfunction* hfunc, VEDAmodule hmod, const char* name) {
	GUARDED(
		if(!hmod || !hfunc || !name)
			return VEDA_ERROR_INVALID_ARGS;
		*hfunc = hmod->getFunction(name);
		L_TRACE("[ve:%i] vedaModuleGetFunction(%p, %p, %s)", hmod->ctx()->device().vedaId(), *hfunc, hmod, name);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief To load the VE device code into the VE memory.
 * @param module Pointer to hold the reference of the VEDA module.
 * @param fname Library name of the file name containing VE device code.
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_VALUE If the provided VE library name is not valid.
 * @return VEDA_ERROR_MODULE_NOT_FOUND Internal error from the AVEO library
 */
VEDAresult vedaModuleLoad(VEDAmodule* module, const char* fname) {
	GUARDED(
		auto ctx = veda::contexts::current();
		*module = ctx->moduleLoad(fname);
		L_TRACE("[ve:%i] vedaModuleLoad(%p, %s)", ctx->device().vedaId(), *module, fname);
	)
}

//------------------------------------------------------------------------------
/**
 * @brief To unload/remove the VE device code from the VE memory.
 * @param hmod Pointer to the VEDA module corresponding to the VE device code..
 * @return VEDA_SUCCESS on Success	
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_UNKOWN_CONTEXT If VEDA context is not initialized.
 */
VEDAresult vedaModuleUnload(VEDAmodule hmod) {
	GUARDED(
		L_TRACE("[ve:%i] vedaModuleUnload(%p)", hmod->ctx()->device().vedaId(), hmod);
		hmod->ctx()->moduleUnload(hmod);
	)
}

//------------------------------------------------------------------------------
} // extern "C"
