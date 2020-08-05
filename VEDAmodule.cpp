#include "veda_internal.h"

//------------------------------------------------------------------------------
__VEDAmodule::__VEDAmodule(VEDAcontext ctx, const veo_lib lib) : m_ctx(ctx), m_lib(lib) {
}

//------------------------------------------------------------------------------
__VEDAmodule::~__VEDAmodule(void) {
}

//------------------------------------------------------------------------------
VEDAcontext	__VEDAmodule::ctx	(void) const	{	return m_ctx;		}
veo_lib		__VEDAmodule::lib	(void) const	{	return m_lib;		}

//------------------------------------------------------------------------------
