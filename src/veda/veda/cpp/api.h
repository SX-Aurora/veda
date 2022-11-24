#pragma once

#include "../api.h"
#include <tuple>
#include <string_view>
#include <memory>
#include <type_traits>
#include <vector>
#include <mutex>
#include <map>

#include "types.h"
#include "Exception.h"
#include "Ptr.h"
#include "Args.h"
#include "Future.h"
#include "CFunction.h"
#include "Template.h"
#include "HostFunction.h"
#include "Device.h"
#include "Profiler.h"
#include "static.h"

namespace veda {
	using cpp::Device;
	using cpp::Exception;
}