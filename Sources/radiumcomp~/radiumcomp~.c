#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include "typepunning.h"
//#include <math.h>

//#define MAXCHAN 36

static t_class *rad_class;

typedef struct _rad
{
	t_pxobject x_obj;

	double SR;
	int nsize;
	int _nchan;

	short  bypass;
	double iConst0;
	double fConst1;
	double releasems;

	double *Rec2_perm;
	float *fRec2_perm;

	double attackms;

	double  *Rec1_perm;
	float  *fRec1_perm;

	double fConst2;
	double threshold;
	double ratio;

	double *Rec0_perm;
	float *fRec0_perm;

	double outGain;

	//double 	*Zec0[36];
	double **Zec0;

	double 	*Zec1;
	double 	*Zec2;
	double 	*Rec2_tmp;
	double 	*Rec1_tmp;
	double 	*Zec3;
	double 	*Zec4;
	double 	*Rec0_tmp;
	double 	*Zec5;

	float 	*fZec0;
	float 	*fZec1;
	float 	*fZec2;
	float 	*fRec2_tmp;
	float 	*fRec1_tmp;
	float 	*fZec3;
	float 	*fZec4;
	float 	*fRec0_tmp;
	float 	*fZec5;
} t_rad;

void rad_dsp64(t_rad *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void rad_dsp(t_rad *x, t_signal **sp, short *count);

void rad_perform64(t_rad *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
t_int *rad_perform(t_int *w);

void setOutGain(t_rad *x, double outgain);
void setThreshold(t_rad *x, double thrs);
void setRatio(t_rad *x, double ratio);
void setAttacktime(t_rad *x, double msec);
void setReltime(t_rad *x, double msec);
void compBypass(t_rad *x, short bypass);
void rad_free(t_rad* x);
void* rad_new(t_symbol* s, short argc, t_atom* argv);
void rad_assist(t_rad *x, void *b, long msg, long arg, char *dst);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("radiumcomp~", (method)rad_new, (method)rad_free,
		sizeof(t_rad), 0L, A_GIMME, 0);
	class_addmethod(c, (method)rad_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)rad_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setOutGain, "OutGain", A_FLOAT, 0);
	class_addmethod(c, (method)setThreshold, "Threshold", A_FLOAT, 0);
	class_addmethod(c, (method)setRatio, "Ratio", A_FLOAT, 0);
	class_addmethod(c, (method)setAttacktime, "Attack", A_FLOAT, 0);
	class_addmethod(c, (method)setReltime, "Release", A_FLOAT, 0);
	class_addmethod(c, (method)compBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)rad_assist, "assist", A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	rad_class = c;

	post("--------------------------------------------------------------------------");
	post("The Radium compressor by Kjetil S. Matheussen");
	post("Max/MSP implementation with 64-bit perform routine: George Nikolopoulos - January 2018 -");
	post("Arguments: Channels - Threshold - Ratio - Attack - Release - Output Gain");

	//return 0;
}

void rad_dsp64(t_rad *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->SR != samplerate) x->SR = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		rad_perform64, 0, NULL);
}

void rad_dsp(t_rad *x, t_signal **sp, short *count)
{
	dsp_add(rad_perform, 7, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
}

void rad_perform64(t_rad *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double** input, ** output;

	input = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->_nchan);
	output = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->_nchan);

	for (int j = 0; j < x->_nchan; j++) {
		input[j] = ins[j];
		output[j] = outs[j];
	}
	t_double *gr = outs[x->_nchan];

	int n = vectorsize;

	if (x->nsize != n) {

		x->nsize = n;

		sysmem_freeptr(x->Zec0);
		sysmem_freeptr(x->Zec1);
		sysmem_freeptr(x->Zec2);

		sysmem_freeptr(x->Rec2_tmp);
		sysmem_freeptr(x->Rec1_tmp);

		sysmem_freeptr(x->Zec3);
		sysmem_freeptr(x->Zec4);

		sysmem_freeptr(x->Rec0_tmp);

		sysmem_freeptr(x->Zec5);

		for (int j = 0; j < x->_nchan; j++) {
			x->Zec0[j] = (double *)sysmem_newptr(x->nsize * sizeof(double));
		}
		
		x->Zec1 = (double *)sysmem_newptr(x->nsize * sizeof(double));
		x->Zec2 = (double *)sysmem_newptr(x->nsize * sizeof(double));

		x->Rec2_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));
		x->Rec1_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));

		x->Zec3 = (double *)sysmem_newptr(x->nsize * sizeof(double));
		x->Zec4 = (double *)sysmem_newptr(x->nsize * sizeof(double));

		x->Rec0_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));

		x->Zec5 = (double *)sysmem_newptr(x->nsize * sizeof(double));

		for (int i = 0; i < x->nsize; i++) {
			for (int j = 0; j < x->_nchan; j++) {
				x->Zec0[j][i] = 0;
			}
			x->Zec1[i] = 0;
			x->Zec2[i] = 0;

			x->Rec2_tmp[i] = 0;
			x->Rec1_tmp[i] = 0;

			x->Zec3[i] = 0;
			x->Zec4[i] = 0;

			x->Rec0_tmp[i] = 0;

			x->Zec5[i] = 0;
		}
	}


	int 	iSlow0 = (int)(x->bypass);
	double 	fSlow1 = exp((0 - (x->fConst1 / max(x->fConst1, (0.001 * x->releasems)))));
	double 	fSlow2 = (1.0 - fSlow1);
	double* 	fRec2 = &x->Rec2_tmp[4];
	double 	fSlow3 = max(x->fConst1, (0.001 * x->attackms));
	double 	fSlow4 = exp((0 - (x->fConst1 / fSlow3)));
	double 	fSlow5 = (1.0 - fSlow4);
	double* 	fRec1 = &x->Rec1_tmp[4];
	double 	fSlow6 = exp((0 - (x->fConst2 / fSlow3)));
	double 	fSlow7 = x->threshold;
	double 	fSlow8 = ((1.0 / (double)(x->ratio)) - 1.0);
	double 	fSlow9 = (1.0 - fSlow6);
	double* 	fRec0 = &x->Rec0_tmp[4];
	double 	fSlow10 = pow(10, (0.05 * x->outGain));

	for (int i = 0; i < n; i++) {
		double add = 0;

		//x->Zec0[i] = ((iSlow0) ? 0 : input1[i]);
	
		//x->Zec1[i] = ((iSlow0) ? 0 : input2[i]);

		//x->Zec2[i] = fabs((fabs(x->Zec1[i]) + fabs(x->Zec0[i])));

		for (int j = 0; j < x->_nchan; j++) {
			x->Zec0[j][i] = ((iSlow0) ? 0 : input[j][i]);
		
			add += fabs(x->Zec0[j][i]);
		}
		x->Zec2[i] = add;
	}

	// SECTION : 3
	// LOOP 0x1e3fbb0
	// pre processing
	for (int i = 0; i < 4; i++) x->Rec2_tmp[i] = x->Rec2_perm[i];
	// exec code
	for (int i = 0; i < n; i++) {
		fRec2[i] = ((fSlow2 * x->Zec2[i]) + (fSlow1 * max(x->Zec2[i], fRec2[i - 1])));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->Rec2_perm[i] = x->Rec2_tmp[n + i];

	// SECTION : 4
	// LOOP 0x1e3f6d0
	// pre processing
	for (int i = 0; i < 4; i++) x->Rec1_tmp[i] = x->Rec1_perm[i];
	// exec code
	for (int i = 0; i<n; i++) {
		fRec1[i] = ((fSlow5 * fRec2[i]) + (fSlow4 * fRec1[i - 1]));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->Rec1_perm[i] = x->Rec1_tmp[n + i];

	// SECTION : 5
	// LOOP 0x1e46fd0
	// exec code
	for (int i = 0; i < n; i++) {
		x->Zec3[i] = (double)((8.685889638065037 * ((8.262958288192749e-08 * (double)(pun_float_to_int((float)fRec1[i]))) - 87.989971088f)));
	}

	// SECTION : 6
	// LOOP 0x1e46a90
	// exec code
	for (int i = 0; i<n; i++) {
		//fbargraph0 = fZec3[i];
		x->Zec4[i] = (fSlow8 * max((x->Zec3[i] - fSlow7), 0.0));
	}

	// SECTION : 7
	// LOOP 0x1e3f2d0
	// pre processing
	for (int i = 0; i < 4; i++) x->Rec0_tmp[i] = x->Rec0_perm[i];
	// exec code
	for (int i = 0; i<n; i++) {
		//fbargraph1 = fZec4[i];
		fRec0[i] = ((fSlow9 * x->Zec4[i]) + (fSlow6 * fRec0[i - 1]));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->Rec0_perm[i] = x->Rec0_tmp[n + i];

	// SECTION : 8
	// LOOP 0x1e4b140
	// exec code
	for (int i = 0; i<n; i++) {
		x->Zec5[i] = (double)(pun_int_to_float((int)(8388608 * (126.94269504 + max(-126.0, (0.16609640464202244 * fRec0[i]))))));
	}

	// SECTION : 9
	// LOOP 0x1e3f0a0
	// exec code
	for (int i = 0; i<n; i++) {

		//*output1++ = (double)((iSlow0) ? (double)input1[i] : (fSlow10 * (x->Zec0[i] * x->Zec5[i])));
		//*output2++ = (double)((iSlow0) ? (double)input2[i] : (fSlow10 * (x->Zec1[i] * x->Zec5[i])));
		for (int j = 0; j < x->_nchan; j++) {
			output[j][i] = (double)((iSlow0) ? input[j][i] : (fSlow10 * (x->Zec0[j][i] * x->Zec5[i])));
			gr[i] = x->Zec5[i];
		}
	}
}

t_int *rad_perform(t_int *w) {

	t_rad *x = (t_rad *)(w[1]);
	float *input1 = (t_float *)(w[2]);
	float *input2 = (t_float *)(w[3]);
	float *output1 = (t_float *)(w[4]);
	float *output2 = (t_float *)(w[5]);
	float *output3 = (t_float *)(w[6]);

	int n = (t_int)w[7];

	if (x->nsize != n) {

		x->nsize = n;

		sysmem_freeptr(x->fZec0);
		sysmem_freeptr(x->fZec1);
		sysmem_freeptr(x->fZec2);

		sysmem_freeptr(x->fRec2_tmp);
		sysmem_freeptr(x->fRec1_tmp);

		sysmem_freeptr(x->fZec3);
		sysmem_freeptr(x->fZec4);

		sysmem_freeptr(x->fRec0_tmp);

		sysmem_freeptr(x->fZec5);

		for (int j = 0; j < x->_nchan; j++) {
			x->Zec0[j] = (double *)sysmem_newptr(x->nsize * sizeof(double));
		}
		x->fZec1 = (float *)sysmem_newptr(x->nsize * sizeof(float));
		x->fZec2 = (float *)sysmem_newptr(x->nsize * sizeof(float));

		x->fRec2_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));
		x->fRec1_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));

		x->fZec3 = (float *)sysmem_newptr(x->nsize * sizeof(float));
		x->fZec4 = (float *)sysmem_newptr(x->nsize * sizeof(float));

		x->fRec0_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));

		x->fZec5 = (float *)sysmem_newptr(x->nsize * sizeof(float));

		for (int i = 0; i < x->nsize; i++) {
			x->fZec0[i] = 0;
			x->fZec1[i] = 0;
			x->fZec2[i] = 0;

			x->fRec2_tmp[i] = 0;
			x->fRec1_tmp[i] = 0;

			x->fZec3[i] = 0;
			x->fZec4[i] = 0;

			x->fRec0_tmp[i] = 0;

			x->fZec5[i] = 0;
		}
	}


	int 	iSlow0 = (int)(x->bypass);
	float 	fSlow1 = (float)(expf((0 - ((float)(x->fConst1) / max((float)(x->fConst1), (0.001f * (float)x->releasems))))));
	float 	fSlow2 = (1.0f - fSlow1);
	float* 	fRec2 = &x->fRec2_tmp[4];
	float 	fSlow3 = max((float)(x->fConst1), (0.001f * (float)(x->attackms)));
	float 	fSlow4 = (float)(expf((0 - ((float)x->fConst1 / fSlow3))));
	float 	fSlow5 = (1.0f - fSlow4);
	float* 	fRec1 = &x->fRec1_tmp[4];
	float 	fSlow6 = (float)(expf((0 - ((float)(x->fConst2) / fSlow3))));
	float 	fSlow7 = (float)x->threshold;
	float 	fSlow8 = ((1.0f / (float)(x->ratio)) - 1.0f);
	float 	fSlow9 = (1.0f - fSlow6);
	float* 	fRec0 = &x->fRec0_tmp[4];
	float 	fSlow10 = powf(10, (0.05f * (float)(x->outGain)));

	for (int i = 0; i<n; i++) {
		x->fZec0[i] = ((iSlow0) ? 0 : input1[i]);
	
		x->fZec1[i] = ((iSlow0) ? 0 : input2[i]);

		x->fZec2[i] = fabsf((fabsf(x->fZec1[i]) + fabsf(x->fZec0[i])));
	}

	// SECTION : 3
	// LOOP 0x1e3fbb0
	// pre processing
	for (int i = 0; i < 4; i++) x->fRec2_tmp[i] = x->fRec2_perm[i];
	// exec code
	for (int i = 0; i < n; i++) {
		fRec2[i] = ((fSlow2 * x->fZec2[i]) + (fSlow1 * max(x->fZec2[i], fRec2[i - 1])));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->fRec2_perm[i] = x->fRec2_tmp[n + i];

	// SECTION : 4
	// LOOP 0x1e3f6d0
	// pre processing
	for (int i = 0; i < 4; i++) x->fRec1_tmp[i] = x->fRec1_perm[i];
	// exec code
	for (int i = 0; i < n; i++) {
		fRec1[i] = ((fSlow5 * fRec2[i]) + (fSlow4 * fRec1[i - 1]));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->fRec1_perm[i] = x->fRec1_tmp[n + i];

	// SECTION : 5
	// LOOP 0x1e46fd0
	// exec code
	for (int i = 0; i < n; i++) {
		x->fZec3[i] = (float)((8.685889638065037f * ((8.262958288192749e-08f * (float)(pun_float_to_int((float)fRec1[i]))) - 87.989971088f)));
	}

	// SECTION : 6
	// LOOP 0x1e46a90
	// exec code
	for (int i = 0; i < n; i++) {
		//fbargraph0 = fZec3[i];
		x->fZec4[i] = (fSlow8 * max((x->fZec3[i] - fSlow7), 0.0f));
	}

	// SECTION : 7
	// LOOP 0x1e3f2d0
	// pre processing
	for (int i = 0; i < 4; i++) x->fRec0_tmp[i] = x->fRec0_perm[i];
	// exec code
	for (int i = 0; i < n; i++) {
		//fbargraph1 = fZec4[i];
		fRec0[i] = ((fSlow9 * x->fZec4[i]) + (fSlow6 * fRec0[i - 1]));
	}
	// post processing
	for (int i = 0; i < 4; i++) x->fRec0_perm[i] = x->fRec0_tmp[n + i];

	// SECTION : 8
	// LOOP 0x1e4b140
	// exec code
	for (int i = 0; i < n; i++) {
		x->fZec5[i] = (float)(pun_int_to_float((int)(8388608 * (126.94269504f + max(-126.0f, (0.16609640464202244f * fRec0[i]))))));
	}

	// SECTION : 9
	// LOOP 0x1e3f0a0
	// exec code
	for (int i = 0; i<n; i++) {
		*output1++ = (float)((iSlow0) ? (float)input1[i] : (fSlow10 * (x->fZec0[i] * x->fZec5[i])));
		*output2++ = (float)((iSlow0) ? (float)input2[i] : (fSlow10 * (x->fZec1[i] * x->fZec5[i])));
		*output3++ = x->fZec5[i];
	}

	return (w + 8);
}

void setOutGain(t_rad *x, double outgain)
{
	double t_ogain;
	if (outgain >= -80.0 && outgain <= 80.0) {
		t_ogain = outgain;
	}
	else {
		t_ogain = 0.0; // default
		post("Output Gain to default: 0.0");
	}
	x->outGain = t_ogain;
}

void setThreshold(t_rad *x, double thrs)
{
	double t_thrs;
	if (thrs >= -80.0 && thrs <= 80.0) {
		t_thrs = thrs;
	}
	else {
		t_thrs = -20.0; // default
		post("Threshold to default: -20.0");
	}
	x->threshold = t_thrs;
}

void setRatio(t_rad *x, double ratio)
{
	double t_ratio;
	if (ratio >= 0.1 && ratio <= 60.0) {
		t_ratio = ratio;
	}
	else {
		t_ratio = 2.0; // default
		post("Ratio to default: 2.0");
	}
	x->ratio = t_ratio;
}

void setAttacktime(t_rad *x, double msec)
{
	double t_msec;
	if (msec >= 0.0 && msec <= 500.0) {
		t_msec = msec;
	}
	else {
		t_msec = 50.148; // default
		post("Attack time to default: 50.148 ms");
	}
	x->attackms = t_msec;
}

void setReltime(t_rad *x, double msec)
{
	double t_msec;
	if (msec >= 0.0 && msec <= 1000.0) {
		t_msec = msec;
	}
	else {
		t_msec = 100.237; // default
		post("Release time to default: 100.237 ms");
	}
	x->releasems = t_msec;
}

void compBypass(t_rad *x, short bypass)
{
	
	x->bypass = bypass;
	if (x->bypass == 1) post("Bypass is ON");

}

void rad_free(t_rad* x)
{
	dsp_free((t_pxobject*)x);

	sysmem_freeptr(x->Rec2_perm);
	sysmem_freeptr(x->Rec1_perm);
	sysmem_freeptr(x->Rec0_perm);

	sysmem_freeptr(x->Zec0);
	sysmem_freeptr(x->Zec1);
	sysmem_freeptr(x->Zec2);

	sysmem_freeptr(x->Rec2_tmp);
	sysmem_freeptr(x->Rec1_tmp);

	sysmem_freeptr(x->Zec3);
	sysmem_freeptr(x->Zec4);

	sysmem_freeptr(x->Rec0_tmp);

	sysmem_freeptr(x->Zec5);

	sysmem_freeptr(x->fRec2_perm);
	sysmem_freeptr(x->fRec1_perm);
	sysmem_freeptr(x->fRec0_perm);

	sysmem_freeptr(x->fZec0);
	sysmem_freeptr(x->fZec1);
	sysmem_freeptr(x->fZec2);

	sysmem_freeptr(x->fRec2_tmp);
	sysmem_freeptr(x->fRec1_tmp);

	sysmem_freeptr(x->fZec3);
	sysmem_freeptr(x->fZec4);

	sysmem_freeptr(x->fRec0_tmp);

	sysmem_freeptr(x->fZec5);
}

void* rad_new(t_symbol* s, short argc, t_atom* argv)
{
	t_rad* x = (t_rad*)object_alloc(rad_class);

	double numChan = 1.0;
	atom_arg_getdouble(&numChan, 0, argc, argv);

	//if (numChan > MAXCHAN) numChan = MAXCHAN;

	x->_nchan = (int)numChan;

	dsp_setup((t_pxobject *)x, x->_nchan);

	for (int j = 0; j < x->_nchan + 1; j++) {
		outlet_new((t_object *)x, "signal");
	}

	((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different

	x->SR = sys_getsr();

	x->nsize = sys_getblksize();

	x->bypass = 0;
	x->iConst0 = min(192000, max(1, x->SR));
	x->fConst1 = (1.0 / (double)(x->iConst0));
	double rel = 100.237;
	x->Rec0_perm = (double *)sysmem_newptr(4 * sizeof(double));
	x->Rec1_perm = (double *)sysmem_newptr(4 * sizeof(double));
	x->Rec2_perm = (double *)sysmem_newptr(4 * sizeof(double));

	x->fRec0_perm = (float *)sysmem_newptr(4 * sizeof(float));
	x->fRec1_perm = (float *)sysmem_newptr(4 * sizeof(float));
	x->fRec2_perm = (float *)sysmem_newptr(4 * sizeof(float));

	for (int i = 0; i<4; i++) x->fRec2_perm[i] = 0;
	for (int i = 0; i<4; i++) x->Rec2_perm[i] = 0;

	double at = 50.148;

	for (int i = 0; i<4; i++) x->fRec1_perm[i] = 0;
	for (int i = 0; i<4; i++) x->Rec1_perm[i] = 0;

	x->fConst2 = (2.0 / (double)(x->iConst0));
	double thresh = -2e+01;
	double rt = 2.0;

	for (int i = 0; i<4; i++) x->fRec0_perm[i] = 0;
	for (int i = 0; i<4; i++) x->Rec0_perm[i] = 0;

	double outg = 0.0;

	atom_arg_getdouble(&thresh, 1, argc, argv);
	atom_arg_getdouble(&rt, 2, argc, argv);
	atom_arg_getdouble(&at, 3, argc, argv);
	atom_arg_getdouble(&rel, 4, argc, argv);
	atom_arg_getdouble(&outg, 5, argc, argv);

	x->threshold = thresh;
	x->ratio = rt;
	x->attackms = at;
	x->releasems = rel;
	x->outGain = outg;

	x->Zec0 = (double**)sysmem_newptrclear(x->_nchan * sizeof(double*));
	
	for (int j = 0; j < x->_nchan; j++) {
		x->Zec0[j] = (double *)sysmem_newptr(x->nsize * sizeof(double));
	}
	x->Zec1 = (double *)sysmem_newptr(x->nsize * sizeof(double));
	x->Zec2 = (double *)sysmem_newptr(x->nsize * sizeof(double));

	x->Rec2_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));
	x->Rec1_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));

	x->Zec3 = (double *)sysmem_newptr(x->nsize * sizeof(double));
	x->Zec4 = (double *)sysmem_newptr(x->nsize * sizeof(double));

	x->Rec0_tmp = (double *)sysmem_newptr((x->nsize + 4) * sizeof(double));

	x->Zec5 = (double *)sysmem_newptr(x->nsize * sizeof(double));

	x->fZec0 = (float *)sysmem_newptr(x->nsize * sizeof(float));
	x->fZec1 = (float *)sysmem_newptr(x->nsize * sizeof(float));
	x->fZec2 = (float *)sysmem_newptr(x->nsize * sizeof(float));

	x->fRec2_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));
	x->fRec1_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));

	x->fZec3 = (float *)sysmem_newptr(x->nsize * sizeof(float));
	x->fZec4 = (float *)sysmem_newptr(x->nsize * sizeof(float));

	x->fRec0_tmp = (float *)sysmem_newptr((x->nsize + 4) * sizeof(float));

	x->fZec5 = (float *)sysmem_newptr(x->nsize * sizeof(float));

	for (int i = 0; i < x->nsize; i++) {

		for (int j = 0; j < x->_nchan; j++) {
			x->Zec0[j][i] = 0;
		}
		x->Zec1[i] = 0;
		x->Zec2[i] = 0;

		x->Rec2_tmp[i] = 0;
		x->Rec1_tmp[i] = 0;

		x->Zec3[i] = 0;
		x->Zec4[i] = 0;

		x->Rec0_tmp[i] = 0;

		x->Zec5[i] = 0;

		x->fZec0[i] = 0;
		x->fZec1[i] = 0;
		x->fZec2[i] = 0;

		x->fRec2_tmp[i] = 0;
		x->fRec1_tmp[i] = 0;

		x->fZec3[i] = 0;
		x->fZec4[i] = 0;

		x->fRec0_tmp[i] = 0;

		x->fZec5[i] = 0;
	}

	return x;
}

void rad_assist(t_rad *x, void *b, long msg, long arg, char *dst) {
	if (msg == ASSIST_INLET) {

		for (int i = 0; i < x->_nchan; i++) {
			if (arg == i) sprintf(dst, "(signal) Input %d", i + 1);
		}
	}
	else if (msg == ASSIST_OUTLET) {
		for (int i = 0; i < x->_nchan; i++) {
			if (arg == i) sprintf(dst, "(signal) Output %d", i + 1);
		}
		if (arg == x->_nchan) sprintf(dst, "(signal) Amount of Compression");
	}
}