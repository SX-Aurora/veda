#define VGEN_B		D8
#define VGEN_T		uint8_t
#define VGEN_L		"%u"
#define VGEN_C		(uint32_t)
#include "veda_memset.tpl.h"

#define VGEN_B		D16
#define VGEN_T		uint16_t
#define VGEN_L		"%u"
#define VGEN_C		(uint32_t)
#include "veda_memset.tpl.h"

#define VGEN_B		D32
#define VGEN_T		uint32_t
#define VGEN_L		"%u"
#define VGEN_C		
#include "veda_memset.tpl.h"

#define VGEN_B		D64
#define VGEN_T		uint64_t
#define VGEN_L		"%llu"
#define VGEN_C		
#include "veda_memset.tpl.h"

#include "veda_memset128.tpl.h"
