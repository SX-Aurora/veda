#pragma once

#include <string_view>

#define VEDA_THROW(...)		throw veda::cpp::Exception(__VA_ARGS__, __FILE__, __LINE__)
#define VEDA_THROW_IF(...)	for(auto __res__ = __VA_ARGS__; __res__ != VEDA_SUCCESS;) throw veda::cpp::Exception(__res__, __FILE__, __LINE__)

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
class Exception final {
	VEDAresult		m_error;
	const std::string_view	m_file;
	const int		m_line;

public:
	inline Exception(VEDAresult error, const std::string_view file, const int line) :
		m_error(error), m_file(file), m_line(line)
	{}

	inline	VEDAresult		error	(void) const { return m_error;	}
	inline	int			line	(void) const { return m_line;	}
	inline	std::string_view	file	(void) const { return m_file;	}
	std::string_view		what	(void) const;
	std::string_view		message	(void) const;
};

//------------------------------------------------------------------------------
	}
}