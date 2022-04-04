#pragma once

namespace veda {
	struct Contexts {
		static	VEDAcontext	current		(void);
		static	VEDAcontext	pop		(void);
		static	int		countInstances	(VEDAcontext ctx);
		static	void		push		(VEDAcontext ctx);
		static	void		remove		(VEDAcontext ctx);
		static	void		set		(VEDAcontext ctx);
		static	void		shutdown	(void);
	};
}