#include <vera.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <cstring>
#include <chrono>

#define NOW() std::chrono::high_resolution_clock::now()
#define time(start, end) (std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1000000.0)
#define CHECK(err) check(err, __FILE__, __LINE__)

void check(veraError_t err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		printf("Error: %i %s @ %s (%i)\n", err, veraGetErrorName(err), file, line);
		assert(false);
		exit(1);
	}
}
void MyCallback(VEDAstream stream, veraError_t status, void *data){
 printf("Inside Mycallback %d ", (size_t)data);
}
void hostFunc(VEDAstream stream, veraError_t status, void *data){
 printf("Inside hostFunc %d\n", (size_t)data);
}

int main(int argc, char** argv) {
	int devcnt;
        printf("\nTEST CASE ID: FT_VERA_08:");
	CHECK(veraGetDeviceCount(&devcnt));
        printf("PASSED\n");
	printf("veraGetDeviceCount(%i)\n", devcnt);
        printf("\nTEST CASE ID: FT_VERA_01:");
        CHECK(veraInit());
        printf("PASSED\n");
	VEDAdevice id;
        printf("\nTEST CASE ID: FT_VERA_03:");
        veraGetDevice(&id);
        if(id != 0)
        {
        	printf("FAILED\n");
                exit(1);
        }

        printf("\nTEST CASE ID: FT_VERA_09:");
        const char* ver_str=NULL;
        veraRuntimeGetVersion(&ver_str);
	if( ver_str != NULL)
        	printf("PASSED\n");
	else{
        	printf("FAILED\n");
                exit(1);
	}
        printf("\nveraRuntimeGetVersion=%s\n",ver_str);
	veraFunction_t func_ex;

	for(int dev = 0; dev < devcnt; dev++) {
		printf("\n# ------------------------------------- #\n");
		printf("# RUNNING TESTS ON %i                    #\n", dev);
		printf("# ------------------------------------- #\n\n");

        	printf("\nTEST CASE ID: FT_VERA_02:");
		CHECK(veraSetDevice(dev));
        	printf("PASSED\n");
		printf("veraSetDevice(%i)\n", dev);

		veraDeviceProp prop;
        	printf("\nTEST CASE ID: FT_VERA_04:");
		CHECK(veraGetDeviceProperties(&prop, dev));
        	printf("PASSED\n");
		printf("veraGetDeviceProperties(\"%s\", %i)\n", prop.name, 0);

        	printf("\nTEST CASE ID: FT_VERA_07:");
		for(int i = 0; i < prop.multiProcessorCount; i++) {
			float temp;
			CHECK(veraDeviceGetTemp(&temp, i, dev));
			printf("veraDeviceGetTemp(%f, %i, %i)\n", temp, i, dev);
		}
        	printf("PASSED\n");

		size_t memTotal = 0, memFree = 0;
		CHECK(veraMemGetInfo(&memFree, &memTotal));
		printf("veraMemGetInfo(%llu, %llu)\n", memFree, memTotal);

		float power;
        	printf("\nTEST CASE ID: FT_VERA_06:");
		CHECK(veraDeviceGetPower(&power, 0));
		printf("veraDeviceGetPower(%f, %i)\n", power, 0);
        	printf("PASSED\n");

		int value;
        	printf("\nTEST CASE ID: FT_VERA_05:");
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrClockRate, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrClockRate, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrClockBase, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrClockBase, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrMultiProcessorCount, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrMultiProcessorCount, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrMemoryClockRate, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrMemoryClockRate, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL1DCacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL1DCacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL1ICacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL1ICacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrL2CacheSize, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrL2CacheSize, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrABIVersion, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrABIVersion, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrSingleToDoublePrecisionPerfRatio, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrSingleToDoublePrecisionPerfRatio, %i)\n", value, dev);
		CHECK(veraDeviceGetAttribute(&value, veraDevAttrFirmwareVersion, dev));
		printf("veraDeviceGetAttribute(%i, veraDevAttrFirmwareVersion, %i)\n", value, dev);
        	printf("PASSED\n");

		size_t cnt = 128;
		size_t size = cnt * sizeof(int);
		int* host = (int*)malloc(size);
		for(size_t i = 0; i < cnt; i++)
			host[i] = i;

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != i) {
				printf("expected host[%i] to be %p but is %p\n", i, i, host[i]);
				return 1;
			}
		}

		void* ptr;
		CHECK(veraMallocAsync(&ptr, size, 0));
		printf("veraMallocAsync(%p, %llu, %i)\n", ptr, size, 0);

		CHECK(veraMemcpyAsync(ptr, host, size, veraMemcpyHostToDevice, 0));
		printf("veraMemcpyHtoDAsync(%p, %p, %llu, %i)\n", ptr, host, size, 0);

		CHECK(veraMemsetAsync(ptr, 0xDEADBEEF, cnt, 0));
		printf("veraMemsetAsync(%p, %08X, %llu, %i)\n", ptr, 0xDEADBEEF, cnt, 0);

		CHECK(veraMemcpyAsync(host, ptr, size, veraMemcpyDeviceToHost, 0));
		printf("veraMemcpyAsync(%p, %p, %llu, %i)\n", host, ptr, size, 0);

		CHECK(veraDeviceSynchronize());
		printf("veraDeviceSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != 0xDEADBEEF) {
				printf("expected host[%i] to be %p but is %p\n", i, 0xDEADBEEF, host[i]);
				return 1;
			}
		}

		veraModule_t mod;
		const char* modName = "./libvera_device.vso";
        	printf("\nTEST CASE ID: FT_VERA_38:");
	        veraError_t ret1 = veraModuleLoad(&mod, "");
        	if(ret1 == VEDA_ERROR_INVALID_VALUE)
       		    	printf("PASSED\n");
        	else
            		printf("FAILED\n");

        	printf("\nTEST CASE ID: FT_VERA_10:");
		CHECK(veraModuleLoad(&mod, modName));
        	printf("PASSED\n");
		printf("veraModuleLoad(%p, \"%s\")\n", mod, modName);

		veraFunction_t func;
		const char* funcName = "ve_test";
        	printf("\nTEST CASE ID: FT_VERA_12:");
		CHECK(veraModuleGetFunction(&func, mod, funcName));
        	printf("PASSED\n");
		printf("veraModuleGetFunction(%p, %p, \"%s\")\n", func, mod, funcName);
		const char* funcName_ex = "ve_exception2";
		CHECK(veraModuleGetFunction(&func_ex, mod, funcName_ex));
		
        	printf("\nTEST CASE ID: FT_VERA_13:");
		CHECK(veraDeviceSynchronize());
        	printf("PASSED\n");
		printf("veraDeviceSynchronize()\n");
		void* ptr2;
		CHECK(veraMallocAsync(&ptr2, 0, 0));
                int num = 0;
                printf("\nTEST CASE ID: FT_VERA_18:");
                CHECK(veraStreamCnt(&num));
		if(num == 1)
                        printf("PASSED\n");
                else{
                        printf("FAILED\n");
                	exit(1);
		}
                printf("\nTEST CASE ID: FT_VERA_20:");
                CHECK(veraLaunchHostFunc(0, (VEDAhost_function)hostFunc, NULL));
                printf("PASSED\n");

                printf("veraStreamCnt(%i)\n", num);
        
                std::vector<VEDAstream> streams;
                streams.resize(num);
        
		printf("veraLaunchKernel(%p, %i, %p, %p, %llu)\n", func, 0, VEDAptr<int>(VERA2VEDA(ptr)).ptr(), ptr2, cnt);
                printf("\nTEST CASE ID: FT_VERA_18:");
		CHECK(veraLaunchKernel(func, 0, VEDAptr<int>(VERA2VEDA(ptr)).ptr(), ptr2, cnt));
                printf("PASSED\n");
                printf("\nTEST CASE ID: FT_VERA_14:");
		CHECK(veraStreamSynchronize(0));
                printf("PASSED\n");
        		
                printf("veraStreamQuery(%i)\n", num);
                printf("\nTEST CASE ID: FT_VERA_15:");
                for(VEDAstream stream : streams){
                        veraError_t ret=veraStreamQuery(stream);
                        if(ret == VEDA_ERROR_VEO_STATE_RUNNING)
                                printf("PASSED\n");
                        else{
                                printf("FAILED\n");
                                exit(1);
                        }
                }
        
                printf("veraStreamAddCallback(%i)\n", num);
                printf("\nTEST CASE ID: FT_VERA_17:");
                for(VEDAstream stream : streams)                        
			CHECK(veraStreamAddCallback(stream, MyCallback, NULL, 0));
                printf("PASSED\n");

		CHECK(veraMemcpyAsync(host, ptr2, size, veraMemcpyDeviceToHost, 0));
		printf("veraMemcpyAsync(%p, %p, %llu, %i)\n", host, ptr2, size, 0);

		CHECK(veraDeviceSynchronize());
		printf("veraDeviceSynchronize()\n");

		for(size_t i = 0; i < cnt; i++) {
			if(host[i] != (cnt - 1 - i)) {
				printf("expected host[%i] to be %p but is %p\n", i, (cnt - 1 - i), host[i]);
				return 1;
			}
		}
                printf("\nTEST CASE ID: FT_VERA_37:");
                int *host_mem = NULL;
                CHECK(veraMallocHost((void**)&host_mem, 64));
                if(host_mem == NULL)
                {
                        printf("FAILED\n");
                        exit(1);
                }
                printf("PASSED\n");

		size_t size_test = 32;
		void* ptr3;

        	printf("\nTEST CASE ID: FT_VERA_21:");
        	CHECK(veraMalloc(&ptr3, size_test));
		VEDAptr<char> ptr4 = VEDAptr<char>(VERA2VEDA(ptr3));
        	if(ptr4.size() != 32){
                        printf("FAILED\n");
                	exit(1);
		}
		else
        		printf("PASSED\n");
	        int32_t *host2;
        	printf("\nTEST CASE ID: FT_VERA_27:");
		veraHostAlloc((void**)&host2,sizeof(char) * size_test,1);
	        memset(host2, '\0', sizeof(char) * size_test);
		printf("host2=%x %s\n",host2, host2);
        	printf("PASSED\n");
        	size_t cnt1 = 1024*1024*10;
                size = cnt1 * sizeof(size);
        
                int* host4 = (int*)malloc(size);

        	printf("\nTEST CASE ID: FT_VERA_22:");
                for(size_t i = 0; i < cnt1; i++){
			CHECK(veraMemcpyAsync(host4+i,&i,sizeof(int),veraMemcpyHostToHost,0));
		}
        	printf("PASSED\n");
        
                auto start = NOW(), end = NOW();        
                // Copy to VE0
                CHECK(veraSetDevice(0));
                void* ptr0;
                CHECK(veraMallocAsync(&ptr0, size, 0));
                printf("\nTEST CASE ID: FT_VERA_24:");
	      	printf("\nTEST CASE ID: FT_VERA_32:");
                for(int i = 0; i < 10; i++) {
                        start = NOW();
                        CHECK(veraMemcpyAsync(ptr0, host4, size, veraMemcpyHostToDevice, 0));
                        CHECK(veraDeviceSynchronize());
                        end = NOW();
                }
        
                memset(host4, 0, size);
                for(int i = 0; i < 10; i++) {
                        start = NOW();
                        CHECK(veraMemcpyAsync(host4, ptr0, size, veraMemcpyDeviceToHost, 0));
                        CHECK(veraDeviceSynchronize());
                        end = NOW();
                }
                for(size_t i = 0; i < cnt1; i++)
                        if(host4[i] != i){
                                printf("FAILED\n");
                                exit(1);
        		}
                printf("PASSED\n");
		if(devcnt > 1){ 
			memset(host4, 0, size);
			// Copy from VE0 to VE1
			CHECK(veraSetDevice(1));
			void* ptr1;
			CHECK(veraMallocAsync(&ptr1, size, 0));
			CHECK(veraDeviceSynchronize());
		
			printf("\nTEST CASE ID: FT_VERA_25:");
			for(int i = 0; i < 10; i++) {
				start = NOW();
				CHECK(veraMemcpyAsync(ptr1, ptr0, size, veraMemcpyDeviceToDevice, 0));
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}
		
			printf("\nTEST CASE ID: FT_VERA_23:");
			for(int i = 0; i < 10; i++) {
				start = NOW();
				CHECK(veraMemcpyAsync(host4, ptr1, size, veraMemcpyDeviceToHost, 0));
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}
		
			// Check results
			for(size_t i = 0; i < cnt1; i++)
				if(host4[i] != i){
					printf("FAILED\n");
					exit(1);
				}
			printf("PASSED\n");

			int* host4_ = (int*)malloc(size);

			printf("\nTEST CASE ID: FT_VERA_33:");
			for(size_t i = 0; i < cnt1; i++){
				CHECK(veraMemcpy(host4_+i,&i,sizeof(int),veraMemcpyHostToHost));
			}
			printf("PASSED\n");

			start = NOW();
			end = NOW();
			// Copy to VE0
			CHECK(veraSetDevice(0));
			void* ptr0_;
			CHECK(veraMalloc(&ptr0_, size));
			printf("\nTEST CASE ID: FT_VERA_34:");
			for(int i = 0; i < 10; i++) {
				start = NOW();
				CHECK(veraMemcpy(ptr0_, host4_, size, veraMemcpyHostToDevice));
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}

			printf("\nTEST CASE ID: FT_VERA_35:");
			for(int i = 0; i < 10; i++) {
				start = NOW();
				CHECK(veraMemcpy(host4_, ptr0_, size, veraMemcpyDeviceToHost));
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}
			for(size_t i = 0; i < cnt1; i++)
				if(host4_[i] != i){
					printf("FAILED\n");
					exit(1);
				}
			printf("PASSED\n");
			memset(host4_, 0, size);

			// Copy from VE0 to VE1
			CHECK(veraSetDevice(1));
			void* ptr1_;
			CHECK(veraMalloc(&ptr1_, size));
			CHECK(veraDeviceSynchronize());

			printf("\nTEST CASE ID: FT_VERA_36:");
			for(int i = 0; i < 10; i++) {
				CHECK(veraMemcpy(ptr1_, ptr0_, size, veraMemcpyDeviceToDevice));
				start = NOW();
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}

			for(int i = 0; i < 10; i++) {
				start = NOW();
				CHECK(veraMemcpy(host4_, ptr1_, size, veraMemcpyDeviceToHost));
				CHECK(veraDeviceSynchronize());
				end = NOW();
			}

			// Check results
			for(size_t i = 0; i < cnt1; i++)
				if(host4_[i] != i){
					printf("FAILED\n");
					exit(1);
				}

			printf("PASSED\n");
		
			start = NOW();
			veraPointerAttributes attr;
			printf("\nTEST CASE ID: FT_VERA_31:");
			CHECK(veraPointerGetAttributes(&attr, ptr));
			if(attr.type != veraMemoryTypeDevice || attr.hostPointer != 0){
				printf("FAILED\n");
				exit(1);
			}
			printf("PASSED\n");
			CHECK(veraFreeAsync(ptr1, 0));
			CHECK(veraSetDevice(0));
			CHECK(veraFree(ptr0_));
			end = NOW();
			printf("free = %fms\n", time(start, end));
			free(host4);

		}
      		printf("\nTEST CASE ID: FT_VERA_11:");
		CHECK(veraModuleUnload(mod));
        	printf("PASSED\n");
		printf("veraModuleUnload(%p)\n", mod);
		CHECK(veraFreeAsync(ptr, 0));
		printf("veraFreeAsync(%p, %i)\n", ptr, 0);
		CHECK(veraFreeAsync(ptr2, 0));
		printf("veraFreeAsync(%p, %i)\n", ptr, 0);
        	printf("\nTEST CASE ID: FT_VERA_26:");
		veraFree(ptr3);
        	printf("PASSED\n");
        	printf("\nTEST CASE ID: FT_VERA_28:");
		veraFreeHost(host2);
        	printf("PASSED\n");
		//Not Implemented Functions
		veraDeviceReset();
		veraExtent e;
		veraMalloc3D(NULL, e); 
		veraMemcpyKind kind;
		veraMemcpy2D(NULL, 0, NULL, 0, 0, 0, kind);
		veraMemcpy2DAsync(NULL, 0, NULL, 0, 0, 0, kind, 0);
		veraMemset2D(NULL, 0, 0, 0, 0);
		veraMemset2DAsync(NULL, 0, 0, 0, 0, 0);
		veraPitchedPtr pitchedDevPtr;
		veraMemset3D(pitchedDevPtr, 0, e);
		veraMemset3DAsync(pitchedDevPtr, 0, e, 0);
		veraSetValidDevices(NULL, 0);
		free(host);

	}
	void** ptr_vera = 0;
        size_t pitch_size = 0;
        int w = 4, h = 4;

      	printf("\nTEST CASE ID: FT_VERA_29:");
        CHECK(veraMallocPitch((void**)&ptr_vera, &pitch_size, sizeof(char)*w, h));
	VEDAptr<char> ptr_2d = VERA2VEDA(ptr_vera);
        if(pitch_size != sizeof(char)*w)
        {
                printf("FAILED\n");
                exit(1);
        }
        size_t mem_size=0;
        CHECK(vedaMemSize(&mem_size, ptr_2d));
        if(mem_size != w*h*sizeof(char))
        {
                printf("FAILED\n");
                exit(1);
        }
        printf("PASSED\n");
      	printf("\nTEST CASE ID: FT_VERA_30:");
	size_t free=0,total=0;
        CHECK(veraMemGetInfo(&free,&total));
        if(total <= 0 || free < 0)
        {
                printf("FAILED\n");
                exit(1);
        }
        printf("PASSED\n");
        printf("\nTEST CASE ID: FT_VERA_16:");
	CHECK(veraLaunchKernel(func_ex, 0, 0));
	veraError_t ret= veraStreamSynchronize(0);
        if(ret == VEDA_ERROR_VEO_COMMAND_EXCEPTION)
              	printf("PASSED\n");
        else{
              	printf("FAILED %d\n",ret);
               	exit(1);
	}

	printf("\n# ------------------------------------- #\n");
	printf("# All Tests passed!                     #\n");
	printf("# ------------------------------------- #\n\n");
	return 0;
}
