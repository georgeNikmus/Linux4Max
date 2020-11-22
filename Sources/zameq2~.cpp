#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

static t_class *zameq2_class;

typedef struct _zameq2
{
	t_pxobject x_obj;

	double sample_rate;

	double master; //parameters
	double *freqs; //freql,freq1,freq2,freqh
	double *gains; //gainl,gain1,gain2,gainh
	double *qs;
	double x1, x2, y1, y2;
	double x1a, x2a, y1a, y2a;
	double zln1, zln2, zld1, zld2;
	double zhn1, zhn2, zhd1, zhd2;
	double a0x, a1x, a2x, b0x, b1x, b2x, gainx;
	double a0y, a1y, a2y, b0y, b1y, b2y, gainy;
	double *Bl, *Al, *Bh, *Ah;

	bool BYPASS;
}t_zameq2;

void* zameq2_new(t_symbol* s, short argc, t_atom* argv);

void zameq2_dsp64(t_zameq2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zameq2_dsp(t_zameq2 *x, t_signal **sp, short *count);

void zameq2_perform64(t_zameq2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zameq2_perform(t_int *w);
void zameq2_free(t_zameq2* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void set_master(t_zameq2 *x, double v);
void freqs_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv);
void gains_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv);
void qs_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv);

void zamBypass(t_zameq2 *x, bool bypass);

void peq(double G0, double G, double GB, double w0, double Dw, double *a0, double *a1, double *a2, double *b0, double *b1, double *b2, double *gn);
void highshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]);
void lowshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]);

void activate(t_zameq2 *x);


void ext_main(void *r)
{
	t_class *c;

	c = class_new("zameq2~", (method)zameq2_new, (method)zameq2_free, sizeof(t_zameq2), 0, 0);
	class_addmethod(c, (method)zameq2_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zameq2_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)freqs_list, "fl/f1/f2/fh", A_GIMME, 0);
	class_addmethod(c, (method)gains_list, "gl/g1/g2/gh", A_GIMME, 0);
	class_addmethod(c, (method)qs_list, "q1/q2", A_GIMME, 0);


	class_addmethod(c, (method)set_master, "master", A_FLOAT, 0);

	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zameq2_class = c;

	post("------------------------------------------------------------------");
	post("The Zam Equalizer for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zameq2_dsp64(t_zameq2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		zameq2_perform64, 0, NULL);
}

void zameq2_perform64(t_zameq2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *output0 = outs[0];

	int n = vectorsize;
	double srate = x->sample_rate;

	if (!x->BYPASS) {
		double dcgain = 1.f;

		double qq1 = pow(2.0, 1.0 / x->qs[0]) / (pow(2.0, x->qs[0]) - 1.0); //q from octave bw
		double boost1 = from_dB(x->gains[1]);
		double fc1 = x->freqs[1] / srate;
		double w01 = fc1 * 2.f*PI;
		double bwgain1 = sqrt(boost1);
		double bw1 = fc1 / qq1;

		double qq2 = pow(2.0, 1.0 / x->qs[1]) / (pow(2.0, x->qs[1]) - 1.0); //q from octave bw
		double boost2 = from_dB(x->gains[2]);
		double fc2 = x->freqs[2] / srate;
		double w02 = fc2 * 2.0*PI;
		double bwgain2 = sqrt(boost2);
		double bw2 = fc2 / qq2;

		double boostl = from_dB(x->gains[0]);
		double All = sqrt(boostl);
		double bwl = 2.0*PI*x->freqs[0] / srate;
		double bwgaindbl = to_dB(All);

		double boosth = from_dB(x->gains[3]);
		double Ahh = sqrt(boosth);
		double bwh = 2.0*PI*x->freqs[3] / srate;
		double bwgaindbh = to_dB(Ahh);

		peq(dcgain, boost1, bwgain1, w01, bw1, &x->a0x, &x->a1x, &x->a2x, &x->b0x, &x->b1x, &x->b2x, &x->gainx);
		peq(dcgain, boost2, bwgain2, w02, bw2, &x->a0y, &x->a1y, &x->a2y, &x->b0y, &x->b1y, &x->b2y, &x->gainy);
		lowshelfeq(0.0, x->gains[0], bwgaindbl, 2.0*PI*x->freqs[0] / srate, bwl, 0.707, x->Bl, x->Al);
		highshelfeq(0.0, x->gains[3], bwgaindbh, 2.0*PI*x->freqs[3] / srate, bwh, 0.707, x->Bh, x->Ah);

		for (int i = 0; i < n; i++) {
			double tmp, tmpl, tmph;
			double in = input0[i];
			x->x1 = sanitize_denormal(x->x1);
			x->x2 = sanitize_denormal(x->x2);
			x->y1 = sanitize_denormal(x->y1);
			x->y2 = sanitize_denormal(x->y2);
			x->x1a = sanitize_denormal(x->x1a);
			x->x2a = sanitize_denormal(x->x2a);
			x->y1a = sanitize_denormal(x->y1a);
			x->y2a = sanitize_denormal(x->y2a);
			x->zln1 = sanitize_denormal(x->zln1);
			x->zln2 = sanitize_denormal(x->zln2);
			x->zld1 = sanitize_denormal(x->zld1);
			x->zld2 = sanitize_denormal(x->zld2);
			x->zhn1 = sanitize_denormal(x->zhn1);
			x->zhn2 = sanitize_denormal(x->zhn2);
			x->zhd1 = sanitize_denormal(x->zhd1);
			x->zhd2 = sanitize_denormal(x->zhd2);
			in = sanitize_denormal(in);

			//lowshelf
			tmpl = in * x->Bl[0] +
				x->zln1 * x->Bl[1] +
				x->zln2 * x->Bl[2] -
				x->zld1 * x->Al[1] -
				x->zld2 * x->Al[2];
			x->zln2 = x->zln1;
			x->zld2 = x->zld1;
			x->zln1 = in;
			x->zld1 = tmpl;

			//highshelf
			tmph = tmpl * x->Bh[0] +
				x->zhn1 * x->Bh[1] +
				x->zhn2 * x->Bh[2] -
				x->zhd1 * x->Ah[1] -
				x->zhd2 * x->Ah[2];
			x->zhn2 = x->zhn1;
			x->zhd2 = x->zhd1;
			x->zhn1 = tmpl;
			x->zhd1 = tmph;

			//parametric1
			tmp = tmph * x->b0x + x->x1 * x->b1x + x->x2 * x->b2x - x->y1 * x->a1x - x->y2 * x->a2x;
			x->x2 = x->x1;
			x->y2 = x->y1;
			x->x1 = tmph;
			x->y1 = tmp;

			//parametric2
			output0[i] = tmp * x->b0y + x->x1a * x->b1y + x->x2a * x->b2y - x->y1a * x->a1y - x->y2a * x->a2y;
			x->x2a = x->x1a;
			x->y2a = x->y1a;
			x->x1a = tmp;
			x->y1a = output0[i];

			output0[i] *= from_dB(x->master);
		}
	}
	else {
		while (n--) {
			*output0++ = *input0++;
		}
	}
}

void peq(double G0, double G, double GB, double w0, double Dw,double *a0, double *a1, double *a2, double *b0, double *b1, double *b2, double *gn) {

	double F, G00, F00, num, den, G1, G01, G11, F01, F11, W2, Dww, C, D, B, A;
	F = fabs(G*G - GB * GB);
	G00 = fabs(G*G - G0 * G0);
	F00 = fabs(GB*GB - G0 * G0);
	num = G0 * G0 * (w0*w0 - PI * PI)*(w0*w0 - PI * PI)
		+ G * G * F00 * PI*PI * Dw*Dw / F;
	den = (w0*w0 - PI * PI)*(w0*w0 - PI * PI)
		+ F00 * PI*PI * Dw*Dw / F;
	G1 = sqrt(num / den);
	G01 = fabs(G*G - G0 * G1);
	G11 = fabs(G*G - G1 * G1);
	F01 = fabs(GB*GB - G0 * G1);
	F11 = fabs(GB*GB - G1 * G1);
	W2 = sqrt(G11 / G00) * tan(w0 / 2.0)*tan(w0 / 2.0);
	Dww = (1.0 + sqrt(F00 / F11) * W2) * tan(Dw / 2.0);
	C = F11 * Dww*Dww - 2.f * W2 * (F01 - sqrt(F00 * F11));
	D = 2.0 * W2 * (G01 - sqrt(G00 * G11));
	A = sqrt((C + D) / F);
	B = sqrt((G*G * C + GB * GB * D) / F);
	*gn = G1;
	*b0 = (G1 + G0 * W2 + B) / (1.0 + W2 + A);
	*b1 = -2.0*(G1 - G0 * W2) / (1.0 + W2 + A);
	*b2 = (G1 - B + G0 * W2) / (1.0 + W2 + A);
	*a0 = 1.0;
	*a1 = -2.0*(1.0 - W2) / (1.0 + W2 + A);
	*a2 = (1 + W2 - A) / (1.0 + W2 + A);

	*b1 = sanitize_denormal(*b1);
	*b2 = sanitize_denormal(*b2);
	*a0 = sanitize_denormal(*a0);
	*a1 = sanitize_denormal(*a1);
	*a2 = sanitize_denormal(*a2);
	*gn = sanitize_denormal(*gn);
	if (IS_DENORM_NAN_SAMPLE(*b0)) { *b0 = 1.0; }
}

void lowshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]) {
	double alpha, b0, b1, b2, a0, a1, a2;
	G = pow(10.0, G / 20.0);
	double AA = sqrt(G);

	alpha = sin(w0) / 2.0 * sqrt((AA + 1.0 / AA)*(1.0 / q - 1.0) + 2.0);
	b0 = AA * ((AA + 1.0) - (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha);
	b1 = 2.0*AA*((AA - 1.0) - (AA + 1.0)*cos(w0));
	b2 = AA * ((AA + 1.0) - (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha);
	a0 = (AA + 1.0) + (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha;
	a1 = -2.0*((AA - 1.0) + (AA + 1.0)*cos(w0));
	a2 = (AA + 1.0) + (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha;

	B[0] = b0 / a0;
	B[1] = b1 / a0;
	B[2] = b2 / a0;
	A[0] = 1.0;
	A[1] = a1 / a0;
	A[2] = a2 / a0;
}

void highshelfeq(double, double G, double, double w0, double, double q, double B[], double A[])
{
	double alpha, b0, b1, b2, a0, a1, a2;
	G = pow(10.0, G / 20.0);
	double AA = sqrt(G);

	alpha = sin(w0) / 2.0 * sqrt((AA + 1.0 / AA)*(1.0 / q - 1.0) + 2.0);
	b0 = AA * ((AA + 1.0) + (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha);
	b1 = -2.0*AA*((AA - 1.0) + (AA + 1.0)*cos(w0));
	b2 = AA * ((AA + 1.0) + (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha);
	a0 = (AA + 1.0) - (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha;
	a1 = 2.0*((AA - 1.0) - (AA + 1.0)*cos(w0));
	a2 = (AA + 1.0) - (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha;

	B[0] = b0 / a0;
	B[1] = b1 / a0;
	B[2] = b2 / a0;
	A[0] = 1.0;
	A[1] = a1 / a0;
	A[2] = a2 / a0;
}

void activate(t_zameq2 *x)
{
	x->x1 = x->x2 = x->y1 = x->y2 = 0.0;
	x->x1a = x->x2a = x->y1a = x->y2a = 0.0;
	x->a0x = x->a1x = x->a2x = x->b0x = x->b1x = x->b2x = x->gainx = 0.0;
	x->a0y = x->a1y = x->a2y = x->b0y = x->b1y = x->b2y = x->gainy = 0.0;
	x->zln1 = x->zln2 = x->zld1 = x->zld2 = 0.0;
	x->zhn1 = x->zhn2 = x->zhd1 = x->zhd2 = 0.0;

	for (int i = 0; i < 3; ++i) {
		x->Bl[i] = x->Al[i] = x->Bh[i] = x->Ah[i] = 0.0;
		x->Bl[i] = x->Al[i] = x->Bh[i] = x->Ah[i] = 0.0;
		x->Bl[i] = x->Al[i] = x->Bh[i] = x->Ah[i] = 0.0;
	}
}

void* zameq2_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zameq2* x = (t_zameq2*)object_alloc(zameq2_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();

	x->Bl = (double*)sysmem_newptr(3 * sizeof(double));
	x->Al = (double*)sysmem_newptr(3 * sizeof(double));
	x->Bh = (double*)sysmem_newptr(3 * sizeof(double));
	x->Ah = (double*)sysmem_newptr(3 * sizeof(double));

	x->freqs = (double*)sysmem_newptr(4 * sizeof(double));
	x->gains = (double*)sysmem_newptr(4 * sizeof(double));
	x->qs = (double*)sysmem_newptr(2 * sizeof(double));

	x->gains[1] = 0.0;
	x->qs[0] = 1.0;
	x->freqs[1] = 500.0;
	x->gains[2] = 0.0;
	x->qs[1] = 1.0;
	x->freqs[2] = 3000.0;
	x->gains[0] = 0.0;
	x->freqs[0] = 250.0;
	x->gains[3] = 0.0;
	x->freqs[3] = 8000.0;
	x->master = 0.0;
	//x->togglepeaks = 0.0;

	x->BYPASS = false;
	/* reset filter values */
	activate(x);
	return x;
}

void zamBypass(t_zameq2 *x, bool bypass)
{
	bool t_by;
	if (bypass == false) {
		t_by = bypass;
	}
	else {
		t_by = true;
		post("Bypass is ON");
	}
	x->BYPASS = t_by;
}

double sanitize_denormal(double v) {
	if (IS_DENORM_NAN_SAMPLE(v))
		return 0.f;
	return v;
}

double from_dB(double gdb) {
	return (exp(gdb / 20.0*log(10.0)));
}

double to_dB(double g) {
	return (20.0*log10(g));
}

void set_master(t_zameq2 *x, double v)
{
	if (v > 12.0) {
		v = 12.0;
		post("Maximum Master Value: 12 dB");
	}
	else if (v < -12.0) {
		v = -12.0;
		post("Minimum Master Value: -12 dB");
	}
	x->master = v;
}

void freqs_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *freqs = x->freqs;

	for (i = 0; i < argc; i++) {
		freqs[i] = atom_getfloat(argv + i);
		if (freqs[i] < 20.0) {
			freqs[i] = 20.0;
			post("Minimum Filter Frequency: 20 Hz");
		}
		else if (freqs[i] > 15000.0) {
			freqs[i] = 15000.0;
			post("Maximum Filter Frequency: 15000 Hz");
		}
	}
}

void qs_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *qs = x->qs;

	for (i = 0; i < argc; i++) {
		qs[i] = atom_getfloat(argv + i);
		if (qs[i] < 0.1) {
			qs[i] = 0.1;
			post("Minimum Q Value: 0.1");
		}
		else if (qs[i] > 6.0) {
			qs[i] = 6.0;
			post("Maximum Q Value: 6");
		}
	}
}

void gains_list(t_zameq2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *gains = x->gains;

	for (i = 0; i < argc; i++) {
		gains[i] = atom_getfloat(argv + i);
		if (gains[i] < -50.0) {
			gains[i] = -50.0;
			post("Minimum Filter Gain: -50 dB");
		}
		else if (gains[i] > 20.0) {
			gains[i] = 20.0;
			post("Maximum Filter Gain: 20 dB");
		}
	}
}

void zameq2_free(t_zameq2* x)
{
	dsp_free((t_pxobject*)x);
}