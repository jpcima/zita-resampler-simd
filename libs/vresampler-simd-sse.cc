#include <string.h>
#include <math.h>
#include <x86intrin.h>
#include <zita-resampler/vresampler.h>

#if defined(__GNUC__)
#  define ALIGNED(x) __attribute__((__aligned__(x)))
#elif defined(_MSC_VER)
#  define ALIGNED(x) __declspec(align(x))
#else
#  define ALIGNED(x) alignas(x)
#endif

#if defined(__GNUC__) && __GNUC__ > 3
#  define INLINE inline __attribute__ ((__always_inline__))
#elif defined(_MSC_VER)
#  define INLINE __forceinline
#else
#  define INLINE inline
#endif

static INLINE __m128 mm_rev_ps(__m128 x)
{
    return _mm_shuffle_ps(x, x, _MM_SHUFFLE(0, 1, 2, 3));
}

static INLINE float mm_vsum_ps(__m128 x)
{
    __m128  shuf  = _mm_shuffle_ps(x, x, _MM_SHUFFLE(2, 3, 0, 1));
    __m128  sums  = _mm_add_ps(x, shuf);
    return _mm_cvtss_f32(_mm_add_ss(sums, _mm_movehl_ps(shuf, sums)));
}

int VResampler::process_sse ()
{
    if (!_table) return 1;

    int hl = _table->_hl;
    unsigned int np = _table->_np;
    unsigned int in = _index;
    unsigned int nr = _nread;
    int nz = _nzero;
    double ph = _phase;
    double dp = _pstep;
    unsigned int n = (2 * hl - nr);
    float *p1 = _buff + in;
    float *p2 = p1 + n;
    float *c1 = _c1;
    float *c2 = _c2;

    while (out_count)
    {
        if (nr)
        {
            if (inp_count == 0) break;
            if (inp_data)
            {
                p2 [0] = inp_data [0];
                inp_data += 1;
                nz = 0;
            }
            else
            {
                p2 [0] = 0;
                if (nz < 2 * hl) nz++;
            }
            nr--;
            p2 += 1;
            inp_count--;
        }
        else
        {
            if (out_data)
            {
                if (nz < 2 * hl)
                {
                    const unsigned int k = (unsigned int) ph;
                    float *q1 = _table->_ctab + hl * k;
                    float *q2 = _table->_ctab + hl * (np - k);
                    {
                        const float b = (float)(ph - k);
                        const float a = 1.0f - b;
                        __m128 va = _mm_set_ps(a, a, a, a);
                        __m128 vb = _mm_set_ps(b, b, b, b);

                        for (int i = 0; i < hl;)
                        {
                            __m128 vc1;
                            __m128 vc2;

                            vc1 = va * _mm_loadu_ps(q1 + i) + vb * _mm_loadu_ps(q1 + i + hl);
                            vc2 = va * _mm_loadu_ps(q2 + i) + vb * _mm_loadu_ps(q2 + i - hl);
                            _mm_storeu_ps(c1 + i, vc1);
                            _mm_storeu_ps(c2 + i, vc2);
                            i += 4;

                            vc1 = va * _mm_loadu_ps(q1 + i) + vb * _mm_loadu_ps(q1 + i + hl);
                            vc2 = va * _mm_loadu_ps(q2 + i) + vb * _mm_loadu_ps(q2 + i - hl);
                            _mm_storeu_ps(c1 + i, vc1);
                            _mm_storeu_ps(c2 + i, vc2);
                            i += 4;

                            vc1 = va * _mm_loadu_ps(q1 + i) + vb * _mm_loadu_ps(q1 + i + hl);
                            vc2 = va * _mm_loadu_ps(q2 + i) + vb * _mm_loadu_ps(q2 + i - hl);
                            _mm_storeu_ps(c1 + i, vc1);
                            _mm_storeu_ps(c2 + i, vc2);
                            i += 4;

                            vc1 = va * _mm_loadu_ps(q1 + i) + vb * _mm_loadu_ps(q1 + i + hl);
                            vc2 = va * _mm_loadu_ps(q2 + i) + vb * _mm_loadu_ps(q2 + i - hl);
                            _mm_storeu_ps(c1 + i, vc1);
                            _mm_storeu_ps(c2 + i, vc2);
                            i += 4;
                        }
                    }
                    {
                        q1 = p1;
                        q2 = p2;

                        __m128 vz = _mm_set_ps(1e-25f, 1e-25f, 1e-25f, 1e-25f);
                        __m128 va = vz;
                        for (int i = 0; i < hl;)
                        {
                            va = _mm_add_ps(
                                va, _mm_mul_ps(_mm_loadu_ps(q1 + i),
                                               _mm_loadu_ps(c1 + i)));
                            va = _mm_add_ps(
                                va, _mm_mul_ps(mm_rev_ps(_mm_loadu_ps(q1 - (i + 1) * 4)),
                                               _mm_loadu_ps(c2 + i)));
                            i += 4;

                            va = _mm_add_ps(
                                va, _mm_mul_ps(_mm_loadu_ps(q1 + i),
                                               _mm_loadu_ps(c1 + i)));
                            va = _mm_add_ps(
                                va, _mm_mul_ps(mm_rev_ps(_mm_loadu_ps(q1 - (i + 1) * 4)),
                                               _mm_loadu_ps(c2 + i)));
                            i += 4;

                            va = _mm_add_ps(
                                va, _mm_mul_ps(_mm_loadu_ps(q1 + i),
                                               _mm_loadu_ps(c1 + i)));
                            va = _mm_add_ps(
                                va, _mm_mul_ps(mm_rev_ps(_mm_loadu_ps(q1 - (i + 1) * 4)),
                                               _mm_loadu_ps(c2 + i)));
                            i += 4;

                            va = _mm_add_ps(
                                va, _mm_mul_ps(_mm_loadu_ps(q1 + i),
                                               _mm_loadu_ps(c1 + i)));
                            va = _mm_add_ps(
                                va, _mm_mul_ps(mm_rev_ps(_mm_loadu_ps(q1 - (i + 1) * 4)),
                                               _mm_loadu_ps(c2 + i)));
                            i += 4;
                        }
                        *out_data++ = mm_vsum_ps(_mm_sub_ps(va, vz));
                    }
                }
                else
                {
                    *out_data++ = 0;
                }
            }
            out_count--;

            double dd =  _qstep - dp;
            if (fabs (dd) < 1e-30) dp = _qstep;
            else dp += _wstep * dd;
            ph += dp;
            if (ph >= np)
            {
                nr = (unsigned int) floor( ph / np);
                ph -= nr * np;
                in += nr;
                p1 += nr;
                if (in >= _inmax)
                {
                    n = (2 * hl - nr);
                    memcpy (_buff, p1, n * sizeof (float));
                    in = 0;
                    p1 = _buff;
                    p2 = p1 + n;
                }
            }
        }
    }
    _index = in;
    _nread = nr;
    _phase = ph;
    _pstep = dp;
    _nzero = nz;

    return 0;
}
