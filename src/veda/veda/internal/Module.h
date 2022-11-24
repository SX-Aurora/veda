#pragma once

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
class Module {
	using Functions = std::map<std::string, veo_sym>;

	Context* const	m_ctx;
	const veo_lib	m_lib;
	Functions	m_functions;

public:
			Module		(Context* ctx, const veo_lib lib);
			Module		(const Module&) = delete;
	Context*	ctx		(void) const;
	VEDAfunction	getFunction	(const std::string_view name);
	veo_lib		lib		(void) const;
	void		unload		(void) const;
	virtual		~Module		(void);
};

//------------------------------------------------------------------------------
	}
}