#include <veda/internal/api.h>

#define SENSOR_BUFFER_SIZE 16

namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
Context&		Device::ctx		(void) 						{ return m_ctx;							}
VEDAdevice		Device::vedaId		(void) const					{ return m_vedaId;						}
bool			Device::isNUMA		(void) const					{ return numaCnt() != 1;					}
const Device::Cores&	Device::coreIds		(void) const					{ return m_cores;						}
float			Device::powerCurrent	(void) const					{ return readSensor<float>("sensor_12")/1000.0f / numaCnt();	}
float			Device::powerCurrentEdge(void) const					{ return readSensor<float>("sensor_13")/1000.0f / numaCnt();	}
float			Device::powerVoltage	(void) const					{ return readSensor<float>("sensor_8")/1000000.0f;		}
float			Device::powerVoltageEdge(void) const					{ return readSensor<float>("sensor_9")/1000000.0f;		}
int			Device::aveoId		(void) const					{ return m_aveoId;						}
int			Device::cacheL1d	(void) const					{ return m_cacheL1d;						}
int			Device::cacheL1i	(void) const					{ return m_cacheL1i;						}
int			Device::cacheL2		(void) const					{ return m_cacheL2;						}
int			Device::cacheLLC	(void) const					{ return m_cacheLLC;						}
int			Device::clockBase	(void) const					{ return m_clockBase;						}
int			Device::clockMemory	(void) const					{ return m_clockMemory;						}
int			Device::clockRate	(void) const					{ return m_clockRate;						}
int			Device::cores		(void) const					{ return (int)m_cores.size();					}
int			Device::model		(void) const					{ return m_model;						}
int			Device::numaCnt		(void) const					{ return m_numaCnt;						}
int			Device::numaId		(void) const					{ return m_numaId;						}
int			Device::sensorId	(void) const					{ return m_sensorId;						}
int			Device::type		(void) const					{ return m_type;						}
int			Device::versionAbi	(void) const					{ return m_versionAbi;						}
int			Device::versionFirmware	(void) const					{ return m_versionFirmware;					}
size_t			Device::memorySize	(void) const					{ return m_memorySize;						}
size_t			Device::numaMemBlockSize(void) const					{ return m_numaMemBlockSize;					}
uint64_t		Device::readSensor	(const char* file, const bool isHex) const	{ return devices::readSensor(sensorId(), file, isHex);		}

//------------------------------------------------------------------------------
Device::Device(const VEDAdevice vedaId, const int aveoId, const int sensorId, const int numaId, const int numaCnt) :
	m_vedaId		(vedaId),
	m_aveoId		(aveoId),
	m_sensorId		(sensorId),
	m_numaId		(numaId),
	m_numaCnt		(numaCnt),
	m_memorySize		(readSensor<size_t>	("memory_size") * 1024 * 1024 * 1024),
	m_numaMemBlockSize	(readSensor<size_t>	("numa_mem_block_size", true)),
	m_clockRate		(readSensor<int>	("clock_chip")),
	m_clockBase		(readSensor<int>	("clock_base")),
	m_clockMemory		(readSensor<int>	("clock_memory")),
	m_cacheL1d		(readSensor<int>	("cache_l1d")),
	m_cacheL1i		(readSensor<int>	("cache_l1i")),
	m_cacheL2		(readSensor<int>	("cache_l2")),
	m_cacheLLC		(readSensor<int>	("cache_llc") / numaCnt),
	m_versionAbi		(readSensor<int>	("abi_version")),
	m_versionFirmware	(readSensor<int>	("fw_version")),
	m_model			(readSensor<int>	("model")),
	m_type			(readSensor<int>	("type")),
	m_ctx			(*this)
{
	int active = 0;
	if(isNUMA()) {
		char buffer[SENSOR_BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), "numa%i_cores", numaId);
		active = readSensor<int>(buffer, true);
	} else {
		active = readSensor<int>("cores_enable", true);
	}
	ASSERT(active);

	int bit = 1;
	for(int i = 0; i < (sizeof(int)*8); i++, bit <<= 1)
		if(active & bit)
			m_cores.emplace_back(i);
}

//------------------------------------------------------------------------------
float Device::power(void) const {
	auto watts = powerVoltage() * powerCurrent();
	watts += powerVoltageEdge() * powerCurrentEdge();
	watts += 5.0;
	return watts;
}

//------------------------------------------------------------------------------
float Device::coreTemp(const int coreIdx) const {
	if(coreIdx < 0 || coreIdx >= cores())
		VEDA_THROW(VEDA_ERROR_INVALID_VALUE);
	
	auto sensor  = m_cores[coreIdx] + 14; // offseted by 14
	char buffer[SENSOR_BUFFER_SIZE];
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
float Device::distance(const Device& other) const {
	if(vedaId() == other.vedaId()) {
		return 0.0f;
	} else if(aveoId() == other.aveoId()) {
		ASSERT(numaId() != other.numaId());
		return 0.5f;
	}
	return 1.0f;
}

//------------------------------------------------------------------------------
std::string Device::name(void) const {
	std::string name;
	name.reserve(28);
		
	auto version		= model();
	auto cores		= this->cores() * numaCnt();
	auto memory		= memorySize()/1024/1024/1024;
	auto clock_memory	= clockMemory();
	auto clock_rate		= clockRate();

	name.append("NEC SX-Aurora Tsubasa VE");
	name.append([&] {
		if(version == 1) {
			if(memory == 48) {
				if(clock_rate >= 1500)	return clock_memory < 1700 ? "10A" : "10AE";
				else			return clock_memory < 1700 ? "10B" : "10BE";
			} else if(memory == 24) {
				return clock_memory < 1700 ? "10C" : "10CE";
			}
		} else if(version == 2) {
			if(cores == 10)	return "20A";
			if(cores == 8)	return "20B";
		} else if(version == 3) {
			return "30";
		}
		return "???";
	}());

	return name;
}

//------------------------------------------------------------------------------
	}
}
