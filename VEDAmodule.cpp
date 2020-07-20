#include "veda_internal.h"

//------------------------------------------------------------------------------
__VEDAmodule::__VEDAmodule(VEDAproc proc, const veo_lib lib) : m_proc(proc), m_lib(lib), m_usage(0) {
}

//------------------------------------------------------------------------------
__VEDAmodule::~__VEDAmodule(void) {
}

//------------------------------------------------------------------------------
VEDAproc	__VEDAmodule::proc	(void) const	{	return m_proc;		}
veo_lib		__VEDAmodule::lib	(void) const	{	return m_lib;		}
int			__VEDAmodule::usage	(void) const	{	return m_usage;		}
int			__VEDAmodule::inc	(void)			{	return ++m_usage;	}
int			__VEDAmodule::dec	(void)			{	return --m_usage;	}

//------------------------------------------------------------------------------
