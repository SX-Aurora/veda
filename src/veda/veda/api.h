#pragma once

#ifdef __ve__
#error Inclusion of veda.h in device code detected! veda.h is only meant for host code, use veda_device.h instead. If you want to use VEDA data types in a shared header file, use veda_types.h.
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

//------------------------------------------------------------------------------
// Pure C interface
//------------------------------------------------------------------------------
#include <veda/enums.h>
#include <veda/types.h>
#include <veda/macros.h>
#include <veda/version.h>

#ifdef __cplusplus
extern "C" {
#endif
// implementation of VEDA API functions
/**
 * \defgroup vedaapi VEDA API
 *
 * To use VEDA API functions, include "veda.h" header.
 */
/** @{ */
//------------------------------------------------------------------------------
// Common API ------------------------------------------------------------------
VEDAresult	vedaArgsCreate			(VEDAargs* args);
VEDAresult	vedaArgsDestroy			(VEDAargs args);
VEDAresult	vedaArgsSetF32			(VEDAargs args, const int idx, const float value);
VEDAresult	vedaArgsSetF64			(VEDAargs args, const int idx, const double value);
VEDAresult	vedaArgsSetI16			(VEDAargs args, const int idx, const int16_t value);
VEDAresult	vedaArgsSetI32			(VEDAargs args, const int idx, const int32_t value);
VEDAresult	vedaArgsSetI64			(VEDAargs args, const int idx, const int64_t value);
VEDAresult	vedaArgsSetI8			(VEDAargs args, const int idx, const int8_t value);
VEDAresult	vedaArgsSetStack		(VEDAargs args, const int idx, void* ptr, VEDAargs_intent intent, const size_t size);
VEDAresult	vedaArgsSetU16			(VEDAargs args, const int idx, const uint16_t value);
VEDAresult	vedaArgsSetU32			(VEDAargs args, const int idx, const uint32_t value);
VEDAresult	vedaArgsSetU64			(VEDAargs args, const int idx, const uint64_t value);
VEDAresult	vedaArgsSetU8			(VEDAargs args, const int idx, const uint8_t value);
VEDAresult	vedaCtxCreate			(VEDAcontext* pctx, int mode, VEDAdevice dev);
VEDAresult	vedaCtxDestroy			(VEDAcontext ctx);
VEDAresult	vedaCtxGet			(VEDAcontext* ctx, const VEDAdevice device);
VEDAresult	vedaCtxGetApiVersion		(VEDAcontext ctx, uint32_t* version);
VEDAresult	vedaCtxGetCurrent		(VEDAcontext* pctx);
VEDAresult	vedaCtxGetDevice		(VEDAdevice* device);
VEDAresult	vedaCtxPopCurrent		(VEDAcontext* pctx);
VEDAresult	vedaCtxPushCurrent		(VEDAcontext ctx);
VEDAresult	vedaCtxSetCurrent		(VEDAcontext ctx);
VEDAresult	vedaCtxStreamCnt		(int* cnt);
VEDAresult	vedaCtxSynchronize		(void);
VEDAresult	vedaDeviceDistance		(float* distance, VEDAdevice devA, VEDAdevice devB);
VEDAresult	vedaDeviceGet			(VEDAdevice* device, int ordinal);
VEDAresult	vedaDeviceGetAVEOId		(int* id, VEDAdevice dev);
VEDAresult	vedaDeviceGetAttribute		(int* pi, VEDAdevice_attribute attrib, VEDAdevice dev);
VEDAresult	vedaDeviceGetCount		(int* count);
VEDAresult	vedaDeviceGetCurrent		(float* current, VEDAdevice dev);
VEDAresult	vedaDeviceGetCurrentEdge	(float* current, VEDAdevice dev);
VEDAresult	vedaDeviceGetNUMAId		(int* id, VEDAdevice dev);
VEDAresult	vedaDeviceGetName 		(char* name, int len, VEDAdevice dev);
VEDAresult	vedaDeviceGetPhysicalId		(int* id, VEDAdevice dev);
VEDAresult	vedaDeviceGetPower		(float* power, VEDAdevice dev);
VEDAresult	vedaDeviceGetTemp		(float* tempC, const int coreIdx, VEDAdevice dev);
VEDAresult	vedaDeviceGetVoltage		(float* voltage, VEDAdevice dev);
VEDAresult	vedaDeviceGetVoltageEdge	(float* voltage, VEDAdevice dev);
VEDAresult	vedaDevicePrimaryCtxGetState	(VEDAdevice dev, uint32_t* flags, int* active);
VEDAresult	vedaDevicePrimaryCtxRelease	(VEDAdevice dev);
VEDAresult	vedaDevicePrimaryCtxReset	(VEDAdevice dev);
VEDAresult	vedaDevicePrimaryCtxRetain	(VEDAcontext* pctx, VEDAdevice dev);
VEDAresult	vedaDevicePrimaryCtxSetFlags	(VEDAdevice dev, uint32_t flags);
VEDAresult	vedaDeviceTotalMem		(size_t* bytes, VEDAdevice dev);
VEDAresult	vedaDriverGetVersion 		(const char** str);
VEDAresult	vedaExit			(void);
VEDAresult	vedaGetErrorName		(VEDAresult error, const char** pStr);
VEDAresult	vedaGetErrorString		(VEDAresult error, const char** pStr);
//------------------------------------------------------------------------------
/**
 * @brief Return the latest VEDA version.
 * @param str Pointer to hold the VEDA version.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 */
VEDAresult	vedaGetVersion			(const char** str);
VEDAresult	vedaGetArchitecture		(int* arch);
VEDAresult	vedaInit			(uint32_t Flags);
VEDAresult	vedaLaunchHostFunc		(VEDAstream stream, VEDAhost_function fn, void* userData);
VEDAresult	vedaLaunchHostFuncEx		(VEDAstream stream, VEDAhost_function fn, void* userData, uint64_t* result);
VEDAresult	vedaLaunchKernel		(VEDAfunction f, VEDAstream stream, VEDAargs);
VEDAresult	vedaLaunchKernelEx		(VEDAfunction f, VEDAstream stream, VEDAargs, const int destroyArgs, uint64_t* result);
VEDAresult	vedaMemAllocHost		(void** pp, size_t bytesiz);
VEDAresult	vedaMemFreeHost			(void* ptr);
VEDAresult	vedaMemGetInfo			(size_t* free, size_t* total);
VEDAresult	vedaModuleGetFunction		(VEDAfunction* hfunc, VEDAmodule hmod, const char* name);
VEDAresult	vedaModuleLoad			(VEDAmodule* module, const char* fname);
VEDAresult	vedaModuleUnload		(VEDAmodule hmod);
VEDAresult	vedaStreamAddCallback		(VEDAstream stream, VEDAstream_callback callback, void* userData, unsigned int flags);
VEDAresult	vedaStreamGetFlags		(VEDAstream hStream, uint32_t* flags);
VEDAresult	vedaStreamQuery			(VEDAstream hStream);
VEDAresult	vedaStreamSynchronize		(VEDAstream hStream);

// VEDAhmemptr API -------------------------------------------------------------
VEDAresult	vedaArgsSetHMEM			(VEDAargs args, const int idx, const VEDAhmemptr value);
VEDAresult	vedaHMemAlloc			(VEDAhmemptr* ptr, size_t size);
VEDAresult	vedaHMemFree			(VEDAhmemptr ptr);
VEDAresult	vedaHMemPtr			(void** ptr, VEDAhmemptr hptr);
VEDAresult	vedaHMemcpy			(void* dst, void* src, size_t ByteCount);
VEDAresult	vedaHMemcpyDtoX			(void* dst, VEDAdeviceptr src, size_t ByteCount);
VEDAresult	vedaHMemcpyDtoXAsync		(void* dst, VEDAdeviceptr src, size_t ByteCount, VEDAstream stream);
VEDAresult	vedaHMemcpyXtoD			(VEDAdeviceptr dst, void* src, size_t ByteCount);
VEDAresult	vedaHMemcpyXtoDAsync		(VEDAdeviceptr dst, void* src, size_t ByteCount, VEDAstream stream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult	vedaHMemsetD128			(VEDAhmemptr dstDevice, uint64_t x, uint64_t y, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @param hStream Stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult	vedaHMemsetD128Async		(VEDAhmemptr dstDevice, uint64_t x, uint64_t y, size_t N, VEDAstream hStream);
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 16-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD16			(VEDAhmemptr dstDevice, uint16_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 16-bit values to the specified value value.
 */
VEDAresult	vedaHMemsetD16Async		(VEDAhmemptr dstDevice, uint16_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch      memory pitch
 * @param x     first 64bit value
 * @param y     second 64bit value
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult	vedaHMemsetD2D128		(VEDAhmemptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch      memory pitch
 * @param x     first 64bit value
 * @param y     second 64bit value
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @param hStream stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult	vedaHMemsetD2D128Async		(VEDAhmemptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D16		(VEDAhmemptr dstDevice, size_t dstPitch, uint16_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D16Async		(VEDAhmemptr dstDevice, size_t dstPitch, uint16_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 32-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D32		(VEDAhmemptr dstDevice, size_t dstPitch, uint32_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 32-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D32Async		(VEDAhmemptr dstDevice, size_t dstPitch, uint32_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  memory pitch.
 * @param value 32bit value.
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD2D64		(VEDAhmemptr dstDevice, size_t dstPitch, uint64_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the
 * number of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D64Async		(VEDAhmemptr dstDevice, size_t dstPitch, uint64_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 8-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D8			(VEDAhmemptr dstDevice, size_t dstPitch, uint8_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 8-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaHMemsetD2D8Async		(VEDAhmemptr dstDevice, size_t dstPitch, uint8_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 32-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD32			(VEDAhmemptr dstDevice, uint32_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 32-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD32Async		(VEDAhmemptr dstDevice, uint32_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD64			(VEDAhmemptr dstDevice, uint64_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD64Async		(VEDAhmemptr dstDevice, uint64_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 8-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD8			(VEDAhmemptr dstDevice, uint8_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice HMEM Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 8-bit values to the specified value "value".
 */
VEDAresult	vedaHMemsetD8Async		(VEDAhmemptr dstDevice, uint8_t value, size_t N, VEDAstream hStream);

// VEDAdeviceptr API -----------------------------------------------------------
VEDAresult	vedaArgsSetPtr			(VEDAargs args, const int idx, const VEDAdeviceptr value);
VEDAresult	vedaArgsSetVPtr			(VEDAargs args, const int idx, const VEDAdeviceptr value);
VEDAresult	vedaMemAlloc			(VEDAdeviceptr* ptr, size_t size);
VEDAresult	vedaMemAllocAsync		(VEDAdeviceptr* ptr, size_t size, VEDAstream stream);
VEDAresult	vedaMemAllocOverrideOnce	(VEDAdeviceptr ptr);
VEDAresult	vedaMemAllocPitch		(VEDAdeviceptr* ptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte);
VEDAresult	vedaMemAllocPitchAsync		(VEDAdeviceptr* ptr, size_t* pPitch, size_t WidthInBytes, size_t Height, uint32_t ElementSizeByte, VEDAstream stream);
VEDAresult	vedaMemFree			(VEDAdeviceptr ptr);
VEDAresult	vedaMemFreeAsync		(VEDAdeviceptr ptr, VEDAstream stream);
VEDAresult	vedaMemGetAddressRange		(VEDAdeviceptr* base, size_t* size, VEDAdeviceptr ptr);
VEDAresult	vedaMemGetDevice		(VEDAdevice* dev, VEDAdeviceptr ptr);
VEDAresult	vedaMemPtr			(void** ptr, VEDAdeviceptr vptr);
VEDAresult	vedaMemPtrSize			(void** ptr, size_t* size, VEDAdeviceptr vptr);
VEDAresult	vedaMemRelease			(VEDAdeviceptr ptr);
VEDAresult	vedaMemReport			(void);
VEDAresult	vedaMemSize			(size_t* size, VEDAdeviceptr ptr);
VEDAresult	vedaMemSwap			(VEDAdeviceptr A, VEDAdeviceptr B);
VEDAresult	vedaMemSwapAsync		(VEDAdeviceptr A, VEDAdeviceptr B, VEDAstream hStream);
VEDAresult	vedaMemcpy			(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount);
VEDAresult	vedaMemcpyAsync			(VEDAdeviceptr dst, VEDAdeviceptr src, size_t ByteCount, VEDAstream hStream);
VEDAresult	vedaMemcpyDtoD			(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t ByteCount);
VEDAresult	vedaMemcpyDtoDAsync		(VEDAdeviceptr dstDevice, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream);
VEDAresult	vedaMemcpyDtoH			(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount);
VEDAresult	vedaMemcpyDtoHAsync		(void* dstHost, VEDAdeviceptr srcDevice, size_t ByteCount, VEDAstream hStream);
VEDAresult	vedaMemcpyHtoD			(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount);
VEDAresult	vedaMemcpyHtoDAsync		(VEDAdeviceptr dstDevice, const void* srcHost, size_t ByteCount, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified value us.
 */
VEDAresult	vedaMemsetD128			(VEDAdeviceptr dstDevice, uint64_t x, uint64_t y, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param x first 64bit value
 * @param y second 64bit value
 * @param N Number of Elements.
 * @param hStream Stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified values x and y.
 */
VEDAresult	vedaMemsetD128Async		(VEDAdeviceptr dstDevice, uint64_t x, uint64_t y, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 16-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD16			(VEDAdeviceptr dstDevice, uint16_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 16-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD16Async		(VEDAdeviceptr dstDevice, uint16_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch	memory pitch
 * @param x	first 64bit value
 * @param y	second 64bit value
 * @param Width	Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified values x and y.
 */
VEDAresult	vedaMemsetD2D128		(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch      memory pitch
 * @param x     first 64bit value
 * @param y     second 64bit value
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @param hStream stream
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 128-bit values to the specified values x and y.
 */
VEDAresult	vedaMemsetD2D128Async		(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t x, uint64_t y, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D16			(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D16Async		(VEDAdeviceptr dstDevice, size_t dstPitch, uint16_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 32-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D32			(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 32-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D32Async		(VEDAdeviceptr dstDevice, size_t dstPitch, uint32_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  memory pitch.
 * @param value 32bit value.
 * @param Width Width of 2D memset.
 * @param Height Height of 2D memset.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD2D64			(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 16-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the 
 * number of bytes between each row.
 */
VEDAresult	vedaMemsetD2D64Async		(VEDAdeviceptr dstDevice, size_t dstPitch, uint64_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 8-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D8			(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t value, size_t Width, size_t Height);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param dstPitch  Pitch of destination device pointer.
 * @param value Value to set.
 * @param Width Width of row.
 * @param Height Number of rows.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the 2D memory range of Width 8-bit values to the specified value "value".
 * Height specifies the number of rows to set, and dstPitch specifies the number
 * of bytes between each row.
 */
VEDAresult	vedaMemsetD2D8Async		(VEDAdeviceptr dstDevice, size_t dstPitch, uint8_t value, size_t Width, size_t Height, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 32-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD32			(VEDAdeviceptr dstDevice, uint32_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 32-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD32Async		(VEDAdeviceptr dstDevice, uint32_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD64			(VEDAdeviceptr dstDevice, uint64_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 64-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD64Async		(VEDAdeviceptr dstDevice, uint64_t value, size_t N, VEDAstream hStream);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 8-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD8			(VEDAdeviceptr dstDevice, uint8_t value, size_t N);
//------------------------------------------------------------------------------
/**
 * @brief Initializes device memory.
 * @param dstDevice Destination device pointer.
 * @param value Value to set.
 * @param N Number of Elements.
 * @param hStream The stream establishing the stream ordering contract.
 * @retval VEDA_SUCCESS on Success
 * @retval VEDA_ERROR_NOT_INITIALIZED VEDA library not initialized
 * @retval VEDA_ERROR_INVALID_DEVICE VEDA device id is not valid.
 * @retval VEDA_ERROR_UNKNOWN_CONTEXT VEDA context is not set for the calling thread.
 * @retval VEDA_ERROR_CONTEXT_IS_DESTROYED VEDA current context is already destroyed.\n
 *
 * Sets the memory range of N 8-bit values to the specified value "value".
 */
VEDAresult	vedaMemsetD8Async		(VEDAdeviceptr dstDevice, uint8_t value, size_t N, VEDAstream hStream);

// Profiling API ---------------------------------------------------------------
VEDAresult	vedaProfilerSetCallback		(VEDAprofiler_callback callback);
VEDAresult	vedaProfilerGetCallback		(VEDAprofiler_callback* callback);
VEDAresult	vedaProfilerTypeName		(VEDAprofiler_type type, const char** name, const VEDAstream stream);

// Inlined API -----------------------------------------------------------------
inline VEDAresult vedaFunctionGetName(VEDAfunction func, const char** name) {
	*name = func.kernel;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------
inline VEDAresult vedaFunctionGetPtr(VEDAfunction func, void** ptr) {
	*ptr = (void*)func.ptr;
	return VEDA_SUCCESS;
}

//------------------------------------------------------------------------------

#ifdef __cplusplus
}

//------------------------------------------------------------------------------
// Advanced C++ interface
//------------------------------------------------------------------------------
#include <type_traits>
#include <veda_ptr.h>

#include "api_cpp_args.h"
#include "api_cpp_launch.h"
#include "api_cpp_memset.h"

#endif
/** @} */
//------------------------------------------------------------------------------

