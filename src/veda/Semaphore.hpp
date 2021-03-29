#pragma once

namespace veda {
	class Semaphore {
		static std::atomic<uint32_t>	s_count;
		static std::atomic<int>		s_shutdown;

	public:
		static void acquire	(void);
		static void release	(void);
		static void shutdown	(void);
	};
}