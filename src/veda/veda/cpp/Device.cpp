#include <veda/internal/api.h>
#include <veda/cpp/api.h>

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
bool		Device::isActive		(void) const			{ return m_device->ctx().isActive();			}
bool		Device::isNUMA			(void) const			{ return m_device->isNUMA();				}
float		Device::current			(void) const			{ return m_device->powerCurrent();			}
float		Device::currentEdge		(void) const			{ return m_device->powerCurrentEdge();			}
float		Device::distance		(const Device& other) const	{ return m_device->distance(*other.m_device);		}
float		Device::power			(void) const			{ return m_device->power();				}
float		Device::temp			(const int coreIdx) const	{ return m_device->coreTemp(coreIdx);			}
float		Device::voltage			(void) const			{ return m_device->powerVoltage();			}
float		Device::voltageEdge		(void) const			{ return m_device->powerVoltageEdge();			}
int		Device::abi			(void) const			{ return m_device->versionAbi();			}
int		Device::aveoId			(void) const			{ return m_device->aveoId();				}
int		Device::cacheL1d		(void) const			{ return m_device->cacheL1d();				}
int		Device::cacheL1i		(void) const			{ return m_device->cacheL1i();				}
int		Device::cacheL2			(void) const			{ return m_device->cacheL2();				}
int		Device::cacheLLC		(void) const			{ return m_device->cacheLLC();				}
int		Device::clockBase		(void) const			{ return m_device->clockBase();				}
int		Device::clockMemory		(void) const			{ return m_device->clockMemory();			}
int		Device::clockRate		(void) const			{ return m_device->clockRate();				}
int		Device::cores			(void) const			{ return m_device->cores();				}
int		Device::count			(void)				{ return internal::devices::count();			}
int		Device::firmware		(void) const			{ return m_device->versionFirmware();			}
int		Device::model			(void) const			{ return m_device->model();				}
int		Device::numaCnt			(void) const			{ return m_device->numaCnt();				}
int		Device::numaId			(void) const			{ return m_device->numaId();				}
int		Device::physicalId		(void) const			{ return m_device->sensorId();				}
int		Device::singleToDoublePerfRatio	(void) const			{ return 2;						}
int		Device::streamCnt		(void) const			{ return mode() == VEDA_CONTEXT_MODE_OMP ? 1 : cores();	}
int		Device::vedaId			(void) const			{ return m_device->vedaId();				}
size_t		Device::numaMemBlockSize	(void) const			{ return m_device->numaMemBlockSize();			}
size_t		Device::totalMem		(void) const			{ return m_device->memorySize();			}
size_t		Device::usedMem			(void) const			{ return ctx()->memUsed();				}
std::string	Device::name			(void) const			{ return m_device->name();				}

//------------------------------------------------------------------------------
internal::Context* Device::ctx(const bool checkIsActive) const {
	auto& ctx = m_device->ctx();
	if(!ctx.isActive())
		ctx.init(m_mode);
	return &ctx;
}

//------------------------------------------------------------------------------
Device::Device(VEDAcontext ctx) :
	m_device([ctx] {
		if(ctx == 0)
			VEDA_THROW(VEDA_ERROR_INVALID_ARGS);
		return &ctx->device();
	}()),
	m_mode(ctx->mode())
{}

//------------------------------------------------------------------------------
Device::Device(int ordinal, const VEDAcontext_mode mode) :
	m_device(&internal::devices::get(ordinal)),
	m_mode	(mode)
{}

//------------------------------------------------------------------------------
const Device::Cores& Device::coreIds(void) const {
	return m_device->coreIds();
}

//------------------------------------------------------------------------------
void Device::destroy(void) const {
	auto c = ctx();
	veda::internal::contexts::remove(c);
	c->destroy();
}

//------------------------------------------------------------------------------
Device	Device::getCurrent	(void)					{ return internal::contexts::current();	}
Device	Device::popCurrent	(void)					{ return internal::contexts::pop();	}
Module	Device::load		(const std::string_view name) const	{ return ctx()->moduleLoad(name);	}
void	Device::pushCurrent	(void) const				{ veda::internal::contexts::push(ctx());}
void	Device::setCurrent	(void) const				{ veda::internal::contexts::set(ctx());	}

//------------------------------------------------------------------------------
void Device::sync(const StreamId stream) const {
	if(stream == -1)	ctx()->sync();
	else			ctx()->sync(stream);
}

//------------------------------------------------------------------------------
VEDAcontext_mode Device::mode(void) const {
	auto& ctx = m_device->ctx();
	return ctx.isActive() ? ctx.mode() : m_mode;
}

//------------------------------------------------------------------------------
VEDAresult Device::query(const StreamId stream) const {
	return ctx()->query(stream);
}

//------------------------------------------------------------------------------
template<> VEDA Device::alloc_<VEDA>(const size_t size, const StreamId stream) const {
	auto ptr = ctx()->memAlloc(size, stream);
	L_TRACE("%p = veda::VEDA::malloc(%llu, %i)", ptr, size, stream);
	return ptr;
}

//------------------------------------------------------------------------------
template<> HMEM Device::alloc_<HMEM>(const size_t size, const StreamId stream) const {
	auto ptr = ctx()->hmemAlloc(size);
	L_TRACE("%p = veda::HMEM::malloc(%llu, %i)", ptr, size, stream);
	return ptr;
}

//------------------------------------------------------------------------------
template<> void Device::free_<VEDA>(const VEDA ptr, const StreamId stream) const {
	L_TRACE("veda::VEDA::free(%p, %i)", ptr, stream);
	return ctx()->memFree(ptr, stream);
}

//------------------------------------------------------------------------------
template<> void Device::free_<HMEM>(const HMEM ptr, const StreamId stream) const {
	L_TRACE("veda::HMEM::free(%p, %i)", ptr);
	internal::hmemfree(ptr);
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<VEDA, VEDA>(VEDA dst, VEDA src, const size_t size, const StreamId stream) const {
	internal::memcpy(dst, src, size, stream);
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<void*, VEDA>(void* dst, VEDA src, const size_t size,const StreamId stream) const {
	ctx()->memcpyD2H(dst, src, size, stream);
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<VEDA, void*>(VEDA dst, void* src, const size_t size, const StreamId stream) const {
	ctx()->memcpyH2D(dst, src, size, stream);
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<HMEM, HMEM>(HMEM dst, HMEM src, const size_t size, const StreamId stream) const {
	internal::hmemcpy(dst, src, size);	
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<void*, HMEM>(void* dst, HMEM src, const size_t size, const StreamId stream) const {
	internal::hmemcpy((HMEM)dst, src, size);
}

//------------------------------------------------------------------------------
template<> void Device::memcpy_<HMEM, void*>(HMEM dst, void* src, const size_t size, const StreamId stream) const {
	internal::hmemcpy(dst, (HMEM)src, size);
}

//------------------------------------------------------------------------------
template<> void Device::memset_(HMEM ptr, const uint16_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(HMEM ptr, const uint32_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(HMEM ptr, const uint64_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(HMEM ptr, const uint8_t  value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(VEDA ptr, const uint16_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(VEDA ptr, const uint32_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(VEDA ptr, const uint64_t value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}
template<> void Device::memset_(VEDA ptr, const uint8_t  value, const size_t cnt, const StreamId stream) const	{	ctx()->memset(ptr, value, cnt, stream);	}

//------------------------------------------------------------------------------
	}
}