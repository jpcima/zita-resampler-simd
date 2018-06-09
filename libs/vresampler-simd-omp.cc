#define TARGET_ATTRIBUTE
#define PROCESS_FUNCTION process_omp
#define PRAGMA_OMP(x) _Pragma(x)
#include "vresampler-simd-generic.cc"
