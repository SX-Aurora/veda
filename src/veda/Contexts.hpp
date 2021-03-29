#pragma once

namespace veda {
	struct Contexts {
		static	VEDAcontext	current	(void);
		static	VEDAcontext	pop	(void);
		static	void		push	(VEDAcontext ctx);
		static	void		set	(VEDAcontext ctx);
	};
}