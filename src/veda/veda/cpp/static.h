#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
void			exit		(void);
void			init		(uint32_t flags = 0);
std::string_view	driverVersion	(void);
std::string_view	version		(void);

//------------------------------------------------------------------------------
	}
}

namespace veda {
	using cpp::exit;
	using cpp::init;
	using cpp::driverVersion;
	using cpp::version;
}