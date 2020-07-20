#pragma once

struct __VEDAmodule {
private:
	VEDAproc const	m_proc;
	const veo_lib	m_lib;
	int 			m_usage;

public:
				__VEDAmodule	(VEDAproc proc, const veo_lib lib);
	virtual		~__VEDAmodule	(void);
	VEDAproc	proc			(void) const;
	veo_lib		lib				(void) const;
	int			usage			(void) const;
	int			inc				(void);
	int			dec				(void);
};
