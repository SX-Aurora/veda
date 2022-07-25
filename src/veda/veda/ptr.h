#pragma once

//------------------------------------------------------------------------------
template<typename T>
inline size_t VEDAptr<T>::size(void) const {
	size_t s = 0;
	vedaMemSize(&s, m_ptr);
	return s;
}

//------------------------------------------------------------------------------
template<typename T>
inline T* VEDAptr<T>::ptr(void) const {
	T* p = 0;
	vedaMemPtr((void**)&p, m_ptr);
	return p;
}

//------------------------------------------------------------------------------
template<typename T>
inline typename VEDAptr<T>::Tuple VEDAptr<T>::ptrSize(void) const {
	T* p = 0;
	size_t s = 0;
	vedaMemPtrSize((void**)&p, &s, m_ptr);
	return {p, s};
}

//------------------------------------------------------------------------------
template<typename T>
inline T* HMEMptr<T>::ptr(void) const {
	T* p = 0;
	vedaHMemPtr((void**)&p, m_ptr);
	return p;
}

//------------------------------------------------------------------------------
