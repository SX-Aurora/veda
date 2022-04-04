#include "veda/internal.h"

namespace veda {
//------------------------------------------------------------------------------
Context&	Device::ctx		(void) 						{	return m_ctx;								}
VEDAdevice	Device::vedaId		(void) const					{	return m_vedaId;							}
bool		Device::isNUMA		(void) const					{	return m_isNUMA;							}
float		Device::powerCurrent	(void) const					{	return readSensor<float>("sensor_12")/1000.0f / (isNUMA() ? 2 : 1);	}
float		Device::powerCurrentEdge(void) const					{	return readSensor<float>("sensor_13")/1000.0f / (isNUMA() ? 2 : 1);	}
float		Device::powerVoltage	(void) const					{	return readSensor<float>("sensor_8")/1000000.0f;			}
float		Device::powerVoltageEdge(void) const					{	return readSensor<float>("sensor_9")/1000000.0f;			}
int		Device::aveoId		(void) const					{	return m_aveoId;							}
int		Device::cacheL1d	(void) const					{	return m_cacheL1d;							}
int		Device::cacheL1i	(void) const					{	return m_cacheL1i;							}
int		Device::cacheL2		(void) const					{	return m_cacheL2;							}
int		Device::cacheLLC	(void) const					{	return m_cacheLLC;							}
int		Device::clockBase	(void) const					{	return m_clockBase;							}
int		Device::clockMemory	(void) const					{	return m_clockMemory;							}
int		Device::clockRate	(void) const					{	return m_clockRate;							}
int		Device::cores		(void) const					{	return (int)m_cores.size();						}
int		Device::model		(void) const					{	return m_model;								}
int		Device::numaId		(void) const					{	return m_numaId;							}
int		Device::sensorId	(void) const					{	return m_sensorId;							}
int		Device::versionAbi	(void) const					{	return m_versionAbi;							}
int		Device::versionFirmware	(void) const					{	return m_versionFirmware;						}
size_t		Device::memorySize	(void) const					{	return m_memorySize;							}
uint64_t	Device::readSensor	(const char* file, const bool isHex) const	{	return Devices::readSensor(sensorId(), file, isHex);			}

//------------------------------------------------------------------------------
Device::Device(const VEDAdevice vedaId, const int aveoId, const int sensorId, const int numaId) :
	m_vedaId		(vedaId),
	m_aveoId		(aveoId),
	m_sensorId		(sensorId),
	m_numaId		(numaId),
	m_isNUMA		(readSensor<bool>	("partitioning_mode")),
	m_memorySize		(readSensor<size_t>	("memory_size") * 1024 * 1024 * 1024),
	m_clockRate		(readSensor<int>	("clock_chip")),
	m_clockBase		(readSensor<int>	("clock_base")),
	m_clockMemory		(readSensor<int>	("clock_memory")),
	m_cacheL1d		(readSensor<int>	("cache_l1d")),
	m_cacheL1i		(readSensor<int>	("cache_l1i")),
	m_cacheL2		(readSensor<int>	("cache_l2")),
	m_cacheLLC		(readSensor<int>	("cache_llc") / (isNUMA() ? 2 : 1)),
	m_versionAbi		(readSensor<int>	("abi_version")),
	m_versionFirmware	(readSensor<int>	("fw_version")),
	m_model			(readSensor<int>	("model")),
	m_ctx			(*this)
{
	int active = 0;
	if(isNUMA()) {
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "numa%i_cores", numaId);
		active = readSensor<int>(buffer, true);
	} else {
		active = readSensor<int>("cores_enable", true);
	}
	ASSERT(active);

	int bit = 1;
	for(int i = 0; i < 32; i++, bit <<= 1)
		if(active & bit)
			m_cores.emplace_back(i);
}

//------------------------------------------------------------------------------
float Device::coreTemp(const int coreIdx) const {
	if(coreIdx < 0 || coreIdx >= cores())
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	
	auto sensor  = m_cores[coreIdx] + 14; // offseted by 14
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "sensor_%i", sensor);
	return readSensor<float>(buffer)/1000000.0f;	
}

//------------------------------------------------------------------------------
void Device::report(void) const {
	printf("Device #%i [Aveo: %i, Sensor: %i, NUMA: %i, Cores: (", vedaId(), aveoId(), sensorId(), numaId());
	bool isFirst = true;
	for(auto core : m_cores) {
		if(isFirst)	isFirst = false;
		else		printf(", ");
		printf("%i", core);
	}
	printf(")]\n");		
}

//------------------------------------------------------------------------------
}