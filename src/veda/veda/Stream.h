#pragma once

namespace veda {
	struct Stream {
		veo_thr_ctxt*						ctx;
		std::vector<std::tuple<uint64_t, bool, uint64_t*>>	calls;
		std::mutex						mutex;

		inline Stream(void)	: ctx(0) {}
		inline Stream(Stream&&)	: ctx(0) {}
	};
}