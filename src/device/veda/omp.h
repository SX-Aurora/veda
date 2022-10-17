#pragma once

#ifdef __cplusplus
#include <functional>
#include <type_traits>
#include <cassert>
#include <algorithm>

template<typename T>	inline	size_t veda_omp_vlen		(void)	{ return 512; }
template<>		inline	size_t veda_omp_vlen<int64_t>	(void)	{ return 256; }
template<>		inline	size_t veda_omp_vlen<uint64_t>	(void)	{ return 256; }
template<>		inline	size_t veda_omp_vlen<double>	(void)	{ return 256; }

//------------------------------------------------------------------------------
// WITH OPENMP
//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp_check(void) {
	static_assert(std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, size_t>::value);
	static_assert(std::is_convertible<F, std::function<void(const T, const T)>>::value);
}

#ifdef _OPENMP
#include <omp.h>

//------------------------------------------------------------------------------
template<typename T>
class veda_omp_simd_schedule {
	const	T	m_cnt;
			T	m_step;

public:
	inline veda_omp_simd_schedule(const T cnt, const T vl) :
		m_cnt	(cnt),
		m_step	(0)
	{
		assert(vl > 1);
		T nthreads = omp_get_max_threads();
		/** This tries to assign multiples of vl to the threads, even if some of
		 * them then don't have anything to do */
		m_step	= (cnt + nthreads - 1) / nthreads;
		m_step	= ((m_step + vl - 1) / vl) * vl;
	}

	inline T cnt	(void) const {	return m_cnt;	}
	inline T step	(void) const {	return m_step;	}

	inline std::tuple<T, T> operator()(void) const {
		T tx	= omp_get_thread_num();
		T min	= tx * step();
		T max	= std::min(min + step(), cnt());
		return std::make_tuple(min, max);
	}
};

//------------------------------------------------------------------------------
template<typename T>
class veda_omp_schedule {
	T	m_batch;
	T	m_remain;

public:
	inline veda_omp_schedule(const T cnt) :
		m_batch	(0),
		m_remain(0)
	{
		T nthreads = omp_get_max_threads();
		/** This tries to distribute the work as equal as possible to the cores */
		m_batch		= cnt / nthreads;
		m_remain	= cnt % nthreads;
	}

	inline std::tuple<T, T> operator()(void) const {
		T tx	= omp_get_thread_num();
		T min	= m_batch * tx + (tx < m_remain ? tx : m_remain);
		T max	= min + m_batch + (tx < m_remain ? 1 : 0);
		return std::make_tuple(min, max);
	}
};

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp(const T cnt, F func) {
	if(__builtin_expect((cnt <= 0),0))
		return;

	veda_omp_check<T, F>();
	T nthreads = omp_get_max_threads();

	if(cnt == 1 || nthreads == 1) {
		func(T(0), cnt);
	} else {
		veda_omp_schedule<T> schedule(cnt);
		#pragma omp parallel
		{
			T min, max;
			std::tie(min, max) = schedule();
			if(min < max)
				func(min, max);
		}
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp_simd(const T cnt, F func, const T vl = 256) {
	if(__builtin_expect((cnt <= 0),0))
		return;

	veda_omp_check<T, F>();
	T nthreads = omp_get_max_threads();

	if(nthreads > 1 && cnt > vl) {
		veda_omp_simd_schedule<T> schedule(cnt, vl);
		#pragma omp parallel
		{
			T min, max;
			std::tie(min, max) = schedule();
			if(min < max)
				func(min, max);
		}
	} else {
		func(T(0), cnt);
	}
}

//------------------------------------------------------------------------------
template<typename T, typename F, typename M, typename R>
inline M veda_omp_simd_reduce(const T cnt, F func, R reduction, M min, const T vl = 256) {
	if(__builtin_expect((cnt <= 0),0))
		return;

	veda_omp_check<T, F>();
	T nthreads = omp_get_max_threads();

	if(nthreads > 1 && cnt > vl) {
		veda_omp_simd_schedule<T> schedule(cnt, vl);

		M result = min;
		#pragma omp parallel
		{
			T min, max;
			std::tie(min, max) = schedule();
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

#else
//------------------------------------------------------------------------------
// Without OMP
//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp(const T cnt, F func) {
	if(__builtin_expect((cnt <= 0),0))
		return;

	veda_omp_check<T, F>();
	func(T(0), cnt);
}

//------------------------------------------------------------------------------
template<typename T, typename F>
inline void veda_omp_simd(const T cnt, F func, const T vl = 256) {
	if(__builtin_expect((cnt <= 0),0))
		return;

	veda_omp_check<T, F>();
	func(T(0), cnt);
}

//------------------------------------------------------------------------------
template<typename T, typename F, typename M, typename R>
inline M veda_omp_simd_reduce(const T cnt, const T vl, F func, R reduction, M min) {
	if(__builtin_expect((cnt <= 0),0))
		return;
		
	veda_omp_check<T, F>();
	return func(T(0), cnt);
}
#endif
#else
	#error "VEDA OMP API requires C++"
#endif
