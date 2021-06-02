#include <veda.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#define CHECK(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char* name = 0;
		vedaGetErrorName(err, &name);
		printf("Error: %i %s @ %s (%i)\n", err, name, file, line);
		assert(false);
		exit(1);
	}
}

int main(int argc, char** argv) {
	CHECK(vedaInit(0));
	
	const char* aveo = 0;
	CHECK(vedaDriverGetVersion(&aveo));
	printf("╔ veda-smi ═════════════════════════════════════════════════════════════════════╗\n");
	printf("║ VEDA Version: %-10s AVEO Version: %-10s                             ║\n", VEDA_VERSION, aveo);
	printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n\n");
	
	int cnt = 0;
	CHECK(vedaDeviceGetCount(&cnt));

	char name[256];

	for(int dev = 0; dev < cnt; dev++) {
		CHECK(vedaDeviceGetName(name, sizeof(name), dev));
		size_t total = 0;
		CHECK(vedaDeviceTotalMem(&total, dev));

		int physical = 0, aveo = 0, numa = 0;
		CHECK(vedaDeviceGetPhysicalId	(&physical, dev));
		CHECK(vedaDeviceGetAVEOId	(&aveo, dev));
		CHECK(vedaDeviceGetNUMAId	(&numa, dev));

		int cores, clock_rate, clock_base, clock_memory, l1d, l1i, l2, llc, abi, firmware;
		CHECK(vedaDeviceGetAttribute(&cores, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev));
		CHECK(vedaDeviceGetAttribute(&clock_rate,	VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, dev));
		CHECK(vedaDeviceGetAttribute(&clock_base,	VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, dev));
		CHECK(vedaDeviceGetAttribute(&clock_memory,	VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev));
		CHECK(vedaDeviceGetAttribute(&l1d,		VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&l1i,		VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&l2,		VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&llc,		VEDA_DEVICE_ATTRIBUTE_LLC_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&abi,		VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, dev));
		CHECK(vedaDeviceGetAttribute(&firmware,		VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, dev));

		printf("┌── #%-2i %s ", dev, name);
		size_t len = strlen(name) + 8;
		for(size_t i = 0; i < (79-len); i++)
			printf("─");
		printf("┐\n");
		printf("  ┌ Physical: %1i.%1i\n", physical, numa);
		printf("  ├ AVEO:     %1i.%1i\n", aveo, numa);
		printf("  ├ Clock:    current: %i MHz, base: %i MHz, memory: %i MHz\n", clock_rate, clock_base, clock_memory);
		printf("  ├ Firmware: %i\n", firmware);
		printf("  ├ Memory:   %llu MiB\n", total/1024/1024);
		printf("  ├ Cache:    LLC: %ikB, L2: %ikB, L1d: %ikB, L1i: %ikB\n", llc, l2, l1d, l1i);
		printf("  ├ Temp:     ");
		
		for(int i = 0; i < cores; i++) {
			float temp = 0;
			CHECK(vedaDeviceGetTemp(&temp, i, dev));
			printf("%2.1f°C ", temp);
		}
		printf("\n");

		float power = 0, current = 0, voltage = 0;
		CHECK(vedaDeviceGetPower(&power, dev));
		CHECK(vedaDeviceGetCurrent(&current, dev));
		CHECK(vedaDeviceGetVoltage(&voltage, dev));
		printf("  └ Power:    %3.1fW (%3.1fV, %3.1fA)\n", power, voltage, current);
		printf("└───────────────────────────────────────────────────────────────────────────────┘\n\n");
	}

	CHECK(vedaExit());
	return 0;
}