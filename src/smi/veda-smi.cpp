#include <veda/cpp/api.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

int main(int argc, char** argv) {
	using namespace veda;
	try {
		init();
	
		auto aveo = driverVersion().begin();
		printf("╔ veda-smi ═════════════════════════════════════════════════════════════════════╗\n");
		printf("║ VEDA Version: %-10s AVEO Version: %-10s Arch: ve%i                   ║\n", VEDA_VERSION, aveo, architecture());
		printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n\n");
		
		auto cnt = Device::count();

		for(int i = 0; i < cnt; i++) {
			Device dev(i);

			const auto name = dev.name();
			printf("┌── #%-2i %s ", dev.vedaId(), name.c_str());
			size_t len = name.size() + 8;
			for(size_t i = 0; i < (79-len); i++)
				printf("─");
			printf("┐\n");
			printf("  ┌ IDs:      VEDA: %1i, AVEO: %1i, Physical: %1i", dev.vedaId(), dev.aveoId(), dev.physicalId());
			if(dev.isNUMA())
				printf(", NUMA: %1i/%1i", dev.numaId(), dev.numaCnt());
			printf("\n");
			printf("  ├ Cores:    %i\n", dev.cores());
			printf("  ├ Clock:    current: %i MHz, base: %i MHz, memory: %i MHz\n", dev.clockRate(), dev.clockBase(), dev.clockMemory());
			printf("  ├ Firmware: %i\n", dev.firmware());
			printf("  ├ Memory:   %llu MiB", dev.totalMem()/1024/1024);
			if(dev.isNUMA())
				printf(", NUMA MemBlockSize: %llu MiB", dev.numaMemBlockSize()/1024/1024);
			printf("\n");
			printf("  ├ Cache:    LLC: %ikB, L2: %ikB, L1d: %ikB, L1i: %ikB\n", dev.cacheLLC(), dev.cacheL2(), dev.cacheL1d(), dev.cacheL1i());
			printf("  ├ Temp:     ");
			
			auto cores = dev.cores();
			for(int i = 0; i < cores; i++)
				printf("%2.1f°C ", dev.temp(i));
			printf("\n");
			printf("  └ Power:    %3.1fW (AUX: %3.1fV, %3.1fA Edge: %3.1fV, %3.1fA)\n", dev.power(), dev.voltage(), dev.current(), dev.voltageEdge(), dev.currentEdge());
			printf("└───────────────────────────────────────────────────────────────────────────────┘\n\n");
		}

		exit();
	} catch(const Exception& e) {
		printf("[Error] %s @ %s (%i)", e.what().begin(), e.file().begin(), e.line());
	}
	return 0;
}
