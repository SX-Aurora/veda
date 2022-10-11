#include <veda/internal.h>

extern "C" {
// implementation of VEDA API functions
/**
 * \defgroup vedaapi VEDA API
 *
 * To use VEDA API functions, include "veda.h" header.
 */
/** @{ */
//------------------------------------------------------------------------------
#define VGEN_F	vedaMemset
#define VGEN_P	VEDAdeviceptr
#include "veda_memset.types.h"
#undef VGEN_F
#undef VGEN_P

//------------------------------------------------------------------------------
} // extern "C"