#include "veda_internal.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class Semaphore {
	std::atomic<uint32_t>	m_value;
	std::atomic<int>		m_shutdown;

public:
	VEDAresult shutdown(void) {
		if(m_shutdown.load())
			return VEDA_ERROR_UNKNOWN;
		m_shutdown++;
		// wait until all other requests have been dealt with
		while(m_value.load() != 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		return VEDA_SUCCESS;
	}

	VEDAresult acquire(void) {
		if(m_shutdown.load())
			return VEDA_ERROR_SHUTTING_DOWN;
		m_value++;
		return VEDA_SUCCESS;
	}

	void release(void) {
		m_value--;
	}
};

static Semaphore sem;

//------------------------------------------------------------------------------
VEDAresult vedaSemAcquire(void) {
	return sem.acquire();
}

//------------------------------------------------------------------------------
void vedaSemRelease(void) {
	return sem.release();
}

//------------------------------------------------------------------------------
VEDAresult vedaSemShutdown(void) {
	return sem.shutdown();
}

//------------------------------------------------------------------------------
static bool s_initialized = false;

//------------------------------------------------------------------------------
VEDAresult vedaSetInitialized(const bool value) {
	if(value && s_initialized)
		return VEDA_ERROR_ALREADY_INITIALIZED;
	else if(!value && !s_initialized)
		return VEDA_ERROR_NOT_INITIALIZED;
	s_initialized = value;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
VEDAresult vedaIsInitialized(void) {
	return s_initialized ? VEDA_SUCCESS : VEDA_ERROR_NOT_INITIALIZED;
}

//------------------------------------------------------------------------------
VEDAresult vedaVEOtoVEDA(const int err) {
	switch(err) {
		case VEO_COMMAND_OK:			return VEDA_SUCCESS;
		case VEO_COMMAND_EXCEPTION:		return VEDA_ERROR_VEO_COMMAND_EXCEPTION;
		case VEO_COMMAND_ERROR:			return VEDA_ERROR_VEO_COMMAND_ERROR;
		case VEO_COMMAND_UNFINISHED:	return VEDA_ERROR_VEO_COMMAND_UNFINISHED;
	}
	return VEDA_ERROR_VEO_COMMAND_UNKNOWN_ERROR;
}

//------------------------------------------------------------------------------