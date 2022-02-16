#pragma once

#include "veda.h"
#include "vera_enums.h"
#include "vera_types.h"

#ifndef __cplusplus
#error "VERA requires C++!"
#endif

#define veraLaunchKernel(...)	vedaLaunchKernel(__VA_ARGS__)
#define VERA2VEDA(P)		VEDAdeviceptr((uint64_t)P)

//------------------------------------------------------------------------------
veraError_t	veraInit		(void);
veraError_t	veraDeviceReset		(void);
veraError_t	veraGetDeviceProperties	(veraDeviceProp* prop, int device);
veraError_t	veraMalloc3D		(veraPitchedPtr* pitchedDevPtr, veraExtent extent);
veraError_t	veraMemcpy		(void* dst, const void* src, size_t count, veraMemcpyKind kind);
veraError_t	veraMemcpy2D		(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind);
veraError_t	veraMemcpy2DAsync	(void* dst, size_t dpitch, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	veraMemcpyAsync		(void* dst, const void* src, size_t count, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	veraMemset2D		(void* devPtr, size_t pitch, int value, size_t width, size_t height);
veraError_t	veraMemset2DAsync	(void* devPtr, size_t pitch, int value, size_t width, size_t height, veraStream_t stream = 0);
veraError_t	veraMemset3D		(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent);
veraError_t	veraMemset3DAsync	(veraPitchedPtr pitchedDevPtr, int value, veraExtent extent, veraStream_t stream = 0);
veraError_t	veraPointerGetAttributes(veraPointerAttributes* attributes, const void* ptr);
veraError_t	veraSetDevice		(int device);
veraError_t	veraSetValidDevices	(int* device_arr, int len);
veraError_t	veraStreamAddCallback	(veraStream_t stream, veraStreamCallback_t callback, void* userData, unsigned int flags);

//------------------------------------------------------------------------------
inline const char*	veraGetErrorName	(veraError_t error)							{	const char* str; vedaGetErrorName(error, &str); return str;					}
inline const char*	veraGetErrorString	(veraError_t error)							{	const char* str; vedaGetErrorString(error, &str); return str;					}
inline veraError_t	veraDeviceGetAttribute	(int* value, veraDeviceAttr attr, int device)				{	CVEDA(veraInit()); return vedaDeviceGetAttribute(value, (VEDAdevice_attribute)attr, device);	}
inline veraError_t	veraDeviceGetPower	(float* temp, int device)						{	CVEDA(veraInit()); return vedaDeviceGetPower(temp, device);					}
inline veraError_t	veraDeviceGetTemp	(float* temp, const int coreIdx, int device)				{	CVEDA(veraInit()); return vedaDeviceGetTemp(temp, coreIdx, device);				}
inline veraError_t	veraDeviceSynchronize	(void)									{	CVEDA(veraInit()); return vedaCtxSynchronize();							}
inline veraError_t	veraDriverGetVersion	(const char** driverVersion)						{	CVEDA(veraInit()); return vedaDriverGetVersion(driverVersion);					}
inline veraError_t	veraFree		(void* devPtr)								{	CVEDA(veraInit()); return vedaMemFree(VERA2VEDA(devPtr));					}
inline veraError_t	veraFreeAsync		(void* devPtr, veraStream_t stream = 0)					{	CVEDA(veraInit()); return vedaMemFreeAsync(VERA2VEDA(devPtr), stream);				}
inline veraError_t	veraFreeHost		(void* ptr)								{	CVEDA(veraInit()); return vedaMemFreeHost(ptr);							}
inline veraError_t	veraGetDevice		(int* device)								{	CVEDA(veraInit()); return vedaCtxGetDevice(device);						}
inline veraError_t	veraGetDeviceCount	(int* count)								{	CVEDA(veraInit()); return vedaDeviceGetCount(count);						}
inline veraError_t	veraHostAlloc		(void** pHost, size_t size, unsigned int flags)				{	CVEDA(veraInit()); return vedaMemAllocHost(pHost, size);					}
inline veraError_t	veraLaunchHostFunc	(veraStream_t stream, veraHostFn_t fn, void* userData)			{	CVEDA(veraInit()); return vedaLaunchHostFunc(stream, fn, userData);				}
inline veraError_t	veraLaunchHostFuncEx	(veraStream_t stream, veraHostFn_t fn, void* userData, uint64_t* result){	CVEDA(veraInit()); return vedaLaunchHostFuncEx(stream, fn, userData, result);			}
inline veraError_t	veraMalloc		(void** devPtr, size_t size)						{	CVEDA(veraInit()); return vedaMemAlloc((VEDAdeviceptr*)devPtr, size);				}
inline veraError_t	veraMallocAsync		(void** devPtr, size_t size, veraStream_t stream = 0)			{	CVEDA(veraInit()); return vedaMemAllocAsync((VEDAdeviceptr*)devPtr, size, stream);		}
inline veraError_t	veraMallocHost		(void** ptr, size_t size)						{	CVEDA(veraInit()); return vedaMemAllocHost(ptr, size);						}
inline veraError_t	veraMallocPitch		(void** devPtr, size_t* pitch, size_t width, size_t height)		{	CVEDA(veraInit()); return vedaMemAllocPitch((VEDAdeviceptr*)devPtr, pitch, width, height, 1);	}
inline veraError_t	veraMemGetInfo		(size_t* free, size_t* total)						{	CVEDA(veraInit()); return vedaMemGetInfo(free, total);						}
inline veraError_t	veraMemset		(void* devPtr, int value, size_t count)					{	CVEDA(veraInit()); return vedaMemsetD32(VERA2VEDA(devPtr), value, count);			}
inline veraError_t	veraMemsetAsync		(void* devPtr, int value, size_t count, veraStream_t stream = 0)	{	CVEDA(veraInit()); return vedaMemsetD32Async(VERA2VEDA(devPtr), value, count, stream);		}
inline veraError_t	veraModuleGetFunction	(veraFunction_t* func, veraModule_t mod, const char* name)		{	CVEDA(veraInit()); return vedaModuleGetFunction(func, mod, name);				}
inline veraError_t	veraModuleUnload	(veraModule_t mod)							{	CVEDA(veraInit()); return vedaModuleUnload(mod);						}
inline veraError_t	veraModuleLoad		(veraModule_t* mod, const char* name)					{	CVEDA(veraInit()); return vedaModuleLoad(mod, name);						}
inline veraError_t	veraRuntimeGetVersion	(const char** runtimeVersion)						{	CVEDA(veraInit()); return veraDriverGetVersion(runtimeVersion);					}
inline veraError_t	veraStreamCnt		(int* cnt)								{	CVEDA(veraInit()); return vedaCtxStreamCnt(cnt);						}
inline veraError_t	veraStreamQuery		(veraStream_t stream)							{	CVEDA(veraInit()); return vedaStreamQuery(stream);						}
inline veraError_t	veraStreamSynchronize	(veraStream_t stream)							{	CVEDA(veraInit()); return vedaStreamSynchronize(stream);					}

//------------------------------------------------------------------------------
inline veraExtent make_veraExtent(size_t w, size_t h, size_t d) {
	veraExtent extent;
	extent.depth	= d;
	extent.height	= h;
	extent.width	= w;
	return extent;
}

//------------------------------------------------------------------------------
inline veraPitchedPtr make_veraPitchedPtr(void* d, size_t p, size_t xsz, size_t ysz) {
	veraPitchedPtr ptr;
	ptr.ptr		= d;
	ptr.pitch	= p;
	ptr.xsize	= xsz;
	ptr.ysize	= ysz;
	return ptr;
}

//------------------------------------------------------------------------------

// Unsupported API
#if 0
veraError_t 	veraGetLastError					(void);
veraError_t 	veraPeekAtLastError					(void);
veraError_t	veraChooseDevice					(int* device, const veraDeviceProp* prop);
veraError_t	veraStreamWaitEvent					(veraStream_t stream, cudaEvent_t event, unsigned int flags);
veraError_t	veraMemcpy3D						(const cudaMemcpy3DParms* p);
veraError_t	veraMemcpy3DAsync					(const cudaMemcpy3DParms* p, veraStream_t stream = 0);
veraError_t	veraMallocManaged					(void** devPtr, size_t size, unsigned int flags = cudaMemAttachGlobal);
veraError_t	veraMemRangeGetAttribute				(void* data, size_t dataSize, cudaMemRangeAttribute attribute, const void* devPtr, size_t count);
veraError_t	veraMemRangeGetAttributes				(void** data, size_t* dataSizes, cudaMemRangeAttribute ** attributes, size_t numAttributes, const void* devPtr, size_t count);
veraError_t	veraMemPrefetchAsync					(const void* devPtr, size_t count, int dstDevice, veraStream_t stream = 0);
veraError_t	veraThreadExchangeStreamCaptureMode			(cudaStreamCaptureMode ** mode);
veraError_t	veraMalloc3DArray					(cudaArray_t* array, const cudaChannelFormatDesc* desc, veraExtent extent, unsigned int flags);
veraError_t	veraMallocArray						(cudaArray_t* array, const cudaChannelFormatDesc* desc, size_t width, size_t height, unsigned int flags);
veraError_t	cudaArrayGetInfo					(cudaChannelFormatDesc* desc, veraExtent* extent, unsigned int* flags, cudaArray_t array);
veraError_t	cudaCtxResetPersistingL2Cache				(void);
veraError_t	cudaDestroyExternalMemory				(cudaExternalMemory_t extMem);
veraError_t	cudaDestroyExternalSemaphore				(cudaExternalSemaphore_t extSem);
veraError_t	cudaDeviceCanAccessPeer					(int* canAccessPeer, int device, int peerDevice);
veraError_t	cudaDeviceDisablePeerAccess				(int peerDevice);
veraError_t	cudaDeviceEnablePeerAccess				(int peerDevice, unsigned int flags);
veraError_t	cudaDeviceGetByPCIBusId					(int* device, const char* pciBusId);
veraError_t	cudaDeviceGetCacheConfig				(cudaFuncCache ** pCacheConfig);
veraError_t	cudaDeviceGetLimit					(size_t* pValue, cudaLimit limit);
veraError_t	cudaDeviceGetNvSciSyncAttributes			(void* nvSciSyncAttrList, int device, int flags);
veraError_t	cudaDeviceGetP2PAttribute				(int* value, cudaDeviceP2PAttr attr, int srcDevice, int dstDevice);
veraError_t	cudaDeviceGetPCIBusId					(char* pciBusId, int len, int device);
veraError_t	cudaDeviceGetSharedMemConfig				(cudaSharedMemConfig ** pConfig);
veraError_t	cudaDeviceGetStreamPriorityRange			(int* leastPriority, int* greatestPriority);
veraError_t	cudaDeviceSetCacheConfig				(cudaFuncCache cacheConfig);
veraError_t	cudaDeviceSetLimit					(cudaLimit limit, size_t value);
veraError_t	cudaDeviceSetSharedMemConfig				(cudaSharedMemConfig config);
veraError_t	cudaEventCreate						(cudaEvent_t* event);
veraError_t	cudaEventCreateWithFlags				(cudaEvent_t* event, unsigned int flags);
veraError_t	cudaEventDestroy					(cudaEvent_t event);
veraError_t	cudaEventElapsedTime					(float* ms, cudaEvent_t start, cudaEvent_t end);
veraError_t	cudaEventQuery						(cudaEvent_t event);
veraError_t	cudaEventRecord						(cudaEvent_t event, veraStream_t stream = 0);
veraError_t	cudaEventSynchronize					(cudaEvent_t event);
veraError_t	cudaExternalMemoryGetMappedBuffer			(void** devPtr, cudaExternalMemory_t extMem, const cudaExternalMemoryBufferDesc* bufferDesc);
veraError_t	cudaFreeArray						(cudaArray_t array);
veraError_t	cudaFreeMipmappedArray					(cudaMipmappedArray_t mipmappedArray);
veraError_t	cudaFuncGetAttributes					(cudaFuncAttributes* attr, const void* func);
veraError_t	cudaFuncSetAttribute					(const void* func, cudaFuncAttribute attr, int value);
veraError_t	cudaFuncSetCacheConfig					(const void* func, cudaFuncCache cacheConfig);
veraError_t	cudaFuncSetSharedMemConfig				(const void* func, cudaSharedMemConfig config);
veraError_t	cudaGetDeviceFlags					(unsigned int* flags);
veraError_t	cudaGetMipmappedArrayLevel				(cudaArray_t* levelArray, cudaMipmappedArray_const_t mipmappedArray, unsigned int level);
veraError_t	cudaGetSymbolAddress					(void** devPtr, const void* symbol);
veraError_t	cudaGetSymbolSize					(size_t* size, const void* symbol);
veraError_t	cudaHostGetDevicePointer				(void** pDevice, void* pHost, unsigned int flags);
veraError_t	cudaHostGetFlags					(unsigned int* pFlags, void* pHost);
veraError_t	cudaHostRegister					(void* ptr, size_t size, unsigned int flags);
veraError_t	cudaHostUnregister					(void* ptr);
veraError_t	cudaImportExternalMemory				(cudaExternalMemory_t* extMem_out, const cudaExternalMemoryHandleDesc* memHandleDesc);
veraError_t	cudaImportExternalSemaphore				(cudaExternalSemaphore_t* extSem_out, const cudaExternalSemaphoreHandleDesc* semHandleDesc);
veraError_t	cudaIpcCloseMemHandle					(void* devPtr);
veraError_t	cudaIpcGetEventHandle					(cudaIpcEventHandle_t* handle, cudaEvent_t event);
veraError_t	cudaIpcGetMemHandle					(cudaIpcMemHandle_t* handle, void* devPtr);
veraError_t	cudaIpcOpenEventHandle					(cudaEvent_t* event, cudaIpcEventHandle_t handle);
veraError_t	cudaIpcOpenMemHandle					(void** devPtr, cudaIpcMemHandle_t handle, unsigned int flags);
veraError_t	cudaLaunchCooperativeKernel				(const void* func, dim3 gridDim, dim3 blockDim, void** args, size_t sharedMem, veraStream_t stream = 0);
veraError_t	cudaLaunchCooperativeKernelMultiDevice			(cudaLaunchParams* launchParamsList, unsigned int numDevices, unsigned int flags);
veraError_t	cudaMallocMipmappedArray				(cudaMipmappedArray_t* mipmappedArray, const cudaChannelFormatDesc* desc, veraExtent extent, unsigned int numLevels, unsigned int flags);
veraError_t	cudaMemAdvise						(const void* devPtr, size_t count, cudaMemoryAdvise advice, int device);
veraError_t	cudaMemcpy2DArrayToArray				(cudaArray_t dst, size_t wOffsetDst, size_t hOffsetDst, cudaArray_const_t src, size_t wOffsetSrc, size_t hOffsetSrc, size_t width, size_t height, veraMemcpyKind kind = cudaMemcpyDeviceToDevice);
veraError_t	cudaMemcpy2DFromArray					(void* dst, size_t dpitch, cudaArray_const_t src, size_t wOffset, size_t hOffset, size_t width, size_t height, veraMemcpyKind kind);
veraError_t	cudaMemcpy2DFromArrayAsync				(void* dst, size_t dpitch, cudaArray_const_t src, size_t wOffset, size_t hOffset, size_t width, size_t height, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	cudaMemcpy2DToArray					(cudaArray_t dst, size_t wOffset, size_t hOffset, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind);
veraError_t	cudaMemcpy2DToArrayAsync				(cudaArray_t dst, size_t wOffset, size_t hOffset, const void* src, size_t spitch, size_t width, size_t height, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	cudaMemcpy3DPeer					(const cudaMemcpy3DPeerParms* p);
veraError_t	cudaMemcpy3DPeerAsync					(const cudaMemcpy3DPeerParms* p, veraStream_t stream = 0);
veraError_t	cudaMemcpyFromSymbol					(void* dst, const void* symbol, size_t count, size_t offset, veraMemcpyKind kind = cudaMemcpyDeviceToHost);
veraError_t	cudaMemcpyFromSymbolAsync				(void* dst, const void* symbol, size_t count, size_t offset, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	cudaMemcpyPeer						(void* dst, int dstDevice, const void* src, int srcDevice, size_t count);
veraError_t	cudaMemcpyPeerAsync					(void* dst, int dstDevice, const void* src, int srcDevice, size_t count, veraStream_t stream = 0);
veraError_t	cudaMemcpyToSymbol					(const void* symbol, const void* src, size_t count, size_t offset, veraMemcpyKind kind = cudaMemcpyHostToDevice);
veraError_t	cudaMemcpyToSymbolAsync					(const void* symbol, const void* src, size_t count, size_t offset, veraMemcpyKind kind, veraStream_t stream = 0);
veraError_t	cudaOccupancyAvailableDynamicSMemPerBlock		(size_t* dynamicSmemSize, const void* func, int numBlocks, int blockSize);
veraError_t	cudaOccupancyMaxActiveBlocksPerMultiprocessor		(int* numBlocks, const void* func, int blockSize, size_t dynamicSMemSize);
veraError_t	cudaOccupancyMaxActiveBlocksPerMultiprocessorWithFlags	(int* numBlocks, const void* func, int blockSize, size_t dynamicSMemSize, unsigned int flags);
veraError_t	cudaSetDeviceFlags					(unsigned int flags);
veraError_t	cudaSetDoubleForDevice					(double* d);
veraError_t	cudaSetDoubleForHost					(double* d);
veraError_t	cudaSignalExternalSemaphoresAsync			(const cudaExternalSemaphore_t* extSemArray, const cudaExternalSemaphoreSignalParams* paramsArray, unsigned int numExtSems, veraStream_t stream = 0);
veraError_t	cudaStreamAttachMemAsync				(veraStream_t stream, void* devPtr, size_t length, unsigned int flags = cudaMemAttachSingle);
veraError_t	cudaStreamBeginCapture					(veraStream_t stream, cudaStreamCaptureMode mode);
veraError_t	cudaStreamCopyAttributes				(veraStream_t dst, veraStream_t src);
veraError_t	cudaStreamCreateWithFlags				(veraStream_t* pStream, unsigned int flags);
veraError_t	cudaStreamCreateWithPriority				(veraStream_t* pStream, unsigned int flags, int priority);
veraError_t	cudaStreamEndCapture					(veraStream_t stream, cudaGraph_t* pGraph);
veraError_t	cudaStreamGetAttribute					(veraStream_t hStream, cudaStreamAttrID attr, cudaStreamAttrValue* value_out);
veraError_t	cudaStreamGetCaptureInfo				(veraStream_t stream, cudaStreamCaptureStatus ** pCaptureStatus, unsigned long long* pId);
veraError_t	cudaStreamGetFlags					(veraStream_t hStream, unsigned int* flags);
veraError_t	cudaStreamGetPriority					(veraStream_t hStream, int* priority);
veraError_t	cudaStreamIsCapturing					(veraStream_t stream, cudaStreamCaptureStatus ** pCaptureStatus);
veraError_t	cudaStreamSetAttribute					(veraStream_t hStream, cudaStreamAttrID attr, const cudaStreamAttrValue* value);
veraError_t	cudaWaitExternalSemaphoresAsync				(const cudaExternalSemaphore_t* extSemArray, const cudaExternalSemaphoreWaitParams* paramsArray, unsigned int numExtSems, veraStream_t stream = 0);
cudaPos		make_cudaPos						(size_t x, size_t y, size_t z);
void*		cudaGetParameterBuffer					(size_t alignment, size_t size);
void*		cudaGetParameterBufferV2				(void* func, dim3 gridDimension, dim3 blockDimension, unsigned int sharedMemSize);
#endif
