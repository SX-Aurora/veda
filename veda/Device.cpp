#include "veda.h"

namespace veda {
//------------------------------------------------------------------------------
VEDAcontext	Device::ctx		(void) const	{	return m_ctx;						}
VEDAdevice	Device::vedaId		(void) const	{	return m_vedaId;					}
bool		Device::isNUMA		(void) const	{	return m_isNUMA;					}
float		Device::powerCurrent	(void) const	{	return readSensor<float>("sensor_12")/1000.0f;		}
float		Device::powerVoltage	(void) const	{	return readSensor<float>("sensor_8")/1000000.0f;	}
int		Device::aveoId		(void) const	{	return m_aveoId;					}
int		Device::cacheL1d	(void) const	{	return m_cacheL1d;					}
int		Device::cacheL1i	(void) const	{	return m_cacheL1i;					}
int		Device::cacheL2		(void) const	{	return m_cacheL2;					}
int		Device::clockBase	(void) const	{	return m_clockBase;					}
int		Device::clockMemory	(void) const	{	return m_clockMemory;					}
int		Device::clockRate	(void) const	{	return m_clockRate;					}
int		Device::cores		(void) const	{	return (int)m_cores.size();				}
int		Device::memorySize	(void) const	{	return m_memorySize;					}
int		Device::sensorId	(void) const	{	return m_sensorId;					}
int		Device::versionAbi	(void) const	{	return m_versionAbi;					}
int		Device::versionFirmware	(void) const	{	return m_versionFirmware;				}
void		Device::clearCtx	(void)		{	m_ctx = 0;						}

//------------------------------------------------------------------------------
Device::Device(const VEDAdevice vedaId, const int aveoId, const int sensorId) :
	m_vedaId		(vedaId),
	m_aveoId		(aveoId),
	m_sensorId		(sensorId),
	m_memorySize		(readSensor<size_t>	("memory_size") * 1024 * 1024 * 1024),
	m_clockRate		(readSensor<int>	("clock_chip")),
	m_clockBase		(readSensor<int>	("clock_base")),
	m_clockMemory		(readSensor<int>	("clock_memory")),
	m_cacheL1d		(readSensor<int>	("cache_l1d")),
	m_cacheL1i		(readSensor<int>	("cache_l1i")),
	m_cacheL2		(readSensor<int>	("cache_l2")),
	m_versionAbi		(readSensor<int>	("abi_version")),
	m_versionFirmware	(readSensor<int>	("fw_version")),
	m_isNUMA		(readSensor<bool>	("partitioning_mode")),
	m_ctx			(0)
{
	auto active = readSensor<int>("cores_enable");
	int bit = 1;
	for(int i = 0; i < 32; i++, bit <<= 1)
		if(active & bit)
			m_cores.emplace_back(i);
}

//------------------------------------------------------------------------------
Device::~Device(void) {
	if(m_ctx)
		delete m_ctx;
}

//------------------------------------------------------------------------------
void Device::memReport(void) const {
	if(ctx())
		ctx()->memReport();
}

//------------------------------------------------------------------------------
VEDAcontext Device::createCtx(const VEDAcontext_mode mode) {
	if(m_ctx)
		throw VEDA_ERROR_CANNOT_CREATE_CONTEXT;
	m_ctx = new __VEDAcontext(*this, mode);
	return m_ctx;
}

//------------------------------------------------------------------------------
float Device::coreTemp(const int coreIdx) const {
	if(coreIdx >= cores())
		throw VEDA_ERROR_INVALID_VALUE;
	
	auto sensor  = m_cores[coreIdx];
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "sensor_%i", sensor);
	return readSensor<float>(buffer)/1000000.0f;	
}

//------------------------------------------------------------------------------
uint64_t Device::readSensor(const char* file, const bool isHex) const {
	if(file == 0)
		throw VEDA_ERROR_NO_SENSOR_FILE;

	char buffer[128];
	snprintf(buffer, sizeof(buffer), "/sys/class/ve/ve%i/%s", sensorId(), file);

	uint64_t value = 0;
	std::ifstream f(file, std::ios::binary);
	if(!f.good())
		throw VEDA_ERROR_CANT_READ_SENSOR_FILE;
	if(isHex)	f >> std::hex >> value;
	else		f >> value;
	f.close();

	return value;
}

//------------------------------------------------------------------------------
}