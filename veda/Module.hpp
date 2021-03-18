#pragma once

namespace veda {
	class Module {
		Context* const	m_ctx;
		const veo_lib	m_lib;

	public:
				Module		(Context* ctx, const veo_lib lib);
				Module		(const Module&) = delete;
		Context*	ctx		(void) const;
		VEDAfunction	getFunction	(const char* name);
		veo_lib		lib		(void) const;
		virtual		~Module		(void);
	};
}