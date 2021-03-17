#pragma once

namespace veda {
	class Module {
		Context* const	m_ctx;
		const veo_lib	m_lib;

	public:
				Module		(Context* ctx, const veo_lib lib);
		virtual		~Module		(void);
		Context*	ctx		(void) const;
		veo_lib		lib		(void) const;
		VEDAfunction	getFunction	(const char* name);
	};
}