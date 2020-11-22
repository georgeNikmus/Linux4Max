#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

//#include <math.h>
//#include <cmath>

static t_class *zitarev_class;

typedef struct _zitarev {
	t_pxobject x_obj;

	double revlevel; //-20, -70.0 - 40.0
	double revdrywet; //0.0 -1.0 - 1.0
	double reveq2freq; //1500.0 160.0 - 10000.0
	double reveq2level; //0.0 -15.0 -15.0
	double reveq1freq; //315.0 40.0 - 2500.0
	double reveq1level; //0.0  -15.0 - 15.0
	double midRT60; //2.0 1.0 - 8.0
	double hfdamp; //6000.0 1500.0 - 23520.0
	double lowRT60; //3.0 1.0 - 8.0
	double lfX; //200.0 50.0 - 1000.0
	double Indel; //60.0 20.0 - 100.0 ms

	double fSamplingFreq;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	int iConst7;
	double fConst8;
	int iConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	int iConst13;
	int iConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	int iConst18;
	int iConst19;
	double fConst20;
	double fConst21;
	double fConst22;
	int iConst23;
	int iConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	int iConst28;
	int iConst29;
	double fConst30;
	double fConst31;
	double fConst32;
	int iConst33;
	int iConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	int iConst38;
	int iConst39;
	double fConst40;
	double fConst41;
	double fConst42;
	int iConst43;
	int iConst44;

	long IOTA;

	double *fRec0; //2
	double *fRec1; //2	
	double *fVec0; // [16384];
	double *fRec15; // [2];
	double *fRec14; // [2];
	double *fVec1; // [32768];
	double *fVec2; //[16384];
	double *fVec3;// [2048];
	double *fRec12;// [2];
	double *fRec19;// [2];
	double *fRec18;//[2];
	double *fVec4;// [32768];
	double *fVec5;// [4096];
	double *fRec16;//[2];
	double *fRec23;// [2];
	double *fRec22;// [2];
	double *fVec6;// [16384];
	double *fVec7;// [4096];
	double *fRec20;// [2];
	double *fRec27;// [2];
	double *fRec26;// [2];
	double *fVec8;// [32768];
	double *fVec9;// [4096];
	double *fRec24;// [2];
	double *fRec31;// [2];
	double *fRec30;// [2];
	double *fVec10;// [16384];
	double *fVec11;// [2048];
	double *fRec28;// [2];
	double *fRec35;// [2];
	double *fRec34;// [2];
	double *fVec12;// [16384];
	double *fVec13;// [4096];
	double *fRec32;// [2];
	double *fRec39;//[2];
	double *fRec38;// [2];
	double *fVec14;// [16384];
	double *fVec15;// [4096];
	double *fRec36;// [2];
	double *fRec43;// [2];
	double *fRec42;// [2];
	double *fVec16;// [16384];
	double *fVec17;// [2048];
	double *fRec40;// [2];
	double *fRec4;// [3];
	double *fRec5;// [3];
	double *fRec6;// [3];
	double *fRec7;// [3];
	double *fRec8;// [3];
	double *fRec9;// [3];
	double *fRec10;// [3];
	double *fRec11;// [3];
	double *fRec3;// [3];
	double *fRec2;// [3];
	double *fRec45;// [3];
	double *fRec44;// [3];

}t_zitarev;

void zitarev_dsp64(t_zitarev *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);

void zitarev_perform64(t_zitarev *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

double mydsp_faustpower2_f(double value);

void setLevel(t_zitarev *x, double lev);
void setDryWet(t_zitarev *x, double drwt);
void setEq2fr(t_zitarev *x, double eq2fr);
void setEq2Lev(t_zitarev *x, double eq2lv);
void setEq1fr(t_zitarev *x, double eq1fr);
void setEq1Lev(t_zitarev *x, double eq1lv);
void setMidRT(t_zitarev *x, double midrt);
void setHfDamp(t_zitarev *x, double hfdamp);
void setLowRT(t_zitarev *x, double lowrt);
void setLFX(t_zitarev *x, double lfx);
void setInDel(t_zitarev *x, double indel);

void setDef(t_zitarev *x);

void* zitarev_new(t_symbol* s, short argc, t_atom* argv);
void zitarev_free(t_zitarev* x);

int C74_EXPORT main(void) {

	t_class *c;
	c = class_new("zitarev~", (method)zitarev_new, (method)zitarev_free, sizeof(t_zitarev), 0L, A_GIMME, 0);

	class_addmethod(c, (method)zitarev_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setLevel, "revlevel", A_FLOAT, 0);
	class_addmethod(c, (method)setDryWet, "drywet", A_FLOAT, 0);
	class_addmethod(c, (method)setEq2fr, "eq2freq", A_FLOAT, 0);
	class_addmethod(c, (method)setEq2Lev, "eq2lev", A_FLOAT, 0);
	class_addmethod(c, (method)setEq1fr, "eq1freq", A_FLOAT, 0);
	class_addmethod(c, (method)setEq1Lev, "eq1lev", A_FLOAT, 0);
	class_addmethod(c, (method)setMidRT, "midrt", A_FLOAT, 0);
	class_addmethod(c, (method)setLowRT, "lowrt", A_FLOAT, 0);
	class_addmethod(c, (method)setHfDamp, "hfdamp", A_FLOAT, 0);
	class_addmethod(c, (method)setLFX, "lfx", A_FLOAT, 0);
	class_addmethod(c, (method)setInDel, "inputdel", A_FLOAT, 0);

	class_addmethod(c, (method)setDef, "setDefault", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zitarev_class = c;

	post("--------------------------------------------------------------------------");
	post("Zita Reverb");
	post("Max/MSP implementation with 64-bit perform routine: George Nikolopoulos - 2018 -");
	post("Default: reverb level = -20.0 Dry/Wet = 0.0 Eq2freq = 1500.0 Eq2level = 0.0 ");
	post(" Eq1freq = 315.0 Eq1level = 0.0 midRT60 = 2.0 lowRT60 = 3.0 hf damping = 6000.0 LFX = 200.0 Input Delay = 60.0ms");

	return 0;
}

void* zitarev_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zitarev* x = (t_zitarev*)object_alloc(zitarev_class);

	dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different
	x->fSamplingFreq = sys_getsr();

	x->fConst0 = min(192000.0, max(1.0, (double)x->fSamplingFreq));
	x->fConst1 = (6.28318548 / x->fConst0);
	x->fConst2 = floor(((0.219990999 * x->fConst0) + 0.5));
	x->fConst3 = ((0.0 - (6.90775537 * x->fConst2)) / x->fConst0);
	x->fConst4 = (6.28318548 / x->fConst0);
	x->fConst5 = (3.14159274 / x->fConst0);
	x->fConst6 = floor(((0.0191229992 * x->fConst0) + 0.5));
	x->iConst7 = (int)(min(16384.0, max(0.0, (x->fConst2 - x->fConst6))));
	x->fConst8 = (0.00100000005 * x->fConst0);
	x->iConst9 = (int)(min(1024.0, max(0.0, (x->fConst6 + -1.0))));
	x->fConst10 = floor(((0.256891012 * x->fConst0) + 0.5));
	x->fConst11 = ((0.0 - (6.90775537 * x->fConst10)) / x->fConst0);
	x->fConst12 = floor(((0.0273330007 * x->fConst0) + 0.5));
	x->iConst13 = (int)(min(16384.0, max(0.0, (x->fConst10 - x->fConst12))));
	x->iConst14 = (int)(min(2048.0, max(0.0, (x->fConst12 + -1.0))));
	x->fConst15 = floor(((0.192303002 * x->fConst0) + 0.5));
	x->fConst16 = ((0.0 - (6.90775537 * x->fConst15)) / x->fConst0);
	x->fConst17 = floor(((0.0292910002 * x->fConst0) + 0.5));
	x->iConst18 = (int)(min(8192.0, max(0.0, (x->fConst15 - x->fConst17))));
	x->iConst19 = (int)(min(2048.0, max(0.0, (x->fConst17 + -1.0))));
	x->fConst20 = floor(((0.210389003 * x->fConst0) + 0.5));
	x->fConst21 = ((0.0 - (6.90775537 * x->fConst20)) / x->fConst0);
	x->fConst22 = floor(((0.0244210009 * x->fConst0) + 0.5));
	x->iConst23 = (int)(min(16384.0, max(0.0, (x->fConst20 - x->fConst22))));
	x->iConst24 = (int)(min(2048.0, max(0.0, (x->fConst22 + -1.0))));
	x->fConst25 = floor(((0.125 * x->fConst0) + 0.5));
	x->fConst26 = ((0.0 - (6.90775537 * x->fConst25)) / x->fConst0);
	x->fConst27 = floor(((0.0134579996 * x->fConst0) + 0.5));
	x->iConst28 = (int)(min(8192.0, max(0.0, (x->fConst25 - x->fConst27))));
	x->iConst29 = (int)(min(1024.0, max(0.0, (x->fConst27 + -1.0))));
	x->fConst30 = floor(((0.127837002 * x->fConst0) + 0.5));
	x->fConst31 = ((0.0 - (6.90775537 * x->fConst30)) / x->fConst0);
	x->fConst32 = floor(((0.0316039994 * x->fConst0) + 0.5));
	x->iConst33 = (int)(min(8192.0, max(0.0, (x->fConst30 - x->fConst32))));
	x->iConst34 = (int)(min(2048.0, max(0.0, (x->fConst32 + -1.0))));
	x->fConst35 = floor(((0.174713001 * x->fConst0) + 0.5));
	x->fConst36 = ((0.0 - (6.90775537 * x->fConst35)) / x->fConst0);
	x->fConst37 = floor(((0.0229039993 * x->fConst0) + 0.5));
	x->iConst38 = (int)(min(8192.0, max(0.0, (x->fConst35 - x->fConst37))));
	x->iConst39 = (int)(min(2048.0, max(0.0, (x->fConst37 + -1.0))));
	x->fConst40 = floor(((0.153128996 * x->fConst0) + 0.5));
	x->fConst41 = ((0.0 - (6.90775537 * x->fConst40)) / x->fConst0);
	x->fConst42 = floor(((0.0203460008 * x->fConst0) + 0.5));
	x->iConst43 = (int)(min(8192.0, max(0.0, (x->fConst40 - x->fConst42))));
	x->iConst44 = (int)(min(1024.0, max(0.0, (x->fConst42 + -1.0))));


	x->fRec0 = (double *)sysmem_newptr(2 * sizeof(double)); //2
	x->fRec1 = (double *)sysmem_newptr(2 * sizeof(double)); //2	
	x->fVec0 = (double *)sysmem_newptr(16384 * sizeof(double)); //16384
	x->fRec15 = (double *)sysmem_newptr(2 * sizeof(double)); //2
	x->fRec14 = (double *)sysmem_newptr(2 * sizeof(double)); //2
	x->fVec1 = (double *)sysmem_newptr(32768 * sizeof(double)); //32768
	x->fVec2 = (double *)sysmem_newptr(16384 * sizeof(double)); //16384
	x->fVec3 = (double *)sysmem_newptr(2048 * sizeof(double));//2048
	x->fRec12 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec19 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec18 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fVec4 = (double *)sysmem_newptr(32768 * sizeof(double));//32768
	x->fVec5 = (double *)sysmem_newptr(4096 * sizeof(double));//4096
	x->fRec16 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec23 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec22 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fVec6 = (double *)sysmem_newptr(16384 * sizeof(double));//16384
	x->fVec7 = (double *)sysmem_newptr(4096 * sizeof(double));//4096
	x->fRec20 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec27 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec26 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fVec8 = (double *)sysmem_newptr(32768 * sizeof(double));//32768
	x->fVec9 = (double *)sysmem_newptr(4096 * sizeof(double));//4096
	x->fRec24 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec31 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fRec30 = (double *)sysmem_newptr(2 * sizeof(double));//2
	x->fVec10 = (double *)sysmem_newptr(16384 * sizeof(double));//16384
	x->fVec11 = (double *)sysmem_newptr(2048 * sizeof(double));//2048
	x->fRec28 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec35 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec34 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fVec12 = (double *)sysmem_newptr(16384 * sizeof(double));// [16384];
	x->fVec13 = (double *)sysmem_newptr(4096 * sizeof(double));// [4096];
	x->fRec32 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec39 = (double *)sysmem_newptr(2 * sizeof(double));//[2];
	x->fRec38 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fVec14 = (double *)sysmem_newptr(16384 * sizeof(double));// [16384];
	x->fVec15 = (double *)sysmem_newptr(4096 * sizeof(double));// [4096];
	x->fRec36 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec43 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec42 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fVec16 = (double *)sysmem_newptr(16384 * sizeof(double));// [16384];
	x->fVec17 = (double *)sysmem_newptr(2048 * sizeof(double));// [2048];
	x->fRec40 = (double *)sysmem_newptr(2 * sizeof(double));// [2];
	x->fRec4 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec5 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec6 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec7 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec8 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec9 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec10 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec11 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec3 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec2 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec45 = (double *)sysmem_newptr(3 * sizeof(double));// [3];
	x->fRec44 = (double *)sysmem_newptr(3 * sizeof(double));// [3];

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->fRec0[l0] = 0.0;
	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec1[l1] = 0.0;
	}

	x->IOTA = 0;
	for (int l2 = 0; (l2 < 16384); l2 = (l2 + 1)) {
		x->fVec0[l2] = 0.0;
	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		x->fRec15[l3] = 0.0;
	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec14[l4] = 0.0;
	}
	for (int l5 = 0; (l5 < 32768); l5 = (l5 + 1)) {
		x->fVec1[l5] = 0.0;
	}
	for (int l6 = 0; (l6 < 16384); l6 = (l6 + 1)) {
		x->fVec2[l6] = 0.0;
	}
	for (int l7 = 0; (l7 < 2048); l7 = (l7 + 1)) {
		x->fVec3[l7] = 0.0;
	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec12[l8] = 0.0;
	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		x->fRec19[l9] = 0.0;
	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		x->fRec18[l10] = 0.0;
	}
	for (int l11 = 0; (l11 < 32768); l11 = (l11 + 1)) {
		x->fVec4[l11] = 0.0;
	}
	for (int l12 = 0; (l12 < 4096); l12 = (l12 + 1)) {
		x->fVec5[l12] = 0.0;
	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		x->fRec16[l13] = 0.0;
	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->fRec23[l14] = 0.0;
	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		x->fRec22[l15] = 0.0;
	}
	for (int l16 = 0; (l16 < 16384); l16 = (l16 + 1)) {
		x->fVec6[l16] = 0.0;
	}
	for (int l17 = 0; (l17 < 4096); l17 = (l17 + 1)) {
		x->fVec7[l17] = 0.0;
	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		x->fRec20[l18] = 0.0;
	}
	for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
		x->fRec27[l19] = 0.0;
	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		x->fRec26[l20] = 0.0;
	}
	for (int l21 = 0; (l21 < 32768); l21 = (l21 + 1)) {
		x->fVec8[l21] = 0.0;
	}
	for (int l22 = 0; (l22 < 4096); l22 = (l22 + 1)) {
		x->fVec9[l22] = 0.0;
	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		x->fRec24[l23] = 0.0;
	}
	for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
		x->fRec31[l24] = 0.0;
	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		x->fRec30[l25] = 0.0;
	}
	for (int l26 = 0; (l26 < 16384); l26 = (l26 + 1)) {
		x->fVec10[l26] = 0.0;
	}
	for (int l27 = 0; (l27 < 2048); l27 = (l27 + 1)) {
		x->fVec11[l27] = 0.0;
	}
	for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
		x->fRec28[l28] = 0.0;
	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		x->fRec35[l29] = 0.0;
	}
	for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
		x->fRec34[l30] = 0.0;
	}
	for (int l31 = 0; (l31 < 16384); l31 = (l31 + 1)) {
		x->fVec12[l31] = 0.0;
	}
	for (int l32 = 0; (l32 < 4096); l32 = (l32 + 1)) {
		x->fVec13[l32] = 0.0;
	}
	for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
		x->fRec32[l33] = 0.0;
	}
	for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
		x->fRec39[l34] = 0.0;
	}
	for (int l35 = 0; (l35 < 2); l35 = (l35 + 1)) {
		x->fRec38[l35] = 0.0;
	}
	for (int l36 = 0; (l36 < 16384); l36 = (l36 + 1)) {
		x->fVec14[l36] = 0.0;
	}
	for (int l37 = 0; (l37 < 4096); l37 = (l37 + 1)) {
		x->fVec15[l37] = 0.0;
	}
	for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
		x->fRec36[l38] = 0.0;
	}
	for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
		x->fRec43[l39] = 0.0;
	}
	for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
		x->fRec42[l40] = 0.0;
	}
	for (int l41 = 0; (l41 < 16384); l41 = (l41 + 1)) {
		x->fVec16[l41] = 0.0;
	}
	for (int l42 = 0; (l42 < 2048); l42 = (l42 + 1)) {
		x->fVec17[l42] = 0.0;
	}
	for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
		x->fRec40[l43] = 0.0;
	}
	for (int l44 = 0; (l44 < 3); l44 = (l44 + 1)) {
		x->fRec4[l44] = 0.0;
	}
	for (int l45 = 0; (l45 < 3); l45 = (l45 + 1)) {
		x->fRec5[l45] = 0.0;
	}
	for (int l46 = 0; (l46 < 3); l46 = (l46 + 1)) {
		x->fRec6[l46] = 0.0;
	}
	for (int l47 = 0; (l47 < 3); l47 = (l47 + 1)) {
		x->fRec7[l47] = 0.0;
	}
	for (int l48 = 0; (l48 < 3); l48 = (l48 + 1)) {
		x->fRec8[l48] = 0.0;
	}
	for (int l49 = 0; (l49 < 3); l49 = (l49 + 1)) {
		x->fRec9[l49] = 0.0;
	}
	for (int l50 = 0; (l50 < 3); l50 = (l50 + 1)) {
		x->fRec10[l50] = 0.0;
	}
	for (int l51 = 0; (l51 < 3); l51 = (l51 + 1)) {
		x->fRec11[l51] = 0.0;
	}
	for (int l52 = 0; (l52 < 3); l52 = (l52 + 1)) {
		x->fRec3[l52] = 0.0;
	}
	for (int l53 = 0; (l53 < 3); l53 = (l53 + 1)) {
		x->fRec2[l53] = 0.0;
	}
	for (int l54 = 0; (l54 < 3); l54 = (l54 + 1)) {
		x->fRec45[l54] = 0.0;
	}
	for (int l55 = 0; (l55 < 3); l55 = (l55 + 1)) {
		x->fRec44[l55] = 0.0;
	}

	x->revlevel = -20.0; //-20, -70.0 - 40.0
	x->revdrywet = 0.0; //0.0 -1.0 - 1.0
	x->reveq2freq = 1500.0; //1500.0 160.0 - 10000.0
	x->reveq2level = 0.0; //0.0 -15.0 -15.0
	x->reveq1freq = 315.0; //315.0 40.0 - 2500.0
	x->reveq1level = 0.0; //0.0  -15.0 - 15.0
	x->midRT60 = 2.0; //2.0 1.0 - 8.0
	x->hfdamp = 6000.0; //6000.0 1500.0 - 23520.0
	x->lowRT60 = 3.0; //3.0 1.0 - 8.0
	x->lfX = 200.0; //200.0 50.0 - 1000.0
	x->Indel = 60.0; //60.0 20.0 - 100.0 ms

	return x;
}

void zitarev_dsp64(t_zitarev *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->fSamplingFreq != samplerate) x->fSamplingFreq = samplerate;
	object_method(dsp64, gensym("dsp_add64"), x,
		zitarev_perform64, 0, NULL);
}

void zitarev_perform64(t_zitarev *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	int n = vectorsize;

	double fSlow0 = (0.00100000005 * pow(10.0, (0.0500000007 * (double)x->revlevel)));
	double fSlow1 = (0.00100000005 * (double)x->revdrywet);
	double fSlow2 = (double)x->reveq2freq;
	double fSlow3 = pow(10.0, (0.0500000007 * (double)x->reveq2level));
	double fSlow4 = (x->fConst1 * (fSlow2 / sqrt(max(0.0, fSlow3))));
	double fSlow5 = ((1.0 - fSlow4) / (fSlow4 + 1.0));
	double fSlow6 = (double)x->reveq1freq;
	double fSlow7 = pow(10.0, (0.0500000007 * (double)x->reveq1level));
	double fSlow8 = (x->fConst1 * (fSlow6 / sqrt(max(0.0, fSlow7))));
	double fSlow9 = ((1.0 - fSlow8) / (fSlow8 + 1.0));
	double fSlow10 = (double)x->midRT60;
	double fSlow11 = exp((x->fConst3 / fSlow10));
	double fSlow12 = mydsp_faustpower2_f(fSlow11);
	double fSlow13 = cos((x->fConst4 * (double)x->hfdamp));
	double fSlow14 = (1.0 - (fSlow12 * fSlow13));
	double fSlow15 = (1.0 - fSlow12);
	double fSlow16 = (fSlow14 / fSlow15);
	double fSlow17 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow14) / mydsp_faustpower2_f(fSlow15)) + -1.0)));
	double fSlow18 = (fSlow16 - fSlow17);
	double fSlow19 = ((fSlow17 + (1.0 - fSlow16)) * fSlow11);
	double fSlow20 = (double)x->lowRT60;
	double fSlow21 = ((exp((x->fConst3 / fSlow20)) / fSlow11) + -1.0);
	double fSlow22 = (1.0 / tan((x->fConst5 * (double)x->lfX)));
	double fSlow23 = (fSlow22 + 1.0);
	double fSlow24 = (0.0 - ((1.0 - fSlow22) / fSlow23));
	double fSlow25 = (1.0 / fSlow23);
	int iSlow26 = (int)(min(8192.0, max(0.0, (x->fConst8 * (double)x->Indel))));
	double fSlow27 = expf((x->fConst11 / fSlow10));
	double fSlow28 = mydsp_faustpower2_f(fSlow27);
	double fSlow29 = (1.0 - (fSlow13 * fSlow28));
	double fSlow30 = (1.0 - fSlow28);
	double fSlow31 = (fSlow29 / fSlow30);
	double fSlow32 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow29) / mydsp_faustpower2_f(fSlow30)) + -1.0)));
	double fSlow33 = (fSlow31 - fSlow32);
	double fSlow34 = ((fSlow32 + (1.0 - fSlow31)) * fSlow27);
	double fSlow35 = ((exp((x->fConst11 / fSlow20)) / fSlow27) + -1.0);
	double fSlow36 = exp((x->fConst16 / fSlow10));
	double fSlow37 = mydsp_faustpower2_f(fSlow36);
	double fSlow38 = (1.0 - (fSlow13 * fSlow37));
	double fSlow39 = (1.0 - fSlow37);
	double fSlow40 = (fSlow38 / fSlow39);
	double fSlow41 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow38) / mydsp_faustpower2_f(fSlow39)) + -1.0)));
	double fSlow42 = (fSlow40 - fSlow41);
	double fSlow43 = ((fSlow41 + (1.0 - fSlow40)) * fSlow36);
	double fSlow44 = ((exp((x->fConst16 / fSlow20)) / fSlow36) + -1.0);
	double fSlow45 = exp((x->fConst21 / fSlow10));
	double fSlow46 = mydsp_faustpower2_f(fSlow45);
	double fSlow47 = (1.0 - (fSlow13 * fSlow46));
	double fSlow48 = (1.0 - fSlow46);
	double fSlow49 = (fSlow47 / fSlow48);
	double fSlow50 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow47) / mydsp_faustpower2_f(fSlow48)) + -1.0)));
	double fSlow51 = (fSlow49 - fSlow50);
	double fSlow52 = ((fSlow50 + (1.0 - fSlow49)) * fSlow45);
	double fSlow53 = ((exp((x->fConst21 / fSlow20)) / fSlow45) + -1.0);
	double fSlow54 = exp((x->fConst26 / fSlow10));
	double fSlow55 = mydsp_faustpower2_f(fSlow54);
	double fSlow56 = (1.0 - (fSlow13 * fSlow55));
	double fSlow57 = (1.0 - fSlow55);
	double fSlow58 = (fSlow56 / fSlow57);
	double fSlow59 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow56) / mydsp_faustpower2_f(fSlow57)) + -1.0)));
	double fSlow60 = (fSlow58 - fSlow59);
	double fSlow61 = ((fSlow59 + (1.0 - fSlow58)) * fSlow54);
	double fSlow62 = ((exp((x->fConst26 / fSlow20)) / fSlow54) + -1.0);
	double fSlow63 = exp((x->fConst31 / fSlow10));
	double fSlow64 = mydsp_faustpower2_f(fSlow63);
	double fSlow65 = (1.0 - (fSlow13 * fSlow64));
	double fSlow66 = (1.0 - fSlow64);
	double fSlow67 = (fSlow65 / fSlow66);
	double fSlow68 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow65) / mydsp_faustpower2_f(fSlow66)) + -1.0)));
	double fSlow69 = (fSlow67 - fSlow68);
	double fSlow70 = ((fSlow68 + (1.0 - fSlow67)) * fSlow63);
	double fSlow71 = ((exp((x->fConst31 / fSlow20)) / fSlow63) + -1.0);
	double fSlow72 = exp((x->fConst36 / fSlow10));
	double fSlow73 = mydsp_faustpower2_f(fSlow72);
	double fSlow74 = (1.0 - (fSlow13 * fSlow73));
	double fSlow75 = (1.0 - fSlow73);
	double fSlow76 = (fSlow74 / fSlow75);
	double fSlow77 = sqrt(max(0.0f, ((mydsp_faustpower2_f(fSlow74) / mydsp_faustpower2_f(fSlow75)) + -1.0)));
	double fSlow78 = (fSlow76 - fSlow77);
	double fSlow79 = ((fSlow77 + (1.0 - fSlow76)) * fSlow72);
	double fSlow80 = ((exp((x->fConst36 / fSlow20)) / fSlow72) + -1.0);
	double fSlow81 = exp((x->fConst41 / fSlow10));
	double fSlow82 = mydsp_faustpower2_f(fSlow81);
	double fSlow83 = (1.0 - (fSlow13 * fSlow82));
	double fSlow84 = (1.0 - fSlow82);
	double fSlow85 = (fSlow83 / fSlow84);
	double fSlow86 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow83) / mydsp_faustpower2_f(fSlow84)) + -1.0)));
	double fSlow87 = (fSlow85 - fSlow86);
	double fSlow88 = ((fSlow86 + (1.0 - fSlow85)) * fSlow81);
	double fSlow89 = ((exp((x->fConst41 / fSlow20)) / fSlow81) + -1.0f);
	double fSlow90 = ((0.0 - cos((x->fConst1 * fSlow6))) * (fSlow9 + 1.0));
	double fSlow91 = ((0.0 - cos((x->fConst1 * fSlow2))) * (fSlow5 + 1.0));
	while(n--) {
		x->fRec0[0] = (fSlow0 + (0.999000013 * x->fRec0[1]));
		x->fRec1[0] = (fSlow1 + (0.999000013 * x->fRec1[1]));
		double fTemp0 = (x->fRec1[0] + 1.0);
		double fTemp1 = (double)*input0++;
		x->fVec0[(x->IOTA & 16383)] = fTemp1;
		double fTemp2 = (1.0 - (0.5 * fTemp0));
		x->fRec15[0] = ((fSlow24 * x->fRec15[1]) + (fSlow25 * (x->fRec11[1] + x->fRec11[2])));
		x->fRec14[0] = ((fSlow18 * x->fRec14[1]) + (fSlow19 * (x->fRec11[1] + (fSlow21 * x->fRec15[0]))));
		x->fVec1[(x->IOTA & 32767)] = ((0.353553385 * x->fRec14[0]) + 9.99999968e-21);
		double fTemp3 = (double)*input1++;
		x->fVec2[(x->IOTA & 16383)] = fTemp3;
		double fTemp4 = (0.300000012 * x->fVec2[((x->IOTA - iSlow26) & 16383)]);
		double fTemp5 = (((0.600000024 * x->fRec12[1]) + x->fVec1[((x->IOTA - x->iConst7) & 32767)]) - fTemp4);
		x->fVec3[(x->IOTA & 2047)] = fTemp5;
		x->fRec12[0] = x->fVec3[((x->IOTA - x->iConst9) & 2047)];
		double fRec13 = (0.0 - (0.600000024 * fTemp5));
		x->fRec19[0] = ((fSlow24 * x->fRec19[1]) + (fSlow25 * (x->fRec7[1] + x->fRec7[2])));
		x->fRec18[0] = ((fSlow33 * x->fRec18[1]) + (fSlow34 * (x->fRec7[1] + (fSlow35 * x->fRec19[0]))));
		x->fVec4[(x->IOTA & 32767)] = ((0.353553385 * x->fRec18[0]) + 9.99999968e-21);
		double fTemp6 = (((0.600000024 * x->fRec16[1]) + x->fVec4[((x->IOTA - x->iConst13) & 32767)]) - fTemp4);
		x->fVec5[(x->IOTA & 4095)] = fTemp6;
		x->fRec16[0] = x->fVec5[((x->IOTA - x->iConst14) & 4095)];
		double fRec17 = (0.0 - (0.600000024 * fTemp6));
		x->fRec23[0] = ((fSlow24 * x->fRec23[1]) + (fSlow25 * (x->fRec9[1] + x->fRec9[2])));
		x->fRec22[0] = ((fSlow42 * x->fRec22[1]) + (fSlow43 * (x->fRec9[1] + (fSlow44 * x->fRec23[0]))));
		x->fVec6[(x->IOTA & 16383)] = ((0.353553385 * x->fRec22[0]) + 9.99999968e-21);
		double fTemp7 = (x->fVec6[((x->IOTA - x->iConst18) & 16383)] + (fTemp4 + (0.600000024 * x->fRec20[1])));
		x->fVec7[(x->IOTA & 4095)] = fTemp7;
		x->fRec20[0] = x->fVec7[((x->IOTA - x->iConst19) & 4095)];
		double fRec21 = (0.0 - (0.600000024 * fTemp7));
		x->fRec27[0] = ((fSlow24 * x->fRec27[1]) + (fSlow25 * (x->fRec5[1] + x->fRec5[2])));
		x->fRec26[0] = ((fSlow51 * x->fRec26[1]) + (fSlow52 * (x->fRec5[1] + (fSlow53 * x->fRec27[0]))));
		x->fVec8[(x->IOTA & 32767)] = ((0.353553385 * x->fRec26[0]) + 9.99999968e-21);
		double fTemp8 = (fTemp4 + ((0.600000024 * x->fRec24[1]) + x->fVec8[((x->IOTA - x->iConst23) & 32767)]));
		x->fVec9[(x->IOTA & 4095)] = fTemp8;
		x->fRec24[0] = x->fVec9[((x->IOTA - x->iConst24) & 4095)];
		double fRec25 = (0.0 - (0.600000024 * fTemp8));
		x->fRec31[0] = ((fSlow24 * x->fRec31[1]) + (fSlow25 * (x->fRec10[1] + x->fRec10[2])));
		x->fRec30[0] = ((fSlow60 * x->fRec30[1]) + (fSlow61 * (x->fRec10[1] + (fSlow62 * x->fRec31[0]))));
		x->fVec10[(x->IOTA & 16383)] = ((0.353553385 * x->fRec30[0]) + 9.99999968e-21);
		double fTemp9 = (0.300000012 * x->fVec0[((x->IOTA - iSlow26) & 16383)]);
		double fTemp10 = (x->fVec10[((x->IOTA - x->iConst28) & 16383)] - (fTemp9 + (0.600000024 * x->fRec28[1])));
		x->fVec11[(x->IOTA & 2047)] = fTemp10;
		x->fRec28[0] = x->fVec11[((x->IOTA - x->iConst29) & 2047)];
		double fRec29 = (0.600000024 * fTemp10);
		x->fRec35[0] = ((fSlow24 * x->fRec35[1]) + (fSlow25 * (x->fRec6[1] + x->fRec6[2])));
		x->fRec34[0] = ((fSlow69 * x->fRec34[1]) + (fSlow70 * (x->fRec6[1] + (fSlow71 * x->fRec35[0]))));
		x->fVec12[(x->IOTA & 16383)] = ((0.353553385 * x->fRec34[0]) + 9.99999968e-21);
		double fTemp11 = (x->fVec12[((x->IOTA - x->iConst33) & 16383)] - (fTemp9 + (0.600000024 * x->fRec32[1])));
		x->fVec13[(x->IOTA & 4095)] = fTemp11;
		x->fRec32[0] = x->fVec13[((x->IOTA - x->iConst34) & 4095)];
		double fRec33 = (0.600000024 * fTemp11);
		x->fRec39[0] = ((fSlow24 * x->fRec39[1]) + (fSlow25 * (x->fRec8[1] + x->fRec8[2])));
		x->fRec38[0] = ((fSlow78 * x->fRec38[1]) + (fSlow79 * (x->fRec8[1] + (fSlow80 * x->fRec39[0]))));
		x->fVec14[(x->IOTA & 16383)] = ((0.353553385 * x->fRec38[0]) + 9.99999968e-21);
		double fTemp12 = ((fTemp9 + x->fVec14[((x->IOTA - x->iConst38) & 16383)]) - (0.600000024 * x->fRec36[1]));
		x->fVec15[(x->IOTA & 4095)] = fTemp12;
		x->fRec36[0] = x->fVec15[((x->IOTA - x->iConst39) & 4095)];
		double fRec37 = (0.600000024 * fTemp12);
		x->fRec43[0] = ((fSlow24 * x->fRec43[1]) + (fSlow25 * (x->fRec4[1] + x->fRec4[2])));
		x->fRec42[0] = ((fSlow87 * x->fRec42[1]) + (fSlow88 * (x->fRec4[1] + (fSlow89 * x->fRec43[0]))));
		x->fVec16[(x->IOTA & 16383)] = ((0.353553385 * x->fRec42[0]) + 9.99999968e-21);
		double fTemp13 = ((x->fVec16[((x->IOTA - x->iConst43) & 16383)] + fTemp9) - (0.600000024 * x->fRec40[1]));
		x->fVec17[(x->IOTA & 2047)] = fTemp13;
		x->fRec40[0] = x->fVec17[((x->IOTA - x->iConst44) & 2047)];
		double fRec41 = (0.600000024 * fTemp13);
		double fTemp14 = (fRec41 + fRec37);
		double fTemp15 = (fRec29 + (fRec33 + fTemp14));
		x->fRec4[0] = (x->fRec12[1] + (x->fRec16[1] + (x->fRec20[1] + (x->fRec24[1] + (x->fRec28[1] + (x->fRec32[1] + (x->fRec36[1] + (x->fRec40[1] + (fRec13 + (fRec17 + (fRec21 + (fRec25 + fTemp15))))))))))));
		x->fRec5[0] = ((x->fRec28[1] + (x->fRec32[1] + (x->fRec36[1] + (x->fRec40[1] + fTemp15)))) - (x->fRec12[1] + (x->fRec16[1] + (x->fRec20[1] + (x->fRec24[1] + (fRec13 + (fRec17 + (fRec25 + fRec21))))))));
		double fTemp16 = (fRec33 + fRec29);
		x->fRec6[0] = ((x->fRec20[1] + (x->fRec24[1] + (x->fRec36[1] + (x->fRec40[1] + (fRec21 + (fRec25 + fTemp14)))))) - (x->fRec12[1] + (x->fRec16[1] + (x->fRec28[1] + (x->fRec32[1] + (fRec13 + (fRec17 + fTemp16)))))));
		x->fRec7[0] = ((x->fRec12[1] + (x->fRec16[1] + (x->fRec36[1] + (x->fRec40[1] + (fRec13 + (fRec17 + fTemp14)))))) - (x->fRec20[1] + (x->fRec24[1] + (x->fRec28[1] + (x->fRec32[1] + (fRec21 + (fRec25 + fTemp16)))))));
		double fTemp17 = (fRec41 + fRec33);
		double fTemp18 = (fRec37 + fRec29);
		x->fRec8[0] = ((x->fRec16[1] + (x->fRec24[1] + (x->fRec32[1] + (x->fRec40[1] + (fRec17 + (fRec25 + fTemp17)))))) - (x->fRec12[1] + (x->fRec20[1] + (x->fRec28[1] + (x->fRec36[1] + (fRec13 + (fRec21 + fTemp18)))))));
		x->fRec9[0] = ((x->fRec12[1] + (x->fRec20[1] + (x->fRec32[1] + (x->fRec40[1] + (fRec13 + (fRec21 + fTemp17)))))) - (x->fRec16[1] + (x->fRec24[1] + (x->fRec28[1] + (x->fRec36[1] + (fRec17 + (fRec25 + fTemp18)))))));
		double fTemp19 = (fRec41 + fRec29);
		double fTemp20 = (fRec37 + fRec33);
		x->fRec10[0] = ((x->fRec12[1] + (x->fRec24[1] + (x->fRec28[1] + (x->fRec40[1] + (fRec13 + (fRec25 + fTemp19)))))) - (x->fRec16[1] + (x->fRec20[1] + (x->fRec32[1] + (x->fRec36[1] + (fRec17 + (fRec21 + fTemp20)))))));
		x->fRec11[0] = ((x->fRec16[1] + (x->fRec20[1] + (x->fRec28[1] + (x->fRec40[1] + (fRec17 + (fRec21 + fTemp19)))))) - (x->fRec12[1] + (x->fRec24[1] + (x->fRec32[1] + (x->fRec36[1] + (fRec13 + (fRec25 + fTemp20)))))));
		double fTemp21 = (0.370000005 * (x->fRec5[0] + x->fRec6[0]));
		double fTemp22 = (fSlow90 * x->fRec3[1]);
		x->fRec3[0] = (fTemp21 - (fTemp22 + (fSlow9 * x->fRec3[2])));
		double fTemp23 = (fSlow9 * x->fRec3[0]);
		double fTemp24 = (0.5 * ((fTemp23 + (x->fRec3[2] + (fTemp21 + fTemp22))) + (fSlow7 * ((fTemp23 + (fTemp22 + x->fRec3[2])) - fTemp21))));
		double fTemp25 = (fSlow91 * x->fRec2[1]);
		x->fRec2[0] = (fTemp24 - (fTemp25 + (fSlow5 * x->fRec2[2])));
		double fTemp26 = (fSlow5 * x->fRec2[0]);
		
		*output0++ = (double)((0.5 * (x->fRec0[0] * ((fTemp0 * fTemp1) + (fTemp2 * ((fTemp26 + (x->fRec2[2] + (fTemp24 + fTemp25))) + (fSlow3 * ((fTemp26 + (fTemp25 + x->fRec2[2])) - fTemp24))))))));
		double fTemp27 = (0.370000005 * (x->fRec5[0] - x->fRec6[0]));
		double fTemp28 = (fSlow90 * x->fRec45[1]);
		x->fRec45[0] = (fTemp27 - (fTemp28 + (fSlow9 * x->fRec45[2])));
		double fTemp29 = (fSlow9 * x->fRec45[0]);
		double fTemp30 = (0.5 * ((fTemp29 + (x->fRec45[2] + (fTemp27 + fTemp28))) + (fSlow7 * ((fTemp29 + (fTemp28 + x->fRec45[2])) - fTemp27))));
		double fTemp31 = (fSlow91 * x->fRec44[1]);
		x->fRec44[0] = (fTemp30 - (fTemp31 + (fSlow5 * x->fRec44[2])));
		double fTemp32 = (fSlow5 * x->fRec44[0]);
		*output1++ = (double)((0.5 * (x->fRec0[0] * ((fTemp0 * fTemp3) + (fTemp2 * ((fTemp32 + (x->fRec44[2] + (fTemp30 + fTemp31))) + (fSlow3 * ((fTemp32 + (fTemp31 + x->fRec44[2])) - fTemp30))))))));
		
		x->fRec0[1] = x->fRec0[0];
		x->fRec1[1] = x->fRec1[0];
		x->IOTA = (x->IOTA + 1);
		x->fRec15[1] = x->fRec15[0];
		x->fRec14[1] = x->fRec14[0];
		x->fRec12[1] = x->fRec12[0];
		x->fRec19[1] = x->fRec19[0];
		x->fRec18[1] = x->fRec18[0];
		x->fRec16[1] = x->fRec16[0];
		x->fRec23[1] = x->fRec23[0];
		x->fRec22[1] = x->fRec22[0];
		x->fRec20[1] = x->fRec20[0];
		x->fRec27[1] = x->fRec27[0];
		x->fRec26[1] = x->fRec26[0];
		x->fRec24[1] = x->fRec24[0];
		x->fRec31[1] = x->fRec31[0];
		x->fRec30[1] = x->fRec30[0];
		x->fRec28[1] = x->fRec28[0];
		x->fRec35[1] = x->fRec35[0];
		x->fRec34[1] = x->fRec34[0];
		x->fRec32[1] = x->fRec32[0];
		x->fRec39[1] = x->fRec39[0];
		x->fRec38[1] = x->fRec38[0];
		x->fRec36[1] = x->fRec36[0];
		x->fRec43[1] = x->fRec43[0];
		x->fRec42[1] = x->fRec42[0];
		x->fRec40[1] = x->fRec40[0];
		x->fRec4[2] = x->fRec4[1];
		x->fRec4[1] = x->fRec4[0];
		x->fRec5[2] = x->fRec5[1];
		x->fRec5[1] = x->fRec5[0];
		x->fRec6[2] = x->fRec6[1];
		x->fRec6[1] = x->fRec6[0];
		x->fRec7[2] = x->fRec7[1];
		x->fRec7[1] = x->fRec7[0];
		x->fRec8[2] = x->fRec8[1];
		x->fRec8[1] = x->fRec8[0];
		x->fRec9[2] = x->fRec9[1];
		x->fRec9[1] = x->fRec9[0];
		x->fRec10[2] = x->fRec10[1];
		x->fRec10[1] = x->fRec10[0];
		x->fRec11[2] = x->fRec11[1];
		x->fRec11[1] = x->fRec11[0];
		x->fRec3[2] = x->fRec3[1];
		x->fRec3[1] = x->fRec3[0];
		x->fRec2[2] = x->fRec2[1];
		x->fRec2[1] = x->fRec2[0];
		x->fRec45[2] = x->fRec45[1];
		x->fRec45[1] = x->fRec45[0];
		x->fRec44[2] = x->fRec44[1];
		x->fRec44[1] = x->fRec44[0];

	}
}

double mydsp_faustpower2_f(double value) {
	return (value * value);
}

void zitarev_free(t_zitarev* x) {
	dsp_free((t_pxobject*)x);

	sysmem_freeptr(x->fRec0);
	sysmem_freeptr(x->fRec1);
	sysmem_freeptr(x->fVec0);
	sysmem_freeptr(x->fRec15);
	sysmem_freeptr(x->fRec14);
	sysmem_freeptr(x->fVec1);
	sysmem_freeptr(x->fVec2);
	sysmem_freeptr(x->fVec3);
	sysmem_freeptr(x->fRec12);
	sysmem_freeptr(x->fRec19);
	sysmem_freeptr(x->fRec18);
	sysmem_freeptr(x->fVec4);
	sysmem_freeptr(x->fVec5);
	sysmem_freeptr(x->fRec16);
	sysmem_freeptr(x->fRec23);
	sysmem_freeptr(x->fRec22);
	sysmem_freeptr(x->fVec6);
	sysmem_freeptr(x->fVec7);
	sysmem_freeptr(x->fRec20);
	sysmem_freeptr(x->fRec27);
	sysmem_freeptr(x->fRec26);
	sysmem_freeptr(x->fVec8);
	sysmem_freeptr(x->fVec9);
	sysmem_freeptr(x->fRec24);
	sysmem_freeptr(x->fRec31);
	sysmem_freeptr(x->fRec30);
	sysmem_freeptr(x->fVec10);
	sysmem_freeptr(x->fVec11);
	sysmem_freeptr(x->fRec28);
	sysmem_freeptr(x->fRec35);
	sysmem_freeptr(x->fRec34);
	sysmem_freeptr(x->fVec12);
	sysmem_freeptr(x->fVec13);
	sysmem_freeptr(x->fRec32);
	sysmem_freeptr(x->fRec39);
	sysmem_freeptr(x->fRec38);
	sysmem_freeptr(x->fVec14);
	sysmem_freeptr(x->fVec15);
	sysmem_freeptr(x->fRec36);
	sysmem_freeptr(x->fRec43);
	sysmem_freeptr(x->fRec42);
	sysmem_freeptr(x->fVec16);
	sysmem_freeptr(x->fVec17);
	sysmem_freeptr(x->fRec40);
	sysmem_freeptr(x->fRec4);
	sysmem_freeptr(x->fRec5);
	sysmem_freeptr(x->fRec6);
	sysmem_freeptr(x->fRec7);
	sysmem_freeptr(x->fRec8);
	sysmem_freeptr(x->fRec9);
	sysmem_freeptr(x->fRec10);
	sysmem_freeptr(x->fRec11);
	sysmem_freeptr(x->fRec3);
	sysmem_freeptr(x->fRec2);
	sysmem_freeptr(x->fRec45);
	sysmem_freeptr(x->fRec44);
}

void setLevel(t_zitarev *x, double lev) {
	if (lev < -70.0) x->revlevel = -70.0;
	else if (lev > 40.0) x->revlevel = 40.0;
	else x->revlevel = lev;

}

void setDryWet(t_zitarev *x, double drwt) {
	if (drwt < -1.0) x->revdrywet = -1.0;
	else if (drwt > 1.0) x->revdrywet = 1.0;
	else x->revdrywet = drwt;
}

void setEq2fr(t_zitarev *x, double eq2fr) {
	if (eq2fr < 160.0) x->reveq2freq = 160.0;
	else if (eq2fr > 10000.0) x->reveq2freq = 10000.0;
	else x->reveq2freq = eq2fr;
}

void setEq2Lev(t_zitarev *x, double eq2lv) {
	if (eq2lv < -15.0) x->reveq2level = -15.0;
	else if (eq2lv > 15.0) x->reveq2level = 15.0;
	else x->reveq2level = eq2lv;
}

void setEq1fr(t_zitarev *x, double eq1fr) {
	if (eq1fr < 40.0) x->reveq1freq = 40.0;
	else if (eq1fr > 2500.0) x->reveq1freq = 2500.0;
	else x->reveq1freq = eq1fr;
}

void setEq1Lev(t_zitarev *x, double eq1lv) {
	if (eq1lv < -15.0) x->reveq1level = -15.0;
	else if (eq1lv > 15.0) x->reveq1level = 15.0;
	else x->reveq1level = eq1lv;
}

void setMidRT(t_zitarev *x, double midrt) {
	if (midrt < 1.0) x->midRT60 = 1.0;
	else if (midrt > 8.0) x->midRT60 = 8.0;
	else x->midRT60 = midrt;
}

void setHfDamp(t_zitarev *x, double hfdamp) {
	if (hfdamp < 1500.0) x->hfdamp = 1500.0;
	else if (hfdamp > 23520.0) x->hfdamp = 23520.0;
	else x->hfdamp = hfdamp;
}

void setLowRT(t_zitarev *x, double lowrt) {
	if (lowrt < 1.0) x->lowRT60 = 1.0;
	else if (lowrt > 8.0) x->lowRT60 = 8.0;
	else x->lowRT60 = lowrt;
}

void setLFX(t_zitarev *x, double lfx) {
	if (lfx < 50.0) x->lfX = 50.0;
	else if (lfx > 1000.0) x->lfX = 1000.0;
	else x->lfX = lfx;
}

void setInDel(t_zitarev *x, double indel) {
	if (indel < 20.0) x->Indel = 20.0;
	else if (indel > 100.0) x->Indel = 100.0;
	else x->Indel = indel;
}

void setDef(t_zitarev *x) {
	x->revlevel = -20.0; //-20, -70.0 - 40.0
	x->revdrywet = 0.0; //0.0 -1.0 - 1.0
	x->reveq2freq = 1500.0; //1500.0 160.0 - 10000.0
	x->reveq2level = 0.0; //0.0 -15.0 -15.0
	x->reveq1freq = 315.0; //315.0 40.0 - 2500.0
	x->reveq1level = 0.0; //0.0  -15.0 - 15.0
	x->midRT60 = 2.0; //2.0 1.0 - 8.0
	x->hfdamp = 6000.0; //6000.0 1500.0 - 23520.0
	x->lowRT60 = 3.0; //3.0 1.0 - 8.0
	x->lfX = 200.0; //200.0 50.0 - 1000.0
	x->Indel = 60.0; //60.0 20.0 - 100.0 ms
}