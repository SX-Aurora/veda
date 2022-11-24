#pragma once

namespace veda {
//------------------------------------------------------------------------------
struct _0 final : public cpp::Template {	static constexpr int idx = 0;	};
struct _1 final : public cpp::Template {	static constexpr int idx = 1;	};
struct _2 final : public cpp::Template {	static constexpr int idx = 2;	};
struct _3 final : public cpp::Template {	static constexpr int idx = 3;	};
struct _4 final : public cpp::Template {	static constexpr int idx = 4;	};
struct _5 final : public cpp::Template {	static constexpr int idx = 5;	};
struct _6 final : public cpp::Template {	static constexpr int idx = 6;	};
struct _7 final : public cpp::Template {	static constexpr int idx = 7;	};
struct _8 final : public cpp::Template {	static constexpr int idx = 8;	};
struct _9 final : public cpp::Template {	static constexpr int idx = 9;	};

template<auto V>
struct Literal : public cpp::Literal { static constexpr auto value = V; };

//------------------------------------------------------------------------------
}
