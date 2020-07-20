#include "veda_internal.h"

//------------------------------------------------------------------------------
VEDAresult vedaKernelGetName(const char** name, VEDAkernel kernel) {
	switch(kernel) {
		case VEDA_KERNEL_MEMSET_U8:		*name = "veda_memset_u8";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U16:	*name = "veda_memset_u16";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U32:	*name = "veda_memset_u32";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U64:	*name = "veda_memset_u64";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U8_2D:	*name = "veda_memset_u8_2d";	return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U16_2D:	*name = "veda_memset_u16_2d";	return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U32_2D:	*name = "veda_memset_u32_2d";	return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMSET_U64_2D:	*name = "veda_memset_u64_2d";	return VEDA_SUCCESS;
		case VEDA_KERNEL_MEMCPY_D2D:	*name = "veda_memcpy_d2d";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEM_ALLOC:		*name = "veda_mem_alloc";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEM_FREE:		*name = "veda_mem_free";		return VEDA_SUCCESS;
		case VEDA_KERNEL_MEM_PTR:		*name = "veda_mem_ptr";			return VEDA_SUCCESS;
	}

	return VEDA_ERROR_UNKNOWN_KERNEL;
}

//------------------------------------------------------------------------------