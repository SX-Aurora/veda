#pragma once

#include "Template_internal.h"
#include "Template_types.h"

namespace veda {
//------------------------------------------------------------------------------
template<typename... T>
class Template final {
	template<typename R> static constexpr auto is_veda	= std::is_base_of<cpp::Base<VEDA>, R>::value ? 1 : 0;
	template<typename R> static constexpr auto is_hmem	= std::is_base_of<cpp::Base<HMEM>, R>::value ? 2 : 0;
	template<typename R> static constexpr auto is_tmpl	= std::is_base_of<cpp::Template,   R>::value ? 3 : 0;

	template<int type, typename R>	struct clear_;
	template<typename R>		struct clear_<0, R> {	using type = R;									};
	template<typename R>		struct clear_<1, R> {	using type = VEDA;								};
	template<typename R>		struct clear_<2, R> {	using type = HMEM;								};
	template<typename R>		struct clear_<3, R> {	using type = typename std::tuple_element<R::idx, std::tuple<T...>>::type;	};

	template<typename R>
	using clear = clear_<is_veda<R> + is_hmem<R> + is_tmpl<R>, R>;

public:
	template<typename R>
	struct Return final {
		#include "Template_impl.h"
	};

	template<typename... A>
	using Args = typename Return<void>::template Args<A...>;
};

namespace Function {
	template<typename R>
	struct Return {
		template<typename... A>
		using Args = typename Template<>::Return<R>::template Args<A...>;
	};

	template<typename... A>
	using Args = typename Template<>::Return<void>::template Args<A...>;
}

//------------------------------------------------------------------------------
}