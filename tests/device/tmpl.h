void veda_template(void) {}
void veda_template(int) {}
void veda_template(float, const char*) {}
int veda_template(const char* const) {}
float veda_template(int, int) {}
long veda_template(double, const char*) {}
void* veda_template(float, const void*) {}

template<typename T>
void veda_template(T, T, T) {}
template void veda_template<float>(float, float, float);

template<typename T>
void veda_template(T, T) {}
template void veda_template<void*>(void*, void*);
template void veda_template<bla::complex<float>>(bla::complex<float>, bla::complex<float>);

template<typename T, typename D>
T veda_template(T, D, float, const char*) {}
template int veda_template<int, char>(int, char, float, const char*);

template<typename T>
void veda_template(T* const, T* const) {}
template void veda_template<char>(char* const, char* const);

template<typename T>
T veda_template(void) {}
template void* veda_template<void*>(void);
template float* veda_template<float*>(void);

template<typename T>
void* veda_template(void*, T, T, void*) {}
template void* veda_template<void*>(void*, void*, void*, void*);
template void* veda_template<float*>(void*, float*, float*, void*);

void veda_template(A) {}
void veda_template(const A*) {}
void veda_template(A*) {}
A veda_template(A, int) {}
void veda_template(A, A, A) {}

void veda_template(B<5>) {}
void veda_template(C<int>) {}
void veda_template(bla::complex<float>) {}
void veda_template(bla::complex<double>) {}
void veda_template(bla::complex<float>, bla::complex<float>) {}

template<typename T, int X, typename D>
T veda_template(D, T) {}
template void* veda_template<void*, 5, char>(char, void*);
