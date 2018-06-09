#define TARGET_ATTRIBUTE __attribute__((target("avx")))
#define PROCESS_FUNCTION process_avx
#define PRAGMA_OMP(x) _Pragma(x)
#include "vresampler-simd-generic.cc"
