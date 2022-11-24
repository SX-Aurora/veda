#pragma once

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
struct Guard {
	inline	Guard	(void)	{	checkInitialized(); Semaphore::acquire();	}
	inline	~Guard	(void)	{	Semaphore::release();				}
};

//------------------------------------------------------------------------------
	}
}
