/*
    Copyright (C) 2004-2009 Fons Adriaensen <fons@kokkinizita.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef __FILTERS_H
#define __FILTERS_H

#include <math.h>

class Fil4Paramsect
{
	public:

	//enum { SECT, FREQ, BAND, GAIN };

	void init (void)
	{
		_f = 0.25;
		_b = _g = 1.0;
		_a = _s1 = _s2 = _z1 = _z2 = 0.0;
	}

	bool proc (int k, double *sig, double f, double b, double g)
	{
		double s1, s2, d1, d2, a, da, x, y;
		bool  u2 = false;

		s1 = _s1;
		s2 = _s2;
		a = _a;
		d1 = 0.0;
		d2 = 0.0;
		da = 0.0;

		if (f != _f)
		{
			if      (f < 0.5 * _f) f = 0.5 * _f;
			else if (f > 2.0 * _f) f = 2.0 * _f;
			_f = f;
			_s1 = -cos(6.283185 * f);
			d1 = (_s1 - s1) / k;
			u2 = true;
		}

		if (g != _g)
		{
			if      (g < 0.5 * _g) g = 0.5 * _g;
			else if (g > 2.0 * _g) g = 2.0 * _g;
			_g = g;
			_a = 0.5 * (g - 1.0);
			da = (_a - a) / k;
			u2 = true;
		}

		if (b != _b)
		{
			if      (b < 0.5 * _b) b = 0.5 * _b;
			else if (b > 2.0 * _b) b = 2.0 * _b;
			_b = b;
			u2 = true;
		}

		if (u2)
		{
			b *= 7 * f / sqrt(g);
			_s2 = (1.0 - b) / (1.0 + b);
			d2 = (_s2 - s2) / k;
		}

		while (k--)
		{
			s1 += d1;
			s2 += d2;
			a += da;
			x = *sig;
			FIX_DENORM_DOUBLE(x);
			y = x - s2 * _z2;
			FIX_DENORM_DOUBLE(y);
			*sig++ -= a * (_z2 + s2 * y - x);
			y -= s1 * _z1;
			_z2 = _z1 + s1 * y;
			FIX_DENORM_DOUBLE(y);
			//_z1 = y + 1e-20;
			_z1 = y;
		}
/*
#ifndef NO_NAN_PROTECTION
		if (isnan(_z1)) _z1 = 0;
		if (isnan(_z2)) _z2 = 0;
#endif
*/
#ifndef NO_NAN_PROTECTION
		if (IS_NAN_SAMPLE(_z1)) _z1 = 0.0;
		if (IS_NAN_SAMPLE(_z2)) _z2 = 0.0;
#endif
		return u2;
	}

	double s1 () const { return _s1 * (1.0 + _s2); }
	double s2 () const { return _s2; }
	double g0 () const { return .5 * (_g - 1.0) * (1.0 - _s2); }

	private:

	double  _f, _b, _g;
	double  _s1, _s2, _a;
	double  _z1, _z2;
};

#endif
