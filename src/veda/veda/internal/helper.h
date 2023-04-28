namespace veda {
	namespace internal {
//------------------------------------------------------------------------------
void hmemcpy	(VEDAhmemptr dst,VEDAhmemptr src, const size_t size);
void hmemfree	(VEDAhmemptr hmem);
void memcpy	(VEDAdeviceptr dst, VEDAdeviceptr src, const size_t size, const VEDAstream stream);

//------------------------------------------------------------------------------
	}
}