#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

static t_class *zamgeq31_class;

typedef struct _zamgeq31
{
	t_pxobject x_obj;

	double sample_rate;

	//double gain[29], gainold[29], master; //parameters
	double *gain, *gainold, master; //parameters

	//double omegaL[29], omegaU[29], k[29], v[29], a[29], cm[29][21], a0m[29][21];
	double *omegaL, *omegaU, *k, *v, *a, **cm, **a0m;
	int *m;
	double **w11, **w12, **w21, **w22;

	bool BYPASS;
}t_zamgeq31;

void* zamgeq31_new(t_symbol* s, short argc, t_atom* argv);

void zamgeq31_dsp64(t_zamgeq31 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zamgeq31_dsp(t_zamgeq31 *x, t_signal **sp, short *count);

void zamgeq31_perform64(t_zamgeq31 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zamgeq31_perform(t_int *w);
void zamgeq31_free(t_zamgeq31* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void geq(t_zamgeq31 *x, int i, double srate, double g);
double run_filter(t_zamgeq31 *x, int i, double in);

void set_master(t_zamgeq31 *x, double v);
void gains_list(t_zamgeq31 *x, t_symbol *msg, short argc, t_atom *argv);

void zamBypass(t_zamgeq31 *x, bool bypass);

void activate(t_zamgeq31 *x);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamgeq31~", (method)zamgeq31_new, (method)zamgeq31_free, sizeof(t_zamgeq31), 0, 0);
	class_addmethod(c, (method)zamgeq31_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamgeq31_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)gains_list, "setgains", A_GIMME, 0);
	class_addmethod(c, (method)set_master, "master", A_FLOAT, 0);

	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamgeq31_class = c;

	post("------------------------------------------------------------------");
	post("The Zam 1/3 octave band graphic equaliser for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamgeq31_dsp64(t_zamgeq31 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		zamgeq31_perform64, 0, NULL);
}

void zamgeq31_perform64(t_zamgeq31 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *output0 = outs[0];

	int n = vectorsize;
	double srate = x->sample_rate;

	if (!x->BYPASS) {
		int i, j;
		for (i = 0; i < 29; i++) {
			if (x->gain[i] != x->gainold[i]) {
				geq(x, i, srate, x->gain[i]);
				x->gainold[i] = x->gain[i];
			}
		}

		for (i = 0; i < n; i++) {
			double tmp, filtered;
			double in = input0[i];
			in = sanitize_denormal(in);
			tmp = in;
			filtered = tmp;

			for (j = 0; j < 29; j++) {
				if (x->gain[j] != 0.f) {
					filtered = run_filter(x, j, tmp);
					tmp = filtered;
				}
			}
			//output0[i] = input0[i];
			output0[i] = (double)(filtered * from_dB(x->master));
		}
	}
	else {
		while (n--) {
			*output0++ = *input0++;
		}
	}
}

double run_filter(t_zamgeq31 *x, int i, double in)
{
	int j;
	double out, a1out, a2out, v1, v2;
	in = sanitize_denormal(in);
	a1out = 0.;
	a2out = 0.;
	v1 = v2 = 0.;
	out = in;

	// higher order
	for (j = 1; j <= x->m[i] / 2; j++) {

		// allpass 1
		a1out = x->a[i] * (x->w11[i][j] + x->a[i] * x->w12[i][j]) - x->w12[i][j];

		// allpass 2
		a2out = x->a[i] * (x->w21[i][j] + x->a[i] * x->w22[i][j]) - x->w22[i][j];

		// intermediate values
		v1 = (a2out - 2. * a1out) + x->k[i] * (2. * (-x->cm[i][j]) * a2out +
			x->k[i] * (2. * a1out + a2out));
		v2 = a2out + 2. * a1out;

		// 4th order output section
		out = in + x->v[i] * 2. * (-x->cm[i][j])*(-x->a0m[i][j] * (x->k[i] * in - v1) + a2out) +
			x->v[i] * (2. + x->v[i])*x->k[i] * (x->a0m[i][j] * (x->k[i] * in - v1) + v2);

		// update state
		x->w12[i][j] = x->w11[i][j] + x->a[i] * x->w12[i][j];
		x->w11[i][j] = x->a0m[i][j] * (x->k[i] * in - v1);
		x->w22[i][j] = x->w21[i][j] + x->a[i] * x->w22[i][j];
		x->w21[i][j] = a1out;

		in = out;
	}
	return out;
}

void activate(t_zamgeq31 *x)
{
	int i, j;
	for (i = 0; i < 29; i++) {
		for (j = 0; j < 21; j++) {
			x->w11[i][j] = 0.;
			x->w12[i][j] = 0.;
			x->w21[i][j] = 0.;
			x->w22[i][j] = 0.;
		}
	}
}

void geq(t_zamgeq31 *x, int i, double srate, double g)
{
	int mm;
	double w, omegaU, omegaL, omegaB, omegaM, glinear, alpham;
	// 2dB tolerance
	//const int stack[29] = {	2, 1, 1, 1, 1, 1, 1, 1,
	//			1, 1, 1, 1, 1, 1, 1, 1,
	//			1, 1, 1, 1, 1, 1, 1, 1,
	//			1, 1, 1, 2, 3 };

	// 0.15dB tolerance
	const int stack[29] = { 8, 8, 8, 8, 8, 8, 9, 9,
				9, 9, 9, 9, 9, 9, 9, 9,
				9, 9, 9, 9, 9, 9, 9, 10,
				11, 12, 15, 20, 20 };

	const double omegaLimit = 0.95 * PI;

	w = 2. * PI / srate;
	omegaU = w * x->omegaU[i];
	omegaL = w * x->omegaL[i];

	if (omegaU > omegaLimit) {
		x->m[i] = 0;
		return;
	}

	omegaB = omegaU - omegaL;
	omegaM = 2. * atan(sqrt(tan(omegaU / 2.) * tan(omegaL / 2.)));

	glinear = from_dB(g);
	x->m[i] = (int)(2.0 * stack[i]);

	x->k[i] = (1. / pow(glinear, (1. / (2. * x->m[i])))) * tan(omegaB / 2.);
	x->v[i] = pow(glinear, (1. / x->m[i])) - 1;
	x->a[i] = cos(omegaM);

	for (mm = 1; mm <= x->m[i] / 2; mm++) {
		alpham = (0.5 - (2. * mm - 1) / (2. * x->m[i])) * PI;
		x->cm[i][mm] = cos(alpham);
		x->a0m[i][mm] = 1. / (1. + 2. * x->k[i] * x->cm[i][mm] + x->k[i] * x->k[i]);
	}
}

void* zamgeq31_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamgeq31* x = (t_zamgeq31*)object_alloc(zamgeq31_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();

	x->gain = (double*)sysmem_newptr(29 * sizeof(double));
	x->gainold = (double*)sysmem_newptr(29 * sizeof(double));
	x->omegaL = (double*)sysmem_newptr(29 * sizeof(double));
	x->omegaU = (double*)sysmem_newptr(29 * sizeof(double));
	x->k = (double*)sysmem_newptr(29 * sizeof(double));
	x->v = (double*)sysmem_newptr(29 * sizeof(double));
	x->a = (double*)sysmem_newptr(29 * sizeof(double));

	x->m = (int*)sysmem_newptr(29 * sizeof(int));

	x->cm = (double**)sysmem_newptr(29 * sizeof(double*));
	x->a0m = (double**)sysmem_newptr(29 * sizeof(double*));
	x->w11 = (double**)sysmem_newptr(29 * sizeof(double*));
	x->w12 = (double**)sysmem_newptr(29 * sizeof(double*));
	x->w21 = (double**)sysmem_newptr(29 * sizeof(double*));
	x->w22 = (double**)sysmem_newptr(29 * sizeof(double*));

	for (int i = 0; i < 29; i++){
		x->cm[i] = (double*)sysmem_newptr(21 * sizeof(double));
		x->a0m[i] = (double*)sysmem_newptr(21 * sizeof(double));
		x->w11[i] = (double*)sysmem_newptr(21 * sizeof(double));
		x->w12[i] = (double*)sysmem_newptr(21 * sizeof(double));
		x->w21[i] = (double*)sysmem_newptr(21 * sizeof(double));
		x->w22[i] = (double*)sysmem_newptr(21 * sizeof(double));
	}

	for (int i = 0; i < 29; i++) {
		x->gain[i] = 0.0;
	}

	x->omegaL[0] = 28.2;
	x->omegaL[1] = 35.5;
	x->omegaL[2] = 44.7;
	x->omegaL[3] = 56.2;
	x->omegaL[4] = 70.8;
	x->omegaL[5] = 89.1;
	x->omegaL[6] = 112.;
	x->omegaL[7] = 141.;
	x->omegaL[8] = 178.;
	x->omegaL[9] = 224.;
	x->omegaL[10] = 282.;
	x->omegaL[11] = 355.;
	x->omegaL[12] = 447.;
	x->omegaL[13] = 562.;
	x->omegaL[14] = 708.;
	x->omegaL[15] = 891.;
	x->omegaL[16] = 1120.;
	x->omegaL[17] = 1410.;
	x->omegaL[18] = 1780.;
	x->omegaL[19] = 2240.;
	x->omegaL[20] = 2820.;
	x->omegaL[21] = 3550.;
	x->omegaL[22] = 4470.;
	x->omegaL[23] = 5620.;
	x->omegaL[24] = 7080.;
	x->omegaL[25] = 8910.;
	x->omegaL[26] = 11200.;
	x->omegaL[27] = 14100.;
	x->omegaL[28] = 17800.;

	x->omegaU[0] = 35.5;
	x->omegaU[1] = 44.7;
	x->omegaU[2] = 56.2;
	x->omegaU[3] = 70.8;
	x->omegaU[4] = 89.1;
	x->omegaU[5] = 112.;
	x->omegaU[6] = 141.;
	x->omegaU[7] = 178.;
	x->omegaU[8] = 224.;
	x->omegaU[9] = 282.;
	x->omegaU[10] = 355.;
	x->omegaU[11] = 447.;
	x->omegaU[12] = 562.;
	x->omegaU[13] = 708.;
	x->omegaU[14] = 891.;
	x->omegaU[15] = 1120.;
	x->omegaU[16] = 1410.;
	x->omegaU[17] = 1780.;
	x->omegaU[18] = 2240.;
	x->omegaU[19] = 2820.;
	x->omegaU[20] = 3550.;
	x->omegaU[21] = 4470.;
	x->omegaU[22] = 5620.;
	x->omegaU[23] = 7080.;
	x->omegaU[24] = 8910.;
	x->omegaU[25] = 11200.;
	x->omegaU[26] = 14100.;
	x->omegaU[27] = 17800.;
	x->omegaU[28] = 22400.;

	x->master = 0.f;
	for (int i = 0; i < 29; ++i) {
		geq(x, i, x->sample_rate, 0.);
		x->gainold[i] = 0.;
	}

	x->BYPASS = false;
	/* reset filter values */
	activate(x);
	return x;
}

void zamBypass(t_zamgeq31 *x, bool bypass)
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
		return 0.;
	return v;
}

double from_dB(double gdb) {
	return (exp(gdb / 20.0*log(10.0)));
}

double to_dB(double g) {
	return (20.0*log10(g));
}

void set_master(t_zamgeq31 *x, double v)
{
	if (v > 30.0) {
		v = 30.0;
		post("Maximum Master Value: 30 dB");
	}
	else if (v < -30.0) {
		v = -30.0;
		post("Minimum Master Value: -30 dB");
	}
	x->master = v;
}

void gains_list(t_zamgeq31 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *gains = x->gain;

	for (i = 0; i < argc; i++) {
		gains[i] = atom_getfloat(argv + i);
		if (gains[i] < -12.0) {
			gains[i] = -12.0;
			post("Minimum Filter Gain: -12 dB");
		}
		else if (gains[i] > 12.0) {
			gains[i] = 12.0;
			post("Maximum Filter Gain: 12 dB");
		}
	}
}

void zamgeq31_free(t_zamgeq31* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->gain);
	sysmem_freeptr(x->gainold);
	sysmem_freeptr(x->omegaL);
	sysmem_freeptr(x->omegaU);
	sysmem_freeptr(x->k);
	sysmem_freeptr(x->v);
	sysmem_freeptr(x->a);
	sysmem_freeptr(x->cm);
	sysmem_freeptr(x->a0m);
	sysmem_freeptr(x->m);
	sysmem_freeptr(x->w11);
	sysmem_freeptr(x->w12);
	sysmem_freeptr(x->w21);
	sysmem_freeptr(x->w22);
}