#define TARGET_ATTRIBUTE
#define CPUTEST_FUNCTION cputest_omp
#define FLOAT_VECTOR_SIZE 4
#define PRAGMA_OMP(x) _Pragma(x)
#include "cputest-generic.cc"
