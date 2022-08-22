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
/**
 * @brief Instantiate the VEDA function argument handler.
 * @param args Handle for VEDA function argument handler
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_OUT_OF_MEMORY If no memory left on VEDA device to allocate arguments on VEDA device. \n 
 *
 * Further with the help of this VEDA function argument handler VEDA hybrid programmer can set the arguments for the VEDA device function.
 */
VEDAresult vedaArgsCreate(VEDAargs* args) {
	*args = veo_args_alloc();
	return *args ? VEDA_SUCCESS : VEDA_ERROR_OUT_OF_MEMORY;
}

//------------------------------------------------------------------------------
/**
 * @brief Destroy the VEDA function argument handler.
 * @param args Handle for VEDA function argument handler
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsDestroy(VEDAargs args) {
	if(!args)
		return VEDA_ERROR_INVALID_ARGS;
	veo_args_free(args);
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 8 bit integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetI8(VEDAargs args, const int idx, const int8_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i8(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 16 bit integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetI16(VEDAargs args, const int idx, const int16_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i16(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 32 bit integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetI32(VEDAargs args, const int idx, const int32_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i32(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 64 bit integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetI64(VEDAargs args, const int idx, const int64_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_i64(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 8 bit unsigned integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetU8(VEDAargs args, const int idx, const uint8_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u8(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 16 bit unsigned integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetU16(VEDAargs args, const int idx, const uint16_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u16(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 32 bit unsigned integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetU32(VEDAargs args, const int idx, const uint32_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u32(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for 64 bit unsigned integer value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetU64(VEDAargs args, const int idx, const uint64_t value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u64(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments by converting the heterogeneous memory address to raw device memory address.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.\n 
 *
 * Set the VEDA device memory address as an argument by converting the heterogeneous memory address to VEDA device memory address.
 */
VEDAresult vedaArgsSetHMEM(VEDAargs args, const int idx, const VEDAhmemptr value) {
	if(!args)			return VEDA_ERROR_INVALID_ARGS;
	if(!veo_is_ve_addr(value))	return VEDA_ERROR_INVALID_VALUE;
	CVEO(veo_args_set_u64(args, idx, (uint64_t)veo_get_hmem_addr((void*)value)));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments as a device memory location.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.\n 
 *
 * Set the VEDA device memory location as an argument
 */
VEDAresult vedaArgsSetPtr(VEDAargs args, const int idx, const VEDAdeviceptr value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_u64(args, idx, (uint64_t)VEDAptr<>(value).ptr()));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments as a host memory location.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.\n 
 *
 * Set the VEDA host memory location as an argument.
 */
VEDAresult vedaArgsSetVPtr(VEDAargs args, const int idx, const VEDAdeviceptr value) {
	return vedaArgsSetU64(args, idx, (uint64_t)value);
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for single precision floating point value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetF32(VEDAargs args, const int idx, const float value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_float(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments for double precision floating point value.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param value Value to be set for the VEDA function argument. 
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.
 */
VEDAresult vedaArgsSetF64(VEDAargs args, const int idx, const double value) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_double(args, idx, value));
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA function arguments to the buffer on stack.
 * @param args Handle to VEDA function argument.
 * @param idx VEDA function argument number
 * @param ptr Pointer to the buffer which needs to be copied to the VEDA device function stack.
 * @param intent Defines the intent of the buffer as below –\n 
 * VEDA_ARGS_INTENT_IN – If the buffer should be treated as the Input buffer to the VEDA device function.\n 
 * VEDA_ARGS_INTENT_OUT – If the buffer should be treated as the Output buffer to the VEDA device function and some output is expected.\n 
 * VEDA_ARGS_INTENT_INOUT – If the buffer should be treated as both Input and Output.\n 
 * @param size Size of the buffer.
 * @return VEDA_SUCCESS on Success
 * @return VEDA_ERROR_NOT_INITIALIZED If VEDA device is not initialized.
 * @return VEDA_ERROR_INVALID_ARGS If VEDA argument handler is not initialized.\n 
 *
 * Set the VEDA function argument to the buffer on stack.
 */
VEDAresult vedaArgsSetStack(VEDAargs args, const int idx, void* ptr, VEDAargs_intent intent, const size_t size) {
	if(!args)	return VEDA_ERROR_INVALID_ARGS;
	CVEO(veo_args_set_stack(args, (veo_args_intent)intent, idx, (char*)ptr, size));
	return VEDA_SUCCESS;
}
/** @} */
//------------------------------------------------------------------------------
} // extern "C"
