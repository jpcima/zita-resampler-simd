#include <string.h>
#include <math.h>
#include <zita-resampler/vresampler.h>

TARGET_ATTRIBUTE
int VResampler::PROCESS_FUNCTION ()
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
                        PRAGMA_OMP("omp simd")
                        for (int i = 0; i < hl; i++)
                        {
                            c1 [i] = a * q1 [i] + b * q1 [i + hl];
                            c2 [i] = a * q2 [i] + b * q2 [i - hl];
                        }
                    }
                    {
                        q1 = p1;
                        q2 = p2;
                        float a = 1e-25f;
                        PRAGMA_OMP("omp simd reduction(+: a)")
                        for (int i = 0; i < hl; i++)
                            a += *q1++ * c1 [i] + *--q2 * c2 [i];
                        *out_data++ = a - 1e-25f;
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
