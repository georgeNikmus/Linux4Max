#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

//#include "math.h"
//#include <cmath>

static t_class *violinf_class;

typedef struct _violinf {
	t_pxobject x_obj;

	double strlength;
	double strpressure;
	double strvelocity;
	double strposition;
	double strout;

	double fSamplingFreq;

	int *iRec10;
	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double *fRec26;
	double *fRec27;
	double *fRec22;
	double *fRec28;
	double *fRec29;

	long IOTA;

	double *fRec30;
	double *fVec0;
	double *fRec32;
	double *fVec1;
	double *fRec18;
	double *fRec20;
	double *fRec16;
	double fConst6;
	double fConst7;
	double *fRec14;
	double fConst8;
	int *iRec6;
	double *fRec2;
	double *fRec0;

}t_violinf;

void violinf_dsp64(t_violinf *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);

void violinf_perform64(t_violinf *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

double mydsp_faustpower2_f(double value);
double mydsp_faustpower4_f(double value);

void setVelocity(t_violinf *x, double vel);
void setLength(t_violinf *x, double lngth);
void setPressure(t_violinf *x, double prs);
void setPosition(t_violinf *x, double pos);
void setOutGain(t_violinf *x, double gn);

void setDef(t_violinf *x);
void clearall(t_violinf *x);

void* violinf_new(t_symbol* s, short argc, t_atom* argv);
void violinf_free(t_violinf* x);

void ext_main(void *r) {

	t_class *c;
	c = class_new("violinf~", (method)violinf_new, (method)violinf_free, sizeof(t_violinf), 0L, A_GIMME, 0);

	class_addmethod(c, (method)violinf_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setVelocity, "velocity", A_FLOAT, 0);
	class_addmethod(c, (method)setLength, "length", A_FLOAT, 0);
	class_addmethod(c, (method)setPressure, "pressure", A_FLOAT, 0);
	class_addmethod(c, (method)setPosition, "pos", A_FLOAT, 0);
	class_addmethod(c, (method)setOutGain, "OutGain", A_FLOAT, 0);

	class_addmethod(c, (method)setDef, "setDefault", 0);
	class_addmethod(c, (method)clearall, "clearall", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	violinf_class = c;

	post("--------------------------------------------------------------------------");
	post("The violin physical model by Romain Michon");
	post("Max/MSP implementation with 64-bit perform routine: George Nikolopoulos - 2018 -");
	post("Default: out Gain = 0.5 position = 0.7 length = 0.75 velocity = 0.0 pressure = 0.5");
	post(" ");

	//return 0;
}

void* violinf_new(t_symbol* s, short argc, t_atom* argv)
{
	t_violinf* x = (t_violinf*)object_alloc(violinf_class);

	dsp_setup((t_pxobject *)x, 0);
	outlet_new((t_object *)x, "signal");
	//outlet_new((t_object *)x, "signal");

	//((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different
	x->fSamplingFreq = sys_getsr();

	x->fConst0 = min(192000.0, max(1.0, (double)x->fSamplingFreq));
	x->fConst1 = tan((1570.79639 / x->fConst0));
	x->fConst2 = (1.0 / x->fConst1);
	x->fConst3 = (1.0 / (((x->fConst2 + 0.5) / x->fConst1) + 1.0));
	x->fConst4 = (0.00882352982 * x->fConst0);
	x->fConst5 = (0.00147058826 * x->fConst0);
	x->fConst6 = (((x->fConst2 + -0.5) / x->fConst1) + 1.0);
	x->fConst7 = (2.0 * (1.0 - (1.0 / mydsp_faustpower2_f(x->fConst1))));
	x->fConst8 = (0.0 - x->fConst2);

	x->iRec10 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec26 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec27= (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec28 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptr(4 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fVec0 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec16 = (double *)sysmem_newptr(4 * sizeof(double));
	x->iRec6 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec2 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec14= (double *)sysmem_newptr(3 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->iRec10[l0] = 0;
	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec26[l1] = 0.0;
	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		x->fRec27[l2] = 0.0;
	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		x->fRec22[l3] = 0.0;
	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec28[l4] = 0.0;
	}
	for (int l5 = 0; (l5 < 4); l5 = (l5 + 1)) {
		x->fRec29[l5] = 0.0;
	}
	x->IOTA = 0;
	for (int l6 = 0; (l6 < 2048); l6 = (l6 + 1)) {
		x->fRec30[l6] = 0.0;
	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fVec0[l7] = 0.0;
	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec32[l8] = 0.0;
	}
	for (int l9 = 0; (l9 < 2048); l9 = (l9 + 1)) {
		x->fVec1[l9] = 0.0;
	}
	for (int l10 = 0; (l10 < 2048); l10 = (l10 + 1)) {
		x->fRec18[l10] = 0.0;
	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec20[l11] = 0.0;
	}
	for (int l12 = 0; (l12 < 4); l12 = (l12 + 1)) {
		x->fRec16[l12] = 0.0;
	}
	for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
		x->fRec14[l13] = 0.0;
	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->iRec6[l14] = 0;
	}
	for (int l15 = 0; (l15 < 2048); l15 = (l15 + 1)) {
		x->fRec2[l15] = 0.0;
	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec0[l16] = 0.0;
	}

	x->strout = 0.5;
	x->strposition = 0.7;
	x->strlength = 0.75;
	x->strvelocity = 0.0;
	x->strpressure = 0.5;

	return x;
}

void violinf_dsp64(t_violinf *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->fSamplingFreq != samplerate) x->fSamplingFreq = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		violinf_perform64, 0, NULL);
}

void violinf_perform64(t_violinf *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *output0 = outs[0];
	int n = vectorsize;

	double fSlow0 = (double)x->strout;
	double fSlow1 = (0.00100000005 * (double)x->strposition);
	double fSlow2 = (0.00100000005 * (double)x->strlength);
	double fSlow3 = (0.00100000005 * (double)x->strvelocity);
	double fSlow4 = (0.00100000005 * (double)x->strpressure);
	while(n--){
		x->iRec10[0] = 0;
		int iRec11 = x->iRec10[1];
		double fRec15 = ((double)x->iRec6[1] - (0.989264667 * ((0.600000024 * x->fRec16[2]) + (0.200000003 * (x->fRec16[1] + x->fRec16[3])))));
		x->fRec26[0] = (fSlow1 + (0.999000013 * x->fRec26[1]));
		x->fRec27[0] = (fSlow2 + (0.999000013 * x->fRec27[1]));
		double fTemp0 = (x->fRec27[0] + -0.0799999982);
		double fTemp1 = (x->fConst5 * ((1.0 - x->fRec26[0]) * fTemp0));
		double fTemp2 = (fTemp1 + -1.49999499);
		int iTemp3 = (int)fTemp2;
		int iTemp4 = (int)(min(x->fConst4, (double)(max(0, iTemp3))));
		double fTemp5 = floor(fTemp2);
		double fTemp6 = (fTemp1 + (-1.0 - fTemp5));
		double fTemp7 = (0.0 - fTemp6);
		double fTemp8 = (fTemp1 + (-2.0 - fTemp5));
		double fTemp9 = (0.0 - (0.5 * fTemp8));
		double fTemp10 = (fTemp1 + (-3.0 - fTemp5));
		double fTemp11 = (0.0 - (0.333333343 * fTemp10));
		double fTemp12 = (fTemp1 + (-4.0 - fTemp5));
		double fTemp13 = (0.0 - (0.25 * fTemp12));
		double fTemp14 = (fTemp1 - fTemp5);
		int iTemp15 = (int)(min(x->fConst4, (double)(max(0, (iTemp3 + 1)))));
		double fTemp16 = (0.0 - fTemp8);
		double fTemp17 = (0.0 - (0.5 * fTemp10));
		double fTemp18 = (0.0 - (0.333333343 * fTemp12));
		int iTemp19 = (int)(min(x->fConst4, (double)(max(0, (iTemp3 + 2)))));
		double fTemp20 = (0.0 - fTemp10);
		double fTemp21 = (0.0 - (0.5 * fTemp12));
		double fTemp22 = (fTemp6 * fTemp8);
		int iTemp23 = (int)(min(x->fConst4, (double)(max(0, (iTemp3 + 3)))));
		double fTemp24 = (0.0 - fTemp12);
		double fTemp25 = (fTemp22 * fTemp10);
		int iTemp26 = (int)(min(x->fConst4, (double)(max(0, (iTemp3 + 4)))));
		x->fRec22[0] = (((((x->fRec2[((x->IOTA - (iTemp4 + 1)) & 2047)] * fTemp7) * fTemp9) * fTemp11) * fTemp13) + (fTemp14 * ((((((x->fRec2[((x->IOTA - (iTemp15 + 1)) & 2047)] * fTemp16) * fTemp17) * fTemp18) + (0.5 * (((fTemp6 * x->fRec2[((x->IOTA - (iTemp19 + 1)) & 2047)]) * fTemp20) * fTemp21))) + (0.166666672 * ((fTemp22 * x->fRec2[((x->IOTA - (iTemp23 + 1)) & 2047)]) * fTemp24))) + (0.0416666679 * (fTemp25 * x->fRec2[((x->IOTA - (iTemp26 + 1)) & 2047)])))));
		x->fRec28[0] = (fSlow3 + (0.999000013 * x->fRec28[1]));
		x->fRec29[0] = x->fRec0[1];
		x->fRec30[(x->IOTA & 2047)] = (0.0 - (0.99880147 * ((0.800000012 * x->fRec29[2]) + (0.100000001 * (x->fRec29[1] + x->fRec29[3])))));
		int iRec31 = 0;
		double fTemp27 = (x->fConst5 * (x->fRec26[0] * fTemp0));
		double fTemp28 = (fTemp27 + -1.49999499);
		int iTemp29 = (int)(fTemp28);
		int iTemp30 = (int)(min(x->fConst4, (double)(max(0, iTemp29))));
		double fTemp31 = floor(fTemp28);
		double fTemp32 = (fTemp27 + (-1.0 - fTemp31));
		double fTemp33 = (0.0 - fTemp32);
		double fTemp34 = (fTemp27 + (-2.0 - fTemp31));
		double fTemp35 = (0.0 - (0.5 * fTemp34));
		double fTemp36 = (fTemp27 + (-3.0 - fTemp31));
		double fTemp37 = (0.0 - (0.333333343 * fTemp36));
		double fTemp38 = (fTemp27 + (-4.0 - fTemp31));
		double fTemp39 = (0.0 - (0.25 * fTemp38));
		double fTemp40 = (fTemp27 - fTemp31);
		int iTemp41 = (int)(min(x->fConst4, (double)(max(0, (iTemp29 + 1)))));
		double fTemp42 = (0.0 - fTemp34);
		double fTemp43 = (0.0 - (0.5f * fTemp36));
		double fTemp44 = (0.0 - (0.333333343 * fTemp38));
		int iTemp45 = (int)(min(x->fConst4, (double)(max(0, (iTemp29 + 2)))));
		double fTemp46 = (0.0 - fTemp36);
		double fTemp47 = (0.0 - (0.5 * fTemp38));
		double fTemp48 = (fTemp32 * fTemp34);
		int iTemp49 = (int)(min(x->fConst4, (double)(max(0, (iTemp29 + 3)))));
		double fTemp50 = (0.0 - fTemp38);
		double fTemp51 = (fTemp48 * fTemp36);
		int iTemp52 = (int)(min(x->fConst4, (double)(max(0, (iTemp29 + 4)))));
		x->fVec0[0] = (((((x->fRec30[((x->IOTA - (iTemp30 + 2)) & 2047)] * fTemp33) * fTemp35) * fTemp37) * fTemp39) + (fTemp40 * ((((((x->fRec30[((x->IOTA - (iTemp41 + 2)) & 2047)] * fTemp42) * fTemp43) * fTemp44) + (0.5 * (((fTemp32 * x->fRec30[((x->IOTA - (iTemp45 + 2)) & 2047)]) * fTemp46) * fTemp47))) + (0.166666672 * ((fTemp48 * x->fRec30[((x->IOTA - (iTemp49 + 2)) & 2047)]) * fTemp50))) + (0.0416666679 * (fTemp51 * x->fRec30[((x->IOTA - (iTemp52 + 2)) & 2047)])))));
		double fTemp53 = (x->fRec28[0] - (x->fRec22[1] + x->fVec0[1]));
		x->fRec32[0] = (fSlow4 + (0.999000013 * x->fRec32[1]));
		double fTemp54 = (fTemp53 * min(1.0, (1.0 / mydsp_faustpower4_f((fabs(((5.0 - (4.0 * x->fRec32[0])) * fTemp53)) + 0.75)))));
		double fRec23 = (x->fRec22[1] + fTemp54);
		double fTemp55 = (x->fVec0[1] + fTemp54);
		x->fVec1[(x->IOTA & 2047)] = fTemp55;
		double fRec24 = (((((fTemp7 * fTemp9) * fTemp11) * fTemp13) * x->fVec1[((x->IOTA - iTemp4) & 2047)]) + (fTemp14 * ((((((fTemp16 * fTemp17) * fTemp18) * x->fVec1[((x->IOTA - iTemp15) & 2047)]) + (0.5 * (((fTemp6 * fTemp20) * fTemp21) * x->fVec1[((x->IOTA - iTemp19) & 2047)]))) + (0.166666672 * ((fTemp22 * fTemp24) * x->fVec1[((x->IOTA - iTemp23) & 2047)]))) + (0.0416666679 * (fTemp25 * x->fVec1[((x->IOTA - iTemp26) & 2047)])))));
		int iRec25 = iRec31;
		x->fRec18[(x->IOTA & 2047)] = fRec23;
		double fRec19 = (((((fTemp33 * fTemp35) * fTemp37) * fTemp39) * x->fRec18[((x->IOTA - (iTemp30 + 1)) & 2047)]) + (fTemp40 * ((((((fTemp42 * fTemp43) * fTemp44) * x->fRec18[((x->IOTA - (iTemp41 + 1)) & 2047)]) + (0.5 * (((fTemp32 * fTemp46) * fTemp47) * x->fRec18[((x->IOTA - (iTemp45 + 1)) & 2047)]))) + (0.166666672 * ((fTemp48 * fTemp50) * x->fRec18[((x->IOTA - (iTemp49 + 1)) & 2047)]))) + (0.0416666679 * (fTemp51 * x->fRec18[((x->IOTA - (iTemp52 + 1)) & 2047)])))));
		x->fRec20[0] = fRec24;
		int iRec21 = iRec25;
		x->fRec16[0] = x->fRec20[1];
		int iRec17 = iRec21;
		x->fRec14[0] = (x->fRec16[1] - (x->fConst3 * ((x->fConst6 * x->fRec14[2]) + (x->fConst7 * x->fRec14[1]))));
		double fTemp56 = (x->fConst3 * ((x->fConst2 * x->fRec14[0]) + (x->fConst8 * x->fRec14[2])));
		double fRec12 = fTemp56;
		double fRec13 = ((double)iRec17 + fTemp56);
		x->iRec6[0] = iRec11;
		double fRec7 = fRec15;
		double fRec8 = fRec12;
		double fRec9 = fRec13;
		x->fRec2[(x->IOTA & 2047)] = fRec7;
		double fRec3 = fRec19;
		double fRec4 = fRec8;
		double fRec5 = fRec9;
		x->fRec0[0] = fRec3;
		double fRec1 = fRec5;
		double fTemp57 = (fSlow0 * fRec1);
		
		*output0++ = (double)fTemp57;
		
		x->iRec10[1] = x->iRec10[0];
		x->fRec26[1] = x->fRec26[0];
		x->fRec27[1] = x->fRec27[0];
		x->fRec22[1] = x->fRec22[0];
		x->fRec28[1] = x->fRec28[0];
		for (int j0 = 3; (j0 > 0); j0 = (j0 - 1)) {
			x->fRec29[j0] = x->fRec29[(j0 - 1)];
		}
		x->IOTA = (x->IOTA + 1);
		x->fVec0[1] = x->fVec0[0];
		x->fRec32[1] = x->fRec32[0];
		x->fRec20[1] = x->fRec20[0];
		for (int j1 = 3; (j1 > 0); j1 = (j1 - 1)) {
			x->fRec16[j1] = x->fRec16[(j1 - 1)];
		}
		x->fRec14[2] = x->fRec14[1];
		x->fRec14[1] = x->fRec14[0];
		x->iRec6[1] = x->iRec6[0];
		x->fRec0[1] = x->fRec0[0];
	}
}

double mydsp_faustpower4_f(double value) {
	return (((value * value) * value) * value);
}

double mydsp_faustpower2_f(double value) {
	return (value * value);
}

void violinf_free(t_violinf* x) {
	dsp_free((t_pxobject*)x);

	sysmem_freeptr(x->iRec10);
	sysmem_freeptr(x->fRec26);
	sysmem_freeptr(x->fRec27);
	sysmem_freeptr(x->fRec22);
	sysmem_freeptr(x->fRec28);
	sysmem_freeptr(x->fRec29);
	sysmem_freeptr(x->fRec30);
	sysmem_freeptr(x->fVec0);
	sysmem_freeptr(x->fRec32);
	sysmem_freeptr(x->fVec1);
	sysmem_freeptr(x->fRec18);
	sysmem_freeptr(x->fRec20);
	sysmem_freeptr(x->fRec16);
	sysmem_freeptr(x->iRec6);
	sysmem_freeptr(x->fRec2);
	sysmem_freeptr(x->fRec0);
	sysmem_freeptr(x->fRec14);
}

void setVelocity(t_violinf *x, double vel) {
	if (vel < 0.0) x->strvelocity = 0.0;
	else if (vel > 1.0) x->strvelocity = 1.0;
	else x->strvelocity = vel;
}

void setLength(t_violinf *x, double lngth) {
	if (lngth < 0.0) x->strlength = 0.0;
	//else if (lngth > 2.0) x->strlength = 2.0;
	else x->strlength = lngth;
}

void setPressure(t_violinf *x, double prs) {
	if (prs < 0.0) x->strpressure = 0.0;
	else if (prs > 1.0) x->strpressure = 1.0;
	else x->strpressure = prs;
}

void setPosition(t_violinf *x, double pos) {
	if (pos < 0.0) x->strposition = 0.0;
	else if (pos > 1.0) x->strposition = 1.0;
	else x->strposition = pos;
}

void setOutGain(t_violinf *x, double gn) {
	if (gn < 0.0) x->strout = 0.0;
	else if (gn > 1.0) x->strout = 1.0;
	else x->strout = gn;
}

void setDef(t_violinf *x) {
	x->strout = 0.5;
	x->strposition = 0.7;
	x->strlength = 0.75;
	x->strvelocity = 0.0;
	x->strpressure = 0.5;
}

void clearall(t_violinf *x) {
	x->strvelocity = 0.0;

	x->iRec10 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec26 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec27 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec28 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptrclear(4 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fVec0 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec16 = (double *)sysmem_newptrclear(4 * sizeof(double));
	x->iRec6 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec2 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec14 = (double *)sysmem_newptrclear(3 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->iRec10[l0] = 0;
	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec26[l1] = 0.0;
	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		x->fRec27[l2] = 0.0;
	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		x->fRec22[l3] = 0.0;
	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec28[l4] = 0.0;
	}
	for (int l5 = 0; (l5 < 4); l5 = (l5 + 1)) {
		x->fRec29[l5] = 0.0;
	}
	x->IOTA = 0;
	for (int l6 = 0; (l6 < 2048); l6 = (l6 + 1)) {
		x->fRec30[l6] = 0.0;
	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fVec0[l7] = 0.0;
	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec32[l8] = 0.0;
	}
	for (int l9 = 0; (l9 < 2048); l9 = (l9 + 1)) {
		x->fVec1[l9] = 0.0;
	}
	for (int l10 = 0; (l10 < 2048); l10 = (l10 + 1)) {
		x->fRec18[l10] = 0.0;
	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec20[l11] = 0.0;
	}
	for (int l12 = 0; (l12 < 4); l12 = (l12 + 1)) {
		x->fRec16[l12] = 0.0;
	}
	for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
		x->fRec14[l13] = 0.0;
	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->iRec6[l14] = 0;
	}
	for (int l15 = 0; (l15 < 2048); l15 = (l15 + 1)) {
		x->fRec2[l15] = 0.0;
	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec0[l16] = 0.0;
	}
}