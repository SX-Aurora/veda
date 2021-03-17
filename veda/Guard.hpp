#pragma once

namespace veda {
	struct Guard {
		inline	Guard	(void)	{	checkInitialized(); Semaphore::acquire();	}
		inline	~Guard	(void)	{	Semaphore::release();				}
	};
}
