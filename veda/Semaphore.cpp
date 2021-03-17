#include "veda.h"

namespace veda {
//------------------------------------------------------------------------------
std::atomic<uint32_t>	Semaphore::s_count;
std::atomic<int>	Semaphore::s_shutdown;

//------------------------------------------------------------------------------
void Semaphore::shutdown(void) {
	if(s_shutdown.load())
		throw VEDA_ERROR_UNKNOWN;
	s_shutdown++;
	// wait until all other requests have been dealt with
	while(s_count.load() != 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

//------------------------------------------------------------------------------
void Semaphore::acquire(void) {
	if(s_shutdown.load())
		throw VEDA_ERROR_SHUTTING_DOWN;
	s_count++;
}

//------------------------------------------------------------------------------
void Semaphore::release(void) {
	s_count--;
}

//------------------------------------------------------------------------------
}
