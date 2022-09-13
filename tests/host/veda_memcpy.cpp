#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <tuple>

#define CHECK(err) check(err, __FILE__, __LINE__)

uint64_t time_ns(void) {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

static void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char* name = 0;
		vedaGetErrorName(err, &name);
		printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
		assert(false);
		exit(1);
	}
}

static const double	MB[]	= {0.1, 0.2, 0.4, 0.8, 1, 2, 3, 4, 8, 16, 32, 64, 128, 256};
constexpr double	MAX_MB	= 256;
constexpr int		RUNS	= 100;

static void mmalloc(const int dev, VEDAdeviceptr* ptr, const size_t size) {
	VEDAcontext ctx;
	CHECK(vedaDevicePrimaryCtxRetain(&ctx, dev));
	CHECK(vedaCtxPushCurrent(ctx));
	CHECK(vedaMemAlloc(ptr, size));
}

static void mmalloc(const int dev, VEDAhmemptr* ptr, const size_t size) {
	VEDAcontext ctx;
	CHECK(vedaDevicePrimaryCtxRetain(&ctx, dev));
	CHECK(vedaCtxPushCurrent(ctx));
	CHECK(vedaHMemAlloc(ptr, size));
}

static void mmalloc(const int dev, int8_t** ptr, const size_t size) {
	*ptr = new int8_t[size];
}

static void mfree(const int dev, VEDAdeviceptr ptr) {
	CHECK(vedaMemFree(ptr));
	VEDAcontext ctx;
	CHECK(vedaCtxPopCurrent(&ctx));
}

static void mfree(const int dev, VEDAhmemptr ptr) {
	CHECK(vedaHMemFree(ptr));
	VEDAcontext ctx;
	CHECK(vedaCtxPopCurrent(&ctx));
}

static void mfree(const int dev, int8_t* ptr) {
	delete[] ptr;
}

template<typename T>
static std::string device(const int dev) {
	if(std::is_same<T, VEDAdeviceptr>::value) {
		std::string A("ve:");
		A.push_back('0' + dev);
		return A;
	}

	if(std::is_same<T, VEDAhmemptr>::value) {
		std::string A("hm:");
		A.push_back('0' + dev);
		return A;
	}

	return "host";
}

template<typename SRC, typename DST, typename FUNC>
static void exec(const int devA, const int devB, FUNC func) {
	auto A = device<SRC>(devA);
	auto B = device<DST>(devB);

	const auto size = size_t(MAX_MB * 1024 * 1024);
	SRC src; mmalloc(devA, &src, size);
	DST dst; mmalloc(devB, &dst, size);

	for(auto mb : MB) {
		double min = DBL_MAX;
		double max = 0;
		double sum = 0;
		const auto size = size_t(mb * 1024 * 1024);
		for(int i = 0; i < RUNS; i++) {
			auto start = time_ns();
			func(dst, src, size);
			auto end = time_ns();
			auto time = (end - start) / 1000000.0;
			min = std::min(min, time);
			max = std::max(max, time);
			sum += time;
		}
		printf("%s\t%s\t%f\t%f\t%f\t%f\n", A.c_str(), B.c_str(), mb, min, max, sum/RUNS);
	}

	mfree(devA, src);
	mfree(devB, dst);
}

static void traverse(const int dev, const int devCnt) {
	exec<VEDAdeviceptr, int8_t*>	(dev, -1, &vedaMemcpyDtoH);
	exec<int8_t*, VEDAdeviceptr>	(-1, dev, &vedaMemcpyHtoD);
	exec<VEDAhmemptr, int8_t*>	(dev, -1, &vedaHMemcpy);
	exec<int8_t*, VEDAhmemptr>	(-1, dev, &vedaHMemcpy);

	for(int o = dev; o < devCnt; o++) {
		exec<VEDAdeviceptr, VEDAdeviceptr>(dev, o, &vedaMemcpyDtoD);
		if(o == dev)
			exec<VEDAhmemptr, VEDAhmemptr>(dev, o, &vedaHMemcpy);
	}
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));

	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));

	const char* ver = 0;
	CHECK(vedaGetVersion(&ver));
	printf("VERSION: %s\n", ver);

	printf("Src\tDst\tSize\t\tMin\t\tMax\t\tAvg\n");
	for(int dev = 0; dev < devcnt; dev++)
		traverse(dev, devcnt);

	CHECK(vedaExit());
	return 0;
}