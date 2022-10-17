#pragma once

namespace veda {
	namespace contexts {
		VEDAcontext	current		(void);
		VEDAcontext	pop		(void);
		int		countInstances	(VEDAcontext ctx);
		void		push		(VEDAcontext ctx);
		void		remove		(VEDAcontext ctx);
		void		set		(VEDAcontext ctx);
		void		shutdown	(void);
	};
}