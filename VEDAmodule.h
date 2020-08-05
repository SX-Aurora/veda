#pragma once

struct __VEDAmodule {
private:
	VEDAcontext const	m_ctx;
	const veo_lib		m_lib;

public:
				__VEDAmodule	(VEDAcontext ctx, const veo_lib lib);
	virtual		~__VEDAmodule	(void);
	VEDAcontext	ctx				(void) const;
	veo_lib		lib				(void) const;
};
