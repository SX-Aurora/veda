#pragma once

#include "veda_device.h"
#include "veda_kernels.h"
#include <unordered_map>
#include <cstring>
#include <cstdlib>

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)

extern "C" VEDAresult vedaMemFree(VEDAdeviceptr vptr);
