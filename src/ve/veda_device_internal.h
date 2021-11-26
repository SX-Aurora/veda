#pragma once

#include "veda_device.h"
#include "veda_device_omp.h"
#include <veda_internal_types.h>
#include <unordered_map>
#include <cstring>
#include <cstdlib>
#include "veda_kernels.h"

#define MAP_EMPLACE(KEY, ...) std::piecewise_construct, std::forward_as_tuple(KEY), std::forward_as_tuple(__VA_ARGS__)

extern "C" VEDAresult vedaMemFree(VEDAdeviceptr vptr);
