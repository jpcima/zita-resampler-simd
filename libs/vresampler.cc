// ----------------------------------------------------------------------------
//
//  Copyright (C) 2006-2013 Fons Adriaensen <fons@linuxaudio.org>
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <zita-resampler/vresampler.h>


VResampler::VResampler () :
    _table (0),
    _buff  (0),
    _c1 (0),
    _c2 (0)
{
    reset ();
}


VResampler::~VResampler ()
{
    clear ();
}


int VResampler::setup (double       ratio,
                       unsigned int hlen)
{
    if ((hlen < 8) || (hlen > 96) || (16 * ratio < 1) || (ratio > 256)) return 1;
    return setup (ratio, hlen, 1.0 - 2.6 / hlen);
}


int VResampler::setup (double       ratio,
                       unsigned int hlen,
                       double       frel)
{
    unsigned int       h, k, n;
    double             s;
    Resampler_table    *T = 0;

    n = NPHASE;
    s = n / ratio;
    h = hlen;
    k = 250;
    if (ratio < 1)
    {
        frel *= ratio;
        h = (unsigned int)(ceil (h / ratio));
        k = (unsigned int)(ceil (k / ratio));
    }
    T = Resampler_table::create (frel, h, n);
    clear ();
    if (T)
    {
        _table = T;
        _buff  = new float [2 * h - 1 + k];
        _c1 = new float [2 * h];
        _c2 = new float [2 * h];
        _inmax = k;
        _ratio = ratio;
        _pstep = s;
        _qstep = s;
        _wstep = 1;
        return reset ();
    }
    else return 1;
}


void VResampler::clear ()
{
    Resampler_table::destroy (_table);
    delete[] _buff;
    delete[] _c1;
    delete[] _c2;
    _buff  = 0;
    _c1 = 0;
    _c2 = 0;
    _table = 0;
    _inmax = 0;
    _pstep = 0;
    _qstep = 0;
    _wstep = 1;
    reset ();
}


void VResampler::set_phase (double p)
{
    if (!_table) return;
    _phase = (p - floor (p)) * _table->_np;
}


void VResampler::set_rrfilt (double t)
{
    if (!_table) return;
    _wstep =  (t < 1) ? 1 : 1 - exp (-1 / t);
}


void VResampler::set_rratio (double r)
{
    if (!_table) return;
    if (r > 16.0) r = 16.0;
    if (r < 0.95) r = 0.95;
    _qstep = _table->_np / (_ratio * r);
}


double VResampler::inpdist () const
{
    if (!_table) return 0;
    return (int)(_table->_hl + 1 - _nread) - _phase / _table->_np;
}


int VResampler::inpsize () const
{
    if (!_table) return 0;
    return 2 * _table->_hl;
}


int VResampler::reset ()
{
    if (!_table) return 1;

    inp_count = 0;
    out_count = 0;
    inp_data = 0;
    out_data = 0;
    _index = 0;
    _phase = 0;
    _nread = 2 * _table->_hl;
    _nzero = 0;
    return 0;
}
