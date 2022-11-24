#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
// Setter
//------------------------------------------------------------------------------
void Args::Setter::operator=(const Base<VEDA>& ptr)	{ (*this) = ((uint64_t)ptr.raw());									}
void Args::Setter::operator=(const Base<HMEM>& ptr)	{ if(!m_args || veo_args_set_hmem	(m_args, m_idx, ptr.raw()) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const double value)	{ if(!m_args || veo_args_set_double	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const float value)		{ if(!m_args || veo_args_set_float	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const int16_t value)	{ if(!m_args || veo_args_set_i16	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const int32_t value)	{ if(!m_args || veo_args_set_i32	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const int64_t value)	{ if(!m_args || veo_args_set_i64	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const int8_t value)	{ if(!m_args || veo_args_set_i8		(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const uint16_t value)	{ if(!m_args || veo_args_set_u16	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const uint32_t value)	{ if(!m_args || veo_args_set_u32	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const uint64_t value)	{ if(!m_args || veo_args_set_u64	(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const uint8_t value)	{ if(!m_args || veo_args_set_u8		(m_args, m_idx, value) != 0) throw VEDA_ERROR_INVALID_ARGS;	}
void Args::Setter::operator=(const void* ptr)		{ (*this) = ((uint64_t)ptr);										}

//------------------------------------------------------------------------------
void Args::Setter::operator=(const Stack& stack) {
	if(!m_args || veo_args_set_stack(m_args, (veo_args_intent)stack.intent, m_idx, (char*)stack.ptr, stack.size) != 0)
		throw VEDA_ERROR_INVALID_ARGS;
}

//------------------------------------------------------------------------------
// Args
//------------------------------------------------------------------------------
Args::Args(void) : m_args(veo_args_alloc()), m_idx(0) {
}

//------------------------------------------------------------------------------
Args::~Args(void) {
	if(!m_args)
		veo_args_free(m_args);
}

//------------------------------------------------------------------------------
	}
}