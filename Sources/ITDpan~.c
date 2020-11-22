#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include <math.h>

static t_class *itd_class;

typedef struct _itd
{
	t_pxobject x_obj;
	
	double Angle;
	double Ears_dist;
	double Object_dist;
	long io;
	double *fVec0;
	double fConst0;
	double sr;
} t_itd;

void itd_dsp64(t_itd *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void itd_dsp(t_itd *x, t_signal **sp, short *count);

void itd_perform64(t_itd *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
t_int *itd_perform(t_int *w);

void setAngle(t_itd *x, double degrees);
void setEarsDist(t_itd *x, double cm);
void setObjectDist(t_itd *x, double cm);
void itd_free(t_itd* x);
void* itd_new(t_symbol* s, short argc, t_atom* argv);
void itd_assist(t_itd *x, void *b, long msg, long arg, char *dst);

int C74_EXPORT main(void)
{
	t_class *c;

	c = class_new("ITDpan~", (method)itd_new, (method)itd_free,
		sizeof(t_itd), 0L, A_GIMME, 0);
	class_addmethod(c, (method)itd_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)itd_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setAngle, "Angle", A_FLOAT, 0);
	class_addmethod(c, (method)setEarsDist, "Ears_dist", A_FLOAT, 0);
	class_addmethod(c, (method)setObjectDist, "Object_dist", A_FLOAT, 0);
	class_addmethod(c, (method)itd_assist, "assist", A_CANT, 0);
	
	class_dspinit(c);
	class_register(CLASS_BOX, c);
	itd_class = c;

	post("--------------------------------------------------------------------------");
	post("ITD panner from Faust programming language");
	post("ITDpan~ default values : { Angle(0 - 180) : 90, Ears_dist(15 - 20) : 17, Object_dist(100 - 5000) : 100 }");
	post("Translated into Max/MSP and updated for 64-bit perform routine(Max6): George Nikolopoulos - July 2016 -");

	return 0;
}

void itd_dsp64(t_itd *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sr != samplerate) x->sr = samplerate;
	object_method(dsp64, gensym("dsp_add64"), x,
		itd_perform64, 0, NULL);
}

void itd_dsp(t_itd *x, t_signal **sp, short *count)
{
	dsp_add(itd_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void itd_perform64(t_itd *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input = ins[0];
	t_double *output1 = outs[0];
	t_double *output2 = outs[1];
	
	int n = vectorsize;
	long i=x->io;
	double *fVec0 = x->fVec0;
	double fSlow0 = x->Object_dist;
	double fSlow1 = x->Ears_dist;
	double fSlow2 = (cos((0.0174533 * x->Angle)) * fSlow1);

	double fSlow3 = (2.5e-05 * pow(fSlow1,2));
	double fSlow4 = (x->fConst0 * (sqrt(((((0.0001 * fSlow0) - (0.0001 * fSlow2)) * fSlow0) + fSlow3)) - sqrt((fSlow3 + (0.0001 * ((fSlow2 + fSlow0) * fSlow0))))));
	double fSlow5 = max(fSlow4, 0.);
	int iSlow6 = (int)((fSlow5 + 5e-06));
	double fSlow7 = (fSlow5 - (double)(iSlow6));
	double fSlow8 = (fSlow5 - (1.0 + (double)(iSlow6)));
	double fSlow9 = (fSlow5 - ((double)(iSlow6) + 2.0));
	double fSlow10 = (((0. - fSlow7) * (0. - (0.5 * fSlow8))) * (0. - (0.333333 * fSlow9)));
	int iSlow11 = (iSlow6 & 63);
	double fSlow12 = ((1.0 + fSlow5) - (double)iSlow6);
	double fSlow13 = ((0. - fSlow8) * (0. - (0.5 * fSlow9)));
	int iSlow14 = ((1 + iSlow6) & 63);
	double fSlow15 = (0.5 * (fSlow7 * (0. - fSlow9)));
	int iSlow16 = ((2 + iSlow6) & 63);
	double fSlow17 = (0.166667 * (fSlow7 * fSlow8));
	int iSlow18 = ((3 + iSlow6) & 63);
	double fSlow19 = max((0. - fSlow4), 0.);
	int iSlow20 = (int)((5e-06 + fSlow19));
	double fSlow21 = (fSlow19 - (double)(iSlow20));
	double fSlow22 = (fSlow19 - (1.0 + (double)(iSlow20)));
	double fSlow23 = (fSlow19 - (2.0 + (double)(iSlow20)));
	double fSlow24 = (((0. - fSlow21) * (0. - (0.5 * fSlow22))) * (0. - (0.333333 * fSlow23)));
	int iSlow25 = (iSlow20 & 63);
	double fSlow26 = ((1.0 + fSlow19) - (double)(iSlow20));
	double fSlow27 = ((0. - fSlow22) * (0. - (0.5 * fSlow23)));
	int iSlow28 = ((1 + iSlow20) & 63);
	double fSlow29 = (0.5 * (fSlow21 * (0. - fSlow23)));
	int iSlow30 = ((2 + iSlow20) & 63);
	double fSlow31 = (0.166667 * (fSlow21 * fSlow22));
	int iSlow32 = ((3 + iSlow20) & 63);
	while(n--){
		double fTemp0 = *input++;
		fVec0[(i & 63)] = fTemp0;
		*output1++ = ((fSlow10 * fVec0[((i - iSlow11) & 63)]) + (fSlow12 * (((fSlow13 * fVec0[((i - iSlow14) & 63)]) + (fSlow15 * fVec0[((i - iSlow16) & 63)])) + (fSlow17 * fVec0[((i - iSlow18) & 63)]))));
		*output2++ = ((fSlow24 * fVec0[((i - iSlow25) & 63)]) + (fSlow26 * (((fSlow27 * fVec0[((i - iSlow28) & 63)]) + (fSlow29 * fVec0[((i - iSlow30) & 63)])) + (fSlow31 * fVec0[((i - iSlow32) & 63)]))));
		i++;
	}
	x->io = i;
}

t_int *itd_perform(t_int *w)
{
	t_itd *x = (t_itd *)(w[1]);
	float *input = (t_float *)(w[2]);
	float *output1 = (t_float *)(w[3]);
	float *output2 = (t_float *)(w[4]);

	int n = (t_int)w[5];

	long i = x->io;
	double *fVec0 = x->fVec0;
	float fSlow0 = (float)x->Object_dist;
	float fSlow1 = (float)x->Ears_dist;
	float fSlow2 = (cosf((0.0174533f * (float)x->Angle)) * fSlow1);
	float fSlow3 = (2.5e-05f * powf(fSlow1, 2));
	float fSlow4 = ((float)x->fConst0 * (sqrtf(((((0.0001f * fSlow0) - (0.0001f * fSlow2)) * fSlow0) + fSlow3)) - sqrtf((fSlow3 + (0.0001f * ((fSlow2 + fSlow0) * fSlow0))))));
	float fSlow5 = max(fSlow4, 0.f);
	int iSlow6 = (int)((fSlow5 + 5e-06f));
	float fSlow7 = (fSlow5 - (float)(iSlow6));
	float fSlow8 = (fSlow5 - (1.f + (float)(iSlow6)));
	float fSlow9 = (fSlow5 - ((float)(iSlow6)+2.f));
	float fSlow10 = (((0.f - fSlow7) * (0.f - (0.5f * fSlow8))) * (0.f - (0.333333f * fSlow9)));
	int iSlow11 = (iSlow6 & 63);
	float fSlow12 = ((1.f + fSlow5) - (float)iSlow6);
	float fSlow13 = ((0.f - fSlow8) * (0.f - (0.5f * fSlow9)));
	int iSlow14 = ((1 + iSlow6) & 63);
	float fSlow15 = (0.5f * (fSlow7 * (0.f - fSlow9)));
	int iSlow16 = ((2 + iSlow6) & 63);
	float fSlow17 = (0.166667f * (fSlow7 * fSlow8));
	int iSlow18 = ((3 + iSlow6) & 63);
	float fSlow19 = max((0.f - fSlow4), 0.f);
	int iSlow20 = (int)((5e-06f + fSlow19));
	float fSlow21 = (fSlow19 - (float)(iSlow20));
	float fSlow22 = (fSlow19 - (1.f + (float)(iSlow20)));
	float fSlow23 = (fSlow19 - (2.f + (float)(iSlow20)));
	float fSlow24 = (((0.f - fSlow21) * (0.f - (0.5f * fSlow22))) * (0.f - (0.333333f * fSlow23)));
	int iSlow25 = (iSlow20 & 63);
	float fSlow26 = ((1.f + fSlow19) - (float)(iSlow20));
	float fSlow27 = ((0.f - fSlow22) * (0.f - (0.5f * fSlow23)));
	int iSlow28 = ((1 + iSlow20) & 63);
	float fSlow29 = (0.5f * (fSlow21 * (0.f - fSlow23)));
	int iSlow30 = ((2 + iSlow20) & 63);
	float fSlow31 = (0.166667f * (fSlow21 * fSlow22));
	int iSlow32 = ((3 + iSlow20) & 63);
	while (n--){
		double fTemp0 = *input++;
		fVec0[(i & 63)] = fTemp0;
		*output1++ = (float)(((fSlow10 * fVec0[((i - iSlow11) & 63)]) + (fSlow12 * (((fSlow13 * fVec0[((i - iSlow14) & 63)]) + (fSlow15 * fVec0[((i - iSlow16) & 63)])) + (fSlow17 * fVec0[((i - iSlow18) & 63)])))));
		*output2++ = (float)(((fSlow24 * fVec0[((i - iSlow25) & 63)]) + (fSlow26 * (((fSlow27 * fVec0[((i - iSlow28) & 63)]) + (fSlow29 * fVec0[((i - iSlow30) & 63)])) + (fSlow31 * fVec0[((i - iSlow32) & 63)])))));
		i++;
	}
	x->io = i;

	return (w + 6);
}

void setAngle(t_itd *x, double degrees){
	if (degrees > 180 || degrees < 0){
		post("Default Angle value: 90");
		degrees = 90;
	}
	x->Angle = degrees;
}

void setEarsDist(t_itd *x, double cm){
	if (cm > 20 || cm < 15){
		post("Default Ears Distance value: 17 cm");
		cm = 17;
	}
	x->Ears_dist = cm;
}

void setObjectDist(t_itd *x, double cm){
	if (cm > 5000 || cm < 100){
		post("Default Object Distance value: 100");
		cm = 100;
	}
	x->Object_dist = cm;
}

void itd_free(t_itd* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->fVec0);
}

void* itd_new(t_symbol* s, short argc, t_atom* argv)
{
	t_itd* x = (t_itd*)object_alloc(itd_class);

	dsp_setup((t_pxobject *)x, 1);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different

	x->Angle = 90;
	x->Ears_dist = 17;
	x->Object_dist = 100;
	
	x->io = 0;
	x->sr = sys_getsr();
	x->fConst0 = (0.00291545 * min(192000, max(1, x->sr)));
	
	x->fVec0 = (double *)sysmem_newptr(64 * sizeof(double));

	for (int i = 0; (i < 64); i++) {
		x->fVec0[i] = 0.;
	}
	return x;
}

void itd_assist(t_itd *x, void *b, long msg, long arg, char *dst){
	if (msg == ASSIST_INLET){
		sprintf(dst, "(signal) Mono Input");
	}
	else if (msg == ASSIST_OUTLET){
		switch (arg){
		case 0: sprintf(dst, "(signal) Left Output"); break;
		case 1: sprintf(dst, "(signal) Right Output"); break;
		}
	}
}