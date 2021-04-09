#include "veda/veda.hpp"
#include <veo_hmem_macros.h>

extern "C" {
//------------------------------------------------------------------------------
VEDAresult vedaArgsCreate(VEDAargs* args) {
	*args = veo_args_alloc();
	return *args ? VEDA_SUCCESS : VEDA_ERROR_OUT_OF_MEMORY;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsDestroy(VEDAargs args) {
	if(!args)
		return VEDA_ERROR_INVALID_ARGS;
	veo_args_free(args);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetI8(VEDAargs args, const int idx, const int8_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i8(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetI16(VEDAargs args, const int idx, const int16_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i16(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetI32(VEDAargs args, const int idx, const int32_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i32(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetI64(VEDAargs args, const int idx, const int64_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i64(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetU8(VEDAargs args, const int idx, const uint8_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u8(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetU16(VEDAargs args, const int idx, const uint16_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u16(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetU32(VEDAargs args, const int idx, const uint32_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u32(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetU64(VEDAargs args, const int idx, const uint64_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u64(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetHMEM(VEDAargs args, const int idx, const void* value) {
	if(!args)		return VEDA_ERROR_INVALID_ARGS;
	if(!IS_VE(value))	return VEDA_ERROR_INVALID_VALUE;
	CVEO(veo_args_set_u64(args, idx, VIRT_ADDR_VE(value)));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetPtr(VEDAargs args, const int idx, const VEDAdeviceptr value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u64(args, idx, (uint64_t)VEDAptr<>(value).ptr()));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetVPtr(VEDAargs args, const int idx, const VEDAdeviceptr value) {
	return vedaArgsSetU64(args, idx, (uint64_t)value);
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetF32(VEDAargs args, const int idx, const float value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_float(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetF64(VEDAargs args, const int idx, const double value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_double(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaArgsSetStack(VEDAargs args, const int idx, void* ptr, VEDAargs_intent intent, const size_t size) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_stack(args, (veo_args_intent)intent, idx, (char*)ptr, size));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
} // extern "C"