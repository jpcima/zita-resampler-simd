// ----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2012 Fons Adriaensen <fons@linuxaudio.org>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------


#ifndef __VRESAMPLER_H
#define __VRESAMPLER_H


#include <zita-resampler/resampler-table.h>


class VResampler
{
public:

    VResampler ();
    ~VResampler ();

    void detect_cpu();
    bool set_cpu(const char *name);
    const char *get_cpu() const;

    int  setup (double       ratio,
                unsigned int hlen);

    int  setup (double       ratio,
                unsigned int hlen,
                double       frel);

    void   clear ();
    int    reset ();
    int    inpsize () const;
    double inpdist () const;
    int    process();

    void set_phase (double p);
    void set_rrfilt (double t);
    void set_rratio (double r);

    unsigned int         inp_count;
    unsigned int         out_count;
    float               *inp_data;
    float               *out_data;
    void                *inp_list;
    void                *out_list;

private:

    enum { NPHASE = 256 };

    Resampler_table     *_table;
    unsigned int         _inmax;
    unsigned int         _index;
    unsigned int         _nread;
    unsigned int         _nzero;
    double               _ratio;
    double               _phase;
    double               _pstep;
    double               _qstep;
    double               _wstep;
    float               *_buff;
    float               *_c1;
    float               *_c2;
    int    (VResampler::*_process)();

public:
    // Generic Scalar
    int    process_default();
    // X86 SSE
    int    process_sse();
    // X86 AVX
    int    process_avx();
    // X86 AVX-512
    int    process_avx512f();
    // Other OpenMP
    int    process_omp();
};


#endif
