__all__ = ['VEDAdevice_attribute', 'init_device', 'device_cnt']

import numpy as np
import ctypes
import os
import enum

#-------------------------------------------------------------------------------
class VEDAdevice_attribute(enum.Enum):
	VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE				= 0
	VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE				= 1
	VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT			= 2
	VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE				= 3
	VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE				= 4
	VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE				= 5
	VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE				= 6
	VEDA_DEVICE_ATTRIBUTE_ABI_VERSION				= 7
	VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO	= 8
	VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION				= 9

#-------------------------------------------------------------------------------
path	= os.path.dirname(__file__)
path	= os.path.abspath(os.path.abspath(path) + "/lib64")
lib	= ctypes.cdll.LoadLibrary(os.path.join(path, 'libveda.so.0')) # TODO: use VEDA_API_VERSION from CMake!

lib.vedaArgsCreate.argtypes			= [ctypes.POINTER(ctypes.c_void_p)]
lib.vedaArgsDestroy.argtypes			= [ctypes.c_void_p]
lib.vedaArgsSetF32.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_float]
lib.vedaArgsSetF64.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_double]
lib.vedaArgsSetI16.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_short]
lib.vedaArgsSetI32.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
lib.vedaArgsSetI64.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_longlong]
lib.vedaArgsSetI8.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_byte]
lib.vedaArgsSetPtr.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
lib.vedaArgsSetVPtr.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
lib.vedaArgsSetHMEM.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
lib.vedaArgsSetStack.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, ctypes.c_ulonglong]
lib.vedaArgsSetU16.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_ushort]
lib.vedaArgsSetU32.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_uint]
lib.vedaArgsSetU64.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_ulonglong]
lib.vedaArgsSetU8.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_ubyte]
lib.vedaCtxCreate.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_int, ctypes.c_int]
lib.vedaCtxDestroy.argtypes			= [ctypes.c_void_p]
lib.vedaCtxGet.argtypes				= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_int]
lib.vedaCtxGetApiVersion.argtypes		= [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int)]
lib.vedaCtxGetCurrent.argtypes			= [ctypes.POINTER(ctypes.c_void_p)]
lib.vedaCtxGetDevice.argtypes			= [ctypes.POINTER(ctypes.c_int)]
lib.vedaCtxPopCurrent.argtypes			= [ctypes.POINTER(ctypes.c_void_p)]
lib.vedaCtxPushCurrent.argtypes			= [ctypes.c_void_p]
lib.vedaCtxSetCurrent.argtypes			= [ctypes.c_void_p]
lib.vedaCtxStreamCnt.argtypes			= [ctypes.POINTER(ctypes.c_int)]
lib.vedaCtxSynchronize.argtypes			= []
lib.vedaDeviceDistance.argtypes			= [ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.c_int]
lib.vedaDeviceGet.argtypes			= [ctypes.POINTER(ctypes.c_int), ctypes.c_int]
lib.vedaDeviceGetAttribute.argtypes		= [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]
lib.vedaDeviceGetCount.argtypes			= [ctypes.POINTER(ctypes.c_int)]
lib.vedaDeviceGetName.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
lib.vedaDeviceGetPower.argtypes			= [ctypes.POINTER(ctypes.c_float), ctypes.c_int]
lib.vedaDeviceGetTemp.argtypes			= [ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.c_int]
lib.vedaDevicePrimaryCtxGetState.argtypes	= [ctypes.c_int, ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_int)]
lib.vedaDevicePrimaryCtxRelease.argtypes	= [ctypes.c_int]
lib.vedaDevicePrimaryCtxReset.argtypes		= [ctypes.c_int]
lib.vedaDevicePrimaryCtxRetain.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_int]
lib.vedaDevicePrimaryCtxSetFlags.argtypes	= [ctypes.c_int, ctypes.c_uint]
lib.vedaDeviceTotalMem.argtypes			= [ctypes.POINTER(ctypes.c_ulonglong), ctypes.c_int]
lib.vedaDriverGetVersion.argtypes		= [ctypes.POINTER(ctypes.c_char_p)]
lib.vedaExit.argtypes				= []
lib.vedaGetErrorName.argtypes			= [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
lib.vedaGetErrorString.argtypes			= [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
lib.vedaGetVersion.argtypes			= [ctypes.POINTER(ctypes.c_char_p)]
lib.vedaInit.argtypes				= [ctypes.c_uint]
#lib.vedaLaunchHostFunc.argtypes		= [ctypes.c_int, VEDAhost_function fn, ctypes.c_void_p]
lib.vedaLaunchKernel.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
lib.vedaLaunchKernelEx.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
lib.vedaMemAlloc.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_ulonglong]
lib.vedaMemAllocAsync.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemAllocHost.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_ulonglong]
lib.vedaMemAllocPitch.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_ulonglong), ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_uint]
lib.vedaMemAllocPitchAsync.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_ulonglong), ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_uint, ctypes.c_int]
lib.vedaMemFree.argtypes			= [ctypes.c_void_p]
lib.vedaMemFreeAsync.argtypes			= [ctypes.c_void_p, ctypes.c_int]
lib.vedaMemFreeHost.argtypes			= [ctypes.c_void_p]
lib.vedaMemGetAddressRange.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_ulonglong), ctypes.c_void_p]
lib.vedaMemGetDevice.argtypes			= [ctypes.POINTER(ctypes.c_int), ctypes.c_void_p]
lib.vedaMemGetInfo.argtypes			= [ctypes.POINTER(ctypes.c_ulonglong), ctypes.POINTER(ctypes.c_ulonglong)]
lib.vedaMemGetRawPointer.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_void_p]
lib.vedaMemGetHMEMPointer.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_void_p]
lib.vedaMemReport.argtypes			= []
lib.vedaMemcpy.argtypes				= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong]
lib.vedaMemcpyAsync.argtypes			= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemcpyDtoD.argtypes			= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong]
lib.vedaMemcpyDtoDAsync.argtypes		= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemcpyDtoH.argtypes			= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong]
lib.vedaMemcpyDtoHAsync.argtypes		= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemcpyHtoD.argtypes			= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong]
lib.vedaMemcpyHtoDAsync.argtypes		= [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD16.argtypes			= [ctypes.c_void_p, ctypes.c_ushort, ctypes.c_ulonglong]
lib.vedaMemsetD16Async.argtypes			= [ctypes.c_void_p, ctypes.c_ushort, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD2D16.argtypes			= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ushort, ctypes.c_ulonglong, ctypes.c_ulonglong]
lib.vedaMemsetD2D16Async.argtypes		= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ushort, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD2D32.argtypes			= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_uint, ctypes.c_ulonglong, ctypes.c_ulonglong]
lib.vedaMemsetD2D32Async.argtypes		= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_uint, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD2D8.argtypes			= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ubyte, ctypes.c_ulonglong, ctypes.c_ulonglong]
lib.vedaMemsetD2D8Async.argtypes		= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ubyte, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD32.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_ulonglong]
lib.vedaMemsetD32Async.argtypes			= [ctypes.c_void_p, ctypes.c_int, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD64.argtypes			= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ulonglong]
lib.vedaMemsetD64Async.argtypes			= [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaMemsetD8.argtypes			= [ctypes.c_void_p, ctypes.c_ubyte, ctypes.c_ulonglong]
lib.vedaMemsetD8Async.argtypes			= [ctypes.c_void_p, ctypes.c_ubyte, ctypes.c_ulonglong, ctypes.c_int]
lib.vedaModuleGetFunction.argtypes		= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_void_p, ctypes.c_char_p]
lib.vedaModuleLoad.argtypes			= [ctypes.POINTER(ctypes.c_void_p), ctypes.c_char_p]
lib.vedaModuleUnload.argtypes			= [ctypes.c_void_p]
#lib.vedaStreamAddCallback.argtypes		= [ctypes.c_int, VEDAstream_callback callback, ctypes.c_void_p, ctypes.c_uint]
lib.vedaStreamGetFlags.argtypes			= [ctypes.c_int, ctypes.POINTER(ctypes.c_uint)]
lib.vedaStreamQuery.argtypes			= [ctypes.c_int]
lib.vedaStreamSynchronize.argtypes		= [ctypes.c_int]

lib.vedaArgsCreate.restype			= ctypes.c_int
lib.vedaArgsDestroy.restype			= ctypes.c_int
lib.vedaArgsSetF32.restype			= ctypes.c_int
lib.vedaArgsSetF64.restype			= ctypes.c_int
lib.vedaArgsSetI16.restype			= ctypes.c_int
lib.vedaArgsSetI32.restype			= ctypes.c_int
lib.vedaArgsSetI64.restype			= ctypes.c_int
lib.vedaArgsSetI8.restype			= ctypes.c_int
lib.vedaArgsSetPtr.restype			= ctypes.c_int
lib.vedaArgsSetVPtr.restype			= ctypes.c_int
lib.vedaArgsSetHMEM.restype			= ctypes.c_int
lib.vedaArgsSetStack.restype			= ctypes.c_int
lib.vedaArgsSetU16.restype			= ctypes.c_int
lib.vedaArgsSetU32.restype			= ctypes.c_int
lib.vedaArgsSetU64.restype			= ctypes.c_int
lib.vedaArgsSetU8.restype			= ctypes.c_int
lib.vedaCtxCreate.restype			= ctypes.c_int
lib.vedaCtxDestroy.restype			= ctypes.c_int
lib.vedaCtxGet.restype				= ctypes.c_int
lib.vedaCtxGetApiVersion.restype		= ctypes.c_int
lib.vedaCtxGetCurrent.restype			= ctypes.c_int
lib.vedaCtxGetDevice.restype			= ctypes.c_int
lib.vedaCtxPopCurrent.restype			= ctypes.c_int
lib.vedaCtxPushCurrent.restype			= ctypes.c_int
lib.vedaCtxSetCurrent.restype			= ctypes.c_int
lib.vedaCtxStreamCnt.restype			= ctypes.c_int
lib.vedaCtxSynchronize.restype			= ctypes.c_int
lib.vedaDeviceDistance.restype			= ctypes.c_int
lib.vedaDeviceGet.restype			= ctypes.c_int
lib.vedaDeviceGetAttribute.restype		= ctypes.c_int
lib.vedaDeviceGetCount.restype			= ctypes.c_int
lib.vedaDeviceGetName.restype			= ctypes.c_int
lib.vedaDeviceGetPower.restype			= ctypes.c_int
lib.vedaDeviceGetTemp.restype			= ctypes.c_int
lib.vedaDevicePrimaryCtxGetState.restype	= ctypes.c_int
lib.vedaDevicePrimaryCtxRelease.restype		= ctypes.c_int
lib.vedaDevicePrimaryCtxReset.restype		= ctypes.c_int
lib.vedaDevicePrimaryCtxRetain.restype		= ctypes.c_int
lib.vedaDevicePrimaryCtxSetFlags.restype	= ctypes.c_int
lib.vedaDeviceTotalMem.restype			= ctypes.c_int
lib.vedaDriverGetVersion.restype		= ctypes.c_int
lib.vedaExit.restype				= ctypes.c_int
lib.vedaGetErrorName.restype			= ctypes.c_int
lib.vedaGetErrorString.restype			= ctypes.c_int
lib.vedaGetVersion.restype			= ctypes.c_int
lib.vedaInit.restype				= ctypes.c_int
lib.vedaLaunchKernel.restype			= ctypes.c_int
lib.vedaLaunchKernelEx.restype			= ctypes.c_int
lib.vedaMemAlloc.restype			= ctypes.c_int
lib.vedaMemAllocAsync.restype			= ctypes.c_int
lib.vedaMemAllocHost.restype			= ctypes.c_int
lib.vedaMemAllocPitch.restype			= ctypes.c_int
lib.vedaMemAllocPitchAsync.restype		= ctypes.c_int
lib.vedaMemFree.restype				= ctypes.c_int
lib.vedaMemFreeAsync.restype			= ctypes.c_int
lib.vedaMemFreeHost.restype			= ctypes.c_int
lib.vedaMemGetAddressRange.restype		= ctypes.c_int
lib.vedaMemGetDevice.restype			= ctypes.c_int
lib.vedaMemGetInfo.restype			= ctypes.c_int
lib.vedaMemGetRawPointer.restype		= ctypes.c_int
lib.vedaMemGetHMEMPointer.restype		= ctypes.c_int
lib.vedaMemReport.restype			= ctypes.c_int
lib.vedaMemcpy.restype				= ctypes.c_int
lib.vedaMemcpyAsync.restype			= ctypes.c_int
lib.vedaMemcpyDtoD.restype			= ctypes.c_int
lib.vedaMemcpyDtoDAsync.restype			= ctypes.c_int
lib.vedaMemcpyDtoH.restype			= ctypes.c_int
lib.vedaMemcpyDtoHAsync.restype			= ctypes.c_int
lib.vedaMemcpyHtoD.restype			= ctypes.c_int
lib.vedaMemcpyHtoDAsync.restype			= ctypes.c_int
lib.vedaMemsetD16.restype			= ctypes.c_int
lib.vedaMemsetD16Async.restype			= ctypes.c_int
lib.vedaMemsetD2D16.restype			= ctypes.c_int
lib.vedaMemsetD2D16Async.restype		= ctypes.c_int
lib.vedaMemsetD2D32.restype			= ctypes.c_int
lib.vedaMemsetD2D32Async.restype		= ctypes.c_int
lib.vedaMemsetD2D8.restype			= ctypes.c_int
lib.vedaMemsetD2D8Async.restype			= ctypes.c_int
lib.vedaMemsetD32.restype			= ctypes.c_int
lib.vedaMemsetD32Async.restype			= ctypes.c_int
lib.vedaMemsetD64.restype			= ctypes.c_int
lib.vedaMemsetD64Async.restype			= ctypes.c_int
lib.vedaMemsetD8.restype			= ctypes.c_int
lib.vedaMemsetD8Async.restype			= ctypes.c_int
lib.vedaModuleGetFunction.restype		= ctypes.c_int
lib.vedaModuleLoad.restype			= ctypes.c_int
lib.vedaModuleUnload.restype			= ctypes.c_int
lib.vedaStreamGetFlags.restype			= ctypes.c_int
lib.vedaStreamQuery.restype			= ctypes.c_int
lib.vedaStreamSynchronize.restype		= ctypes.c_int

#-------------------------------------------------------------------------------
def check(err):
	if err != 0:
		name = ctypes.c_char_p()
		msg  = ctypes.c_char_p()
		lib.vedaGetErrorName(err, ctypes.byref(name))
		lib.vedaGetErrorString(err, ctypes.byref(msg))
		raise Exception('[VEDA#{}] {}: {}'.format(err, name, msg))

#-------------------------------------------------------------------------------
check(lib.vedaInit(0))

#-------------------------------------------------------------------------------
def check_stream(stream):
	assert stream is None or isinstance(stream, int)

#-------------------------------------------------------------------------------
def get_size(array):
	if isinstance(array, VPtr):
		return array.size()
	elif isinstance(array, np.ndarray):
		return array.nbytes
	else:
		raise Exception('[VEDA] unknown pointer type {}'.format(type(array)))

#-------------------------------------------------------------------------------
def get_ptr(array):
	if isinstance(array, np.ndarray): 
		return ctypes.c_void_p(array.__array_interface__['data'][0])
	elif isinstance(array, VPtr):
		return array.ptr()
	else:
		raise Exception('[VEDA] unknown pointer type {}'.format(type(array)))

#-------------------------------------------------------------------------------
def check_copy(A, B, bytes):
	assert bytes is None or isinstance(bytes, int)
	if bytes is None:
		bytes = get_size(A)
	assert bytes > 0
	assert bytes <= get_size(A)
	assert bytes <= get_size(B)
	return bytes

#-------------------------------------------------------------------------------
def copy(dst, src, stream=None, bytes=None):
	check_stream(stream)
	bytes	= check_copy(dst, src, bytes)
	dst_ptr = get_ptr(dst)
	src_ptr = get_ptr(src)

	if isinstance(dst, VPtr):
		if isinstance(src, VPtr):
			check(lib.vedaMemCpyDtoD(dst_ptr, src_ptr, bytes) if stream is None else lib.vedaMemcpyDtoDAsync(dst_ptr, src_ptr, bytes, stream))
		else:
			check(lib.vedaMemCpyHtoD(dst_ptr, src_ptr, bytes) if stream is None else lib.vedaMemcpyHtoDAsync(dst_ptr, src_ptr, bytes, stream))
	else:
		assert isinstance(src, VPtr)
		check(lib.vedaMemCpyDtoH(dst_ptr, src_ptr, bytes) if stream is None else lib.vedaMemcpyDtoHAsync(dst_ptr, src_ptr, bytes, stream))

#-------------------------------------------------------------------------------
class VBuffer:
	def __init__(self, ptr, size):
		assert isinstance(ptr, ctypes.c_void_p)
		assert isinstance(size, ctypes.c_ulonglong)
		self.m_ptr	= ptr
		self.m_size	= size

	def __del__(self):
		check(lib.vedaMemFree(self.m_ptr))

	def ptr(self):
		return self.m_ptr

	def size(self):
		if self.m_size.value == 0:
			base = ctypes.c_void_p()
			check(lib.vedaMemGetAddressRange(ctypes.byref(base), ctypes.byref(self.m_size), self.ptr()))
		return self.m_size.value

#-------------------------------------------------------------------------------
class VPtr:
	def __init__(self, buf, offset=0):
		assert isinstance(buf, VBuffer)
		self.m_buf	= buf
		self.m_offset	= offset
	
	def __repr__(self):
		str = "<VEDAdeviceptr 0x%016x/%uB>" % (self.ptr().value, self.size())
		return str

	def __add__(self, offset):
		return VPtr(self.buf(), self.offset() + offset)

	def __sub__(self, offset):
		return VPtr(self.buf(), self.offset() - offset)

	def __iadd__(self, offset):
		self.m_offset += offset
		assert self.m_offset >= 0
		assert self.m_offset < 0x1FFFFFFFFF

	def __isub__(self, offset):
		self.m_offset -= offset
		assert self.m_offset >= 0
		assert self.m_offset < 0x1FFFFFFFFF

	def buf(self):
		return self.m_buf

	def size(self):
		return self.buf().size()

	def offset(self):
		return self.m_offset

	def ptr(self):
		return ctypes.c_void_p(self.buf().ptr().value + self.offset())

	def to(self, dst, stream=0, bytes=None):
		copy(self, dst, stream, bytes)

	def __ilshift__(self, src): # dst <<= src, synchronous
		copy(self, src)

	def __irshift__(self, dst): # src >>= dst, synchronous
		copy(dst, self)

	def __lshift__(self, src): # dst << src, asynchronous
		copy(self, src, stream=0)

	def __rshift__(self, dst): # src >> dst, asynchronous
		copy(dst, self, stream=0)

	def to_numpy(self, stream=None):
		dst = np.empty(self.size(), dtype=np.byte)
		self.to(dst, stream=stream)

	def memset(self, value, stream=None, cnt=None):
		check_stream(stream)

		if isinstance(value, (np.byte, np.ubyte)):
			bytes = check_copy(self, self, None if cnt is None else cnt * 1) // 1
			check(lib.vedaMemsetD8(self.ptr(), value, bytes)) if stream is None else check(lib.vedaMemsetD8(self.ptr(), value, bytes))
		elif isinstance(value, (np.short, np.ushort)):
			bytes = check_copy(self, self, None if cnt is None else cnt * 2) // 2
			check(lib.vedaMemsetD16(self.ptr(), value, bytes)) if stream is None else check(lib.vedaMemsetD16(self.ptr(), value, bytes))
		elif isinstance(value, (np.float32, np.intc, np.uint, np.int_, np.uintc, np.single)):
			bytes = check_copy(self, self, None if cnt is None else cnt * 4) // 4
			check(lib.vedaMemsetD32(self.ptr(), value, bytes)) if stream is None else check(lib.vedaMemsetD32(self.ptr(), value, bytes))
		elif isinstance(value, (float, np.float64, np.longlong, np.ulonglong, np.double)):
			bytes = check_copy(self, self, None if cnt is None else cnt * 8) // 8
			check(lib.vedaMemsetD64(self.ptr(), value, bytes)) if stream is None else check(lib.vedaMemsetD64(self.ptr(), value, bytes))
		else:
			raise Exception('[VEDA] unknown data type {}'.format(type(value)))

#-------------------------------------------------------------------------------
class Function:
	def __init__(self, mod, name):
		assert isinstance(name, str)
		self.m_mod	= mod
		self.m_name	= name
		self.m_func	= ctypes.c_void_p()
		cname		= name.encode('UTF-8')

		self.m_mod.m_device.__set_current()
		check(lib.vedaModuleGetFunction(ctypes.byref(self.m_func), self.m_mod.m_lib, cname))

	def __call__(self, *args, stream=None):
		vargs = ctypes.c_void_p
		check(lib.vedaArgsCreate(ctypes.byref(vargs)))

		for i, a in enumerate(args):
			if isinstance(a, (int, ctypes.c_longlong)):
				check(lib.vedaArgsSetI64(vargs, i, a))
			elif isinstance(a, (int, ctypes.c_ulonglong, ctypes.c_void_p)):
				check(lib.vedaArgsSetU64(vargs, i, a))
			elif isinstance(a, (float, ctypes.c_double)):
				check(lib.vedaArgsSetF64(vargs, i, a))
			elif isinstance(a, ctypes.c_float):
				check(lib.vedaArgsSetF32(vargs, i, a))
			elif isinstance(a, ctypes.c_char, ctypes.c_byte):
				check(lib.vedaArgsSetI8(vargs, i, a))
			elif isinstance(a, ctypes.c_ubyte):
				check(lib.vedaArgsSetU8(vargs, i, a))
			elif isinstance(a, ctypes.c_short):
				check(lib.vedaArgsSetI16(vargs, i, a))
			elif isinstance(a, ctypes.c_ushort):
				check(lib.vedaArgsSetU16(vargs, i, a))
			elif isinstance(a, ctypes.c_int):
				check(lib.vedaArgsSetI32(vargs, i, a))
			elif isinstance(a, ctypes.c_uint):
				check(lib.vedaArgsSetU32(vargs, i, a))
			else:
				raise Exception('[VEDA] can\'t launch kernel with argument of type: {}'.format(type(a)))

#-------------------------------------------------------------------------------
class Module:
	def __init__(self, device, name):
		assert isinstance(device, Device)
		assert isinstance(name, str)
		self.m_lib	= ctypes.c_void_p()
		self.m_device	= device
		self.m_name	= name
		cname		= name.encode('UTF-8')

		self.m_device.__set_current()
		check(lib.vedaModuleLoad(ctypes.byref(self.m_lib), cname))

	def __del__(self):
		self.m_device.__set_current()
		check(lib.vedaModuleUnload(self.m_lib))

	def get(self, name):
		return Function(self, name)

#-------------------------------------------------------------------------------
class Device:
	def __init__(self, device_id):
		self.m_ctx		= ctypes.c_void_p()
		self.m_device_id	= device_id
		check(lib.vedaCtxCreate(ctypes.byref(self.m_ctx), 0, device_id))

	def __del__(self):
		check(lib.vedaCtxDestroy(self.m_ctx))

	def id(self):
		return self.m_device_id

	def __set_current(self):
		check(lib.vedaCtxSetCurrent(self.m_ctx))

	def __repr__(self):
		str = "# VEDA Device #{} {}\n".format(self.id(), self.name())
		str += "Cores: {}, Streams: {}\n".format(self.core_cnt(), self.stream_cnt())
		str += "Memory: {}/{}GB\n".format(self.mem_used(), self.mem_total())
		str += "Power: {}W".format(self.power())
		for i in range(self.core_cnt()):
			str += ", CoreTemp#{}: {}°C".format(i, self.temp(i))
		str += "\n"
		str += "ClockRate: {}, ClockBase: {}, MemoryClock: {}\n".format(self.clock_rate(), self.clock_base(), self.memory_clock_rate())
		str += "L1D: {}B, L1I: {}B, L2: {}B\n".format(self.l1d_cache_size(), self.l1i_cache_size(), self.l2_cache_size())
		str += "ABI: {}, Firmware: {}\n".format(self.abi_version(), self.firmware_version())
		return str
    
	def stream_cnt(self):
		cnt = ctypes.c_int()
		check(lib.vedaCtxStreamCnt(ctypes.byref(cnt)))
		return cnt.value

	def new(self, size=0, stream=None):
		check_stream(stream)
		cptr	= ctypes.c_void_p(0)
		csize	= ctypes.c_ulonglong(size)
		if stream is None:
			check(lib.vedaMemAlloc(ctypes.byref(cptr), csize))
		else:
			check(lib.vedaMemAllocAsync(ctypes.byref(cptr), csize, stream))
		return VPtr(VBuffer(cptr, csize))

	def new_as(self, array, stream=None):
		assert isinstance(array, np.ndarray)
		return self.new(get_size(array), stream)		

	def name(self):
		name = ctypes.create_string_buffer(128)
		check(lib.vedaDeviceGetName(ctypes.byref(name), 128, self.id()))
		return bytes(name.value).decode('utf-8')

	def mem_info(self):
		total, free = ctypes.c_ulonglong(), ctypes.c_ulonglong()
		self.__set_current()
		check(lib.vedaMemGetInfo(ctypes.byref(free), ctypes.byref(total)))
		return (free.value, total.value)

	def mem_total(self):
		total = ctypes.c_ulonglong()
		check(lib.vedaDeviceTotalMem(ctypes.byref(total), self.id()))
		return total.value

	def mem_used(self):
		m = self.mem_info()
		return m[1] - m[0]

	def mem_free(self):
		return self.mem_info()[0]

	def power(self):
		power = ctypes.c_float()
		check(lib.vedaDeviceGetPower(ctypes.byref(power), self.id()))
		return power.value

	def temp(self, core):
		assert isinstance(core, int)
		temp = ctypes.c_float()
		check(lib.vedaDeviceGetTemp(ctypes.byref(temp), core, self.id()))
		return temp.value

	def __attribute(self, attr):
		assert isinstance(attr, VEDAdevice_attribute)
		value = ctypes.c_int()
		check(lib.vedaDeviceGetAttribute(ctypes.byref(value), attr.value, self.id()))
		return value.value

	def clock_rate(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE)

	def clock_base(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE)

	def core_cnt(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT)

	def memory_clock_rate(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE)

	def l1d_cache_size(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE)

	def l1i_cache_size(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE)

	def l2_cache_size(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE)

	def abi_version(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_ABI_VERSION)

	def single_to_double_precision_perf_ratio(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO)

	def firmware_version(self):
		return self.__attribute(VEDAdevice_attribute.VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION)

	def distance(self, other):
		assert isinstance(other, Device)
		distance = ctypes.c_float()
		check(lib.vedaDeviceDistance(ctypes.byref(distance), self.id(), other.id()))
		return distance.value

	def sync(self, stream=None):
		check_stream(stream)
		self.__set_current()
		print(stream)
		if isinstance(stream, int):
			check(lib.vedaStreamSynchronize(stream))
		else:
			check(lib.vedaCtxSynchronize())

	def load(self, name):
		return Module(self, name)

	# TODO: expose attributes i.e. cores, etc.

#-------------------------------------------------------------------------------
def init_device(device_id):
	return Device(device_id)

#-------------------------------------------------------------------------------
def device_cnt():
	cnt = ctypes.c_int()
	check(lib.vedaDeviceGetCount(ctypes.byref(cnt)))
	return cnt.value

# VE Python API? VEPA?
"""
dev	= veda.device(0)
dptr	= dev.new(size)
np	= dptr.to_numpy()
dptr >> np
dptr << np
dptr1 >> dptr2
lib	= dev.load("library.vso")
func	= lib.func("myFunc")

func.set_stream(0) # if necessary
func(arg, arg, arg, arg)
"""