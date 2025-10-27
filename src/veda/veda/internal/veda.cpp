#include <veda/internal/api.h>

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
static bool		s_initialized	= false;
static bool		s_memTrace	= false;
static int		s_ompThreads	= 0;
static std::string	s_stdLib;

//------------------------------------------------------------------------------
bool		isMemTrace	(void) {	return s_memTrace;						}
const char*	stdLib		(void) {	return s_stdLib.c_str();					}
int		ompThreads	(void) {	return s_ompThreads;						}
void		checkInitialized(void) {	if(!s_initialized) VEDA_THROW(VEDA_ERROR_NOT_INITIALIZED);	}

//------------------------------------------------------------------------------
void setInitialized(const bool value) {
	if(value && s_initialized)
		VEDA_THROW(VEDA_ERROR_ALREADY_INITIALIZED);
	else if(!value && !s_initialized)
		VEDA_THROW(VEDA_ERROR_NOT_INITIALIZED);

	if(value) {
		// Init MemTrace -----------------------------------------------
		auto memTrace = std::getenv("VEDA_MEM_TRACE");
		s_memTrace = memTrace && std::atoi(memTrace);

		// Init OMP Threads --------------------------------------------
		if(auto env = std::getenv("VE_OMP_NUM_THREADS"))
			s_ompThreads = std::atoi(env);

	
		// Init StdLib Path --------------------------------------------
		// Stolen from: https://stackoverflow.com/questions/33151264/get-dynamic-library-directory-in-c-linux
		Dl_info dl_info;
		dladdr((void*)&veda::internal::setInitialized, &dl_info);
		std::string home(dl_info.dli_fname);
		auto pos = home.find_last_of('/');	assert(pos != std::string::npos);
		pos = home.find_last_of('/', pos-1);	assert(pos != std::string::npos);
		home.replace(pos, std::string::npos, "");

		s_stdLib = home;
		s_stdLib.append("/libve/libveda.vso");
	
		if(!std::getenv("VEORUN_BIN")) {
			#if WITH_SYSTEM_AVEO
				std::string veorun("/opt/nec/ve/veos");
			#else
				std::string veorun(home);
			#endif

			veorun.append("/libexec/aveorun");
			if(std::getenv("VEDA_FTRACE"))
				veorun.append("-ftrace");
			const auto arch = devices::architecture();
			if	(arch == 1)	veorun.append("_ve1");
			else if	(arch == 3)	veorun.append("_ve3");
			else VEDA_THROW(VEDA_ERROR_UNKNOWN);
			setenv("VEORUN_BIN", veorun.c_str(), 1);
		}

		L_TRACE("AVEORUN: %s", std::getenv("VEORUN_BIN"));
		L_TRACE("libveda: %s", s_stdLib.c_str());

		// Set VE_LD_LIBRARY_PATH if is not set ------------------------
		setenv("VE_LD_LIBRARY_PATH", ".", 0);
	}

	// Set Initialized
	Semaphore::init();
	s_initialized = value;
}

//------------------------------------------------------------------------------
VEDAresult VEOtoVEDA(const int err) {
	switch(err) {
		case VEO_COMMAND_OK:		return VEDA_SUCCESS;
		case VEO_COMMAND_EXCEPTION:	return VEDA_ERROR_VEO_COMMAND_EXCEPTION;
		case VEO_COMMAND_ERROR:		return VEDA_ERROR_VEO_COMMAND_ERROR;
		case VEO_COMMAND_UNFINISHED:	return VEDA_ERROR_VEO_COMMAND_UNFINISHED;
	}
	return VEDA_ERROR_VEO_COMMAND_UNKNOWN_ERROR;
}

//------------------------------------------------------------------------------
	}
}
