#define TARGET_ATTRIBUTE __attribute__((target("avx512f")))
#define PROCESS_FUNCTION process_avx512f
#define PRAGMA_OMP(x) _Pragma(x)
#include "vresampler-simd-generic.cc"
