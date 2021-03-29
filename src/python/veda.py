import numpy as np
import ctypes
import os
import enum

#-------------------------------------------------------------------------------
class VEDAdevice_attribute(enum.Enum):
	VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE
	VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE
	VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT
	VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE
	VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE
	VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE
	VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE
	VEDA_DEVICE_ATTRIBUTE_ABI_VERSION
	VEDA_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO
	VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION

#-------------------------------------------------------------------------------
path	= os.path.dirname(__file__)
path	= os.path.abspath(os.path.abspath(path) + "/")
lib	= ctypes.cdll.LoadLibrary(os.path.join(path, 'lib64/libveda.so'))

# TODO: do function bodies
lib.sol_device_used_memory.argtypes	= [ctypes.c_int, ctypes.c_int]
lib.sol_device_used_memory.restype	= ctypes.c_size_t

#-------------------------------------------------------------------------------
def check(err):
	if err == 0:
		return
	name = ctypes.c_char_p()
	msg  = ctypes.c_char_p()
	lib.vedaGetErrorName(err, name.byref())
	lib.vedaGetErrorString(err, msg.byref())
	raise Exception('[VEDA#{}] {}: {}'.format(err, name, msg))

#-------------------------------------------------------------------------------
lib.vedaInit(0)

#-------------------------------------------------------------------------------
def check_stream(stream):
	assert stream is None or isinstance(stream, int)

#-------------------------------------------------------------------------------
def check_copy(dst, src, bytes):
	assert isinstance(dst, VPtr)
	assert bytes  is None or isinstance(bytes, int)
	if bytes is None:
		bytes = dst.size()
	assert bytes > 0
	assert bytes <= dst.size()
	if isinstance(src, VPtr):
		assert bytes <= src.size()
	elif isinstance(src, np.ndarray):
		assert bytes <= src.nbytes
	return bytes

#-------------------------------------------------------------------------------
def get_numpy_ptr(array):
	assert isinstance(array, np.ndarray)
	return array.__array_interface__['data'][0]

#-------------------------------------------------------------------------------
class VPtr:
	def __init__(self, ptr, size):
		self.m_size	= ctypes.c_ulonglong(size)
		self.m_ptr	= ctypes.c_ulonglong(ptr)

	def __del__(self):
		check(lib.vedaMemFree(self.ptr()))

	def __add__(self, offset):
		return VPtr(self.ptr() + offset, self.size())

	def __sub__(self, offset):
		return VPtr(self.ptr() - offset, self.size())

	def __iadd__(self, offset):
		self.m_ptr.value += offset

	def __isub__(self, offset):
		self.m_ptr.value -= offset

	def size(self):
		if self.m_size.value == 0:
			base = ctypes.c_ulonglong()
			lib.vedaMemGetAddressRange(base.byref(), self.m_size.byref(), self.ptr())
		return self.m_size.value

	def ptr(self):
		return self.m_ptr

	def copy_d2d(self, src, stream=None, bytes=None):
		check_stream(stream)
		assert isinstance(src, VPtr)
		bytes = check_copy(self, src, bytes)
		if stream is None:
			check(lib.vedaMemCpyD2D(self.ptr(), src.ptr(), bytes))
		else:
			check(lib.vedaMemcpyD2DAsync(self.ptr(), src.ptr(), bytes, stream))

	def copy_h2d(self, src, stream=None, bytes=None):
		check_stream(stream)
		ptr	= get_numpy_ptr(src)
		bytes	= check_copy(self, src, bytes)
		if stream is None:
			check(lib.vedaMemCpyH2D(self.ptr(), ptr, bytes))
		else:
			check(lib.vedaMemcpyH2DAsync(self.ptr(), ptr, bytes, stream))

	def copy_d2h(self, dst, stream=None, bytes=None):
		check_stream(stream)
		ptr	= get_numpy_ptr(dst)
		bytes	= check_copy(self, dst, bytes)
		if stream is None:
			check(lib.vedaMemCpyD2H(ptr, self.ptr(), bytes))
		else:
			check(lib.vedaMemcpyD2HAsync(ptr, self.ptr(), bytes, stream))

	def __ilshift__(self, src): # dst <<= src, synchronous
		if isinstance(src, np.ndarray):
			self.copy_h2d(src)
		elif isinstance(src, VPtr):
			self.copy_d2d(src)
		else:
			raise Exception("[VEDA] Expected numpy.ndarray or veda.VPtr but got {}".format(type(src)))

	def __irshift__(self, dst): # src >>= dst, synchronous
		if isinstance(dst, np.ndarray):
			self.copy_d2h(dst)
		elif isinstance(dst, VPtr):
			self.copy_d2d(dst)
		else:
			raise Exception("[VEDA] Expected numpy.ndarray or veda.VPtr but got {}".format(type(src)))

	def __lshift__(self, src): # dst << src, asynchronous
		if isinstance(src, np.ndarray):
			self.copy_h2d(src, stream=0)
		elif isinstance(src, VPtr):
			self.copy_d2d(src, stream=0)
		else:
			raise Exception("[VEDA] Expected numpy.ndarray or veda.VPtr but got {}".format(type(src)))

	def __rshift__(self, dst): # src >> dst, asynchronous
		if isinstance(dst, np.ndarray):
			self.copy_d2h(dst, stream=0)
		elif isinstance(dst, VPtr):
			self.copy_d2d(dst, stream=0)
		else:
			raise Exception("[VEDA] Expected numpy.ndarray or veda.VPtr but got {}".format(type(src)))

	def to_numpy(self, stream=None):
		dst = np.empty(self.size(), dtype=np.byte)
		self.copy_d2h(dst, stream=stream)

#-------------------------------------------------------------------------------
class Function:
	def __init__(self, mod, name):
		assert isinstance(name, str)
		self.m_mod	= mod
		self.m_name	= name
		self.m_func	= ctypes.ulonglong()
		cname		= name.encode('UTF-8')

		self.m_mod.m_device.__set_current()
		check(lib.vedaModuleGetFunction(self.m_func.byref(), self.m_mod.m_lib, cname))

	def __call__(self, *args, stream=None):
		vargs = c_void_p
		check(lib.vedaArgsCreate(vargs.byref()))

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
		check(lib.vedaModuleLoad(self.m_lib.byref(), cname))

	def __exit__(self):
		self.m_device.__set_current()
		check(lib.vedaModuleUnload(self.m_lib))

	def get(self, name):
		return Function(self, name)

#-------------------------------------------------------------------------------
class Device:
	def __init__(self, device_id):
		self.m_ctx		= ctypes.c_void_p()
		self.m_device_id	= device_id
		check(lib.vedaCtxCreate(self.m_ctx.byref()))

	def __del__(self):
		check(lib.vedaCtxDestroy(self.m_ctx))

	def id(self):
		return self.m_device_id

	def __set_current(self):
		check(lib.vedaCtxSetCurrent(self.m_ctx))
    
	def stream_cnt(self):
		cnt = ctypes.c_int()
		check(lib.vedaCtxStreamCnt(cnt.byref))
		return cnt.value

	def new(self, size):
		pass

	def name(self):
		name = ctypes.c_char(128)
		check(lib.vedaDeviceGetName(name.byref(), 128, self.id()))
		return name.decode('utf-8')

	def mem_total(self):
		total = ctypes.c_ulonglong()
		check(lib.vedaDeviceTotalMem(total.byref(), self.id()))
		return total.value

	def mem_used(self):
		total, free = ctypes.c_ulonglong(), ctypes.c_ulonglong()
		self.__set_current()
		check(lib.vedaMemGetInfo(free.byref(), total.byref()))
		return total.value - free.value

	def mem_free(self):
		total, free = ctypes.c_ulonglong(), ctypes.c_ulonglong()
		self.__set_current()
		check(lib.vedaMemGetInfo(free.byref(), total.byref()))
		return free.value

	def power(self):
		power = ctypes.c_float()
		check(lib.vedaDeviceGetPower(power.byref(), self.id()))
		return power.value

	def temp(self, core):
		assert isinstance(core, int)
		temp = ctypes.c_float()
		check(lib.vedaDeviceGetTemp(temp.byref(), core, self.id()))
		return temp.value

	def __attribute(self, attr):
		assert isinstance(attr, int)
		value = ctypes.c_int()
		check(lib.vedaDeviceGetAttribute(value.byref(), attr, self.id()))
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
		distance = ctypes.float()
		check(lib.vedaDeviceDistance(distance.byref(), self.id(), other.id()))
		return distance.value

	def sync(self, stream=None):
		check_stream(stream)
		self.__set_current()
		if isinstance(stream, int):
			check(lib.vedaCtxSynchronize())
		else:
			check(lib.vedaStreamSynchronize(stream))

	def load(self, name):
		return Module(self, name)

	# TODO: expose attributes i.e. cores, etc.

#-------------------------------------------------------------------------------
def device(device_id):
	return Device(device_id)

#-------------------------------------------------------------------------------
def device_cnt():
	cnt = ctypes.c_int()
	check(lib.vedaDeviceGetCount(cnt.byref()))
	return cnt.value


# VE Python API? VEPA?

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

