#include <veda/internal/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
std::string_view Exception::what(void) const {
	const char* ptr = 0;
	vedaGetErrorName(error(), &ptr);
	return ptr;
}

//------------------------------------------------------------------------------
std::string_view Exception::message(void) const {
	const char* ptr = 0;
	vedaGetErrorString(error(), &ptr);
	return ptr;
}

//------------------------------------------------------------------------------
	}
}