#pragma once

namespace veda {
	struct Stream {
		veo_thr_ctxt*				ctx;
		std::vector<std::tuple<uint64_t, bool>>	calls;
	};
}