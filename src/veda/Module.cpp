#include "veda.hpp"

namespace veda {
//------------------------------------------------------------------------------
Module::Module(Context* ctx, const veo_lib lib) :
	m_ctx(ctx),
	m_lib(lib)
{}

//------------------------------------------------------------------------------
Module::~Module(void) {
}

//------------------------------------------------------------------------------
Context*	Module::ctx		(void) const		{	return m_ctx;					}
VEDAfunction	Module::getFunction	(const char* name)	{	return ctx()->moduleGetFunction(this, name);	}
veo_lib		Module::lib		(void) const		{	return m_lib;					}

//------------------------------------------------------------------------------
}
