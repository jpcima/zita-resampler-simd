#define TARGET_ATTRIBUTE __attribute__((target("sse")))
#define CPUTEST_FUNCTION cputest_sse
#define FLOAT_VECTOR_SIZE 4
#define PRAGMA_OMP(x) _Pragma(x)
#include "cputest-generic.cc"
