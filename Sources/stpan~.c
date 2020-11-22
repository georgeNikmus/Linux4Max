#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include <math.h>

static t_class *stpan_class;

typedef struct _stpan
{
	t_pxobject x_obj;

	double interpol_time;
	double int_time_sec;
	double pan_pos;
	double *fRec0;
	double *fRec1;
	double fConst0;
} t_stpan;

void stpan_dsp64(t_stpan *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void stpan_dsp(t_stpan *x, t_signal **sp, short *count);

void stpan_perform64(t_stpan *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
t_int *stpan_perform(t_int *w);

void setInterpolTime(t_stpan *x, double ms);
void setPanPos(t_stpan *x, double pos);
void stpan_free(t_stpan* x);
void* stpan_new(t_symbol* s, short argc, t_atom* argv);
void stpan_assist(t_stpan *x, void *b, long msg, long arg, char *dst);

int C74_EXPORT main(void)
{
	t_class *c;

	c = class_new("stpan~", (method)stpan_new, (method)stpan_free,
		sizeof(t_stpan), 0L, A_GIMME, 0);
	class_addmethod(c, (method)stpan_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)stpan_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setInterpolTime, "int_time", A_FLOAT, 0);
	class_addmethod(c, (method)setPanPos, "pan", A_FLOAT, 0);
	class_addmethod(c, (method)stpan_assist, "assist", A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	stpan_class = c;

	post("--------------------------------------------------------------------------");
	post("stpan panner from Faust programming language");
	post("stpan~ default values : { Interpolation Time(0 - 10) : 1, Pan Position(0 - 1) : 0.5 }");
	post("Translated into Max/MSP and updated for 64-bit perform routine(Max6): George Nikolopoulos - July 2016 -");

	return 0;
}

void stpan_dsp64(t_stpan *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	object_method(dsp64, gensym("dsp_add64"), x,
		stpan_perform64, 0, NULL);
}

void stpan_dsp(t_stpan *x, t_signal **sp, short *count)
{
	dsp_add(stpan_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
}

void stpan_perform64(t_stpan *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input1 = ins[0];
	t_double *input2 = ins[1];
	t_double *output1 = outs[0];
	t_double *output2 = outs[1];

	int n = vectorsize;
	
	double *fRec0 = x->fRec0;
	double *fRec1 = x->fRec1;

	double fSlow0 = exp((0. - (x->fConst0 / x->int_time_sec)));
	double fSlow1 = (0.5 * (1. - fSlow0));
	double fSlow2 = (2. * x->pan_pos);
	double fSlow3 = sqrt(max(0., (1. - fSlow2)));
	double fSlow4 = sqrt(min(1., (2. - fSlow2)));
	double fSlow5 = sqrt(min(1., fSlow2));
	double fSlow6 = sqrt(max(0., (fSlow2 - 1.)));
	while (n--){
		double fTemp0 = *input2++;
		double fTemp1 = *input1++;
		fRec0[0] = ((fSlow0 * fRec0[1]) + (fSlow1 * ((fSlow3 * fTemp0) + (fSlow4 * fTemp1))));
		*output1++ = fRec0[0];
		fRec1[0] = ((fSlow0 * fRec1[1]) + (fSlow1 * ((fSlow5 * fTemp0) + (fSlow6 * fTemp1))));
		*output2++ = fRec1[0];
		fRec0[1] = fRec0[0];
		fRec1[1] = fRec1[0];
	}
}

t_int *stpan_perform(t_int *w)
{
	t_stpan *x = (t_stpan *)(w[1]);
	float *input1 = (t_float *)(w[2]);
	float *input2 = (t_float *)(w[3]);
	float *output1 = (t_float *)(w[4]);
	float *output2 = (t_float *)(w[5]);

	int n = (t_int)w[6];

	double *fRec0 = x->fRec0;
	double *fRec1 = x->fRec1; 
	float fSlow0 = expf((0.f - ((float)x->fConst0 / (float)(x->int_time_sec))));
	float fSlow1 = (0.5f * (1.f - fSlow0));
	float fSlow2 = (2.f * (float)(x->pan_pos));
	float fSlow3 = sqrtf(max(0.f, (1.f - fSlow2)));
	float fSlow4 = sqrtf(min(1.f, (2.f - fSlow2)));
	float fSlow5 = sqrtf(min(1.f, fSlow2));
	float fSlow6 = sqrtf(max(0.f, (fSlow2 - 1.f)));
	while (n--){
		float fTemp0 = *input2++;
		float fTemp1 = *input1++;
		fRec0[0] = ((fSlow0 * fRec0[1]) + (fSlow1 * ((fSlow3 * fTemp0) + (fSlow4 * fTemp1))));
		*output1++ = (float)fRec0[0];
		fRec1[0] = ((fSlow0 * fRec1[1]) + (fSlow1 * ((fSlow5 * fTemp0) + (fSlow6 * fTemp1))));
		*output2++ = (float)fRec1[0];
		fRec0[1] = fRec0[0];
		fRec1[1] = fRec1[0];
	}
	return (w + 7);
}

void setInterpolTime(t_stpan *x, double ms){
	if (ms > 10 || ms < 0){
		post("Default Interpolation Time value: 1");
		ms = 1;
	}
	x->interpol_time = ms;
	x->int_time_sec = x->interpol_time / 1000.;
}

void setPanPos(t_stpan *x, double pos){
	if (pos > 1 || pos < 0){
		post("Default Pan Position value: 0.5");
		pos = 0.5;
	}
	x->pan_pos = pos;
}

void stpan_free(t_stpan* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->fRec0);
	sysmem_freeptr(x->fRec1);
}

void* stpan_new(t_symbol* s, short argc, t_atom* argv)
{
	t_stpan* x = (t_stpan*)object_alloc(stpan_class);

	dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different

	x->interpol_time = 1.;
	x->int_time_sec = x->interpol_time / 1000.;
	x->pan_pos = 0.5;

	x->fConst0 = (1.0 / min(192000, max(1, sys_getsr())));

	x->fRec0 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec1 = (double *)sysmem_newptr(2 * sizeof(double));

	for (int i = 0; (i < 2); i++) {
		x->fRec0[i] = 0.;
	}
	for (int i = 0; (i < 2); i++) {
		x->fRec1[i] = 0.;
	}
	return x;
}

void stpan_assist(t_stpan *x, void *b, long msg, long arg, char *dst){
	if (msg == ASSIST_INLET){
		switch (arg){
		case 0: sprintf(dst, "(signal) Stereo Input Left"); break;
		case 1: sprintf(dst, "(signal) Stereo Input Right"); break;
		}
	}
	else if (msg == ASSIST_OUTLET){
		switch (arg){
		case 0: sprintf(dst, "(signal) Left Output"); break;
		case 1: sprintf(dst, "(signal) Right Output"); break;
		}
	}
}