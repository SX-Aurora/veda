#pragma once

namespace veda {
	namespace internal {
		namespace devices {
//------------------------------------------------------------------------------
Device&		get		(const VEDAdevice device);
Device&		get		(const VEDAdeviceptr ptr);
Device&		get		(const VEDAhmemptr ptr);
Device*		tryGet		(const VEDAhmemptr ptr);
Device*		tryGet		(veo_proc_handle* handle);
int		count		(void);
uint64_t	readSensor	(const int sensorId, const char* file, const bool isHex);
void		init		(void);
void		map		(const int aveoProcId, Device& dev);
void		memReport	(void);
void		report		(void);
void		shutdown	(void);
void		unmap		(const int aveoProcId);

//------------------------------------------------------------------------------
		}
	}
}