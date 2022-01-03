#include "veda/veda.hpp"

extern "C" {
//------------------------------------------------------------------------------
#include "veda_error.inc.cpp"

//------------------------------------------------------------------------------
/**
 * @brief Initialize the VEDA driver API library.
 * @param flags Initialization flags for VEDA library.
 * @retval VEDA_SUCCESS on Success
 * Initializes the driver API and must be called before any other function of
 * the VEDA driver API. Currently, the Flags parameter must be 0. If vedaInit()
 * has not been called, any function from the driver API will return 
 * VEDA_ERROR_NOT_INITIALIZED.
 */
VEDAresult vedaInit(uint32_t Flags) {
	// Flags needs to be 0
	if(Flags != 0)
		return VEDA_ERROR_INVALID_VALUE;

	TRY(
		veda::setInitialized(true);
		veda::Devices::init();
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Releases the VEDA driver API library gracefully.
 * @retval VEDA_SUCCESS on Success
 * This function should be called from the hybrid program in order to perform the
 * proper cleanup of the VEDA driver library.
 */
VEDAresult vedaExit(void) {
	TRY(
		veda::Semaphore::shutdown();
		veda::setInitialized(false);
		veda::Devices::shutdown();
		veda::Contexts::shutdown();
	)
}

//------------------------------------------------------------------------------
/**
 * @brief Return the latest AVEO version supported bt VEDA.
 * @param str Pointer to hold the AVEO version.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 */
VEDAresult vedaDriverGetVersion(const char** str) {
	GUARDED(*str = veo_version_string();)
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a VEDA device function f on the VEDA device.
 * @param f Handle to VEDA Device function to launch.
 * @param stream Stream Identifier.
 * @param args Handle to the VEDA device parameters
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * VEDA device function arguements can be allocated and set through the vedaArgsCreate
 * and vedaArgsSet* APIs.
 */ 
VEDAresult vedaLaunchKernel(VEDAfunction f, VEDAstream stream, VEDAargs args) {
	return vedaLaunchKernelEx(f, stream, args, 0, 0);
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a VEDA device function f on the VEDA device with flexibility.
 * @param f Handle to VEDA Device function to launch.
 * @param stream Stream Identifier.
 * @param args Handle to the VEDA device parameters.
 * @param destroyArgs Set 1 if the VEDA arguement needs to be destroyed after VEDA 
 * device function is called else 0.
 * @param checkResult Set 1 if the return value is required from the VEDA device
 * function else 0.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 */
VEDAresult vedaLaunchKernelEx(VEDAfunction f, VEDAstream stream, VEDAargs args, const int destroyArgs, const int checkResult) {
	GUARDED(veda::Contexts::current()->call(f, stream, args, destroyArgs != 0, checkResult);)
}

//------------------------------------------------------------------------------
/**
 * @brief Enqueues a host function call in a stream.
 * @param stream Stream Identifier.
 * @param fn The function to call once preceding stream operations are complete.
 * @param userData User-specified data to be passed to the function.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Enqueues a host function to run in a stream. The function will be called after
 * currently enqueued work and will block work added after it.
 */ 
VEDAresult vedaLaunchHostFunc(VEDAstream stream, VEDAhost_function fn, void* userData) {
	GUARDED(veda::Contexts::current()->call(fn, userData, stream);)
}

//------------------------------------------------------------------------------
} // extern "C"