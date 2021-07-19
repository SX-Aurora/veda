#pragma once

#ifndef __cplusplus
#error "veda_device_omp.h only supports C++!"
#endif

#include <cstdint>
#include <omp.h>
#include <type_traits>
#include <functional>
#include <algorithm>

//------------------------------------------------------------------------------
template<typename T>
inline void veda_omp_schedule(T& min, T& max, const T cnt, const T vl = 1) {
	T nthreads	= omp_get_num_threads();
	T tx		= omp_get_thread_num();
	T smin 		= 0;
	T smax 		= 0;

	if(vl == 1) {
		/** This tries to distribute the work as equal as possible to the cores */
		T	batch	= cnt / nthreads;
		T	remain	= cnt % nthreads;
			min	= batch * tx + (tx < remain ? tx : remain);
			max	= min + batch + (tx < remain ? 1 : 0);
	} else {
		/** This tries to assign multiples of vl to the threads, even if some of
		 * them then don't have anything to do */
		T	tcnt	= (cnt + nthreads - 1) / nthreads;
			tcnt	= ((tcnt + vl - 1) / vl) * vl;
			min	= tx * tcnt;
			max	= std::min((tx+1) * tcnt, cnt);
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp_launch(const T cnt, F func) {
	#pragma omp parallel
	{
		T min, max;
		veda_omp_schedule(min, max, cnt);
		if(min < max)
			func(min, max);
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp(const T cnt, F func) {
	static_assert(std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, size_t>::value);
	static_assert(std::is_convertible<F, std::function<void(const T, const T)>>::value);
	T nthreads = omp_get_max_threads();

	if(nthreads == 1 || cnt == 1) {
		func(T(0), cnt);
	} else {
		veda_omp_launch(cnt, func);
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp_simd(const T cnt, F func, T vl = 0) {
	T nthreads = omp_get_max_threads();

	if(vl == 0) {
		if(sizeof(T) == 8)	vl = 256;
		else			vl = 512;
	}

	if(nthreads > T(1) && cnt > vl) {
		#pragma omp parallel
		{
			T min, max;
			veda_omp_schedule(min, max, cnt, vl);
			if(min < max)
				func(min, max);
		}
	} else {
		func(T(0), cnt);
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F, typename M, typename R>
inline M veda_omp_simd_reduce(const T cnt, F func, R reduction, M min, T vl = 0) {
	T nthreads = omp_get_max_threads();

	if(vl == 0) {
		if(sizeof(T) == 8)	vl = 256;
		else				vl = 512;
	}

	if(nthreads > T(1) && cnt > vl) {
		M result = min;
		#pragma omp parallel
		{
			T min, max;
			veda_omp_schedule(min, max, cnt, vl);
			if(min < max) {
				auto local = func(min, max);
				#pragma omp critical
				result = reduction(result, local);
			}
		}
		return result;
	}
	
	return func(T(0), cnt);
}

//------------------------------------------------------------------------------
