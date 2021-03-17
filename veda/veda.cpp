#include "veda.h"

namespace veda {
//------------------------------------------------------------------------------
static bool		s_initialized	= false;
static int		s_ompThreads	= 0;
static std::string	s_stdLib;

//------------------------------------------------------------------------------
const char*	stdLib		(void) {	return s_stdLib.c_str();				}
int		ompThreads	(void) {	return s_ompThreads;					}
void		checkInitialized(void) {	if(!s_initialized) throw VEDA_ERROR_NOT_INITIALIZED;	}

//------------------------------------------------------------------------------
void setInitialized(const bool value) {
	if(value && s_initialized)
		throw VEDA_ERROR_ALREADY_INITIALIZED;
	else if(!value && !s_initialized)
		throw VEDA_ERROR_NOT_INITIALIZED;

	if(value) {
		// Init OMP Threads ----------------------------------------------------
		auto env = std::getenv("VE_OMP_NUM_THREADS");
		if(env)
			s_ompThreads = std::atoi(env);

		// Init StdLib Path ----------------------------------------------------
		// Stolen from: https://stackoverflow.com/questions/33151264/get-dynamic-library-directory-in-c-linux
		Dl_info dl_info;
		dladdr((void*)&veda::setInitialized, &dl_info);
		std::string home(dl_info.dli_fname);
		auto pos = home.find_last_of('/');	assert(pos != std::string::npos);
		pos = home.find_last_of('/', pos-1);	assert(pos != std::string::npos);
		home.replace(pos, std::string::npos, "");

		// Set Paths -----------------------------------------------------------
		std::string veorun(home);
		veorun.append("/libexec/aveorun");
		setenv("VEORUN_BIN", veorun.c_str(), 1);

		s_stdLib.append(home);
		s_stdLib.append("/libve/libveda.vso");

		// Set VE_LD_LIBRARY_PATH if is not set --------------------------------
		setenv("VE_LD_LIBRARY_PATH", ".", 0);
	}

	// Set Initialized
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
