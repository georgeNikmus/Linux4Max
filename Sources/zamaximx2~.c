#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define MAX_FILT 2
#define MAX_COMP 3
#define MAX_SAMPLES 480
#define DANGER 100000.0
#define EPS 1e-20

#define MAX_DELAY 480
#define MAX_AVG 120

static t_class *zamaximx2_class;

typedef struct _zamaximx2
{
	t_pxobject x_obj;

	double sample_rate;

	double release, ceiling, thresdb, gainred;//parameters
	//int pose[2], posz[2], posc[2];
	int *pose, *posz, *posc;

	//double cn[2][MAX_DELAY];
	//double emaxn[2][MAX_AVG];
	//double z[2][MAX_DELAY];
	double **cn, **emaxn, **z;

	//double emax_old[2];
	double *emax_old;
	//double e_old[2];
	double *e_old;

	bool BYPASS;
}t_zamaximx2;

void* zamaximx2_new(t_symbol* s, short argc, t_atom* argv);

void zamaximx2_dsp64(t_zamaximx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zamaximx2_dsp(t_zamaximx2 *x, t_signal **sp, short *count);

void zamaximx2_perform64(t_zamaximx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zamaximx2_perform(t_int *w);
void zamaximx2_free(t_zamaximx2* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void activate(t_zamaximx2 *x);
double maxsample(double in[]);
void pushsample(t_zamaximx2 *x, double in[], double sample, int *pos, int maxsamples);
double avgall(double samples[]);

void release(t_zamaximx2 *x, double rls);
void ceiling(t_zamaximx2 *x, double cl);
void threshold(t_zamaximx2 *x, double v);
void zamBypass(t_zamaximx2 *x, bool bypass);


void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamaximx2~", (method)zamaximx2_new, (method)zamaximx2_free, sizeof(t_zamaximx2), 0, 0);
	class_addmethod(c, (method)zamaximx2_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamaximx2_assist, "assist", A_CANT, 0);


	class_addmethod(c, (method)release, "release", A_FLOAT, 0);
	class_addmethod(c, (method)ceiling, "ceiling", A_FLOAT, 0);
	class_addmethod(c, (method)threshold, "thresh", A_FLOAT, 0);

	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamaximx2_class = c;

	post("------------------------------------------------------------------");
	post("The ZaMaximX2 Limiter for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamaximx2_dsp64(t_zamaximx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		zamaximx2_perform64, 0, NULL);
}

void zamaximx2_perform64(t_zamaximx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	t_double *output2 = outs[2];

	int n = vectorsize;

	if (!x->BYPASS) {
		int i;
		double N = (double)MAX_DELAY;
		double navg = (double)MAX_AVG;
		double absx[2];
		double c[2];
		double xmax[2];
		double emax[2];
		double avge[2];
		double g[2];
		double srate = x->sample_rate;
		double alpha = 1.0001;
		double aatt = 1. - pow((alpha - 1.) / alpha, 1. / (N + 1.));
		double a;
		double beta;
		double e;
		double target;
		double max;
		double maxx = 0;
		beta = 0.;

		for (i = 0; i < navg; i++) {
			beta += pow(1. - aatt, N + 1. - i);
		}
		beta /= navg;

		double inL, inR;

		for (i = 0; i < n; i++) {
			inL = input0[i];
			inR = input1[i];
			absx[0] = max(fabs(inL), fabs(inR));
			c[0] = max(absx[0], (absx[0] - beta * x->e_old[0]) / (1. - beta));
			xmax[0] = maxsample(&x->cn[0][0]);
			target = xmax[0];

			if (target > x->emax_old[0]) {
				a = 1000 / (aatt * srate);
			}
			else {
				a = 1000 / (x->release * srate);
			}
			emax[0] = (a*target + (1. - a)*x->emax_old[0]);
			//push
			e = avgall(&x->emaxn[0][0]);
			avge[0] = e;
			avge[0] += x->emaxn[0][(x->pose[0] + 1 + MAX_AVG) % MAX_AVG] / (navg + 1.);
			e = avge[0];

			if (e == 0.f) {
				g[0] = 1.;
			}
			else {
				g[0] = sanitize_denormal(min(1., from_dB(x->thresdb) / e));
			}

			//x->gainred = -to_dB(g[0]);
			x->gainred = g[0];

			output0[i] = x->z[0][(x->posz[0] + 1 + MAX_DELAY) % MAX_DELAY] * g[0] * from_dB(x->ceiling - x->thresdb);
			output1[i] = x->z[1][(x->posz[1] + 1 + MAX_DELAY) % MAX_DELAY] * g[0] * from_dB(x->ceiling - x->thresdb);
			output2[i] = x->gainred;

			max = max(fabs(output0[i]), fabs(output1[i]));

			if (maxx < max)
				maxx = max;

			pushsample(x, &x->emaxn[0][0], sanitize_denormal(emax[0]), &x->pose[0], MAX_AVG);
			pushsample(x, &x->cn[0][0], sanitize_denormal(c[0]), &x->posc[0], MAX_DELAY);
			pushsample(x, &x->z[0][0], sanitize_denormal(inL), &x->posz[0], MAX_DELAY);
			pushsample(x, &x->z[1][0], sanitize_denormal(inR), &x->posz[1], MAX_DELAY);

			x->emax_old[0] = sanitize_denormal(emax[0]);
			x->e_old[0] = sanitize_denormal(e);
		}
		//x->outlevel = (maxx == 0.f) ? -160. : to_dB(maxx);
		//	if (outlevel > 0.) printf("g=%f out=%f\n", gainred, outlevel);
	}
	else {
		while (n--) {
			*output0++ = *input0++;
			*output1++ = *input1++;
		}
	}
}

void* zamaximx2_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamaximx2* x = (t_zamaximx2*)object_alloc(zamaximx2_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();
	x->release = 25.0;
	x->ceiling = 0.0;
	x->thresdb = 0.0;
	x->gainred = 0.0;
	//x->outlevel = -45.0;
	/* reset filter values */

	x->pose = (int*)sysmem_newptr(2 * sizeof(int));
	x->posc = (int*)sysmem_newptr(2 * sizeof(int));
	x->posz = (int*)sysmem_newptr(2 * sizeof(int));

	x->emax_old = (double*)sysmem_newptr(2 * sizeof(double));
	x->e_old = (double*)sysmem_newptr(2 * sizeof(double));

	x->cn = (double**)sysmem_newptr(2 * sizeof(double*));
	x->emaxn = (double**)sysmem_newptr(2 * sizeof(double*));
	x->z = (double**)sysmem_newptr(2 * sizeof(double*));

	for (int i = 0; i < 2; i++) {
		x->cn[i] = (double*)sysmem_newptr(MAX_DELAY * sizeof(double));
		x->emaxn[i] = (double*)sysmem_newptr(MAX_AVG * sizeof(double));
		x->z[i] = (double*)sysmem_newptr(MAX_DELAY * sizeof(double));
	}

	activate(x);

	x->BYPASS = 0;
	return x;
}


void activate(t_zamaximx2 *x)
{
	int i;
	//LATENCY_TIME(MAX_DELAY);
	//setLatency(MAX_DELAY);
	x->gainred = 0.0f;
	//x->outlevel = -45.0f;
	for (i = 0; i < MAX_DELAY; i++) {
		x->cn[0][i] = x->z[0][i] = 0.f;
		x->cn[1][i] = x->z[1][i] = 0.f;
	}
	for (i = 0; i < MAX_AVG; i++) {
		x->emaxn[1][i] = 0.f;
		x->emaxn[0][i] = 0.f;
	}
	x->posz[0] = x->posz[1] = 0;
	x->pose[0] = x->pose[1] = 0;
	x->posc[0] = x->posc[1] = 0;
	x->emax_old[0] = x->emax_old[1] = 0.f;
	x->e_old[0] = x->e_old[1] = 0.f;
}

void zamBypass(t_zamaximx2 *x, bool bypass)
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
	return (exp(gdb / 20.f*log(10.f)));
}

double to_dB(double g) {
	return (20.f*log10(g));
}

double normalise(t_zamaximx2 *x ,double in)
{
	if (x->ceiling < x->thresdb) {
		return in;
	}
	return from_dB(-x->thresdb + x->ceiling) * in;
}

void pushsample(t_zamaximx2 *x, double in[], double sample, int *pos, int maxsamples)
{
	(*pos)++;
	if (*pos >= maxsamples) {
		*pos = 0;
	}
	in[*pos] = sample;
}

double avgall(double in[])
{
	int i;
	double avg = 0.f;
	for (i = 0; i < MAX_AVG; i++) {
		avg += in[i];
	}
	avg = avg / (double)(MAX_AVG);
	return avg;
}

double maxsample(double in[])
{
	int i;
	double max = 0.f;
	for (i = 0; i < MAX_DELAY; i++) {
		if (fabs(in[i]) > fabs(max)) {
			max = in[i];
		}
	}
	return max;
}

void release(t_zamaximx2 *x, double rls)
{
	if (rls > 3000.0) {
		rls = 3000.0;
		post("Maximum Release Time: 3000 ms");
	}
	else if (rls < 1.0) {
		rls = 1.0;
		post("Minimum Release Time: 1 ms");
	}
	x->release = rls;
}

void ceiling(t_zamaximx2 *x, double cl)
{
	if (cl < -30.0) {
		cl = -30.0;
		post("Minimum Output Ceiling Value: -30.0");
	}
	else if (cl > 0.0) {
		cl = 0.0;
		post("Maximum Output Ceiling Value: 0");
	}	
	x->ceiling = cl;
}

void threshold(t_zamaximx2 *x, double v)
{
	if (v > 0.0) {
		v = 0.0;
		post("Maximum Threshold Value: 0 db");
	}
	if (v < -30.0) {
		v = -30.0;
		post("Minimum Threshold Value: -30.0");
	}
	x->thresdb = v;
}

void zamaximx2_free(t_zamaximx2* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->posc);
	sysmem_freeptr(x->pose);
	sysmem_freeptr(x->posz);
	sysmem_freeptr(x->emax_old);
	sysmem_freeptr(x->e_old);
	sysmem_freeptr(x->z);
	sysmem_freeptr(x->cn);
	sysmem_freeptr(x->emaxn);

}