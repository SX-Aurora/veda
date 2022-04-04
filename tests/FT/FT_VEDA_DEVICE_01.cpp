#include <veda.h>
#include "veda_ft.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

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

	// Get the VE node count through VEDA FT library 
	int ve_node = Get_VE_Node_Count();
	int devcnt;
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("\nTEST CASE ID: FT_VEDA_DEVICE_01\n");
	printf("\nTEST CASE ID: FT_VEDA_DEVICE_010\n");
	if((ve_node == devcnt) || (getenv("VEDA_VISIBLE_DEVICES")!= NULL))
	{
		printf("PASSED\n");
	}
	else
	{
		printf("FAILED\n");
		exit(0);
	}
	CHECK(vedaDeviceGetCount(&devcnt));
	printf("\nTEST CASE ID: FT_VEDA_DEVICE_06\n");
	int physical_id = 0;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGetPhysicalId(&physical_id, dev));
		printf("Physical ID for VEDA device %d is %d\n", dev, physical_id);
	}
	printf("PASSED\n");
	printf("\nTEST CASE ID: FT_VEDA_DEVICE_07\n");
	int aveo_id = 0;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGetAVEOId(&aveo_id, dev));
		printf("AVEO ID for VEDA device %d is %d\n", dev, aveo_id);
	}
	printf("PASSED\n");
	printf("\nTEST CASE ID: FT_VEDA_DEVICE_08\n");
	int numa_id = 0;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGetNUMAId(&numa_id, dev));
		printf("NUMA ID for VEDA device %d is %d\n", dev, numa_id);
	}
	printf("PASSED\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_09\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_15\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_16\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_17\n");
	size_t byte = 0;
	physical_id = 0;
	size_t memory =0;
	int a_id = 0;
	float veda_device_current,veda_device_voltage,veda_device_power,veda_device_current_edge,veda_device_voltage_edge;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceTotalMem(&byte, dev));
		printf("Total Memory for VEDA device %d is %lu\n", dev, byte);
		CHECK(vedaDeviceGetPhysicalId(&physical_id, dev));
		memory = Get_VE_Node_Memory(physical_id);
		memory = memory *1024 *1024 *1024;
		if(memory != byte)
		{
			printf("FT_VEDA_DEVICE_09 FAILED\n");
			exit(0);
		}
		CHECK(vedaDeviceGetAVEOId(&a_id, dev));
		CHECK(vedaDeviceGetCurrent(&veda_device_current, dev));
		CHECK(vedaDeviceGetVoltage(&veda_device_voltage, dev));
		printf("Total Current for VEDA device %d is %f from VEDA %f\n", dev, veda_device_current, Get_VE_Node_Current(a_id, physical_id));
		CHECK(vedaDeviceGetCurrentEdge(&veda_device_current_edge, dev));
		CHECK(vedaDeviceGetVoltageEdge(&veda_device_voltage_edge, dev));
		CHECK(vedaDeviceGetPower(&veda_device_power, dev));
		if((veda_device_current != Get_VE_Node_Current(a_id,physical_id))
				|| (veda_device_current_edge != Get_VE_Node_Current_Edge(a_id,physical_id)))
		{
			printf("Total Current for VEDA device %d is %f from VEDA %f\n", dev, veda_device_current, Get_VE_Node_Current(a_id,physical_id));
			printf("FT_VEDA_DEVICE_16 FAILED\n");
			exit(0);
		}
		if((veda_device_voltage != Get_VE_Node_Voltage(physical_id))
				|| (veda_device_voltage_edge != Get_VE_Node_Voltage_Edge(physical_id)))
		{
			printf("Total Voltage for VEDA device %d is %f from VEDA %f\n", dev, veda_device_voltage, Get_VE_Node_Voltage(physical_id));
			printf("Total Edge Voltage for VEDA device %d is %f from VEDA %f\n", dev, veda_device_voltage_edge, Get_VE_Node_Voltage_Edge(physical_id));
			printf("FT_VEDA_DEVICE_17 FAILED\n");
			exit(0);
		}
		if(veda_device_power != ((Get_VE_Node_Current(a_id, physical_id) * Get_VE_Node_Voltage(physical_id)))
				+ (Get_VE_Node_Current_Edge(a_id, physical_id) * Get_VE_Node_Voltage_Edge(physical_id)) + 5.0)
		{
			printf("FT_VEDA_DEVICE_15 FAILED\n");
			exit(0);
		}
			
	}
	printf("PASSED\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_012\n");
	float distance;
	CHECK(vedaDeviceDistance(&distance, 0,0));
	if(distance != 0)
	{
		printf("FT_VEDA_DEVICE_12 FAILED\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_013\n");
	int aveo_id1;
	for(int dev = 0; dev < devcnt; dev++) {
		for(int dev1 = 0; dev1 < devcnt; dev1++) {
			CHECK(vedaDeviceDistance(&distance, 0,0));
			CHECK(vedaDeviceGetAVEOId(&aveo_id, dev));
			CHECK(vedaDeviceGetAVEOId(&aveo_id1, dev));
			if(aveo_id == aveo_id1)
			{
				if(distance != 0)
				{
					printf("FT_VEDA_DEVICE_13 FAILED\n");
					exit(0);
				}
			}
			else
			{
				if(distance != 0)
				{
					printf("FT_VEDA_DEVICE_13 FAILED\n");
					exit(0);
				}
			}
		}
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_014\n");
	VEDAdevice device;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGet(&device, dev));
	}

	if(VEDA_ERROR_INVALID_DEVICE != vedaDeviceGet(&device, 10000))
	{
		printf("FT_VEDA_DEVICE_14 FAILED\n");
		exit(0);
	}
	int core,cores, temp_diff=0,chip_clock,base_clock,memory_clock;
	int l1d,l1i, l2, llc, model,abi_v,fw_v;
	char device_name[256]={0};
	float each_core_temp = 0.0 ;
	physical_id=0;
	printf("TEST CASE ID: FT_VEDA_DEVICE_020\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_021\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_022\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_023\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_024\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_025\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_026\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_027\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_028\n");
	printf("TEST CASE ID: FT_VEDA_DEVICE_011\n");
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDeviceGetAVEOId(&aveo_id, dev));
		CHECK(vedaDeviceGetNUMAId(&numa_id, dev));
		CHECK(vedaDeviceGetPhysicalId(&physical_id, dev));
		core = Get_VEDA_device_core(aveo_id, numa_id);
		printf("Total Core for VEDA device %d is %lu\n", dev, core);
		CHECK(vedaDeviceGetAttribute(&cores, VEDA_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev));
		CHECK(vedaDeviceGetAttribute(&base_clock, VEDA_DEVICE_ATTRIBUTE_CLOCK_BASE, dev));
		CHECK(vedaDeviceGetAttribute(&chip_clock, VEDA_DEVICE_ATTRIBUTE_CLOCK_RATE, dev));
		CHECK(vedaDeviceGetAttribute(&memory_clock, VEDA_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev));
		CHECK(vedaDeviceGetAttribute(&l1d, VEDA_DEVICE_ATTRIBUTE_L1D_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&l1i, VEDA_DEVICE_ATTRIBUTE_L1I_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&l2, VEDA_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&llc, VEDA_DEVICE_ATTRIBUTE_LLC_CACHE_SIZE, dev));
		CHECK(vedaDeviceGetAttribute(&model, VEDA_DEVICE_ATTRIBUTE_MODEL, dev));
		CHECK(vedaDeviceGetAttribute(&abi_v, VEDA_DEVICE_ATTRIBUTE_ABI_VERSION, dev));
		CHECK(vedaDeviceGetAttribute(&fw_v, VEDA_DEVICE_ATTRIBUTE_FIREWARE_VERSION, dev));
		CHECK(vedaDeviceGetName(device_name, 255, dev));
		if(core != cores)
		{
			printf("FT_VEDA_DEVICE_011 FAILED\n");
			exit(0);
		}
		if(base_clock != Get_VEDA_device_base_clock(physical_id)){
			printf("FT_VEDA_DEVICE_018 FAILED\n");
			exit(0);
		}
		if(chip_clock != Get_VEDA_device_chip_clock(physical_id)){
			printf("FT_VEDA_DEVICE_019 FAILED\n");
			exit(0);
		}
		if(memory_clock != Get_VEDA_device_memory_clock(physical_id)){
			printf("FT_VEDA_DEVICE_021 FAILED\n");
			exit(0);
		}
		if(l1d != Get_VEDA_device_l1d_cache(physical_id)){
			printf("FT_VEDA_DEVICE_022 FAILED\n");
			exit(0);
		}
		if(l1i != Get_VEDA_device_l1i_cache(physical_id)){
			printf("FT_VEDA_DEVICE_023 FAILED\n");
			exit(0);
		}
		if(l2 != Get_VEDA_device_l2_cache(physical_id)){
			printf("FT_VEDA_DEVICE_024 FAILED\n");
			exit(0);
		}
		if(llc != Get_VEDA_device_llc_cache(aveo_id,physical_id)){
			printf("FT_VEDA_DEVICE_025 FAILED\n");
			exit(0);
		}
		if(model != Get_VEDA_device_model(physical_id)){
			printf("FT_VEDA_DEVICE_026 FAILED\n");
			exit(0);
		}
		if(abi_v != Get_VEDA_device_abi_version(physical_id)){
			printf("FT_VEDA_DEVICE_027 FAILED\n");
			exit(0);
		}
		if(fw_v != Get_VEDA_device_fw_version(physical_id)){
			printf("FT_VEDA_DEVICE_028 FAILED\n");
			exit(0);
		}
		for(int each_core = 0; each_core < core; each_core++) {
			CHECK(vedaDeviceGetTemp(&each_core_temp, each_core, dev));
			float core_temp = Get_VEDA_device_core_temp(aveo_id, numa_id, each_core,physical_id);
			temp_diff=abs(core_temp-each_core_temp);
			if(temp_diff>2)
			{
				printf("FT_VEDA_DEVICE_020 FAILED\n");
				exit(0);
			}
		}
	}
	VEDAcontext cont, cont_after_reset;
	int  active;
	uint32_t flags;
	for(int dev = 0; dev < devcnt; dev++) {
		CHECK(vedaDevicePrimaryCtxGetState(dev, &flags, &active));
		if(active != 0)
		{
			printf("FT_VEDA_DEVICE_034 FAILED\n");
			exit(0);
		}
		CHECK(vedaDevicePrimaryCtxRetain(&cont, dev));
		CHECK(vedaDevicePrimaryCtxGetState(dev, &flags, &active));
		if(active != 1)
		{
			printf("FT_VEDA_DEVICE_033 FAILED\n");
			exit(0);
		}
		CHECK(vedaDevicePrimaryCtxReset(dev));
		CHECK(vedaCtxGet(&cont_after_reset, dev));
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_035\n");
	if(VEDA_ERROR_ALREADY_INITIALIZED != vedaInit(0)){
		printf("FT_VEDA_DEVICE_035 FAILED\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_036\n");
	CHECK(vedaExit());
	if(VEDA_ERROR_NOT_INITIALIZED != vedaExit()){
		printf("FT_VEDA_DEVICE_036 FAILED\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_037\n");
	CHECK(vedaInit(0));
	printf("PASSED\n");

	printf("TEST CASE ID: FT_VEDA_DEVICE_038\n");
	if(VEDA_ERROR_INVALID_VALUE != vedaDeviceGetTemp(&each_core_temp, 50, 0))
	{
		printf("FT_VEDA_DEVICE_038 FAILED\n");
		exit(0);
	}
	printf("TEST CASE ID: FT_VEDA_DEVICE_039\n");
	CHECK(vedaDevicePrimaryCtxRelease(0));
	printf("TEST CASE ID: FT_VEDA_DEVICE_040\n");
	CHECK(vedaDevicePrimaryCtxSetFlags(0,0));

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");

	CHECK(vedaExit());
	printf("vedaExit()\n");
	return 0;
}
