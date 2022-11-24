#pragma once

namespace veda {
	namespace internal {
		namespace profiler {
//------------------------------------------------------------------------------
VEDAprofiler_callback	getCallback	(void);
VEDAprofiler_type	funcType	(void* func);
uint64_t		callbackEnter	(void* data);
uint64_t		callbackExit	(void* data);
void			setCallback	(VEDAprofiler_callback callback);
void			shutdown	(void);

//------------------------------------------------------------------------------
template<typename... Args>
class Data final : public VEDAprofiler_data {
	static constexpr size_t sum(const size_t A) {
		return A;
	}

	template<typename... TArgs>
	static constexpr size_t sum(const size_t A, TArgs... args) {
		return A + sum(args...);
	}

	template<typename T>
	inline size_t copy(const int mask, char* ptr, T arg) {
		if(mask & 1) {
			memcpy(ptr, &arg, sizeof(T));
			return sizeof(T);
		}
		return 0;
	}

	template<typename T, typename... TArgs>
	inline void copy(const int mask, char* ptr, T arg, TArgs... args) {
		if(mask == 0)
			return;
		
		ptr += copy(mask, ptr, arg);
		copy(mask >> 1, ptr, args...);
	}

public:
	char args[sum(sizeof(Args)...)];
	
	inline Data(const VEDAprofiler_type type, const int deviceId, const int streamId, Args... args_) :
		VEDAprofiler_data	(type, deviceId, streamId),
		args			{}
	{
		const char* str;
		vedaProfilerTypeName(type, &str);
		auto mask = int(type) >> 24;
		copy(mask, args, args_...);
	}
};

//------------------------------------------------------------------------------
template<typename F, typename... Args>
inline VEDAprofiler_data* data(const int deviceId, const int streamId, F func, Args... args) {
	if(!getCallback())
		return 0;

	using DATA	= Data<Args...>; // TODO: this allocates too much memory
	auto type	= funcType((void*)func);
	auto data	= (DATA*)malloc(sizeof(DATA));
	new (data) DATA(type, deviceId, streamId, args...);
	return data;
}

//------------------------------------------------------------------------------
inline int hmemDevice(void) {
	return -1;
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
inline int hmemDevice(T arg, Args... args) {
	if constexpr (std::is_same<T, VEDAhmemptr>::value || std::is_same<T, veo_proc_handle*>::value)
		if(auto dev = devices::tryGet(arg))
			return dev->vedaId();
	return hmemDevice(args...);
}

//------------------------------------------------------------------------------
template<typename F, typename... Args>
inline typename std::enable_if<is_function_ptr<F>::value, typename return_type<F>::type>::type wrap(F func, Args... args) {
	if(getCallback()) {
		auto d = data(hmemDevice(args...), 0, func, args...);
		callbackEnter(d);
		auto res = func(args...);
		callbackExit(d);
		return res;
	}
	return func(args...);
}

//------------------------------------------------------------------------------
		}
	}
}