#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

//#include <math.h>
//#include <cmath>
#include  "bass.h"

static t_class *bassf_class;

typedef struct _bassf {
	t_pxobject x_obj;

	double freq;
	double gain;
	double gate;
	double roomsize;
	double pan_angle;
	double rvrb_gain;
	double nonlinear;
	double mod_freq;
	int mod_type;
	double touchLength;
	double spat_width;

	double fSamplingFreq;

	int *iVec0;
	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double *fRec11;
	double *fRec10;
	long IOTA;
	double *fVec1;
	double fConst10;
	int iConst11;
	double *fRec12;
	double *fRec17;
	double fConst12;
	double *fRec16;
	double *fVec2;
	double *fRec18;
	double fConst13;

	double *fRec22;
	double *fRec21;
	double *fRec20;
	double *fRec19;
	double *fRec28;
	double *fRec27;
	double *fRec26;
	double *fRec25;
	double *fRec24;
	double *fRec23;
	double *fRec34;
	double *fRec33;
	double *fRec32;
	double *fRec31;
	double *fRec30;
	double *fRec29;
	
	double *fVec3;
	double *fRec15;
	int *iRec36;
	double fConst14;
	double *fRec35;
	int *iRec39;
	double fConst15;
	double *fRec40;
	double *fVec4;
	double fConst16;
	double *fRec41;
	double *fRec38;
	double *fRec37;
	double *fRec14;
	double fConst17;
	double *fRec13;
	double *fVec5;
	double fConst18;

	double *fVec6;
	int iConst19;
	double *fVec7;
	int iConst20;
	double *fRec8;
	double fConst21;
	double fConst22;
	double fConst23;
	double fConst24;
	double *fRec45;
	double *fRec44;
	double *fVec8;
	double fConst25;
	int iConst26;
	double *fVec9;
	int iConst27;
	double *fRec42;
	double fConst28;
	double fConst29;
	double fConst30;
	double fConst31;
	double *fRec49;
	double *fRec48;
	double *fVec10;
	double fConst32;
	int iConst33;
	double *fVec11;
	int iConst34;
	double *fRec46;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;

	double *fRec53;
	double *fRec52;
	double *fVec12;
	double fConst39;
	int iConst40;
	double *fVec13;
	int iConst41;
	double *fRec50;
	double fConst42;
	double fConst43;
	double fConst44;
	double fConst45;
	double *fRec57;
	double *fRec56;
	double *fVec14;
	double fConst46;
	int iConst47;
	double *fVec15;
	double *fVec16;
	int iConst48;
	double *fRec54;
	double fConst49;
	double fConst50;
	double fConst51;
	double fConst52;
	double *fRec61;
	double *fRec60;
	double *fVec17;
	double fConst53;
	int iConst54;
	double *fVec18;
	int iConst55;
	double *fRec58;
	double fConst56;
	double fConst57;
	double fConst58;
	double fConst59;
	double *fRec65;
	double *fRec64;
	double *fVec19;
	double fConst60;
	int iConst61;
	double *fVec20;
	int iConst62;
	double *fRec62;
	double fConst63;
	double fConst64;
	double fConst65;
	double fConst66;
	double *fRec69;
	double *fRec68;
	double *fVec21;
	double fConst67;
	int iConst68;
	double *fVec22;
	int iConst69;
	double *fRec66;
	double *fRec0;
	double *fRec1;
	double *fRec2;
	double *fRec3;
	double *fRec4;
	double *fRec5;
	double *fRec6;
	double *fRec7;

} t_bassf;

void bassf_dsp64(t_bassf *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);

void bassf_perform64(t_bassf *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

double mydsp_faustpower2_f(double value);

void setFreq(t_bassf *x, double frequency);
void playStart(t_bassf *x, double gt);
void setGain(t_bassf *x, double gn);
void setModFreq(t_bassf *x, double modfreq);
void setModType(t_bassf *x, int type);
void setNLinearity(t_bassf *x, double nlin);
void setTouchLength(t_bassf *x, double tlength);
void setReverb(t_bassf *x, double rvrbgain);
void setRoom(t_bassf *x, double rmSize);
void setAngle(t_bassf *x, double panAngle);
void setWidth(t_bassf *x, double width);

void setDef(t_bassf *x);
void clearall(t_bassf *x);

void* bassf_new(t_symbol* s, short argc, t_atom* argv);
void bassf_free(t_bassf* x);


void ext_main(void *r) {

	t_class *c;
	c = class_new("bassf~", (method)bassf_new, (method)bassf_free, sizeof(t_bassf), 0L, A_GIMME, 0);

	class_addmethod(c, (method)bassf_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setFreq, "freq", A_FLOAT, 0);
	class_addmethod(c, (method)playStart, "gate", A_FLOAT, 0);
	class_addmethod(c, (method)setGain, "gain", A_FLOAT, 0);

	class_addmethod(c, (method)setModFreq, "ModFreq", A_FLOAT, 0);
	class_addmethod(c, (method)setModType, "ModType", A_LONG, 0);
	class_addmethod(c, (method)setNLinearity, "NonLin", A_FLOAT, 0);
	class_addmethod(c, (method)setTouchLength, "Touch", A_FLOAT, 0);
	class_addmethod(c, (method)setReverb, "reverbgn", A_FLOAT, 0);
	class_addmethod(c, (method)setRoom, "room", A_FLOAT, 0);
	class_addmethod(c, (method)setAngle, "angle", A_FLOAT, 0);
	class_addmethod(c, (method)setWidth, "width", A_FLOAT, 0);

	class_addmethod(c, (method)setDef, "setDefault", 0);
	class_addmethod(c, (method)clearall, "clearall", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	bassf_class = c;
	
	post("--------------------------------------------------------------------------");
	post("The bass physical model by Romain Michon");
	post("Max/MSP implementation with 64-bit perform routine: George Nikolopoulos - 2018 -");
	post("Default: 	gain = 1.0 modulation type = 1 modulation freq = 220 non linearity = 0.0 touch length = 0.15");
	post("reverb gain = 0.137 room size = 0.72 pan angle = 0.6 spatial width = 0.5 ");

	//return 0;
}

void* bassf_new(t_symbol* s, short argc, t_atom* argv)
{
	t_bassf* x = (t_bassf*)object_alloc(bassf_class);

	dsp_setup((t_pxobject *)x, 0);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	//((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different
	x->fSamplingFreq = sys_getsr();

	x->fConst0 = min(192000.0, max(1.0, (double)x->fSamplingFreq));
	x->fConst1 = cos((37699.1133 / x->fConst0));
	x->fConst2 = floor(((0.219990999 * x->fConst0) + 0.5));
	x->fConst3 = ((0.0 - (6.90775537 * x->fConst2)) / x->fConst0);
	x->fConst4 = (0.5 * x->fConst3);
	x->fConst5 = (0.333333343 * x->fConst3);
	x->fConst6 = (1.0 / tan((628.318542 / x->fConst0)));
	x->fConst7 = (x->fConst6 + 1.0);
	x->fConst8 = (0.0 - ((1.0 - x->fConst6) / x->fConst7));
	x->fConst9 = (1.0 / x->fConst7);
	x->fConst10 = floor(((0.0191229992 * x->fConst0) + 0.5));
	x->iConst11 = (int)(min(16384.0, max(0.0, (x->fConst2 - x->fConst10))));
	x->fConst12 = exp((0.0 - (700.0 / x->fConst0)));
	x->fConst13 = (6.28318548 / x->fConst0);
	x->fConst14 = exp((0.0 - (140.0 / x->fConst0)));
	x->fConst15 = (3.5 / x->fConst0);
	x->fConst16 = exp((0.0 - (350.0 / x->fConst0)));
	x->fConst17 = (0.0 - (1.99399996 * cos((678.583984 / x->fConst0))));
	x->fConst18 = (0.5 * x->fConst0);
	x->iConst19 = (int)(min(8192.0, max(0.0, (0.0199999996 * x->fConst0))));
	x->iConst20 = (int)(min(1024.0, max(0.0, (x->fConst10 + -1.0))));
	x->fConst21 = floor(((0.256891012 * x->fConst0) + 0.5));
	x->fConst22 = ((0.0 - (6.90775537 * x->fConst21)) / x->fConst0);
	x->fConst23 = (0.5 * x->fConst22);
	x->fConst24 = (0.333333343 * x->fConst22);
	x->fConst25 = floor(((0.0273330007 * x->fConst0) + 0.5));
	x->iConst26 = (int)(min(16384.0, max(0.0, (x->fConst21 - x->fConst25))));
	x->iConst27 = (int)(min(2048.0, max(0.0, (x->fConst25 + -1.0))));
	x->fConst28 = floor(((0.192303002 * x->fConst0) + 0.5));
	x->fConst29 = ((0.0 - (6.90775537 * x->fConst28)) / x->fConst0);
	x->fConst30 = (0.5 * x->fConst29);
	x->fConst31 = (0.333333343 * x->fConst29);
	x->fConst32 = floor(((0.0292910002 * x->fConst0) + 0.5));
	x->iConst33 = (int)(min(8192.0, max(0.0, (x->fConst28 - x->fConst32))));
	x->iConst34 = (int)(min(2048.0, max(0.0, (x->fConst32 + -1.0))));
	x->fConst35 = floor(((0.210389003 * x->fConst0) + 0.5));
	x->fConst36 = ((0.0 - (6.90775537 * x->fConst35)) / x->fConst0);
	x->fConst37 = (0.5 * x->fConst36);
	x->fConst38 = (0.333333343 * x->fConst36);
	x->fConst39 = floor(((0.0244210009 * x->fConst0) + 0.5));
	x->iConst40 = (int)(min(16384.0, max(0.0, (x->fConst35 - x->fConst39))));
	x->iConst41 = (int)(min(2048.0, max(0.0, (x->fConst39 + -1.0))));
	x->fConst42 = floor(((0.125 * x->fConst0) + 0.5));
	x->fConst43 = ((0.0 - (6.90775537 * x->fConst42)) / x->fConst0);
	x->fConst44 = (0.5 * x->fConst43);
	x->fConst45 = (0.333333343 * x->fConst43);
	x->fConst46 = floor(((0.0134579996 * x->fConst0) + 0.5));
	x->iConst47 = (int)(min(8192.0, max(0.0, (x->fConst42 - x->fConst46))));
	x->iConst48 = (int)(min(1024.0, max(0.0, (x->fConst46 + -1.0))));
	x->fConst49 = floor(((0.127837002 * x->fConst0) + 0.5));
	x->fConst50 = ((0.0 - (6.90775537 * x->fConst49)) / x->fConst0);
	x->fConst51 = (0.5 * x->fConst50);
	x->fConst52 = (0.333333343 * x->fConst50);
	x->fConst53 = floor(((0.0316039994 * x->fConst0) + 0.5));
	x->iConst54 = (int)(min(8192.0, max(0.0, (x->fConst49 - x->fConst53))));
	x->iConst55 = (int)(min(2048.0, max(0.0, (x->fConst53 + -1.0))));
	x->fConst56 = floor(((0.174713001 * x->fConst0) + 0.5));
	x->fConst57 = ((0.0 - (6.90775537 * x->fConst56)) / x->fConst0);
	x->fConst58 = (0.5 * x->fConst57);
	x->fConst59 = (0.333333343 * x->fConst57);
	x->fConst60 = floor(((0.0229039993 * x->fConst0) + 0.5));
	x->iConst61 = (int)(min(8192.0, max(0.0, (x->fConst56 - x->fConst60))));
	x->iConst62 = (int)(min(2048.0, max(0.0, (x->fConst60 + -1.0))));
	x->fConst63 = floor(((0.153128996 * x->fConst0) + 0.5));
	x->fConst64 = ((0.0 - (6.90775537 * x->fConst63)) / x->fConst0);
	x->fConst65 = (0.5 * x->fConst64);
	x->fConst66 = (0.333333343 * x->fConst64);
	x->fConst67 = floor(((0.0203460008 * x->fConst0) + 0.5));
	x->iConst68 = (int)(min(8192.0, max(0.0, (x->fConst63 - x->fConst67))));
	x->iConst69 = (int)(min(1024.0, max(0.0, (x->fConst67 + -1.0))));

	x->iVec0 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec11 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec10 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fRec12 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec17 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec16 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec2 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec21 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec19 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec28 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec27 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec26 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec25 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec24 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec23 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec34 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec33 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec31 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec3 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec15 = (double *)sysmem_newptr(2 * sizeof(double));
	x->iRec36 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec35 = (double *)sysmem_newptr(2 * sizeof(double));
	x->iRec39 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec40 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec4 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec41 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec38 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec37 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec14 = (double *)sysmem_newptr(8192 * sizeof(double));
	x->fRec13 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fVec5 = (double *)sysmem_newptr(8192 * sizeof(double));
	x->fVec6 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fVec7 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec8 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec45 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec44 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec8 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fVec9 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec42 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec49 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec48 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec10 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec11 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec46 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec53 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec52 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec12 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fVec13 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec50 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec57 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec56 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec14 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec15 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fVec16 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec54 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec61 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec60 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec17 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec18 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec58 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec65 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec64 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec19 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec20 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec62 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec69 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec68 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec21 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec22 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec66 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec1 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec2 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec3 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec4 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec5 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec6 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec7 = (double *)sysmem_newptr(3 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->iVec0[l0] = 0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec11[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		x->fRec10[l2] = 0.0;

	}
	x->IOTA = 0;
	for (int l3 = 0; (l3 < 32768); l3 = (l3 + 1)) {
		x->fVec1[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec12[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		x->fRec17[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		x->fRec16[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fVec2[l7] = 0.0;

	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec18[l8] = 0.0;

	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		x->fRec22[l9] = 0.0;

	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		x->fRec21[l10] = 0.0;

	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec20[l11] = 0.0;

	}
	for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
		x->fRec19[l12] = 0.0;

	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		x->fRec28[l13] = 0.0;

	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->fRec27[l14] = 0.0;

	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		x->fRec26[l15] = 0.0;

	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec25[l16] = 0.0;

	}
	for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
		x->fRec24[l17] = 0.0;

	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		x->fRec23[l18] = 0.0;

	}
	for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
		x->fRec34[l19] = 0.0;

	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		x->fRec33[l20] = 0.0;

	}
	for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
		x->fRec32[l21] = 0.0;

	}
	for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
		x->fRec31[l22] = 0.0;

	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		x->fRec30[l23] = 0.0;

	}
	for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
		x->fRec29[l24] = 0.0;

	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		x->fVec3[l25] = 0.0;

	}
	for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
		x->fRec15[l26] = 0.0;

	}
	for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
		x->iRec36[l27] = 0;

	}
	for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
		x->fRec35[l28] = 0.0;

	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		x->iRec39[l29] = 0;

	}
	for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
		x->fRec40[l30] = 0.0;

	}
	for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
		x->fVec4[l31] = 0.0;

	}
	for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
		x->fRec41[l32] = 0.0;

	}
	for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
		x->fRec38[l33] = 0.0;

	}
	for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
		x->fRec37[l34] = 0.0;

	}
	for (int l35 = 0; (l35 < 8192); l35 = (l35 + 1)) {
		x->fRec14[l35] = 0.0;

	}
	for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
		x->fRec13[l36] = 0.0;

	}
	for (int l37 = 0; (l37 < 8192); l37 = (l37 + 1)) {
		x->fVec5[l37] = 0.0;

	}
	for (int l38 = 0; (l38 < 4096); l38 = (l38 + 1)) {
		x->fVec6[l38] = 0.0;

	}
	for (int l39 = 0; (l39 < 2048); l39 = (l39 + 1)) {
		x->fVec7[l39] = 0.0;

	}
	for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
		x->fRec8[l40] = 0.0;

	}
	for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
		x->fRec45[l41] = 0.0;

	}
	for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
		x->fRec44[l42] = 0.0;

	}
	for (int l43 = 0; (l43 < 32768); l43 = (l43 + 1)) {
		x->fVec8[l43] = 0.0;

	}
	for (int l44 = 0; (l44 < 4096); l44 = (l44 + 1)) {
		x->fVec9[l44] = 0.0;

	}
	for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
		x->fRec42[l45] = 0.0;

	}
	for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
		x->fRec49[l46] = 0.0;

	}
	for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
		x->fRec48[l47] = 0.0;

	}
	for (int l48 = 0; (l48 < 16384); l48 = (l48 + 1)) {
		x->fVec10[l48] = 0.0;

	}
	for (int l49 = 0; (l49 < 4096); l49 = (l49 + 1)) {
		x->fVec11[l49] = 0.0;

	}
	for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
		x->fRec46[l50] = 0.0;

	}
	for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
		x->fRec53[l51] = 0.0;

	}
	for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
		x->fRec52[l52] = 0.0;

	}
	for (int l53 = 0; (l53 < 32768); l53 = (l53 + 1)) {
		x->fVec12[l53] = 0.0;

	}
	for (int l54 = 0; (l54 < 4096); l54 = (l54 + 1)) {
		x->fVec13[l54] = 0.0;

	}
	for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
		x->fRec50[l55] = 0.0;

	}
	for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
		x->fRec57[l56] = 0.0;

	}
	for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
		x->fRec56[l57] = 0.0;

	}
	for (int l58 = 0; (l58 < 16384); l58 = (l58 + 1)) {
		x->fVec14[l58] = 0.0;

	}
	for (int l59 = 0; (l59 < 4096); l59 = (l59 + 1)) {
		x->fVec15[l59] = 0.0;

	}
	for (int l60 = 0; (l60 < 2048); l60 = (l60 + 1)) {
		x->fVec16[l60] = 0.0;

	}
	for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
		x->fRec54[l61] = 0.0;

	}
	for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
		x->fRec61[l62] = 0.0;

	}
	for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
		x->fRec60[l63] = 0.0;

	}
	for (int l64 = 0; (l64 < 16384); l64 = (l64 + 1)) {
		x->fVec17[l64] = 0.0;

	}
	for (int l65 = 0; (l65 < 4096); l65 = (l65 + 1)) {
		x->fVec18[l65] = 0.0;

	}
	for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
		x->fRec58[l66] = 0.0;

	}
	for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
		x->fRec65[l67] = 0.0;

	}
	for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
		x->fRec64[l68] = 0.0;

	}
	for (int l69 = 0; (l69 < 16384); l69 = (l69 + 1)) {
		x->fVec19[l69] = 0.0;

	}
	for (int l70 = 0; (l70 < 4096); l70 = (l70 + 1)) {
		x->fVec20[l70] = 0.0;

	}
	for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
		x->fRec62[l71] = 0.0;

	}
	for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
		x->fRec69[l72] = 0.0;

	}
	for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
		x->fRec68[l73] = 0.0;

	}
	for (int l74 = 0; (l74 < 16384); l74 = (l74 + 1)) {
		x->fVec21[l74] = 0.0;

	}
	for (int l75 = 0; (l75 < 2048); l75 = (l75 + 1)) {
		x->fVec22[l75] = 0.0;

	}
	for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
		x->fRec66[l76] = 0.0;

	}
	for (int l77 = 0; (l77 < 3); l77 = (l77 + 1)) {
		x->fRec0[l77] = 0.0;

	}
	for (int l78 = 0; (l78 < 3); l78 = (l78 + 1)) {
		x->fRec1[l78] = 0.0;

	}
	for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
		x->fRec2[l79] = 0.0;

	}
	for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
		x->fRec3[l80] = 0.0;

	}
	for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
		x->fRec4[l81] = 0.0;

	}
	for (int l82 = 0; (l82 < 3); l82 = (l82 + 1)) {
		x->fRec5[l82] = 0.0;

	}
	for (int l83 = 0; (l83 < 3); l83 = (l83 + 1)) {
		x->fRec6[l83] = 0.0;

	}
	for (int l84 = 0; (l84 < 3); l84 = (l84 + 1)) {
		x->fRec7[l84] = 0.0;

	}
	
	x->freq = 110.0;
	x->gain = 1.0;
	x->gate = 0.0;
	x->mod_freq = 220.0;
	x->mod_type = 1;
	x->nonlinear = 0.0;
	x->touchLength = 0.15;

	x->rvrb_gain = 0.137;
	x->roomsize = 0.72;
	x->pan_angle = 0.6;
	x->spat_width = 0.5;

	return x;
}

void bassf_dsp64(t_bassf *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->fSamplingFreq != samplerate) x->fSamplingFreq = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		bassf_perform64, 0, NULL);
}

void bassf_perform64(t_bassf *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	//t_double *input = ins[0];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	int n = vectorsize;

	double fSlow0 = (double)x->roomsize;
	double fSlow1 = exp((x->fConst4 / fSlow0));
	double fSlow2 = mydsp_faustpower2_f(fSlow1);
	double fSlow3 = (1.0 - (x->fConst1 * fSlow2));
	double fSlow4 = (1.0 - fSlow2);
	double fSlow5 = (fSlow3 / fSlow4);
	double fSlow6 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow3) / mydsp_faustpower2_f(fSlow4)) + -1.0)));
	double fSlow7 = (fSlow5 - fSlow6);
	double fSlow8 = ((fSlow6 + (1.0 - fSlow5)) * fSlow1);
	double fSlow9 = ((exp((x->fConst5 / fSlow0)) / fSlow1) + -1.0);
	double fSlow10 = (double)x->pan_angle;
	double fSlow11 = (4.0 * fSlow10);
	double fSlow12 = (0.00100000005 * (double)x->rvrb_gain);
	double fSlow13 = (double)x->freq;
	int iSlow14 = (int)(((17.3123398 * (log(fSlow13) + -6.08677483)) + 69.5));
	double fSlow15 = (double)(getValueBassLoopFilterb0((double)iSlow14));
	double fSlow16 = (double)(x->mod_type);
	int iSlow17 = (fSlow16 >= 3.0);
	double fSlow18 = x->gate;
	int iSlow19 = (fSlow18 > 0.0);
	int iSlow20 = (fSlow18 < 1.0);
	double fSlow21 = (x->fConst0 / fSlow13);
	double fSlow22 = (0.00100000005 * (double)x->nonlinear);
	double fSlow23 = (fSlow13 * (double)((fSlow16 == 4.0)));
	int iSlow24 = (fSlow16 != 4.0);
	double fSlow25 = (0.00100000005 * (double)x->mod_freq);
	int iSlow26 = (fSlow16 < 3.0);
	double fSlow27 = (3.14159274 * (double)((fSlow16 == 0.0)));
	double fSlow28 = (1.57079637 * (double)((fSlow16 == 1.0)));
	double fSlow29 = (3.14159274 * (double)((fSlow16 == 2.0)));
	double fSlow30 = (double)(getValueBassLoopFilterb1((double)iSlow14));
	double fSlow31 = (double)(getValueBassLoopFiltera1((double)iSlow14));
	int iSlow32 = (iSlow20 > 0);
	int iSlow33 = (iSlow20 < 1);
	double fSlow34 = exp((0.0 - (x->fConst15 / (double)x->touchLength)));
	double fSlow35 = (double)x->gain;
	int iSlow36 = (int)(min(4096.0, max(0.0f, (x->fConst18 * ((double)x->spat_width / fSlow13)))));
	double fSlow37 = exp((x->fConst23 / fSlow0));
	double fSlow38 = mydsp_faustpower2_f(fSlow37);
	double fSlow39 = (1.0 - (x->fConst1 * fSlow38));
	double fSlow40 = (1.0 - fSlow38);
	double fSlow41 = (fSlow39 / fSlow40);
	double fSlow42 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow39) / mydsp_faustpower2_f(fSlow40)) + -1.0)));
	double fSlow43 = (fSlow41 - fSlow42);
	double fSlow44 = ((fSlow42 + (1.0 - fSlow41)) * fSlow37);
	double fSlow45 = ((exp((x->fConst24 / fSlow0)) / fSlow37) + -1.0);
	double fSlow46 = exp((x->fConst30 / fSlow0));
	double fSlow47 = mydsp_faustpower2_f(fSlow46);
	double fSlow48 = (1.0 - (x->fConst1 * fSlow47));
	double fSlow49 = (1.0 - fSlow47);
	double fSlow50 = (fSlow48 / fSlow49);
	double fSlow51 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow48) / mydsp_faustpower2_f(fSlow49)) + -1.0)));
	double fSlow52 = (fSlow50 - fSlow51);
	double fSlow53 = ((fSlow51 + (1.0 - fSlow50)) * fSlow46);
	double fSlow54 = ((exp((x->fConst31 / fSlow0)) / fSlow46) + -1.0);
	double fSlow55 = exp((x->fConst37 / fSlow0));
	double fSlow56 = mydsp_faustpower2_f(fSlow55);
	double fSlow57 = (1.0 - (x->fConst1 * fSlow56));
	double fSlow58 = (1.0 - fSlow56);
	double fSlow59 = (fSlow57 / fSlow58);
	double fSlow60 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow57) / mydsp_faustpower2_f(fSlow58)) + -1.0)));
	double fSlow61 = (fSlow59 - fSlow60);
	double fSlow62 = ((fSlow60 + (1.0 - fSlow59)) * fSlow55);
	double fSlow63 = ((exp((x->fConst38 / fSlow0)) / fSlow55) + -1.0);
	double fSlow64 = exp((x->fConst44 / fSlow0));
	double fSlow65 = mydsp_faustpower2_f(fSlow64);
	double fSlow66 = (1.0 - (x->fConst1 * fSlow65));
	double fSlow67 = (1.0 - fSlow65);
	double fSlow68 = (fSlow66 / fSlow67);
	double fSlow69 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow66) / mydsp_faustpower2_f(fSlow67)) + -1.0)));
	double fSlow70 = (fSlow68 - fSlow69);
	double fSlow71 = ((fSlow69 + (1.0 - fSlow68)) * fSlow64);
	double fSlow72 = ((exp((x->fConst45 / fSlow0)) / fSlow64) + -1.0);
	double fSlow73 = (4.0 * (1.0 - fSlow10));
	double fSlow74 = exp((x->fConst51 / fSlow0));
	double fSlow75 = mydsp_faustpower2_f(fSlow74);
	double fSlow76 = (1.0 - (x->fConst1 * fSlow75));
	double fSlow77 = (1.0 - fSlow75);
	double fSlow78 = (fSlow76 / fSlow77);
	double fSlow79 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow76) / mydsp_faustpower2_f(fSlow77)) + -1.0)));
	double fSlow80 = (fSlow78 - fSlow79);
	double fSlow81 = ((fSlow79 + (1.0 - fSlow78)) * fSlow74);
	double fSlow82 = ((exp((x->fConst52 / fSlow0)) / fSlow74) + -1.0);
	double fSlow83 = exp((x->fConst58 / fSlow0));
	double fSlow84 = mydsp_faustpower2_f(fSlow83);
	double fSlow85 = (1.0 - (x->fConst1 * fSlow84));
	double fSlow86 = (1.0 - fSlow84);
	double fSlow87 = (fSlow85 / fSlow86);
	double fSlow88 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow85) / mydsp_faustpower2_f(fSlow86)) + -1.0)));
	double fSlow89 = (fSlow87 - fSlow88);
	double fSlow90 = ((fSlow88 + (1.0 - fSlow87)) * fSlow83);
	double fSlow91 = ((exp((x->fConst59 / fSlow0)) / fSlow83) + -1.0);
	double fSlow92 = exp((x->fConst65 / fSlow0));
	double fSlow93 = mydsp_faustpower2_f(fSlow92);
	double fSlow94 = (1.0 - (x->fConst1 * fSlow93));
	double fSlow95 = (1.0 - fSlow93);
	double fSlow96 = (fSlow94 / fSlow95);
	double fSlow97 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow94) / mydsp_faustpower2_f(fSlow95)) + -1.0)));
	double fSlow98 = (fSlow96 - fSlow97);
	double fSlow99 = ((fSlow97 + (1.0 - fSlow96)) * fSlow92);
	double fSlow100 = ((exp((x->fConst66 / fSlow0)) / fSlow92) + -1.0);
	//for (int i = 0; (i < count); i = (i + 1)) {
	while(n--){
		x->iVec0[0] = 1;
		x->fRec11[0] = ((x->fConst8 * x->fRec11[1]) + (x->fConst9 * (x->fRec7[1] + x->fRec7[2])));
		x->fRec10[0] = ((fSlow7 * x->fRec10[1]) + (fSlow8 * (x->fRec7[1] + (fSlow9 * x->fRec11[0]))));
		x->fVec1[(x->IOTA & 32767)] = ((0.353553385 * x->fRec10[0]) + 9.99999968e-21);
		x->fRec12[0] = (fSlow12 + (0.999000013 * x->fRec12[1]));
		x->fRec17[0] = ((fSlow18 * x->fRec17[1]) + 1.0);
		double fTemp0 = (x->fRec17[0] + -1.0);
		int iTemp1 = ((fTemp0 < 2.0) & iSlow19);
		double fTemp2 = (0.0301973838 * (double)iTemp1);
		int iTemp3 = ((fTemp0 >= 2.0) | iSlow20);
		double fTemp4 = (fTemp2 + (x->fConst12 * (double)iTemp3));
		int iTemp5 = (iTemp1 < 1);
		x->fRec16[0] = ((x->fRec16[1] * fTemp4) + (fSlow21 * ((1.0 - fTemp4) * (double)iTemp5)));
		int iTemp6 = (int)x->fRec16[0];
		double fTemp7 = floor(x->fRec16[0]);
		double fTemp8 = ((x->fRec14[((x->IOTA - (min(4097, (int)(max(0, (int)iTemp6))) + 1)) & 8191)] * (fTemp7 + (1.0 - x->fRec16[0]))) + ((x->fRec16[0] - fTemp7) * x->fRec14[((x->IOTA - (min(4097, (int)(max(0, (int)(iTemp6 + 1)))) + 1)) & 8191)]));
		x->fVec2[0] = fTemp8;
		x->fRec18[0] = (fSlow22 + (0.999000013 * x->fRec18[1]));
		x->fRec22[0] = (fSlow25 + (0.999000013 * x->fRec22[1]));
		double fTemp9 = (fSlow23 + ((double)iSlow24 * x->fRec22[0]));
		x->fRec21[0] = (x->fRec21[1] + (x->fConst13 * ((0.0 - x->fRec19[1]) * fTemp9)));
		x->fRec20[0] = ((x->fRec20[1] + (x->fConst13 * (x->fRec21[0] * fTemp9))) + (double)(1.0 - x->iVec0[1]));
		x->fRec19[0] = x->fRec20[0];
		double fTemp10 = (3.14159274 * (x->fRec18[0] * x->fRec19[0]));
		double fTemp11 = sin(fTemp10);
		double fTemp12 = (0.0 - fTemp11);
		double fTemp13 = cos(fTemp10);
		double fTemp14 = ((x->fRec23[1] * fTemp12) + (fTemp8 * fTemp13));
		double fTemp15 = ((fTemp12 * x->fRec24[1]) + (fTemp13 * fTemp14));
		double fTemp16 = ((fTemp12 * x->fRec25[1]) + (fTemp13 * fTemp15));
		double fTemp17 = ((fTemp12 * x->fRec26[1]) + (fTemp13 * fTemp16));
		double fTemp18 = ((fTemp12 * x->fRec27[1]) + (fTemp13 * fTemp17));
		x->fRec28[0] = ((fTemp12 * x->fRec28[1]) + (fTemp13 * fTemp18));
		x->fRec27[0] = ((fTemp11 * fTemp18) + (fTemp13 * x->fRec28[1]));
		x->fRec26[0] = ((fTemp11 * fTemp17) + (fTemp13 * x->fRec27[1]));
		x->fRec25[0] = ((fTemp11 * fTemp16) + (fTemp13 * x->fRec26[1]));
		x->fRec24[0] = ((fTemp11 * fTemp15) + (fTemp13 * x->fRec25[1]));
		x->fRec23[0] = ((fTemp11 * fTemp14) + (fTemp13 * x->fRec24[1]));
		double fTemp19 = (x->fRec18[0] * (((fSlow27 * fTemp8) + (fSlow28 * (fTemp8 + x->fVec2[1]))) + (fSlow29 * mydsp_faustpower2_f(fTemp8))));
		double fTemp20 = sin(fTemp19);
		double fTemp21 = (0.0 - fTemp20);
		double fTemp22 = cos(fTemp19);
		double fTemp23 = ((x->fRec29[1] * fTemp21) + (fTemp8 * fTemp22));
		double fTemp24 = ((fTemp21 * x->fRec30[1]) + (fTemp22 * fTemp23));
		double fTemp25 = ((fTemp21 * x->fRec31[1]) + (fTemp22 * fTemp24));
		double fTemp26 = ((fTemp21 * x->fRec32[1]) + (fTemp22 * fTemp25));
		double fTemp27 = ((fTemp21 * x->fRec33[1]) + (fTemp22 * fTemp26));
		x->fRec34[0] = ((fTemp21 * x->fRec34[1]) + (fTemp22 * fTemp27));
		x->fRec33[0] = ((fTemp20 * fTemp27) + (fTemp22 * x->fRec34[1]));
		x->fRec32[0] = ((fTemp20 * fTemp26) + (fTemp22 * x->fRec33[1]));
		x->fRec31[0] = ((fTemp20 * fTemp25) + (fTemp22 * x->fRec32[1]));
		x->fRec30[0] = ((fTemp20 * fTemp24) + (fTemp22 * x->fRec31[1]));
		x->fRec29[0] = ((fTemp20 * fTemp23) + (fTemp22 * x->fRec30[1]));
		double fTemp28 = (((double)iSlow17 * ((fTemp8 * fTemp11) + (x->fRec23[1] * fTemp13))) + ((double)iSlow26 * ((x->fRec18[0] * ((fTemp8 * fTemp20) + (x->fRec29[1] * fTemp22))) + ((1.0 - x->fRec18[0]) * fTemp8))));
		x->fVec3[0] = fTemp28;
		x->fRec15[0] = (((fSlow15 * fTemp28) + (fSlow30 * x->fVec3[1])) - (fSlow31 * x->fRec15[1]));
		x->iRec36[0] = ((iSlow20 * x->iRec36[1]) + 1);
		int iTemp29 = (x->iRec36[0] + -1);
		int iTemp30 = (((double)iTemp29 < 2.0) & iSlow32);
		double fTemp31 = ((0.0301973838 * (double)iTemp30) + (x->fConst14 * (double)((((double)iTemp29 >= 2.0) | iSlow33))));
		x->fRec35[0] = ((x->fRec35[1] * fTemp31) + ((1.0 - fTemp31) * ((double)iTemp30 + (0.899999976 * (double)(iTemp30 < 1)))));
		x->iRec39[0] = ((1103515245 * x->iRec39[1]) + 12345);
		double fTemp32 = (fTemp2 + (fSlow34 * (double)iTemp3));
		x->fRec40[0] = ((x->fRec40[1] * fTemp32) + (fSlow35 * ((double)iTemp1 * (1.0 - fTemp32))));
		double fTemp33 = ((double)x->iRec39[0] * x->fRec40[0]);
		x->fVec4[0] = fTemp33;
		double fTemp34 = (fTemp2 + (x->fConst16 * (double)iTemp3));
		x->fRec41[0] = ((x->fRec41[1] * fTemp34) + ((1.0 - fTemp34) * (0.0 - ((0.5 * (double)iTemp1) + (0.985000014 * (double)iTemp5)))));
		x->fRec38[0] = ((1.62981451e-11 * ((fTemp33 * (x->fRec41[0] + 1.0)) - (x->fRec41[0] * x->fVec4[1]))) + (0.964999974 * x->fRec38[1]));
		x->fRec37[0] = ((0.0350000001 * x->fRec38[0]) + (0.964999974 * x->fRec37[1]));
		x->fRec14[(x->IOTA & 8191)] = ((x->fRec15[0] * (fSlow18 + ((double)iSlow20 * x->fRec35[0]))) + x->fRec37[0]);
		x->fRec13[0] = (x->fRec14[((x->IOTA - 0) & 8191)] - ((x->fConst17 * x->fRec13[1]) + (0.994009018 * x->fRec13[2])));
		double fTemp35 = ((0.00449325005 * (x->fRec13[0] - x->fRec13[2])) + (0.5 * x->fRec14[((x->IOTA - 0) & 8191)]));
		x->fVec5[(x->IOTA & 8191)] = fTemp35;
		x->fVec6[(x->IOTA & 4095)] = (fSlow11 * (x->fRec12[0] * x->fVec5[((x->IOTA - iSlow36) & 8191)]));
		double fTemp36 = (0.300000012 * x->fVec6[((x->IOTA - x->iConst19) & 4095)]);
		double fTemp37 = (((0.600000024 * x->fRec8[1]) + x->fVec1[((x->IOTA - x->iConst11) & 32767)]) - fTemp36);
		x->fVec7[(x->IOTA & 2047)] = fTemp37;
		x->fRec8[0] = x->fVec7[((x->IOTA - x->iConst20) & 2047)];
		double fRec9 = (0.0 - (0.600000024 * fTemp37));
		x->fRec45[0] = ((x->fConst8 * x->fRec45[1]) + (x->fConst9 * (x->fRec3[1] + x->fRec3[2])));
		x->fRec44[0] = ((fSlow43 * x->fRec44[1]) + (fSlow44 * (x->fRec3[1] + (fSlow45 * x->fRec45[0]))));
		x->fVec8[(x->IOTA & 32767)] = ((0.353553385 * x->fRec44[0]) + 9.99999968e-21);
		double fTemp38 = (((0.600000024 * x->fRec42[1]) + x->fVec8[((x->IOTA - x->iConst26) & 32767)]) - fTemp36);
		x->fVec9[(x->IOTA & 4095)] = fTemp38;
		x->fRec42[0] = x->fVec9[((x->IOTA - x->iConst27) & 4095)];
		double fRec43 = (0.0 - (0.600000024 * fTemp38));
		x->fRec49[0] = ((x->fConst8 * x->fRec49[1]) + (x->fConst9 * (x->fRec5[1] + x->fRec5[2])));
		x->fRec48[0] = ((fSlow52 * x->fRec48[1]) + (fSlow53 * (x->fRec5[1] + (fSlow54 * x->fRec49[0]))));
		x->fVec10[(x->IOTA & 16383)] = ((0.353553385 * x->fRec48[0]) + 9.99999968e-21);
		double fTemp39 = (x->fVec10[((x->IOTA - x->iConst33) & 16383)] + (fTemp36 + (0.600000024 * x->fRec46[1])));
		x->fVec11[(x->IOTA & 4095)] = fTemp39;
		x->fRec46[0] = x->fVec11[((x->IOTA - x->iConst34) & 4095)];
		double fRec47 = (0.0 - (0.600000024 * fTemp39));
		x->fRec53[0] = ((x->fConst8 * x->fRec53[1]) + (x->fConst9 * (x->fRec1[1] + x->fRec1[2])));
		x->fRec52[0] = ((fSlow61 * x->fRec52[1]) + (fSlow62 * (x->fRec1[1] + (fSlow63 * x->fRec53[0]))));
		x->fVec12[(x->IOTA & 32767)] = ((0.353553385 * x->fRec52[0]) + 9.99999968e-21);
		double fTemp40 = (fTemp36 + ((0.600000024 * x->fRec50[1]) + x->fVec12[((x->IOTA - x->iConst40) & 32767)]));
		x->fVec13[(x->IOTA & 4095)] = fTemp40;
		x->fRec50[0] = x->fVec13[((x->IOTA - x->iConst41) & 4095)];
		double fRec51 = (0.0 - (0.600000024 * fTemp40));
		x->fRec57[0] = ((x->fConst8 * x->fRec57[1]) + (x->fConst9 * (x->fRec6[1] + x->fRec6[2])));
		x->fRec56[0] = ((fSlow70 * x->fRec56[1]) + (fSlow71 * (x->fRec6[1] + (fSlow72 * x->fRec57[0]))));
		x->fVec14[(x->IOTA & 16383)] = ((0.353553385 * x->fRec56[0]) + 9.99999968e-21);
		x->fVec15[(x->IOTA & 4095)] = (fSlow73 * (x->fRec12[0] * fTemp35));
		double fTemp41 = (0.300000012 * x->fVec15[((x->IOTA - x->iConst19) & 4095)]);
		double fTemp42 = (x->fVec14[((x->IOTA - x->iConst47) & 16383)] - (fTemp41 + (0.600000024 * x->fRec54[1])));
		x->fVec16[(x->IOTA & 2047)] = fTemp42;
		x->fRec54[0] = x->fVec16[((x->IOTA - x->iConst48) & 2047)];
		double fRec55 = (0.600000024 * fTemp42);
		x->fRec61[0] = ((x->fConst8 * x->fRec61[1]) + (x->fConst9 * (x->fRec2[1] + x->fRec2[2])));
		x->fRec60[0] = ((fSlow80 * x->fRec60[1]) + (fSlow81 * (x->fRec2[1] + (fSlow82 * x->fRec61[0]))));
		x->fVec17[(x->IOTA & 16383)] = ((0.353553385 * x->fRec60[0]) + 9.99999968e-21);
		double fTemp43 = (x->fVec17[((x->IOTA - x->iConst54) & 16383)] - (fTemp41 + (0.600000024 * x->fRec58[1])));
		x->fVec18[(x->IOTA & 4095)] = fTemp43;
		x->fRec58[0] = x->fVec18[((x->IOTA - x->iConst55) & 4095)];
		double fRec59 = (0.600000024 * fTemp43);
		x->fRec65[0] = ((x->fConst8 * x->fRec65[1]) + (x->fConst9 * (x->fRec4[1] + x->fRec4[2])));
		x->fRec64[0] = ((fSlow89 * x->fRec64[1]) + (fSlow90 * (x->fRec4[1] + (fSlow91 * x->fRec65[0]))));
		x->fVec19[(x->IOTA & 16383)] = ((0.353553385 * x->fRec64[0]) + 9.99999968e-21);
		double fTemp44 = ((fTemp41 + x->fVec19[((x->IOTA - x->iConst61) & 16383)]) - (0.600000024 * x->fRec62[1]));
		x->fVec20[(x->IOTA & 4095)] = fTemp44;
		x->fRec62[0] = x->fVec20[((x->IOTA - x->iConst62) & 4095)];
		double fRec63 = (0.600000024 * fTemp44);
		x->fRec69[0] = ((x->fConst8 * x->fRec69[1]) + (x->fConst9 * (x->fRec0[1] + x->fRec0[2])));
		x->fRec68[0] = ((fSlow98 * x->fRec68[1]) + (fSlow99 * (x->fRec0[1] + (fSlow100 * x->fRec69[0]))));
		x->fVec21[(x->IOTA & 16383)] = ((0.353553385 * x->fRec68[0]) + 9.99999968e-21);
		double fTemp45 = ((x->fVec21[((x->IOTA - x->iConst68) & 16383)] + fTemp41) - (0.600000024 * x->fRec66[1]));
		x->fVec22[(x->IOTA & 2047)] = fTemp45;
		x->fRec66[0] = x->fVec22[((x->IOTA - x->iConst69) & 2047)];
		double fRec67 = (0.600000024 * fTemp45);
		double fTemp46 = (fRec67 + fRec63);
		double fTemp47 = (fRec55 + (fRec59 + fTemp46));
		x->fRec0[0] = (x->fRec8[1] + (x->fRec42[1] + (x->fRec46[1] + (x->fRec50[1] + (x->fRec54[1] + (x->fRec58[1] + (x->fRec62[1] + (x->fRec66[1] + (fRec9 + (fRec43 + (fRec47 + (fRec51 + fTemp47))))))))))));
		x->fRec1[0] = ((x->fRec54[1] + (x->fRec58[1] + (x->fRec62[1] + (x->fRec66[1] + fTemp47)))) - (x->fRec8[1] + (x->fRec42[1] + (x->fRec46[1] + (x->fRec50[1] + (fRec9 + (fRec43 + (fRec51 + fRec47))))))));
		double fTemp48 = (fRec59 + fRec55);
		x->fRec2[0] = ((x->fRec46[1] + (x->fRec50[1] + (x->fRec62[1] + (x->fRec66[1] + (fRec47 + (fRec51 + fTemp46)))))) - (x->fRec8[1] + (x->fRec42[1] + (x->fRec54[1] + (x->fRec58[1] + (fRec9 + (fRec43 + fTemp48)))))));
		x->fRec3[0] = ((x->fRec8[1] + (x->fRec42[1] + (x->fRec62[1] + (x->fRec66[1] + (fRec9 + (fRec43 + fTemp46)))))) - (x->fRec46[1] + (x->fRec50[1] + (x->fRec54[1] + (x->fRec58[1] + (fRec47 + (fRec51 + fTemp48)))))));
		double fTemp49 = (fRec67 + fRec59);
		double fTemp50 = (fRec63 + fRec55);
		x->fRec4[0] = ((x->fRec42[1] + (x->fRec50[1] + (x->fRec58[1] + (x->fRec66[1] + (fRec43 + (fRec51 + fTemp49)))))) - (x->fRec8[1] + (x->fRec46[1] + (x->fRec54[1] + (x->fRec62[1] + (fRec9 + (fRec47 + fTemp50)))))));
		x->fRec5[0] = ((x->fRec8[1] + (x->fRec46[1] + (x->fRec58[1] + (x->fRec66[1] + (fRec9 + (fRec47 + fTemp49)))))) - (x->fRec42[1] + (x->fRec50[1] + (x->fRec54[1] + (x->fRec62[1] + (fRec43 + (fRec51 + fTemp50)))))));
		double fTemp51 = (fRec67 + fRec55);
		double fTemp52 = (fRec63 + fRec59);
		x->fRec6[0] = ((x->fRec8[1] + (x->fRec50[1] + (x->fRec54[1] + (x->fRec66[1] + (fRec9 + (fRec51 + fTemp51)))))) - (x->fRec42[1] + (x->fRec46[1] + (x->fRec58[1] + (x->fRec62[1] + (fRec43 + (fRec47 + fTemp52)))))));
		x->fRec7[0] = ((x->fRec42[1] + (x->fRec46[1] + (x->fRec54[1] + (x->fRec66[1] + (fRec43 + (fRec47 + fTemp51)))))) - (x->fRec8[1] + (x->fRec50[1] + (x->fRec58[1] + (x->fRec62[1] + (fRec9 + (fRec51 + fTemp52)))))));
		double fTemp53 = (1.0 - x->fRec12[0]);
		
		*output0++ = (t_double)((0.370000005 * (x->fRec1[0] + x->fRec2[0])) + (fSlow73 * (fTemp53 * fTemp35)));
		*output1++ = (t_double)((0.370000005 * (x->fRec1[0] - x->fRec2[0])) + (fSlow11 * (fTemp53 * x->fVec5[((x->IOTA - iSlow36) & 8191)])));
		
		x->iVec0[1] = x->iVec0[0];
		x->fRec11[1] = x->fRec11[0];
		x->fRec10[1] = x->fRec10[0];
		x->IOTA = (x->IOTA + 1);
		x->fRec12[1] = x->fRec12[0];
		x->fRec17[1] = x->fRec17[0];
		x->fRec16[1] = x->fRec16[0];
		x->fVec2[1] = x->fVec2[0];
		x->fRec18[1] = x->fRec18[0];
		x->fRec22[1] = x->fRec22[0];
		x->fRec21[1] = x->fRec21[0];
		x->fRec20[1] = x->fRec20[0];
		x->fRec19[1] = x->fRec19[0];
		x->fRec28[1] = x->fRec28[0];
		x->fRec27[1] = x->fRec27[0];
		x->fRec26[1] = x->fRec26[0];
		x->fRec25[1] = x->fRec25[0];
		x->fRec24[1] = x->fRec24[0];
		x->fRec23[1] = x->fRec23[0];
		x->fRec34[1] = x->fRec34[0];
		x->fRec33[1] = x->fRec33[0];
		x->fRec32[1] = x->fRec32[0];
		x->fRec31[1] = x->fRec31[0];
		x->fRec30[1] = x->fRec30[0];
		x->fRec29[1] = x->fRec29[0];
		x->fVec3[1] = x->fVec3[0];
		x->fRec15[1] = x->fRec15[0];
		x->iRec36[1] = x->iRec36[0];
		x->fRec35[1] = x->fRec35[0];
		x->iRec39[1] = x->iRec39[0];
		x->fRec40[1] = x->fRec40[0];
		x->fVec4[1] = x->fVec4[0];
		x->fRec41[1] = x->fRec41[0];
		x->fRec38[1] = x->fRec38[0];
		x->fRec37[1] = x->fRec37[0];
		x->fRec13[2] = x->fRec13[1];
		x->fRec13[1] = x->fRec13[0];
		x->fRec8[1] = x->fRec8[0];
		x->fRec45[1] = x->fRec45[0];
		x->fRec44[1] = x->fRec44[0];
		x->fRec42[1] = x->fRec42[0];
		x->fRec49[1] = x->fRec49[0];
		x->fRec48[1] = x->fRec48[0];
		x->fRec46[1] = x->fRec46[0];
		x->fRec53[1] = x->fRec53[0];
		x->fRec52[1] = x->fRec52[0];
		x->fRec50[1] = x->fRec50[0];
		x->fRec57[1] = x->fRec57[0];
		x->fRec56[1] = x->fRec56[0];
		x->fRec54[1] = x->fRec54[0];
		x->fRec61[1] = x->fRec61[0];
		x->fRec60[1] = x->fRec60[0];
		x->fRec58[1] = x->fRec58[0];
		x->fRec65[1] = x->fRec65[0];
		x->fRec64[1] = x->fRec64[0];
		x->fRec62[1] = x->fRec62[0];
		x->fRec69[1] = x->fRec69[0];
		x->fRec68[1] = x->fRec68[0];
		x->fRec66[1] = x->fRec66[0];
		x->fRec0[2] = x->fRec0[1];
		x->fRec0[1] = x->fRec0[0];
		x->fRec1[2] = x->fRec1[1];
		x->fRec1[1] = x->fRec1[0];
		x->fRec2[2] = x->fRec2[1];
		x->fRec2[1] = x->fRec2[0];
		x->fRec3[2] = x->fRec3[1];
		x->fRec3[1] = x->fRec3[0];
		x->fRec4[2] = x->fRec4[1];
		x->fRec4[1] = x->fRec4[0];
		x->fRec5[2] = x->fRec5[1];
		x->fRec5[1] = x->fRec5[0];
		x->fRec6[2] = x->fRec6[1];
		x->fRec6[1] = x->fRec6[0];
		x->fRec7[2] = x->fRec7[1];
		x->fRec7[1] = x->fRec7[0];

	}
}

double mydsp_faustpower2_f(double value) {
	return (value * value);
}

void bassf_free(t_bassf* x) {
	dsp_free((t_pxobject*)x);

	sysmem_freeptr(x->iVec0);
	sysmem_freeptr(x->fRec11);
	sysmem_freeptr(x->fRec10);
	sysmem_freeptr(x->fRec20);
	sysmem_freeptr(x->fRec28);
	sysmem_freeptr(x->fRec12);
	sysmem_freeptr(x->fRec16);
	sysmem_freeptr(x->fRec27);
	sysmem_freeptr(x->fRec13);
	sysmem_freeptr(x->fRec29);
	sysmem_freeptr(x->fRec26);
	sysmem_freeptr(x->fRec25);
	sysmem_freeptr(x->fRec24);
	sysmem_freeptr(x->fRec23);
	sysmem_freeptr(x->fRec22);
	sysmem_freeptr(x->fRec21);
	sysmem_freeptr(x->fVec1);
	sysmem_freeptr(x->fRec19);
	sysmem_freeptr(x->fRec18);
	sysmem_freeptr(x->fRec17);
	sysmem_freeptr(x->fVec2);
	sysmem_freeptr(x->iRec39);
	sysmem_freeptr(x->iRec36);
	sysmem_freeptr(x->fRec33);
	sysmem_freeptr(x->fRec32);
	sysmem_freeptr(x->fRec31);
	sysmem_freeptr(x->fVec3);
	sysmem_freeptr(x->fRec30);
	sysmem_freeptr(x->fRec14);
	sysmem_freeptr(x->fRec15);
	sysmem_freeptr(x->fVec4);
	sysmem_freeptr(x->fVec5);
	sysmem_freeptr(x->fRec42);
	sysmem_freeptr(x->fRec41);
	sysmem_freeptr(x->fRec40);
	sysmem_freeptr(x->fRec45);
	sysmem_freeptr(x->fRec38);
	sysmem_freeptr(x->fRec37);
	sysmem_freeptr(x->fRec35);
	sysmem_freeptr(x->fRec34);
	sysmem_freeptr(x->fVec6);
	sysmem_freeptr(x->fVec7);
	sysmem_freeptr(x->fVec8);
	sysmem_freeptr(x->fRec8);
	sysmem_freeptr(x->fRec46);
	sysmem_freeptr(x->fVec9);
	sysmem_freeptr(x->fVec10);
	sysmem_freeptr(x->fRec44);
	sysmem_freeptr(x->fRec49);
	sysmem_freeptr(x->fRec50);
	sysmem_freeptr(x->fRec53);
	sysmem_freeptr(x->fVec11);
	sysmem_freeptr(x->fVec14);
	sysmem_freeptr(x->fRec52);
	sysmem_freeptr(x->fRec58);
	sysmem_freeptr(x->fVec15);
	sysmem_freeptr(x->fVec16);
	sysmem_freeptr(x->fVec17);
	sysmem_freeptr(x->fRec56);
	sysmem_freeptr(x->fRec57);
	sysmem_freeptr(x->fRec62);
	sysmem_freeptr(x->fVec18);
	sysmem_freeptr(x->fVec19);
	sysmem_freeptr(x->fRec60);
	sysmem_freeptr(x->fRec61);
	sysmem_freeptr(x->fVec21);
	sysmem_freeptr(x->fRec64);
	sysmem_freeptr(x->fVec22);
	sysmem_freeptr(x->fRec65);
	sysmem_freeptr(x->fRec68);
	sysmem_freeptr(x->fRec69);
	sysmem_freeptr(x->fRec0);
	sysmem_freeptr(x->fRec1);
	sysmem_freeptr(x->fRec2);
	sysmem_freeptr(x->fRec3);
	sysmem_freeptr(x->fRec4);
	sysmem_freeptr(x->fRec5);
	sysmem_freeptr(x->fRec6);
	sysmem_freeptr(x->fRec7);
	sysmem_freeptr(x->fVec12);
	sysmem_freeptr(x->fRec48);
	sysmem_freeptr(x->fRec54);
	sysmem_freeptr(x->fVec13);
	sysmem_freeptr(x->fRec66);
	sysmem_freeptr(x->fVec20);
}

void setFreq(t_bassf *x, double frequency) {
	if (frequency < 20.0) x->freq = 20.0;
	else if (frequency > 20000.0) x->freq = 20000.0;
	else x->freq = frequency;
}

void playStart(t_bassf *x, double gt) {
	if (gt < 0.0) x->gate = 0.0;
	else if (gt > 1.0) x->gate = 1.0;
	else x->gate = gt;
}

void setGain(t_bassf *x, double gn) {
	if (gn < 0.0) x->gain = 0.0;
	else if (gn > 1.0) x->gain = 1.0;
	else x->gain = gn;
}

void setModFreq(t_bassf *x, double modfreq) {
	if (modfreq < 20.0) x->mod_freq = 20.0;
	else if (modfreq > 1000.0) x->mod_freq = 1000.0;
	else x->mod_freq = modfreq;
}

void setModType(t_bassf *x, int type) {
	if (type == 1) x->mod_type = 1;
	else if (type == 2) x->mod_type = 2;
	else if (type == 3) x->mod_type = 3;
	else if (type == 4) x->mod_type = 4;
	else x->mod_type = 0;
}

void setNLinearity(t_bassf *x, double nlin) {
	if (nlin < 0.0) x->nonlinear = 0.0;
	else if (nlin > 1.0) x->nonlinear = 1.0;
	else x->nonlinear = nlin;
}

void setTouchLength(t_bassf *x, double tlength) {
	if (tlength < 0.0) x->touchLength = 0.0;
	else if (tlength > 1.0) x->touchLength = 1.0;
	else x->touchLength = tlength;
}

void setReverb(t_bassf *x, double rvrbgain) {
	if (rvrbgain < 0.0) x->rvrb_gain = 0.0;
	else if (rvrbgain > 1.0) x->rvrb_gain = 1.0;
	else x->rvrb_gain = rvrbgain;
}
void setRoom(t_bassf *x, double rmSize) {
	if (rmSize < 0.01) x->roomsize = 0.01;
	else if (rmSize > 2.0) x->roomsize = 2.0;
	else x->roomsize = rmSize;
}

void setAngle(t_bassf *x, double panAngle) {
	if (panAngle < 0.0) x->pan_angle = 0.0;
	else if (panAngle > 1.0) x->pan_angle = 1.0;
	else x->pan_angle = panAngle;
}

void setWidth(t_bassf *x, double width) {
	if (width < 0.0) x->spat_width = 0.0;
	else if (width > 1.0) x->spat_width = 1.0;
	else x->spat_width = width;
}

void setDef(t_bassf *x) {
	x->gain = 1.0;
	x->mod_freq = 220.0;
	x->mod_type = 1;
	x->nonlinear = 0.0;
	x->touchLength = 0.15;

	x->rvrb_gain = 0.137;
	x->roomsize = 0.72;
	x->pan_angle = 0.6;
	x->spat_width = 0.5;
}

void clearall(t_bassf *x) {
	x->gate = 0.0;

	x->iVec0 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec11 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec10 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fRec12 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec17 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec16 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec2 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec21 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec19 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec28 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec27 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec26 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec25 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec24 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec23 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec34 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec33 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec31 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec3 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec15 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->iRec36 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec35 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->iRec39 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec40 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec4 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec41 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec38 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec37 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec14 = (double *)sysmem_newptrclear(8192 * sizeof(double));
	x->fRec13 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fVec5 = (double *)sysmem_newptrclear(8192 * sizeof(double));
	x->fVec6 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fVec7 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec8 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec45 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec44 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec8 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fVec9 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec42 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec49 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec48 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec10 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec11 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec46 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec53 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec52 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec12 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fVec13 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec50 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec57 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec56 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec14 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec15 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fVec16 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec54 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec61 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec60 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec17 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec18 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec58 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec65 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec64 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec19 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec20 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec62 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec69 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec68 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec21 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec22 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec66 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec1 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec2 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec3 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec4 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec5 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec6 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec7 = (double *)sysmem_newptrclear(3 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->iVec0[l0] = 0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec11[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		x->fRec10[l2] = 0.0;

	}
	x->IOTA = 0;
	for (int l3 = 0; (l3 < 32768); l3 = (l3 + 1)) {
		x->fVec1[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec12[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		x->fRec17[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		x->fRec16[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fVec2[l7] = 0.0;

	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec18[l8] = 0.0;

	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		x->fRec22[l9] = 0.0;

	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		x->fRec21[l10] = 0.0;

	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec20[l11] = 0.0;

	}
	for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
		x->fRec19[l12] = 0.0;

	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		x->fRec28[l13] = 0.0;

	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->fRec27[l14] = 0.0;

	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		x->fRec26[l15] = 0.0;

	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec25[l16] = 0.0;

	}
	for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
		x->fRec24[l17] = 0.0;

	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		x->fRec23[l18] = 0.0;

	}
	for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
		x->fRec34[l19] = 0.0;

	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		x->fRec33[l20] = 0.0;

	}
	for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
		x->fRec32[l21] = 0.0;

	}
	for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
		x->fRec31[l22] = 0.0;

	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		x->fRec30[l23] = 0.0;

	}
	for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
		x->fRec29[l24] = 0.0;

	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		x->fVec3[l25] = 0.0;

	}
	for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
		x->fRec15[l26] = 0.0;

	}
	for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
		x->iRec36[l27] = 0;

	}
	for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
		x->fRec35[l28] = 0.0;

	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		x->iRec39[l29] = 0;

	}
	for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
		x->fRec40[l30] = 0.0;

	}
	for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
		x->fVec4[l31] = 0.0;

	}
	for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
		x->fRec41[l32] = 0.0;

	}
	for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
		x->fRec38[l33] = 0.0;

	}
	for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
		x->fRec37[l34] = 0.0;

	}
	for (int l35 = 0; (l35 < 8192); l35 = (l35 + 1)) {
		x->fRec14[l35] = 0.0;

	}
	for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
		x->fRec13[l36] = 0.0;

	}
	for (int l37 = 0; (l37 < 8192); l37 = (l37 + 1)) {
		x->fVec5[l37] = 0.0;

	}
	for (int l38 = 0; (l38 < 4096); l38 = (l38 + 1)) {
		x->fVec6[l38] = 0.0;

	}
	for (int l39 = 0; (l39 < 2048); l39 = (l39 + 1)) {
		x->fVec7[l39] = 0.0;

	}
	for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
		x->fRec8[l40] = 0.0;

	}
	for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
		x->fRec45[l41] = 0.0;

	}
	for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
		x->fRec44[l42] = 0.0;

	}
	for (int l43 = 0; (l43 < 32768); l43 = (l43 + 1)) {
		x->fVec8[l43] = 0.0;

	}
	for (int l44 = 0; (l44 < 4096); l44 = (l44 + 1)) {
		x->fVec9[l44] = 0.0;

	}
	for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
		x->fRec42[l45] = 0.0;

	}
	for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
		x->fRec49[l46] = 0.0;

	}
	for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
		x->fRec48[l47] = 0.0;

	}
	for (int l48 = 0; (l48 < 16384); l48 = (l48 + 1)) {
		x->fVec10[l48] = 0.0;

	}
	for (int l49 = 0; (l49 < 4096); l49 = (l49 + 1)) {
		x->fVec11[l49] = 0.0;

	}
	for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
		x->fRec46[l50] = 0.0;

	}
	for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
		x->fRec53[l51] = 0.0;

	}
	for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
		x->fRec52[l52] = 0.0;

	}
	for (int l53 = 0; (l53 < 32768); l53 = (l53 + 1)) {
		x->fVec12[l53] = 0.0;

	}
	for (int l54 = 0; (l54 < 4096); l54 = (l54 + 1)) {
		x->fVec13[l54] = 0.0;

	}
	for (int l55 = 0; (l55 < 2); l55 = (l55 + 1)) {
		x->fRec50[l55] = 0.0;

	}
	for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
		x->fRec57[l56] = 0.0;

	}
	for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
		x->fRec56[l57] = 0.0;

	}
	for (int l58 = 0; (l58 < 16384); l58 = (l58 + 1)) {
		x->fVec14[l58] = 0.0;

	}
	for (int l59 = 0; (l59 < 4096); l59 = (l59 + 1)) {
		x->fVec15[l59] = 0.0;

	}
	for (int l60 = 0; (l60 < 2048); l60 = (l60 + 1)) {
		x->fVec16[l60] = 0.0;

	}
	for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
		x->fRec54[l61] = 0.0;

	}
	for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
		x->fRec61[l62] = 0.0;

	}
	for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
		x->fRec60[l63] = 0.0;

	}
	for (int l64 = 0; (l64 < 16384); l64 = (l64 + 1)) {
		x->fVec17[l64] = 0.0;

	}
	for (int l65 = 0; (l65 < 4096); l65 = (l65 + 1)) {
		x->fVec18[l65] = 0.0;

	}
	for (int l66 = 0; (l66 < 2); l66 = (l66 + 1)) {
		x->fRec58[l66] = 0.0;

	}
	for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
		x->fRec65[l67] = 0.0;

	}
	for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
		x->fRec64[l68] = 0.0;

	}
	for (int l69 = 0; (l69 < 16384); l69 = (l69 + 1)) {
		x->fVec19[l69] = 0.0;

	}
	for (int l70 = 0; (l70 < 4096); l70 = (l70 + 1)) {
		x->fVec20[l70] = 0.0;

	}
	for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
		x->fRec62[l71] = 0.0;

	}
	for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
		x->fRec69[l72] = 0.0;

	}
	for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
		x->fRec68[l73] = 0.0;

	}
	for (int l74 = 0; (l74 < 16384); l74 = (l74 + 1)) {
		x->fVec21[l74] = 0.0;

	}
	for (int l75 = 0; (l75 < 2048); l75 = (l75 + 1)) {
		x->fVec22[l75] = 0.0;

	}
	for (int l76 = 0; (l76 < 2); l76 = (l76 + 1)) {
		x->fRec66[l76] = 0.0;

	}
	for (int l77 = 0; (l77 < 3); l77 = (l77 + 1)) {
		x->fRec0[l77] = 0.0;

	}
	for (int l78 = 0; (l78 < 3); l78 = (l78 + 1)) {
		x->fRec1[l78] = 0.0;

	}
	for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
		x->fRec2[l79] = 0.0;

	}
	for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
		x->fRec3[l80] = 0.0;

	}
	for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
		x->fRec4[l81] = 0.0;

	}
	for (int l82 = 0; (l82 < 3); l82 = (l82 + 1)) {
		x->fRec5[l82] = 0.0;

	}
	for (int l83 = 0; (l83 < 3); l83 = (l83 + 1)) {
		x->fRec6[l83] = 0.0;

	}
	for (int l84 = 0; (l84 < 3); l84 = (l84 + 1)) {
		x->fRec7[l84] = 0.0;

	}
}