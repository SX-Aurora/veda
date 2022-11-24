#include <veda/cpp/api.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <tungl/c.h>

uint64_t time_ns(void) {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

struct A{};

template<int X>
struct B{};

template<typename X>
struct C{};

namespace bla {
	template<typename T>
	struct complex {
		T x, y;
	};
}

inline void verify(VEDAfunction func) {
	try {
		L_INFO(C_INFO "%s" L_RESET, func.kernel);
	} catch(const veda::Exception& e) {
		L_ERROR("%s @ %s (%i)", e.what().begin(), e.file().begin(), e.line());
	}
}

int main(int argc, char** argv) {
	using namespace veda;

	init();
	auto cnt = Device::count();
	for(int i = 0; i < cnt; i++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", i);
		printf("# ------------------------------------- #\n\n");

		Device dev(i);
		auto mod = dev.load("libveda_test.vso");

		for(auto [kernel, pre, post] : {
			std::make_tuple("veda_template", "", "")
			, std::make_tuple("ns::veda::veda_template", "N2ns4veda", "E")
		}) {
			verify(Function::Args<>(mod, kernel));
			verify(Function::Args<int>(mod, kernel));
			verify(Function::Args<float, const char*>(mod, kernel));
			verify(Function::Args<const char* const>(mod, kernel));
			verify(Function::Args<int, int>(mod, kernel));
			verify(Function::Args<double, const char*>(mod, kernel));
			verify(Function::Args<float, const void*>(mod, kernel));
			verify(Template<float>::Args<_0, _0, _0>(mod, kernel));
			verify(Template<void*>::Args<_0, _0>(mod, kernel));
			verify(Template<int, char>::Return<_0>::Args<_0, _1, float, const char*>(mod, kernel));
			verify(Template<char>::Args<_0*, _0*>(mod, kernel));
			verify(Template<void*>::Return<_0>::Args<>(mod, kernel));
			verify(Template<float*>::Return<_0>::Args<>(mod, kernel));
			verify(Template<void*>::Return<void*>::Args<void*, _0, _0, void*>(mod, kernel));
			verify(Template<float*>::Return<void*>::Args<void*, _0, _0, void*>(mod, kernel));
			verify(Template<void*, Literal<5>, char>::Return<_0>::Args<_2, _0>(mod, kernel));
			verify(Function::Args<A>(mod, kernel));
			verify(Function::Args<A, A, A>(mod, kernel));
			verify(Function::Args<A,int>(mod, kernel));
			verify(Function::Args<const A*>(mod, kernel));
			verify(Function::Args<B<5>>(mod, kernel));
			verify(Function::Args<C<int>>(mod, kernel));
			verify(Function::Args<bla::complex<float>>(mod, kernel));
			verify(Function::Args<bla::complex<double>>(mod, kernel));
			verify(Function::Args<bla::complex<float>, bla::complex<float>>(mod, kernel));
			verify(Template<bla::complex<float>>::Args<_0, _0>(mod, kernel));
		}

		dev.destroy();
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	exit();

	return 0;
}