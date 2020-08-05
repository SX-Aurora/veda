#include "veda_internal.h"

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

//------------------------------------------------------------------------------
static bool s_initialized	= false;
static int	s_ompThreads	= 0;
static Semaphore sem;

//------------------------------------------------------------------------------
VEDAresult	vedaIsInitialized	(void) {	return s_initialized ? VEDA_SUCCESS : VEDA_ERROR_NOT_INITIALIZED;	}
VEDAresult	vedaSemAcquire		(void) {	return sem.acquire();												}
VEDAresult	vedaSemShutdown		(void) {	return sem.shutdown();												}
int			vedaOmpThreads		(void) {	return s_ompThreads;												}
void		vedaSemRelease		(void) {	return sem.release();												}

//------------------------------------------------------------------------------
VEDAresult vedaSetInitialized(const bool value) {
	if(value && s_initialized)
		return VEDA_ERROR_ALREADY_INITIALIZED;
	else if(!value && !s_initialized)
		return VEDA_ERROR_NOT_INITIALIZED;

	auto env = std::getenv("VE_OMP_NUM_THREADS");
	if(env)
		s_ompThreads = std::atoi(env);

	s_initialized = value;
	return VEDA_SUCCESS;
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