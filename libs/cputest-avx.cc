#define TARGET_ATTRIBUTE __attribute__((target("avx")))
#define CPUTEST_FUNCTION cputest_avx
#define FLOAT_VECTOR_SIZE 8
#define PRAGMA_OMP(x) _Pragma(x)
#include "cputest-generic.cc"
