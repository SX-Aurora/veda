#pragma once

struct __VEDAstream {
	__VEDAcontext* const ctx;
	inline __VEDAstream(VEDAcontext _ctx) : ctx(_ctx)	{}
	virtual ~__VEDAstream(void)							{}
};