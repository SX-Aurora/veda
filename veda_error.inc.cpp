VEDAresult vedaGetErrorName(VEDAresult error, const char** pStr) {
	switch(error) {
		case VEDA_SUCCESS:									*pStr = "VEDA_SUCCESS";									return VEDA_SUCCESS;
		case VEDA_ERROR_ALREADY_INITIALIZED:				*pStr = "VEDA_ERROR_ALREADY_INITIALIZED";				return VEDA_SUCCESS;
		case VEDA_ERROR_SHUTTING_DOWN:						*pStr = "VEDA_ERROR_SHUTTING_DOWN";						return VEDA_SUCCESS;
		case VEDA_ERROR_CANNOT_CREATE_CONTEXT:				*pStr = "VEDA_ERROR_CANNOT_CREATE_CONTEXT";				return VEDA_SUCCESS;
		case VEDA_ERROR_CANNOT_CREATE_DEVICE:				*pStr = "VEDA_ERROR_CANNOT_CREATE_DEVICE";				return VEDA_SUCCESS;
		case VEDA_ERROR_CANNOT_CREATE_STREAM:				*pStr = "VEDA_ERROR_CANNOT_CREATE_STREAM";				return VEDA_SUCCESS;
		case VEDA_ERROR_CANNOT_POP_CONTEXT:					*pStr = "VEDA_ERROR_CANNOT_POP_CONTEXT";				return VEDA_SUCCESS;
		case VEDA_ERROR_FUNCTION_NOT_FOUND:					*pStr = "VEDA_ERROR_FUNCTION_NOT_FOUND";				return VEDA_SUCCESS;
		case VEDA_ERROR_INITIALIZING_DEVICE:				*pStr = "VEDA_ERROR_INITIALIZING_DEVICE";				return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_ARGS:						*pStr = "VEDA_ERROR_INVALID_ARGS";						return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_CONTEXT:					*pStr = "VEDA_ERROR_INVALID_CONTEXT";					return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_COREIDX:					*pStr = "VEDA_ERROR_INVALID_COREIDX";					return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_DEVICE:						*pStr = "VEDA_ERROR_INVALID_DEVICE";					return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_MODULE:						*pStr = "VEDA_ERROR_INVALID_MODULE";					return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_STREAM:						*pStr = "VEDA_ERROR_INVALID_STREAM";					return VEDA_SUCCESS;
		case VEDA_ERROR_INVALID_VALUE:						*pStr = "VEDA_ERROR_INVALID_VALUE";						return VEDA_SUCCESS;
		case VEDA_ERROR_MODULE_NOT_FOUND:					*pStr = "VEDA_ERROR_MODULE_NOT_FOUND";					return VEDA_SUCCESS;
		case VEDA_ERROR_NOT_IMPLEMENTED:					*pStr = "VEDA_ERROR_NOT_IMPLEMENTED";					return VEDA_SUCCESS;
		case VEDA_ERROR_NOT_INITIALIZED:					*pStr = "VEDA_ERROR_NOT_INITIALIZED";					return VEDA_SUCCESS;
		case VEDA_ERROR_NO_CONTEXT:							*pStr = "VEDA_ERROR_NO_CONTEXT";						return VEDA_SUCCESS;
		case VEDA_ERROR_NO_DEVICES_FOUND:					*pStr = "VEDA_ERROR_NO_DEVICES_FOUND";					return VEDA_SUCCESS;
		case VEDA_ERROR_NO_SENSOR_FILE:						*pStr = "VEDA_ERROR_NO_SENSOR_FILE";					return VEDA_SUCCESS;
		case VEDA_ERROR_OUT_OF_MEMORY:						*pStr = "VEDA_ERROR_OUT_OF_MEMORY";						return VEDA_SUCCESS;
		case VEDA_ERROR_UNABLE_TO_DETECT_DEVICES:			*pStr = "VEDA_ERROR_UNABLE_TO_DETECT_DEVICES";			return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_CONTEXT:					*pStr = "VEDA_ERROR_UNKNOWN_CONTEXT";					return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_DEVICE:						*pStr = "VEDA_ERROR_UNKNOWN_DEVICE";					return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_KERNEL:						*pStr = "VEDA_ERROR_UNKNOWN_KERNEL";					return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_MODULE:						*pStr = "VEDA_ERROR_UNKNOWN_MODULE";					return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_STREAM:						*pStr = "VEDA_ERROR_UNKNOWN_STREAM";					return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_VPTR:						*pStr = "VEDA_ERROR_UNKNOWN_VPTR";						return VEDA_SUCCESS;
		case VEDA_ERROR_UNKNOWN_PPTR:						*pStr = "VEDA_ERROR_UNKNOWN_PPTR";						return VEDA_SUCCESS;
		case VEDA_ERROR_VEDA_LD_LIBRARY_PATH_NOT_DEFINED:	*pStr = "VEDA_ERROR_VEDA_LD_LIBRARY_PATH_NOT_DEFINED";	return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_COMMAND_ERROR:					*pStr = "VEDA_ERROR_VEO_COMMAND_ERROR";					return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_COMMAND_EXCEPTION:				*pStr = "VEDA_ERROR_VEO_COMMAND_EXCEPTION";				return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_COMMAND_UNFINISHED:				*pStr = "VEDA_ERROR_VEO_COMMAND_UNFINISHED";			return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_COMMAND_UNKNOWN_ERROR:			*pStr = "VEDA_ERROR_VEO_COMMAND_UNKNOWN_ERROR";			return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_STATE_BLOCKED:					*pStr = "VEDA_ERROR_VEO_STATE_BLOCKED";					return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_STATE_RUNNING:					*pStr = "VEDA_ERROR_VEO_STATE_RUNNING";					return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_STATE_SYSCALL:					*pStr = "VEDA_ERROR_VEO_STATE_SYSCALL";					return VEDA_SUCCESS;
		case VEDA_ERROR_VEO_STATE_UNKNOWN:					*pStr = "VEDA_ERROR_VEO_STATE_UNKNOWN";					return VEDA_SUCCESS;
		case VEDA_ERROR_VPTR_ALREADY_ALLOCATED:				*pStr = "VEDA_ERROR_VPTR_ALREADY_ALLOCATED";			return VEDA_SUCCESS;
		case VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED:			*pStr = "VEDA_ERROR_OFFSETTED_VPTR_NOT_ALLOWED";		return VEDA_SUCCESS;
	}
	*pStr = "VEDA_ERROR_UNKNOWN";
	return VEDA_ERROR_UNKNOWN;
}

VEDAresult vedaGetErrorString(VEDAresult error, const char** pStr) {
	*pStr = "TODO";
	return VEDA_SUCCESS;
}
