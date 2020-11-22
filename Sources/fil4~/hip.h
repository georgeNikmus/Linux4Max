/* fil4.lv2 - highpass
 *
 * Copyright (C) 2015 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FIL4_HIP_H
#define _FIL4_HIP_H

#include <math.h>

#define RESHP(X) (0.7 + 0.78 * tanh(1.82 * ((X) -.8)))

typedef struct {
	double y2;
	double z1, z2;
	double a, q, g;
	double alpha, omega, q2;
	double freq, qual; // last settings
	double rate;
	bool  en;
} HighPass;

static void hip_setup (HighPass *f, double rate, double freq, double q) {
	memset (f, 0, sizeof(HighPass));
	//f = (HighPass*)sysmem_newptr(0 * sizeof(HighPass));
	f->rate = rate;
	f->freq = freq;

	f->qual = q;
	f->q2 = RESHP(q);
	if (f->q2 < 0.0)  f->q2 = 0.0;
	if (f->q2 > 1.6) f->q2 = 1.6;

	if (freq > rate / 12.0) freq = rate / 12.0;
	f->alpha = exp (-2.0 * PI * freq / rate);

	f->a = 1.0;
	f->q = 0.0; // start bypassed
	f->g = 0.0; // fade in
	f->en = false;
}

static bool hip_interpolate (HighPass *f, bool en, double freq, double q) {
	// called an interval of max 48 samples
	bool changed = f->en != en;
	f->en = en;

	if (freq != f->freq) {
		f->freq = freq;
		if (freq > f->rate / 12.0) {
			freq = f->rate / 12.0;
		}
		if (freq < 5.0) {
			freq = 5.0;
		}
		f->omega = freq / f->rate;
		f->alpha = exp (-2.0 * PI * f->omega);
		changed = true;
	}

	if (f->qual != q) {
		f->q2 = RESHP(q);
		if (f->q2 < 0.0)  f->q2 = 0.0;
		if (f->q2 > 1.6) f->q2 = 1.6;
		//printf("HI: %f -> %f\n", q, f->q2);
		f->qual = q;
		changed = true;
	}

	const double to = en ? f->alpha : 1.0;
	if (fabs(to - f->a) < 1e-5) {
		f->a = to;
	} else {
		f->a += .01 * (to - f->a);
		changed = true;
	}

	const double tq = en ? f->q2 : 0;
	if (fabs(tq - f->q) < 1e-5) {
		f->q = tq;
	} else {
		f->q += .01 * (tq - f->q);
		changed = true;
	}

	//target gain = 1 + (.5 + q) * 2 * w;
	const double tg = en ? (1.0 + f->omega + 2.0 * f->q * f->omega) : 1.0;
	if (fabs(tg - f->g) < 1e-5) {
		f->g = tg;
	} else {
		f->g += .01 * (tg - f->g);
		changed = true;
	}

	if (!en) {
		//f->z1 += .01 * (f->z2 - f->z1) + 1e-12;
		f->z1 += .01 * (f->z2 - f->z1);
		//f->z2 += .01 * (f->y2 - f->z2) + 1e-12;
	}
/*
#ifndef NO_NAN_PROTECTION
	if (isnan(f->z1)) f->z1 = 0;
	if (isnan(f->z2)) f->z2 = 0;
	if (isnan(f->y2)) f->y2 = 0;
#endif
*/

#ifndef NO_NAN_PROTECTION
	if (IS_NAN_SAMPLE(f->z1)) f->z1 = 0;
	if (IS_NAN_SAMPLE(f->z2)) f->z2 = 0;
	if (IS_NAN_SAMPLE(f->y2)) f->y2 = 0;
#endif
	return changed;
}

static void hip_compute (HighPass *f, int n_samples, double *buf) {
	const double a = f->a;
	const double q = f->q;
	const double g = f->g;

	const double m1 = g/a;
	const double m2 = g*q;

	if (a == 1.0 && q == 0.0 && g == 1.0) {
		// might as well save some computing
		// (all values incl state are filtered)
		return;
	}

	double z1 = f->z1;
	double z2 = f->z2;
	double y2 = f->y2;

	for (int i = 0; i < n_samples; ++i) {
		const double _z1 = z1; // remember previous input
		const double _z2 = z2; // since buf[] is processed in-place

		z1 = m1 * buf[i] - m2 * (y2 - z2); // == g * (buf[i] / a - q * (y2 - z2))
		z2 = a * (z2 + z1 - _z1);
		y2 = a * (y2 + z2 - _z2);
		FIX_DENORM_DOUBLE(y2);
		buf[i] = y2;
	}

	f->y2 = y2;
	//f->z1 = z1 + 1e-12;
	//f->z2 = z2 + 1e-12;
	FIX_DENORM_DOUBLE(z1);
	f->z1 = z1;
	FIX_DENORM_DOUBLE(z2);
	f->z2 = z2;
}
#endif
