#pragma once

//------------------------------------------------------------------------------
// vedaMemsetD* C++ interface
//------------------------------------------------------------------------------
#define VEDA_ENABLE(T, BITS) inline typename std::enable_if<!std::is_same<T, uint##BITS##_t>::value && (sizeof(T)*8) == BITS, VEDAresult>::type

#define VEDA_MEMSET(BITS)\
template<typename T> VEDA_ENABLE(T, BITS) vedaMemsetD##BITS(VEDAdeviceptr dst, T v, size_t N) {\
	return vedaMemsetD##BITS(dst, *(uint##BITS##_t*)&v, N);\
}\
template<typename T> VEDA_ENABLE(T, BITS) vedaMemsetD##BITS##Async(VEDAdeviceptr dst, T v, size_t N, VEDAstream stream) {\
	return vedaMemsetD##BITS##Async(dst, *(uint##BITS##_t*)&v, N, stream);\
}\
template<typename T> VEDA_ENABLE(T, BITS) vedaMemsetD2D##BITS(VEDAdeviceptr dst, size_t dstPitch, T v, size_t Width, size_t Height) {\
	return vedaMemsetD2D##BITS(dst, dstPitch, *(uint##BITS##_t*)&v, Width, Height);\
}\
template<typename T> VEDA_ENABLE(T, BITS) vedaMemsetD2D##BITS##Async(VEDAdeviceptr dst, size_t dstPitch, T v, size_t Width, size_t Height, VEDAstream stream) {\
	return vedaMemsetD2D##BITS##Async(dst, dstPitch, *(uint##BITS##_t*)&v, Width, Height, stream);\
}

VEDA_MEMSET(8)
VEDA_MEMSET(16)
VEDA_MEMSET(32)
VEDA_MEMSET(64)
#undef VEDA_MEMSET

template<typename T> VEDA_ENABLE(T, 64) vedaMemsetD128(VEDAdeviceptr dst, T x, T y, size_t N) {
	return vedaMemsetD128(dst, *(uint64_t*)&x, *(uint64_t*)&y, N);
}
template<typename T> VEDA_ENABLE(T, 64) vedaMemsetD128Async(VEDAdeviceptr dst, T x, T y, size_t N, VEDAstream stream) {
	return vedaMemsetD128Async(dst, *(uint64_t*)&x, *(uint64_t*)&y, N, stream);
}
template<typename T> VEDA_ENABLE(T, 64) vedaMemsetD2D128(VEDAdeviceptr dst, size_t dstPitch, T x, T y, size_t Width, size_t Height) {
	return vedaMemsetD2D128(dst, dstPitch, *(uint64_t*)&x, *(uint64_t*)&y, Width, Height);
}
template<typename T> VEDA_ENABLE(T, 64) vedaMemsetD2D128Async(VEDAdeviceptr dst, size_t dstPitch, T x, T y, size_t Width, size_t Height, VEDAstream stream) {
	return vedaMemsetD2D128Async(dst, dstPitch, *(uint64_t*)&x, *(uint64_t*)&y, Width, Height, stream);
}

#undef VEDA_ENABLE