#include <veda/cpp/api.h>
#include <tungl/c.h>
#include <cstring>
#include <list>

struct complex {
	float a, b;
};

static uint64_t host_func(int a, float b, void* c) {
	L_WARN("%i %f %p", a, b, c);
	return 0;
}

int main(int argc, char** argv) {
	using namespace veda;
	try {
		init();
		auto cnt = Device::count();
		L_INFO("Device Count: %i", cnt);

		std::list<Ptr<VEDA, float>> survivors;

		for(int i = 0; i < cnt; i++) {
			Device dev(i);
			L_INFO("\n-- Device#%i ------------------------", i);

			L_INFO("current %f",			dev.current());
			L_INFO("currentEdge %f",		dev.currentEdge());
			L_INFO("power %f",			dev.power());
			for(int c = 0; c < dev.cores(); c++)
				L_INFO("temp(%i) %f",		c, dev.temp(c));
			L_INFO("voltage %f",			dev.voltage());
			L_INFO("voltageEdge %f",		dev.voltageEdge());
			L_INFO("aveoId %i",			dev.aveoId());
			L_INFO("cacheL1d %i",			dev.cacheL1d());
			L_INFO("cacheL1i %i",			dev.cacheL1i());
			L_INFO("cacheL2 %i",			dev.cacheL2());
			L_INFO("cacheLLC %i",			dev.cacheLLC());
			L_INFO("clockBase %i",			dev.clockBase());
			L_INFO("clockMemory %i",		dev.clockMemory());
			L_INFO("clockRate %i",			dev.clockRate());
			L_INFO("cores %i",			dev.cores());
			L_INFO("model %i",			dev.model());
			L_INFO("numaId %i",			dev.numaId());
			L_INFO("physicalId %i",			dev.physicalId());
			L_INFO("singleToDoublePerfRatio %i",	dev.singleToDoublePerfRatio());
			L_INFO("streamCnt %i",			dev.streamCnt());
			L_INFO("vedaId %i",			dev.vedaId());
			L_INFO("abi %i",			dev.abi());
			L_INFO("firmware %i",			dev.firmware());
			L_INFO("totalMem %lluB",		dev.totalMem());
			L_INFO("name %s",			dev.name().c_str());

			size_t cnt	= 2048;
			size_t size	= cnt * sizeof(float);
			auto mod	= dev.load("libveda_test.vso");
			auto func	= CFunction::Return<uint64_t>(mod, "ve_test");
			auto cpp	= Function::Return<int64_t>::Args<int*, VEDAdeviceptr, size_t, complex>(mod, "ve_test_cpp");
			auto cpp2	= Function::Args<int*, VEDAdeviceptr, size_t, complex>(mod, "ve_test_cpp");
			auto tmpl	= Template<float>::Return<_0>::Args<_0, int*, double>(mod, "ve_test_tmpl");
			auto t		= tmpl(3.14159f, 0, 3.14159);
			L_WARN("T: %f", float(t));

			auto hf		= HostFunction(dev, &host_func);
			hf[0](0, 0.3f, 0);

			auto host	= (float*)malloc(size);
			memset(host, 0, size);

			auto A = dev.alloc<float>(cnt);
			auto B = dev.alloc<float>(0);
			auto C = dev.alloc<float>(0);
			auto D = dev.alloc<float>(0);

			complex cmplx = {1.0, 2.0};

			A.memset(1.0);
			A.to(host, cnt);
			
			auto a = func	(0, B, cnt);			// extern "C" with return_value
			auto b = cpp[0]	((int*)0, C, cnt, cmplx);	// CPP, in stream == 1 with return_value
			cpp((int*)0, D, cnt, cmplx);			// CPP without return_value
			B.to(host);
			A[15].to(B[15]);
			C[23].from(B[15], 15);

			L_WARN("A: %016llX", a.get());
			L_WARN("B: %016llX", b.get());

			// HMEM
			auto H = dev.alloc<float, HMEM>(cnt);
			A.to(H);
			A.from(H);
			H.to(A);
			H.from(A);

			survivors.emplace_back(A);

			free(host);
			L_INFO("-- End#%i ---------------------------", i);
		}

		survivors.clear();

		exit();
	} catch(const Exception& e) {
		L_ERROR("%s @ %s (%i)", e.what().begin(), e.file().begin(), e.line());
	}

	return 0;
}