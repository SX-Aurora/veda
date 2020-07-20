#pragma once

#include "veda_device.h"
#include "veda_kernels.h"
#include <unordered_map>
#include <VEDAptr.h>
#include <cstring>
#include <cstdlib>

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)

#ifndef NDEBUG
#define TRACE(...)	printf("[VEDA Device] " __VA_ARGS__)
#error test
#else
#define TRACE(...)
#endif
