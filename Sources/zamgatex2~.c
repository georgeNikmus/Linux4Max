#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define MAX_GATE 400

static t_class *zamgatex2_class;

typedef struct _zamgatex2
{
	t_pxobject x_obj;

	double sample_rate;

	double attack, release, thresdb, makeup, gateclose, sidechain, openshut,gainr; //parameters

	double *samplesl;
	double *samplesr;
	double gatestate;
	int posl, posr;

	short side_connected;
	bool BYPASS;
}t_zamgatex2;

void* zamgatex2_new(t_symbol* s, short argc, t_atom* argv);

void zamgatex2_dsp64(t_zamgatex2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zamgatex2_dsp(t_zamgatex2 *x, t_signal **sp, short *count);

void zamgatex2_perform64(t_zamgatex2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zamgatex2_perform(t_int *w);
void zamgatex2_free(t_zamgatex2* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void activate(t_zamgatex2 *x);

void pushsamplel(t_zamgatex2 *x, double samples[], double sample);
void pushsampler(t_zamgatex2 *x,double samples[], double sample);
double averageabs(double samples[]);

void attack(t_zamgatex2 *x, double att);
void release(t_zamgatex2 *x, double rls);
void threshold(t_zamgatex2 *x, double v);
void makeup(t_zamgatex2 *x, double mk);

void gateclose(t_zamgatex2 *x, double gt);
void sidechn(t_zamgatex2 *x, bool sd);
void opensh(t_zamgatex2 *x, bool op);
void zamBypass(t_zamgatex2 *x, bool bypass);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamgatex2~", (method)zamgatex2_new, (method)zamgatex2_free, sizeof(t_zamgatex2), 0, 0);
	class_addmethod(c, (method)zamgatex2_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamgatex2_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)attack, "attack", A_FLOAT, 0);
	class_addmethod(c, (method)release, "release", A_FLOAT, 0);
	class_addmethod(c, (method)threshold, "thresh", A_FLOAT, 0);
	class_addmethod(c, (method)makeup, "makeup", A_FLOAT, 0);
	class_addmethod(c, (method)gateclose, "gateclose", A_FLOAT, 0);

	class_addmethod(c, (method)sidechn, "sidechain", A_LONG, 0);
	class_addmethod(c, (method)opensh, "openshut", A_LONG, 0);
	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);
	
	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamgatex2_class = c;

	post("------------------------------------------------------------------");
	post("The ZamGateX2 for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamgatex2_dsp64(t_zamgatex2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	x->side_connected = count[2];

	object_method(dsp64, gensym("dsp_add64"), x,
		zamgatex2_perform64, 0, NULL);
}

void zamgatex2_perform64(t_zamgatex2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *input2 = ins[2];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	t_double *output2 = outs[2];

	int n = vectorsize;

	if (!x->BYPASS) {
		int i;
		bool usesidechain = false;
		double absamplel, absampler, absample;
		double att;
		double rel;
		double g;
		double fs;
		double in0;
		double in1;
		double side;
		double max;
		fs = x->sample_rate;
		g = x->gatestate;
		att = 1000.0 / (x->attack * fs);
		rel = 1000.0 / (x->release * fs);
		if (x->side_connected) {
			usesidechain = (x->sidechain < 0.5) ? false : true;
		}
		double mingate = (x->gateclose == -50.0) ? 0.0 : from_dB(x->gateclose);
		max = 0.0;

		for (i = 0; i < n; i++) {
			in0 = input0[i];
			in1 = input1[i];
			side = input2[i];
			if (usesidechain) {
				pushsamplel(x, x->samplesl, side);
				absample = averageabs(x->samplesl);
			}
			else {
				pushsamplel(x, x->samplesl, in0);
				pushsampler(x, x->samplesr, in1);
				absamplel = averageabs(x->samplesl);
				absampler = averageabs(x->samplesr);
				absample = max(absamplel, absampler);
			}
			if (x->openshut < 0.5) {
				if (absample > from_dB(x->thresdb)) {
					g += att;
					if (g > 1.0)
						g = 1.0;
				}
				else {
					g -= rel;
					if (g < mingate)
						g = mingate;
				}
			}
			else {
				if (absample > from_dB(x->thresdb)) {
					g -= att;
					if (g < mingate)
						g = mingate;
				}
				else {
					g += rel;
					if (g > 1.0)
						g = 1.0;
				}
			}

			x->gatestate = g;

			output0[i] = g * from_dB(x->makeup) * in0;
			output1[i] = g * from_dB(x->makeup) * in1;
			//x->gainr = (g > 0) ? sanitize_denormal(-to_dB(g)) : 45.0;
			x->gainr = (g > 0) ? g : 1.0;

			output2[i] = x->gainr;

			max = (fabs(max(output0[i], output1[i])) > max) ? fabs(max(output0[i], output1[i])) : sanitize_denormal(max);
		}
		//x->outlevel = (max == 0.f) ? -45.f : to_dB(max);
	}
	else {
		while (n--) {
			*output0++ = *input0++;
			*output1++ = *input1++;
		}
	}
}

void* zamgatex2_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamgatex2* x = (t_zamgatex2*)object_alloc(zamgatex2_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 3);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();

	x->samplesl = (double*)sysmem_newptr(MAX_GATE * sizeof(double));
	x->samplesr = (double*)sysmem_newptr(MAX_GATE * sizeof(double));

	x->attack = 50.0;
	x->release = 100.0;
	x->thresdb = -60.0;
	x->gainr = 0.0;
	x->makeup = 0.0;
	//x->outlevel = -45.0;
	x->gateclose = -50.0;
	x->sidechain = false;
	x->openshut = false;

	/* reset filter values */
	activate(x);

	x->BYPASS = 0;
	return x;
}

void activate(t_zamgatex2 *x)
{
	int i;
	x->gatestate = 0.f;
	x->posl = 0;
	x->posr = 0;
	for (i = 0; i < MAX_GATE; i++) {
		x->samplesl[i] = 0.0;
		x->samplesr[i] = 0.0;
	}
}

void zamBypass(t_zamgatex2 *x, bool bypass)
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
	if (IS_DENORM_NAN_SAMPLE(v) || v == INFINITY)
		return 0.f;
	return v;
}

double from_dB(double gdb) {
	return (exp(gdb / 20.f*log(10.f)));
}

double to_dB(double g) {
	return (20.f*log10(g));
}

void pushsamplel(t_zamgatex2 *x, double samples[], double sample)
{
	++x->posl;
	if (x->posl >= MAX_GATE)
		x->posl = 0;
	samples[x->posl] = sample;
}

void pushsampler(t_zamgatex2 *x, double samples[], double sample)
{
	++x->posr;
	if (x->posr >= MAX_GATE)
		x->posr = 0;
	samples[x->posr] = sample;
}

double averageabs(double samples[])
{
	int i;
	double average = 0.f;

	for (i = 0; i < MAX_GATE; i++) {
		average += samples[i] * samples[i];
	}
	average /= (float)MAX_GATE;
	return sqrt(average);
}

void attack(t_zamgatex2 *x, double att)
{
	if (att > 1000.0) {
		att = 1000.0;
		post("Maximum Attack Time: 1000 ms");
	}
	else if (att < 0.1) {
		att = 0.1;
		post("Minimum Atack Time: 0.1 ms");
	}
	x->attack = att;
}

void release(t_zamgatex2 *x, double rls)
{
	if (rls > 3000.0) {
		rls = 3000.0;
		post("Maximum Release Time: 3000 ms");
	}
	else if (rls < 0.1) {
		rls = 0.1;
		post("Minimum Release Time: 0.1 ms");
	}
	x->release = rls;
}

void threshold(t_zamgatex2 *x, double v)
{
	if (v > 0.0) {
		v = 0.0;
		post("Maximum Threshold Value: 0 db");
	}
	if (v < -60.0) {
		v = -60.0;
		post("Minimum Threshold Value: -60.0");
	}
	x->thresdb = v;
}

void makeup(t_zamgatex2 *x, double mk)
{
	if (mk > 0.0) {
		mk = 0.0;
		post("Maximum Makeup Value: 0 db");
	}
	if (mk < -30.0) {
		mk = -30.0;
		post("Minimum Makeup Value: -30.0");
	}
	x->makeup = mk;
}

void gateclose(t_zamgatex2 *x, double gt)
{
	if (gt > 0.0) {
		gt = 0.0;
		post("Maximum Gateclose Value: 0 db");
	}
	if (gt < -50.0) {
		gt = -50.0;
		post("Minimum Gateclose Value: -50.0");
	}
	x->gateclose = gt;
}

void sidechn(t_zamgatex2 *x, bool sd)
{
	bool t_sd;
	if (sd == false) {
		t_sd = sd;
	}
	else {
		t_sd = true;
		post("Sidechain is ON");
	}
	x->sidechain = t_sd;
}

void opensh(t_zamgatex2 *x, bool op)
{
	bool t_op;
	if (op == false) {
		t_op = op;
	}
	else {
		t_op = true;
		post("Open/Shut Mode is ON");
	}
	x->openshut = t_op;
}

void zamgatex2_free(t_zamgatex2* x)
{
	dsp_free((t_pxobject*)x);
	//sysmem_freeptr(x->attack);
	//sysmem_freeptr(x->release);
	//sysmem_freeptr(x->knee);
	//sysmem_freeptr(x->ratio);
	//sysmem_freeptr(x->thresdb);
	//sysmem_freeptr(x->makeup);
	//sysmem_freeptr(x->gainr);
	//sysmem_freeptr(x->toggle);
	//sysmem_freeptr(x->listen);
	//sysmem_freeptr(x->outlevel);

	//sysmem_freeptr(x->old_yl);
	//sysmem_freeptr(x->old_y1);
	//sysmem_freeptr(x->old_yg);
	//sysmem_freeptr(x->pos);
	//sysmem_freeptr(x->outlevelold);
}