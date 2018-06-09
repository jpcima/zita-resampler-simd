#define TARGET_ATTRIBUTE __attribute__((target("sse")))
#define PROCESS_FUNCTION process_sse
#define PRAGMA_OMP(x) _Pragma(x)
#include "vresampler-simd-generic.cc"
