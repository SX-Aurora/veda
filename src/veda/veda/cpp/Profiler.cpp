#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
		namespace Profiler {
//------------------------------------------------------------------------------
VEDAprofiler_callback get(void) {
	return internal::profiler::getCallback();
}

//------------------------------------------------------------------------------
void set(VEDAprofiler_callback callback) {
	internal::profiler::setCallback(callback);
}

//------------------------------------------------------------------------------
		}
	}
}