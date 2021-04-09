#pragma once

template<typename T>
inline size_t VEDAptr<T>::size(void) const {
	size_t s = 0;
	vedaMemSize(&s, m_ptr);
	return s;
}

template<typename T>
inline T* VEDAptr<T>::ptr(void) const {
	T* p = 0;
	vedaMemPtr((void**)&p, m_ptr);
	return p;
}

template<typename T>
inline typename VEDAptr<T>::Tuple VEDAptr<T>::ptrSize(void) const {
	T* p = 0;
	size_t s = 0;
	vedaMemPtrSize((void**)&p, &s, m_ptr);
	return {p, s};
}

#ifndef __NEC__
template<typename T>
inline VEDAptr<T>::operator void*(void) const {
	return hmem();
}

template<typename T>
inline void* VEDAptr<T>::hmem(void) const {
	void* hmem = 0;
	vedaMemHMEM(&hmem, *this);
	return hmem;
}
#endif
