#pragma once

inline VEDAresult __vedaLaunchKernel(VEDAfunction func, VEDAstream stream, uint64_t* result, VEDAargs args, const int idx) {
	return vedaLaunchKernelEx(func, stream, args, 1, result);
}

template<typename T, typename... Args>
inline VEDAresult __vedaLaunchKernel(VEDAfunction func, VEDAstream stream, uint64_t* result, VEDAargs args, const int idx, const T value, Args... vargs) {
	CVEDA(vedaArgsSet(args, idx, value));
	return __vedaLaunchKernel(func, stream, result, args, idx+1, vargs...);
}

template<typename... Args>
inline VEDAresult vedaLaunchKernel(VEDAfunction func, VEDAstream stream, Args... vargs) {
	VEDAargs args = 0;
	CVEDA(vedaArgsCreate(&args));
	return __vedaLaunchKernel(func, stream, 0, args, 0, vargs...);
}

template<typename... Args>
inline VEDAresult vedaLaunchKernelEx(VEDAfunction func, VEDAstream stream, uint64_t* result, Args... vargs) {
	VEDAargs args = 0;
	CVEDA(vedaArgsCreate(&args));
	return __vedaLaunchKernel(func, stream, result, args, 0, vargs...);
}