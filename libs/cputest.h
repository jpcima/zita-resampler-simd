#ifndef ZR_CPUTEST_H
#define ZR_CPUTEST_H

namespace ZitaResampler {
    void cputest_avx512f(float *);
    void cputest_avx(float *);
    void cputest_sse(float *);
    void cputest_omp(float *);
};

#endif  // ZR_CPUTEST_H
