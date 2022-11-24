#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
std::string_view driverVersion	(void) { return veo_version_string();	}
std::string_view version	(void) { return VEDA_STR(VEDA_VERSION);	}

//------------------------------------------------------------------------------
void exit(void) {
	internal::Semaphore::shutdown();
	internal::setInitialized(false);
	internal::devices::shutdown();
	internal::contexts::shutdown();
	internal::profiler::shutdown();
}

//------------------------------------------------------------------------------
void init(uint32_t flags) {
	if(flags != 0) // Flags needs to be 0
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);

	internal::setInitialized(true);
	internal::devices::init();
}

//------------------------------------------------------------------------------
	}
}