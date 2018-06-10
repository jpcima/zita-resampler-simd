#define TARGET_ATTRIBUTE __attribute__((target("avx512f")))
#define CPUTEST_FUNCTION cputest_avx512f
#define FLOAT_VECTOR_SIZE 16
#define PRAGMA_OMP(x) _Pragma(x)
#include "cputest-generic.cc"
