#pragma once

struct VEDAstack {
	void* const		ptr;
	const VEDAargs_intent	intent;
	const size_t		size;

	inline VEDAstack(void* _ptr, VEDAargs_intent _intent, size_t _size) :
		ptr(_ptr), intent(_intent), size(_size) {}
};

inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const VEDAhmemptr value)	{ return vedaArgsSetHMEM(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const VEDAdeviceptr value)	{ return vedaArgsSetVPtr(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const VEDAstack stack)	{ return vedaArgsSetStack(args, idx, stack.ptr, stack.intent, stack.size); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const double value)		{ return vedaArgsSetF64(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const float value)		{ return vedaArgsSetF32(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const int16_t value)	{ return vedaArgsSetI16(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const int32_t value)	{ return vedaArgsSetI32(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const int64_t value)	{ return vedaArgsSetI64(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const int8_t value)		{ return vedaArgsSetI8 (args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const uint16_t value)	{ return vedaArgsSetU16(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const uint32_t value)	{ return vedaArgsSetU32(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const uint64_t value)	{ return vedaArgsSetU64(args, idx, value); }
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const uint8_t value)	{ return vedaArgsSetU8 (args, idx, value); }

inline typename std::enable_if<!std::is_same<uint64_t, unsigned long long int>::value, VEDAresult>::type vedaArgsSet(VEDAargs args, const int idx, const unsigned long long int value) {
	return vedaArgsSetU64(args, idx, value);
}

template<typename T>
inline VEDAresult vedaArgsSet(VEDAargs args, const int idx, const VEDAptr<T>& value) {
	return vedaArgsSet(args, idx, (VEDAdeviceptr)value);
}

template<typename T>
inline typename std::enable_if<std::is_enum<T>::value, VEDAresult>::type vedaArgsSet(VEDAargs args, const int idx, const T value) {
	return vedaArgsSetI32(args, idx, (int32_t)value);
}

template<typename T>
inline typename std::enable_if<std::is_pointer<T>::value, VEDAresult>::type vedaArgsSet(VEDAargs args, const int idx, const T value) {
	return vedaArgsSetI64(args, idx, (int64_t)value);
}

template<typename T>
inline typename std::enable_if<!std::is_enum<T>::value && !std::is_pointer<T>::value, VEDAresult>::type vedaArgsSet(VEDAargs args, const int idx, const T value) {
	static_assert(std::is_same<T, bool>::value, "You can't pass values of type bool to vedaArgsSet, as sizeof(bool) is 1B on VH and 4B on VE.");
	return vedaArgsSetStack(args, idx, &value, VEDA_ARGS_INTENT_IN, sizeof(T));
}