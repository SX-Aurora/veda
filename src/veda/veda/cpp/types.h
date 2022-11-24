#pragma once

namespace veda {
//------------------------------------------------------------------------------
namespace internal {
	class Device;
	class Context;
	class Module;
}

namespace cpp {
	class Device;
}

using StreamId		= VEDAstream;
using Module		= internal::Module*;
using Context		= internal::Context*;
using Stack		= VEDAstack;
using VEDA		= VEDAdeviceptr;
using HMEM		= VEDAhmemptr;

//------------------------------------------------------------------------------
}