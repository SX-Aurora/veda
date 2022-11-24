#pragma once

namespace veda {
	namespace cpp {
		namespace Profiler {
//------------------------------------------------------------------------------
VEDAprofiler_callback	get(void);
void			set(VEDAprofiler_callback callback);

//------------------------------------------------------------------------------
		}
	}
}

namespace veda {
	using cpp::Profiler::get;
	using cpp::Profiler::set;
}