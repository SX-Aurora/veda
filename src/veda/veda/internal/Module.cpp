#include <veda/internal/api.h>

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
Module::Module(Context* ctx, const veo_lib lib) :
	m_ctx(ctx),
	m_lib(lib)
{}

//------------------------------------------------------------------------------
Module::~Module(void) {
}

//------------------------------------------------------------------------------
Context* Module::ctx(void) const { return m_ctx; }
veo_lib	 Module::lib(void) const { return m_lib; }

//------------------------------------------------------------------------------
VEDAfunction Module::getFunction(const std::string_view name) {
	auto ref = m_functions.emplace(name, 0);
	auto& it = ref.first;
	if(ref.second)
		it->second = ctx()->moduleGetFunction(this, name);

	VEDAfunction r;
	r.ptr		= it->second;
	r.kernel	= it->first.c_str();
	return r;
}

//------------------------------------------------------------------------------
void Module::unload(void) const {
	return ctx()->moduleUnload(this);
}

//------------------------------------------------------------------------------
	}
}
