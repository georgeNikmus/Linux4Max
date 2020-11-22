#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

//#include <math.h>
//#include <cmath>
#include  "piano.h"


static t_class *pianof_class;

typedef struct _pianof {
	t_pxobject x_obj;

	double freq;
	double gain;
	double gate;
	double brght_f;
	double dtng_f;
	double hmmr_hrd;
	double stffn_f;
	double rvrb_gain;
	double roomsize;
	double pan_angle;
	double spat_width;

	double fSamplingFreq;

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
	double *fVec0;
	double fConst10;
	int iConst11;
	double *fRec12;
	double fConst12;
	double fConst13;
	double fConst14;
	double *fRec20;
	int *iRec26;
	double *fRec28;
	double fConst15;
	double *fRec27;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double *fRec29;
	double *fRec25;
	double *fRec24;
	double *fRec23;
	double *fRec22;
	double *fRec21;
	double *fVec1;
	double *fRec19;
	double *fRec18;
	double *fRec17;
	double *fVec2;
	double *fRec33;
	double *fRec32;
	double *fRec31;
	double *fVec3;
	double *fRec30;
	double *fRec14;
	double *fRec15;
	double fConst20;
	double *fRec13;
	double fConst21;
	double *fVec4;
	double *fVec5;
	double *fRec43;
	double *fRec42;
	double *fRec41;
	double *fRec40;
	double *fRec39;
	double *fRec38;
	double *fRec37;
	double *fRec36;
	double *fRec35;
	double *fRec34;
	double *fVec6;
	double fConst22;
	double *fVec7;
	int iConst23;
	double *fVec8;
	int iConst24;
	double *fRec8;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double *fRec47;
	double *fRec46;
	double *fVec9;
	double fConst29;
	int iConst30;
	double *fVec10;
	int iConst31;
	double *fRec44;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double *fRec51;
	double *fRec50;
	double *fVec11;
	double fConst36;
	int iConst37;
	double *fVec12;
	int iConst38;
	double *fRec48;
	double fConst39;
	double fConst40;
	double fConst41;
	double fConst42;
	double *fRec55;
	double *fRec54;
	double *fVec13;
	double fConst43;
	int iConst44;
	double *fVec14;
	int iConst45;
	double *fRec52;
	double fConst46;
	double fConst47;
	double fConst48;
	double fConst49;
	double *fRec59;
	double *fRec58;
	double *fVec15;
	double fConst50;
	int iConst51;
	double *fVec16;
	double *fVec17;
	int iConst52;
	double *fRec56;
	double fConst53;
	double fConst54;
	double fConst55;
	double fConst56;
	double *fRec63; 
	double *fRec62;
	double *fVec18;
	double fConst57;
	int iConst58;
	double *fVec19;
	int iConst59;
	double *fRec60;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	double *fRec67;
	double *fRec66;
	double *fVec20;
	double fConst64;
	int iConst65;
	double *fVec21;
	int iConst66;
	double *fRec64;
	double fConst67;
	double fConst68;
	double fConst69;
	double fConst70;
	double *fRec71;
	double *fRec70;
	double *fVec22;
	double fConst71;
	int iConst72;
	double *fVec23;
	int iConst73;
	double *fRec68;
	double *fRec0;
	double *fRec1;
	double *fRec2;
	double *fRec3;
	double *fRec4;
	double *fRec5;
	double *fRec6;
	double *fRec7;
	
} t_pianof;

void pianof_dsp64(t_pianof *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);

void pianof_perform64(t_pianof *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

double mydsp_faustpower2_f(double value);

void setFreq(t_pianof *x, double frequency);
void playStart(t_pianof *x, double gt);
void setGain(t_pianof *x, double gn);
void setBrightness(t_pianof *x, double brght);
void setDetuning(t_pianof *x, double dtng);
void setHammer(t_pianof *x, double hmmr);
void setStiffness(t_pianof *x, double stffn);
void setReverb(t_pianof *x, double rvrbgain);
void setRoom(t_pianof *x, double rmSize);
void setAngle(t_pianof *x, double panAngle);
void setWidth(t_pianof *x, double width);

void setDef(t_pianof *x);
void clearall(t_pianof *x);

void* pianof_new(t_symbol* s, short argc, t_atom* argv);
void pianof_free(t_pianof* x);

double sanitize_denormal(double v);

void ext_main(void *r) {

	t_class *c;
	c = class_new("pianof~", (method)pianof_new, (method)pianof_free, sizeof(t_pianof), 0L, A_GIMME, 0);

	class_addmethod(c, (method)pianof_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)setFreq, "freq", A_FLOAT, 0);
	class_addmethod(c, (method)playStart, "gate", A_FLOAT, 0);
	class_addmethod(c, (method)setGain, "gain", A_FLOAT, 0);
	class_addmethod(c, (method)setBrightness, "brightness", A_FLOAT, 0);
	class_addmethod(c, (method)setDetuning, "detuning", A_FLOAT, 0);
	class_addmethod(c, (method)setHammer, "hammer", A_FLOAT, 0);
	class_addmethod(c, (method)setStiffness, "stiffness", A_FLOAT, 0);
	class_addmethod(c, (method)setReverb, "reverbgn", A_FLOAT, 0);
	class_addmethod(c, (method)setRoom, "room", A_FLOAT, 0);
	class_addmethod(c, (method)setAngle, "angle", A_FLOAT, 0);
	class_addmethod(c, (method)setWidth, "width", A_FLOAT, 0);

	class_addmethod(c, (method)setDef, "setDefault", 0);
	class_addmethod(c, (method)clearall, "clearall", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	pianof_class = c;

	post("--------------------------------------------------------------------------");
	post("The piano physical model by Romain Michon");
	post("Max/MSP implementation with 64-bit perform routine: George Nikolopoulos - 2018 -");
	post("Default: 	gain = 1.0 brightness = 0.0 detuning = 0.1 hammer hardness = 0.1 stiffness = 0.28");
	post("reverb gain = 0.137 room size = 0.72 pan angle = 0.6 spatial width = 0.5 ");

	//return 0;
}

void* pianof_new(t_symbol* s, short argc, t_atom* argv)
{
	t_pianof* x = (t_pianof*)object_alloc(pianof_class);

	dsp_setup((t_pxobject *)x, 0);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	((t_pxobject*)x)->z_misc = Z_NO_INPLACE; // To assure input and output buffers are actually different
	x->fSamplingFreq = sys_getsr();

	x->fConst0 = min(192000.0, max(1.0, double(x->fSamplingFreq)));
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
	x->iConst11 = int(min(16384.0, max(0.0, (x->fConst2 - x->fConst10))));
	x->fConst12 = (1.0 / mydsp_faustpower2_f(x->fConst0));
	x->fConst13 = (0.159154937 * x->fConst0);
	x->fConst14 = (6.28318548 / x->fConst0);
	x->fConst15 = (7.0 / x->fConst0);
	x->fConst16 = exp((0.0 - (5.0 / x->fConst0)));
	x->fConst17 = (10.0 / x->fConst0);
	x->fConst18 = (0.100000001 * x->fConst0);
	x->fConst19 = exp((0.0 - (0.5 / x->fConst0)));
	x->fConst20 = (2.0 / x->fConst0);
	x->fConst21 = (0.0500000007 / x->fConst0);
	x->fConst22 = (0.5 * x->fConst0);
	x->iConst23 = int(min(8192.0, max(0.0, (0.0199999996 * x->fConst0))));
	x->iConst24 = int(min(1024.0, max(0.0, (x->fConst10 + -1.0))));
	x->fConst25 = floor(((0.256891012 * x->fConst0) + 0.5));
	x->fConst26 = ((0.0 - (6.90775537 * x->fConst25)) / x->fConst0);
	x->fConst27 = (0.5 * x->fConst26);
	x->fConst28 = (0.333333343 * x->fConst26);
	x->fConst29 = floor(((0.0273330007 * x->fConst0) + 0.5));
	x->iConst30 = int(min(16384.0, max(0.0, (x->fConst25 - x->fConst29))));
	x->iConst31 = int(min(2048.0, max(0.0, (x->fConst29 + -1.0))));
	x->fConst32 = floor(((0.192303002 * x->fConst0) + 0.5));
	x->fConst33 = ((0.0 - (6.90775537 * x->fConst32)) / x->fConst0);
	x->fConst34 = (0.5 * x->fConst33);
	x->fConst35 = (0.333333343 * x->fConst33);
	x->fConst36 = floor(((0.0292910002 * x->fConst0) + 0.5));
	x->iConst37 = int(min(8192.0, max(0.0, (x->fConst32 - x->fConst36))));
	x->iConst38 = int(min(2048.0, max(0.0, (x->fConst36 + -1.0))));
	x->fConst39 = floor(((0.210389003 * x->fConst0) + 0.5));
	x->fConst40 = ((0.0 - (6.90775537 * x->fConst39)) / x->fConst0);
	x->fConst41 = (0.5 * x->fConst40);
	x->fConst42 = (0.333333343 * x->fConst40);
	x->fConst43 = floor(((0.0244210009 * x->fConst0) + 0.5));
	x->iConst44 = int(min(16384.0, max(0.0, (x->fConst39 - x->fConst43))));
	x->iConst45 = int(min(2048.0, max(0.0, (x->fConst43 + -1.0))));
	x->fConst46 = floor(((0.125 * x->fConst0) + 0.5));
	x->fConst47 = ((0.0 - (6.90775537 * x->fConst46)) / x->fConst0);
	x->fConst48 = (0.5 * x->fConst47);
	x->fConst49 = (0.333333343 * x->fConst47);
	x->fConst50 = floor(((0.0134579996 * x->fConst0) + 0.5));
	x->iConst51 = int(min(8192.0, max(0.0, (x->fConst46 - x->fConst50))));
	x->iConst52 = int(min(1024.0, max(0.0, (x->fConst50 + -1.0))));
	x->fConst53 = floor(((0.127837002 * x->fConst0) + 0.5));
	x->fConst54 = ((0.0 - (6.90775537 * x->fConst53)) / x->fConst0);
	x->fConst55 = (0.5 * x->fConst54);
	x->fConst56 = (0.333333343 * x->fConst54);
	x->fConst57 = floor(((0.0316039994 * x->fConst0) + 0.5));
	x->iConst58 = int(min(8192.0, max(0.0, (x->fConst53 - x->fConst57))));
	x->iConst59 = int(min(2048.0, max(0.0, (x->fConst57 + -1.0))));
	x->fConst60 = floor(((0.174713001f * x->fConst0) + 0.5));
	x->fConst61 = ((0.0 - (6.90775537 * x->fConst60)) / x->fConst0);
	x->fConst62 = (0.5 * x->fConst61);
	x->fConst63 = (0.333333343 * x->fConst61);
	x->fConst64 = floor(((0.0229039993 * x->fConst0) + 0.5));
	x->iConst65 = int(min(8192.0, max(0.0, (x->fConst60 - x->fConst64))));
	x->iConst66 = int(min(2048.0, max(0.0, (x->fConst64 + -1.0))));
	x->fConst67 = floor(((0.153128996 * x->fConst0) + 0.5));
	x->fConst68 = ((0.0 - (6.90775537 * x->fConst67)) / x->fConst0);
	x->fConst69 = (0.5 * x->fConst68);
	x->fConst70 = (0.333333343 * x->fConst68);
	x->fConst71 = floor(((0.0203460008 * x->fConst0) + 0.5));
	x->iConst72 = int(min(8192.0, max(0.0, (x->fConst67 - x->fConst71))));
	x->iConst73 = int(min(1024.0, max(0.0, (x->fConst71 + -1.0))));

	x->fRec11 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec10 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec0 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptr(2 * sizeof(double));
	x->iRec26 = (int *)sysmem_newptr(2 * sizeof(int));
	x->fRec28 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec12 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec27 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec13 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec25 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec24 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec23 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec21 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec19 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec17 = (double *)sysmem_newptr(8192 * sizeof(double));
	x->fVec2 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec33 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec31 = (double *)sysmem_newptr(8192 * sizeof(double));
	x->fVec3 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec14 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec15 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec4 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec5 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec43 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec42 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec41 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec40 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec39 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec38 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec37 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec36 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec35 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec34 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec6 = (double *)sysmem_newptr(8192 * sizeof(double));
	x->fVec7 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fVec8 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec8 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec47 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec46 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec9 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fVec10 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec44 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec51 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec50 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec11 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec14 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec52 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec59 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec58 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec15 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec16 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fVec17 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec56 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec63 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec62 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec18 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec19 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec60 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec21 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec64 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec71 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec70 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec22 = (double *)sysmem_newptr(16384 * sizeof(double));
	x->fVec23 = (double *)sysmem_newptr(2048 * sizeof(double));
	x->fRec68 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec1 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec2 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec3 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec4 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec5 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec6 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fRec7 = (double *)sysmem_newptr(3 * sizeof(double));
	x->fVec12 = (double *)sysmem_newptr(4096 * sizeof(double));
	x->fRec48 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec55 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec54 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec13 = (double *)sysmem_newptr(32768 * sizeof(double));
	x->fRec67 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fRec66 = (double *)sysmem_newptr(2 * sizeof(double));
	x->fVec20 = (double *)sysmem_newptr(16384 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->fRec11[l0] = 0.0f;
	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec10[l1] = 0.0;
	}
	x->IOTA = 0;
	for (int l2 = 0; (l2 < 32768); l2 = (l2 + 1)) {
		x->fVec0[l2] = 0.0;
	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		x->fRec12[l3] = 0.0;
	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec20[l4] = 0.0;
	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		x->iRec26[l5] = 0;
	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		x->fRec28[l6] = 0.0;
	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fRec27[l7] = 0.0;
	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec29[l8] = 0.0;
	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		x->fRec25[l9] = 0.0;
	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		x->fRec24[l10] = 0.0;
	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec23[l11] = 0.0;
	}
	for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
		x->fRec22[l12] = 0.0;
	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		x->fRec21[l13] = 0.0;
	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->fVec1[l14] = 0.0;
	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		x->fRec19[l15] = 0.0;
	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec18[l16] = 0.0;
	}
	for (int l17 = 0; (l17 < 8192); l17 = (l17 + 1)) {
		x->fRec17[l17] = 0.0;
	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		x->fVec2[l18] = 0.0;
	}
	for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
		x->fRec33[l19] = 0.0;
	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		x->fRec32[l20] = 0.0;
	}
	for (int l21 = 0; (l21 < 8192); l21 = (l21 + 1)) {
		x->fRec31[l21] = 0.0;
	}
	for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
		x->fVec3[l22] = 0.0;
	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		x->fRec30[l23] = 0.0;
	}
	for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
		x->fRec14[l24] = 0.0;
	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		x->fRec15[l25] = 0.0;
	}
	for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
		x->fRec13[l26] = 0.0;
	}
	for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
		x->fVec4[l27] = 0.0;
	}
	for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
		x->fVec5[l28] = 0.0;
	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		x->fRec43[l29] = 0.0;
	}
	for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
		x->fRec42[l30] = 0.0;
	}
	for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
		x->fRec41[l31] = 0.0;
	}
	for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
		x->fRec40[l32] = 0.0;
	}
	for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
		x->fRec39[l33] = 0.0;
	}
	for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
		x->fRec38[l34] = 0.0;
	}
	for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) {
		x->fRec37[l35] = 0.0;
	}
	for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
		x->fRec36[l36] = 0.0;
	}
	for (int l37 = 0; (l37 < 3); l37 = (l37 + 1)) {
		x->fRec35[l37] = 0.0;
	}
	for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
		x->fRec34[l38] = 0.0;
	}
	for (int l39 = 0; (l39 < 8192); l39 = (l39 + 1)) {
		x->fVec6[l39] = 0.0;
	}
	for (int l40 = 0; (l40 < 4096); l40 = (l40 + 1)) {
		x->fVec7[l40] = 0.0;
	}
	for (int l41 = 0; (l41 < 2048); l41 = (l41 + 1)) {
		x->fVec8[l41] = 0.0;
	}
	for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
		x->fRec8[l42] = 0.0;
	}
	for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
		x->fRec47[l43] = 0.0;
	}
	for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
		x->fRec46[l44] = 0.0;
	}
	for (int l45 = 0; (l45 < 32768); l45 = (l45 + 1)) {
		x->fVec9[l45] = 0.0;
	}
	for (int l46 = 0; (l46 < 4096); l46 = (l46 + 1)) {
		x->fVec10[l46] = 0.0;
	}
	for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
		x->fRec44[l47] = 0.0;
	}
	for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
		x->fRec51[l48] = 0.0;
	}
	for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
		x->fRec50[l49] = 0.0;
	}
	for (int l50 = 0; (l50 < 16384); l50 = (l50 + 1)) {
		x->fVec11[l50] = 0.0;
	}
	for (int l51 = 0; (l51 < 4096); l51 = (l51 + 1)) {
		x->fVec12[l51] = 0.0;
	}
	for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
		x->fRec48[l52] = 0.0;
	}
	for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
		x->fRec55[l53] = 0.0;
	}
	for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
		x->fRec54[l54] = 0.0;
	}
	for (int l55 = 0; (l55 < 32768); l55 = (l55 + 1)) {
		x->fVec13[l55] = 0.0;
	}
	for (int l56 = 0; (l56 < 4096); l56 = (l56 + 1)) {
		x->fVec14[l56] = 0.0;
	}
	for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
		x->fRec52[l57] = 0.0;
	}
	for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
		x->fRec59[l58] = 0.0;
	}
	for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
		x->fRec58[l59] = 0.0;
	}
	for (int l60 = 0; (l60 < 16384); l60 = (l60 + 1)) {
		x->fVec15[l60] = 0.0;
	}
	for (int l61 = 0; (l61 < 4096); l61 = (l61 + 1)) {
		x->fVec16[l61] = 0.0;
	}
	for (int l62 = 0; (l62 < 2048); l62 = (l62 + 1)) {
		x->fVec17[l62] = 0.0;
	}
	for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
		x->fRec56[l63] = 0.0;
	}
	for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
		x->fRec63[l64] = 0.0;
	}
	for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
		x->fRec62[l65] = 0.0;
	}
	for (int l66 = 0; (l66 < 16384); l66 = (l66 + 1)) {
		x->fVec18[l66] = 0.0;
	}
	for (int l67 = 0; (l67 < 4096); l67 = (l67 + 1)) {
		x->fVec19[l67] = 0.0;
	}
	for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
		x->fRec60[l68] = 0.0;
	}
	for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
		x->fRec67[l69] = 0.0;
	}
	for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
		x->fRec66[l70] = 0.0;
	}
	for (int l71 = 0; (l71 < 16384); l71 = (l71 + 1)) {
		x->fVec20[l71] = 0.0;
	}
	for (int l72 = 0; (l72 < 4096); l72 = (l72 + 1)) {
		x->fVec21[l72] = 0.0;
	}
	for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
		x->fRec64[l73] = 0.0;
	}
	for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
		x->fRec71[l74] = 0.0;
	}
	for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
		x->fRec70[l75] = 0.0;
	}
	for (int l76 = 0; (l76 < 16384); l76 = (l76 + 1)) {
		x->fVec22[l76] = 0.0;
	}
	for (int l77 = 0; (l77 < 2048); l77 = (l77 + 1)) {
		x->fVec23[l77] = 0.0;
	}
	for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
		x->fRec68[l78] = 0.0;
	}
	for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
		x->fRec0[l79] = 0.0;
	}
	for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
		x->fRec1[l80] = 0.0;
	}
	for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
		x->fRec2[l81] = 0.0;
	}
	for (int l82 = 0; (l82 < 3); l82 = (l82 + 1)) {
		x->fRec3[l82] = 0.0;
	}
	for (int l83 = 0; (l83 < 3); l83 = (l83 + 1)) {
		x->fRec4[l83] = 0.0;
	}
	for (int l84 = 0; (l84 < 3); l84 = (l84 + 1)) {
		x->fRec5[l84] = 0.0;
	}
	for (int l85 = 0; (l85 < 3); l85 = (l85 + 1)) {
		x->fRec6[l85] = 0.0;
	}
	for (int l86 = 0; (l86 < 3); l86 = (l86 + 1)) {
		x->fRec7[l86] = 0.0;
	}

	x->freq = 440.0;
	x->gain = 1.0;
	x->gate = 0.0;
	x->brght_f = 0.0;
	x->dtng_f = 0.1;
	x->hmmr_hrd = 0.1;
	x->stffn_f = 0.28;

	x->rvrb_gain = 0.137;
	x->roomsize = 0.72;
	x->pan_angle = 0.6;
	x->spat_width = 0.5;

	return x;
}

void pianof_dsp64(t_pianof *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->fSamplingFreq != samplerate) x->fSamplingFreq = samplerate;
	object_method(dsp64, gensym("dsp_add64"), x,
		pianof_perform64, 0, NULL);
}

void pianof_perform64(t_pianof *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	//t_double *input = ins[0];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	int n = vectorsize;
	
	double fSlow0 = double(x->roomsize);
	double fSlow1 = exp((x->fConst4 / fSlow0));
	double fSlow2 = mydsp_faustpower2_f(fSlow1);
	double fSlow3 = (1.0 - (x->fConst1 * fSlow2));
	double fSlow4 = (1.0 - fSlow2);
	double fSlow5 = (fSlow3 / fSlow4);
	double fSlow6 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow3) / mydsp_faustpower2_f(fSlow4)) + -1.0)));
	double fSlow7 = (fSlow5 - fSlow6);
	double fSlow8 = ((fSlow6 + (1.0 - fSlow5)) * fSlow1);
	double fSlow9 = ((exp((x->fConst5 / fSlow0)) / fSlow1) + -1.0f);
	double fSlow10 = double(x->pan_angle);
	double fSlow11 = (12.0 * fSlow10);
	double fSlow12 = (0.00100000005 * double(x->rvrb_gain));
	double fSlow13 = double(x->freq);
	int iSlow14 = int(((17.3123398 * (log(fSlow13) + -6.08677483)) + 69.5));
	double fSlow15 = double(getValueEQBandWidthFactor(double(iSlow14)));
	double fSlow16 = (x->fConst12 * (mydsp_faustpower2_f(fSlow13) * mydsp_faustpower2_f(fSlow15)));
	double fSlow17 = (0.5 * (1.0 - fSlow16));
	double fSlow18 = (0.0 - fSlow17);
	double fSlow19 = double(getValueEQGain(double(iSlow14)));
	double fSlow20 = double(x->stffn_f);
	double fSlow21 = double(getValueStiffnessCoefficient(double(iSlow14)));
	double fSlow22 = (13.6899996 * (mydsp_faustpower2_f(fSlow20) * mydsp_faustpower2_f(fSlow21)));
	double fSlow23 = (fSlow22 + -1.0);
	double fSlow24 = (5.0 * (double(x->dtng_f) * double(getValueDetuningHz(double(iSlow14)))));
	double fSlow25 = (fSlow13 + fSlow24);
	double fSlow26 = (x->fConst14 * fSlow25);
	double fSlow27 = sin(fSlow26);
	double fSlow28 = (fSlow20 * fSlow21);
	double fSlow29 = (7.4000001 * fSlow28);
	double fSlow30 = (fSlow22 + 1.0);
	double fSlow31 = cos(fSlow26);
	double fSlow32 = (3.0 * atan2((fSlow23 * fSlow27), (fSlow29 + (fSlow30 * fSlow31))));
	double fSlow33 = double(getValueSingleStringPole(double(iSlow14)));
	double fSlow34 = (pow(10.0, (0.0500000007 * (double(getValueSingleStringDecayRate(double(iSlow14))) / fSlow13))) * (1.0 - fSlow33));
	double fSlow35 = double(getValueSingleStringZero(double(iSlow14)));
	double fSlow36 = (fSlow34 * fSlow35);
	double fSlow37 = (1.0 - fSlow35);
	double fSlow38 = (fSlow33 * fSlow37);
	double fSlow39 = (3.0 * fSlow38);
	double fSlow40 = (fSlow36 - fSlow39);
	double fSlow41 = (fSlow38 - fSlow36);
	double fSlow42 = (4.0 * fSlow41);
	double fSlow43 = (fSlow40 + fSlow42);
	double fSlow44 = ((3.0 * fSlow37) - fSlow34);
	double fSlow45 = (0.0 - (fSlow43 / fSlow44));
	double fSlow46 = (((fSlow40 * fSlow31) / fSlow44) + 1.0);
	double fSlow47 = ((fSlow34 + fSlow35) + -1.0);
	double fSlow48 = (4.0 * fSlow47);
	double fSlow49 = (((fSlow48 + (fSlow43 * fSlow31)) / fSlow44) + 1.0);
	double fSlow50 = (fSlow40 * fSlow43);
	double fSlow51 = mydsp_faustpower2_f(fSlow27);
	double fSlow52 = mydsp_faustpower2_f(fSlow44);
	double fSlow53 = (x->fConst13 * (((fSlow32 + atan2((fSlow27 * ((fSlow45 * fSlow46) + ((fSlow40 * fSlow49) / fSlow44))), ((fSlow46 * fSlow49) + ((fSlow50 * fSlow51) / fSlow52)))) + 6.28318548f) / fSlow25));
	double fSlow54 = floor(fSlow53);
	double fSlow55 = (fSlow53 - fSlow54);
	double fSlow56 = double(x->gate);
	double fSlow57 = (0.0 - (fSlow56 + -1.0));
	double fSlow58 = (0.00100000005 * ((0.999599993 * fSlow56) + (0.899999976 * (fSlow57 * double(getValueReleaseLoopGain(double(iSlow14)))))));
	double fSlow59 = double(getValueDCBa1(double(iSlow14)));
	double fSlow60 = (1.0 - fSlow59);
	double fSlow61 = (0.5 * fSlow60);
	double fSlow62 = (0.25 * double(x->brght_f));
	double fSlow63 = double(getValueLoudPole(double(iSlow14)));
	double fSlow64 = (fSlow62 + (0.980000019 - fSlow63));
	double fSlow65 = double(getValueLoudGain(double(iSlow14)));
	double fSlow66 = (fSlow64 * fSlow65);
	double fSlow67 = (1.39698386e-09 * ((double((iSlow14 < 88)) * fSlow64) * fSlow65));
	int iSlow68 = (fSlow56 > 0.0);
	double fSlow69 = exp((0.0 - (x->fConst15 / (double(x->gain) * double(getValueDryTapAmpT60(double(iSlow14)))))));
	int iSlow70 = (fSlow56 < 1.0);
	double fSlow71 = (x->fConst16 * fSlow57);
	double fSlow72 = double(x->hmmr_hrd);
	double fSlow73 = exp((0.0 - (x->fConst17 / fSlow72)));
	double fSlow74 = (x->fConst18 * fSlow72);
	double fSlow75 = (0.200000003 * double(getValueSustainPedalLevel(double(iSlow14))));
	double fSlow76 = (fSlow62 + (-0.0199999996 - fSlow63));
	double fSlow77 = (0.0 - fSlow61);
	double fSlow78 = (3.70000005 * fSlow28);
	double fSlow79 = ((fSlow36 + fSlow42) - fSlow39);
	double fSlow80 = (0.0 - (fSlow79 / fSlow44));
	double fSlow81 = ((((fSlow79 * fSlow31) + fSlow48) / fSlow44) + 1.0);
	double fSlow82 = (fSlow79 * fSlow40);
	int iSlow83 = int((x->fConst13 * (((atan2((((fSlow80 * fSlow46) + ((fSlow81 * fSlow40) / fSlow44)) * fSlow27), ((fSlow81 * fSlow46) + ((fSlow82 * fSlow51) / fSlow52))) + fSlow32) + 6.28318548) / fSlow25)));
	int iSlow84 = min(4097, max(0, (iSlow83 + 1)));
	double fSlow85 = (1.0 / fSlow44);
	double fSlow86 = (fSlow13 - fSlow24);
	double fSlow87 = (x->fConst14 * fSlow86);
	double fSlow88 = sin(fSlow87);
	double fSlow89 = cos(fSlow87);
	double fSlow90 = (3.0 * atan2((fSlow23 * fSlow88), (fSlow29 + (fSlow30 * fSlow89))));
	double fSlow91 = (((fSlow40 * fSlow89) / fSlow44) + 1.0);
	double fSlow92 = ((((fSlow43 * fSlow89) + fSlow48) / fSlow44) + 1.0);
	double fSlow93 = mydsp_faustpower2_f(fSlow88);
	double fSlow94 = (x->fConst13 * (((fSlow90 + atan2((fSlow88 * ((fSlow45 * fSlow91) + ((fSlow40 * fSlow92) / fSlow44))), ((fSlow91 * fSlow92) + ((fSlow50 * fSlow93) / fSlow52)))) + 6.28318548f) / fSlow86));
	double fSlow95 = floor(fSlow94);
	double fSlow96 = (fSlow95 + (1.0f - fSlow94));
	double fSlow97 = ((((fSlow79 * fSlow89) + fSlow48) / fSlow44) + 1.0f);
	int iSlow98 = int((x->fConst13 * (((atan2((((fSlow80 * fSlow91) + ((fSlow97 * fSlow40) / fSlow44)) * fSlow88), ((fSlow97 * fSlow91) + ((fSlow82 * fSlow93) / fSlow52))) + fSlow90) + 6.28318548f) / fSlow86)));
	int iSlow99 = min(4097, max(0, iSlow98));
	double fSlow100 = (fSlow94 - fSlow95);
	int iSlow101 = min(4097, max(0, (iSlow98 + 1)));
	double fSlow102 = (fSlow54 + (1.0 - fSlow53));
	int iSlow103 = min(4097, max(0, iSlow83));
	double fSlow104 = ((0.0 - (x->fConst20 * (fSlow13 * fSlow15))) * cos((x->fConst14 * (fSlow13 / double(getValueStrikePosition(double(iSlow14)))))));
	double fSlow105 = cos((x->fConst14 * fSlow13));
	double fSlow106 = pow(10.0, (0.0500000007 * double(getValueSecondStageAmpRatio(double(iSlow14)))));
	double fSlow107 = pow(10.0, (x->fConst21 * double(getValuer1_1db(double(iSlow14)))));
	double fSlow108 = pow(10.0, (x->fConst21 * double(getValuer1_2db(double(iSlow14)))));
	double fSlow109 = (1.0 - fSlow106);
	double fSlow110 = (0.0 - (2.0 * ((fSlow106 * fSlow107) + (fSlow108 * fSlow109))));
	double fSlow111 = double(getValueBq4_gEarBalled(double(iSlow14)));
	double fSlow112 = (2.0 * fSlow111);
	int iSlow113 = (iSlow14 >= 88);
	double fSlow114 = (2.32830644e-10 * double(iSlow113));
	double fSlow115 = (1.16415322e-10 * double(iSlow113));
	double fSlow116 = pow(10.0, (x->fConst21 * double(getValuer3db(double(iSlow14)))));
	double fSlow117 = ((0.0 - (2.0 * fSlow116)) * cos((x->fConst14 * (fSlow13 * double(getValueThirdPartialFactor(double(iSlow14)))))));
	double fSlow118 = mydsp_faustpower2_f(fSlow116);
	double fSlow119 = pow(10.0, (x->fConst21 * double(getValuer2db(double(iSlow14)))));
	double fSlow120 = ((0.0 - (2.0 * fSlow119)) * cos((x->fConst14 * (fSlow13 * double(getValueSecondPartialFactor(double(iSlow14)))))));
	double fSlow121 = mydsp_faustpower2_f(fSlow119);
	double fSlow122 = (fSlow105 * (0.0 - (2.0 * fSlow107)));
	double fSlow123 = mydsp_faustpower2_f(fSlow107);
	double fSlow124 = (0.0 - (2.0 * fSlow108));
	double fSlow125 = mydsp_faustpower2_f(fSlow108);
	double fSlow126 = ((fSlow106 * fSlow123) + (fSlow125 * fSlow109));
	int iSlow127 = int(min(4096.0, max(0.0f, (x->fConst22 * (double(x->spat_width) / fSlow13)))));
	double fSlow128 = exp((x->fConst27 / fSlow0));
	double fSlow129 = mydsp_faustpower2_f(fSlow128);
	double fSlow130 = (1.0 - (x->fConst1 * fSlow129));
	double fSlow131 = (1.0 - fSlow129);
	double fSlow132 = (fSlow130 / fSlow131);
	double fSlow133 = sqrt(max(0.0f, ((mydsp_faustpower2_f(fSlow130) / mydsp_faustpower2_f(fSlow131)) + -1.0)));
	double fSlow134 = (fSlow132 - fSlow133);
	double fSlow135 = ((fSlow133 + (1.0 - fSlow132)) * fSlow128);
	double fSlow136 = ((exp((x->fConst28 / fSlow0)) / fSlow128) + -1.0);
	double fSlow137 = exp((x->fConst34 / fSlow0));
	double fSlow138 = mydsp_faustpower2_f(fSlow137);
	double fSlow139 = (1.0 - (x->fConst1 * fSlow138));
	double fSlow140 = (1.0 - fSlow138);
	double fSlow141 = (fSlow139 / fSlow140);
	double fSlow142 = sqrt(max(0.0f, ((mydsp_faustpower2_f(fSlow139) / mydsp_faustpower2_f(fSlow140)) + -1.0)));
	double fSlow143 = (fSlow141 - fSlow142);
	double fSlow144 = ((fSlow142 + (1.0 - fSlow141)) * fSlow137);
	double fSlow145 = ((exp((x->fConst35 / fSlow0)) / fSlow137) + -1.0);
	double fSlow146 = exp((x->fConst41 / fSlow0));
	double fSlow147 = mydsp_faustpower2_f(fSlow146);
	double fSlow148 = (1.0 - (x->fConst1 * fSlow147));
	double fSlow149 = (1.0 - fSlow147);
	double fSlow150 = (fSlow148 / fSlow149);
	double fSlow151 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow148) / mydsp_faustpower2_f(fSlow149)) + -1.0)));
	double fSlow152 = (fSlow150 - fSlow151);
	double fSlow153 = ((fSlow151 + (1.0 - fSlow150)) * fSlow146);
	double fSlow154 = ((exp((x->fConst42 / fSlow0)) / fSlow146) + -1.0);
	double fSlow155 = exp((x->fConst48 / fSlow0));
	double fSlow156 = mydsp_faustpower2_f(fSlow155);
	double fSlow157 = (1.0 - (x->fConst1 * fSlow156));
	double fSlow158 = (1.0 - fSlow156);
	double fSlow159 = (fSlow157 / fSlow158);
	double fSlow160 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow157) / mydsp_faustpower2_f(fSlow158)) + -1.0)));
	double fSlow161 = (fSlow159 - fSlow160);
	double fSlow162 = ((fSlow160 + (1.0 - fSlow159)) * fSlow155);
	double fSlow163 = ((exp((x->fConst49 / fSlow0)) / fSlow155) + -1.0);
	double fSlow164 = (12.0 * (1.0 - fSlow10));
	double fSlow165 = exp((x->fConst55 / fSlow0));
	double fSlow166 = mydsp_faustpower2_f(fSlow165);
	double fSlow167 = (1.0 - (x->fConst1 * fSlow166));
	double fSlow168 = (1.0 - fSlow166);
	double fSlow169 = (fSlow167 / fSlow168);
	double fSlow170 = sqrt(max(0.0f, ((mydsp_faustpower2_f(fSlow167) / mydsp_faustpower2_f(fSlow168)) + -1.0)));
	double fSlow171 = (fSlow169 - fSlow170);
	double fSlow172 = ((fSlow170 + (1.0 - fSlow169)) * fSlow165);
	double fSlow173 = ((exp((x->fConst56 / fSlow0)) / fSlow165) + -1.0f);
	double fSlow174 = exp((x->fConst62 / fSlow0));
	double fSlow175 = mydsp_faustpower2_f(fSlow174);
	double fSlow176 = (1.0 - (x->fConst1 * fSlow175));
	double fSlow177 = (1.0 - fSlow175);
	double fSlow178 = (fSlow176 / fSlow177);
	double fSlow179 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow176) / mydsp_faustpower2_f(fSlow177)) + -1.0)));
	double fSlow180 = (fSlow178 - fSlow179);
	double fSlow181 = ((fSlow179 + (1.0 - fSlow178)) * fSlow174);
	double fSlow182 = ((exp((x->fConst63 / fSlow0)) / fSlow174) + -1.0);
	double fSlow183 = exp((x->fConst69 / fSlow0));
	double fSlow184 = mydsp_faustpower2_f(fSlow183);
	double fSlow185 = (1.0 - (x->fConst1 * fSlow184));
	double fSlow186 = (1.0 - fSlow184);
	double fSlow187 = (fSlow185 / fSlow186);
	double fSlow188 = sqrt(max(0.0, ((mydsp_faustpower2_f(fSlow185) / mydsp_faustpower2_f(fSlow186)) + -1.0)));
	double fSlow189 = (fSlow187 - fSlow188);
	double fSlow190 = ((fSlow188 + (1.0f - fSlow187)) * fSlow183);
	double fSlow191 = ((exp((x->fConst70 / fSlow0)) / fSlow183) + -1.0);
	//for (int i = 0; (i < count); i = (i + 1)) {
	while (n--) {
		x->fRec11[0] = ((x->fConst8 * x->fRec11[1]) + (x->fConst9 * (x->fRec7[1] + x->fRec7[2])));
		x->fRec10[0] = ((fSlow7 * x->fRec10[1]) + (fSlow8 * (x->fRec7[1] + (fSlow9 * x->fRec11[0]))));
		x->fVec0[(x->IOTA & 32767)] = ((0.353553385 * x->fRec10[0]) + 9.99999968e-21);
		x->fRec12[0] = (fSlow12 + (0.999000013 * x->fRec12[1]));
		x->fRec20[0] = (fSlow58 + (0.999000013 * x->fRec20[1]));
		x->iRec26[0] = ((1103515245 * x->iRec26[1]) + 12345);
		x->fRec28[0] = ((fSlow56 * x->fRec28[1]) + 1.0);
		double fTemp0 = (x->fRec28[0] + -1.0);
		int iTemp1 = ((fTemp0 < 2.0) & iSlow68);
		double fTemp2 = ((0.0301973838 * double(iTemp1)) + (fSlow69 * double(((fTemp0 >= 2.0) | iSlow70))));
		x->fRec27[0] = ((x->fRec27[1] * fTemp2) + (0.150000006 * (double(iTemp1) * (1.0 - fTemp2))));
		int iTemp3 = (fTemp0 < fSlow74);
		double fTemp4 = (fSlow56 * ((fSlow73 * double(iTemp3)) + (x->fConst19 * double((fTemp0 >= fSlow74)))));
		x->fRec29[0] = ((x->fRec29[1] * (fSlow71 + fTemp4)) + (fSlow75 * (((1.0f - fTemp4) - fSlow71) * double((iTemp3 & iSlow68)))));
		double fTemp5 = (double(x->iRec26[0]) * (x->fRec27[0] + x->fRec29[0]));
		x->fRec25[0] = ((fSlow67 * fTemp5) - (fSlow76 * x->fRec25[1]));
		x->fRec24[0] = ((fSlow66 * x->fRec25[0]) - (fSlow76 * x->fRec24[1]));
		x->fRec23[0] = ((fSlow66 * x->fRec24[0]) - (fSlow76 * x->fRec23[1]));
		x->fRec22[0] = ((fSlow66 * x->fRec23[0]) - (fSlow76 * x->fRec22[1]));
		x->fRec21[0] = (((fSlow61 * x->fRec22[0]) + (fSlow77 * x->fRec22[1])) - (fSlow59 * x->fRec21[1]));
		double fTemp6 = (x->fRec20[0] * (x->fRec21[0] + x->fRec14[1]));
		x->fVec1[0] = fTemp6;
		x->fRec19[0] = (x->fVec1[1] + (fSlow78 * (fTemp6 - x->fRec19[1])));
		x->fRec18[0] = (x->fRec19[1] + (fSlow78 * (x->fRec19[0] - x->fRec18[1])));
		x->fRec17[(x->IOTA & 8191)] = (x->fRec18[1] + (fSlow78 * (x->fRec18[0] - x->fRec17[((x->IOTA - 1) & 8191)])));
		double fTemp7 = (fSlow55 * x->fRec17[((x->IOTA - iSlow84) & 8191)]);
		double fTemp8 = (x->fRec21[0] + (x->fRec20[0] * x->fRec15[1]));
		x->fVec2[0] = fTemp8;
		x->fRec33[0] = (x->fVec2[1] + (fSlow78 * (fTemp8 - x->fRec33[1])));
		x->fRec32[0] = (x->fRec33[1] + (fSlow78 * (x->fRec33[0] - x->fRec32[1])));
		x->fRec31[(x->IOTA & 8191)] = (x->fRec32[1] + (fSlow78 * (x->fRec32[0] - x->fRec31[((x->IOTA - 1) & 8191)])));
		double fTemp9 = (fSlow96 * x->fRec31[((x->IOTA - iSlow99) & 8191)]);
		double fTemp10 = (fSlow100 * x->fRec31[((x->IOTA - iSlow101) & 8191)]);
		double fTemp11 = (fSlow102 * x->fRec17[((x->IOTA - iSlow103) & 8191)]);
		double fTemp12 = (fTemp7 + ((fTemp9 + fTemp10) + fTemp11));
		x->fVec3[0] = fTemp12;
		x->fRec30[0] = (fSlow85 * ((2.0 * ((fSlow47 * fTemp12) + (fSlow41 * x->fVec3[1]))) - (fSlow40 * x->fRec30[1])));
		x->fRec14[0] = (fTemp7 + (x->fRec30[0] + fTemp11));
		x->fRec15[0] = (fTemp10 + (x->fRec30[0] + fTemp9));
		double fRec16 = fTemp12;
		x->fRec13[0] = ((fSlow19 * fRec16) - ((fSlow104 * x->fRec13[1]) + (fSlow16 * x->fRec13[2])));
		x->fVec4[0] = (fSlow114 * fTemp5);
		double fTemp13 = (0.0 - ((0.5 * x->fVec4[1]) + (fSlow115 * fTemp5)));
		x->fVec5[0] = fTemp13;
		x->fRec43[0] = (((fSlow61 * fTemp13) + (fSlow77 * x->fVec5[1])) - (fSlow59 * x->fRec43[1]));
		x->fRec42[0] = ((fSlow66 * x->fRec43[0]) - (fSlow76 * x->fRec42[1]));
		x->fRec41[0] = ((fSlow66 * x->fRec42[0]) - (fSlow76 * x->fRec41[1]));
		x->fRec40[0] = ((fSlow66 * x->fRec41[0]) - (fSlow76 * x->fRec40[1]));
		x->fRec39[0] = ((fSlow66 * x->fRec40[0]) - (fSlow76 * x->fRec39[1]));
		x->fRec38[0] = ((fSlow111 * (x->fRec39[0] - x->fRec39[1])) - ((fSlow117 * x->fRec38[1]) + (fSlow118 * x->fRec38[2])));
		x->fRec37[0] = ((fSlow112 * x->fRec38[0]) - ((fSlow120 * x->fRec37[1]) + (fSlow121 * x->fRec37[2])));
		x->fRec36[0] = (x->fRec37[0] - ((fSlow122 * x->fRec36[1]) + (fSlow123 * x->fRec36[2])));
		x->fRec35[0] = (((fSlow105 * ((fSlow110 * x->fRec36[1]) - (fSlow124 * x->fRec35[1]))) + (x->fRec36[0] + (fSlow126 * x->fRec36[2]))) - (fSlow125 * x->fRec35[2]));
		x->fRec34[0] = ((fSlow60 * x->fRec35[0]) - (fSlow59 * x->fRec34[1]));
		double fTemp14 = ((fSlow18 * x->fRec13[2]) + ((fSlow17 * x->fRec13[0]) + (fRec16 + x->fRec34[0])));
		x->fVec6[(x->IOTA & 8191)] = fTemp14;
		x->fVec7[(x->IOTA & 4095)] = (fSlow11 * (x->fRec12[0] * x->fVec6[((x->IOTA - iSlow127) & 8191)]));
		double fTemp15 = (0.300000012 * x->fVec7[((x->IOTA - x->iConst23) & 4095)]);
		double fTemp16 = (((0.600000024 * x->fRec8[1]) + x->fVec0[((x->IOTA - x->iConst11) & 32767)]) - fTemp15);
		x->fVec8[(x->IOTA & 2047)] = fTemp16;
		x->fRec8[0] = x->fVec8[((x->IOTA - x->iConst24) & 2047)];
		double fRec9 = (0.0 - (0.600000024 * fTemp16));
		x->fRec47[0] = ((x->fConst8 * x->fRec47[1]) + (x->fConst9 * (x->fRec3[1] + x->fRec3[2])));
		x->fRec46[0] = ((fSlow134 * x->fRec46[1]) + (fSlow135 * (x->fRec3[1] + (fSlow136 * x->fRec47[0]))));
		x->fVec9[(x->IOTA & 32767)] = ((0.353553385 * x->fRec46[0]) + 9.99999968e-21);
		double fTemp17 = (((0.600000024 * x->fRec44[1]) + x->fVec9[((x->IOTA - x->iConst30) & 32767)]) - fTemp15);
		x->fVec10[(x->IOTA & 4095)] = fTemp17;
		x->fRec44[0] = x->fVec10[((x->IOTA - x->iConst31) & 4095)];
		double fRec45 = (0.0 - (0.600000024 * fTemp17));
		x->fRec51[0] = ((x->fConst8 * x->fRec51[1]) + (x->fConst9 * (x->fRec5[1] + x->fRec5[2])));
		x->fRec50[0] = ((fSlow143 * x->fRec50[1]) + (fSlow144 * (x->fRec5[1] + (fSlow145 * x->fRec51[0]))));
		x->fVec11[(x->IOTA & 16383)] = ((0.353553385 * x->fRec50[0]) + 9.99999968e-21);
		double fTemp18 = (x->fVec11[((x->IOTA - x->iConst37) & 16383)] + (fTemp15 + (0.600000024 * x->fRec48[1])));
		x->fVec12[(x->IOTA & 4095)] = fTemp18;
		x->fRec48[0] = x->fVec12[((x->IOTA - x->iConst38) & 4095)];
		double fRec49 = (0.0 - (0.600000024 * fTemp18));
		x->fRec55[0] = ((x->fConst8 * x->fRec55[1]) + (x->fConst9 * (x->fRec1[1] + x->fRec1[2])));
		x->fRec54[0] = ((fSlow152 * x->fRec54[1]) + (fSlow153 * (x->fRec1[1] + (fSlow154 * x->fRec55[0]))));
		x->fVec13[(x->IOTA & 32767)] = ((0.353553385 * x->fRec54[0]) + 9.99999968e-21);
		double fTemp19 = (fTemp15 + ((0.600000024 * x->fRec52[1]) + x->fVec13[((x->IOTA - x->iConst44) & 32767)]));
		x->fVec14[(x->IOTA & 4095)] = fTemp19;
		x->fRec52[0] = x->fVec14[((x->IOTA - x->iConst45) & 4095)];
		double fRec53 = (0.0 - (0.600000024 * fTemp19));
		x->fRec59[0] = ((x->fConst8 * x->fRec59[1]) + (x->fConst9 * (x->fRec6[1] + x->fRec6[2])));
		x->fRec58[0] = ((fSlow161 * x->fRec58[1]) + (fSlow162 * (x->fRec6[1] + (fSlow163 * x->fRec59[0]))));
		x->fVec15[(x->IOTA & 16383)] = ((0.353553385 * x->fRec58[0]) + 9.99999968e-21);
		x->fVec16[(x->IOTA & 4095)] = (fSlow164 * (x->fRec12[0] * fTemp14));
		double fTemp20 = (0.300000012 * x->fVec16[((x->IOTA - x->iConst23) & 4095)]);
		double fTemp21 = (x->fVec15[((x->IOTA - x->iConst51) & 16383)] - (fTemp20 + (0.600000024 * x->fRec56[1])));
		x->fVec17[(x->IOTA & 2047)] = fTemp21;
		x->fRec56[0] = x->fVec17[((x->IOTA - x->iConst52) & 2047)];
		double fRec57 = (0.600000024 * fTemp21);
		x->fRec63[0] = ((x->fConst8 * x->fRec63[1]) + (x->fConst9 * (x->fRec2[1] + x->fRec2[2])));
		x->fRec62[0] = ((fSlow171 * x->fRec62[1]) + (fSlow172 * (x->fRec2[1] + (fSlow173 * x->fRec63[0]))));
		x->fVec18[(x->IOTA & 16383)] = ((0.353553385 * x->fRec62[0]) + 9.99999968e-21);
		double fTemp22 = (x->fVec18[((x->IOTA - x->iConst58) & 16383)] - (fTemp20 + (0.600000024 * x->fRec60[1])));
		x->fVec19[(x->IOTA & 4095)] = fTemp22;
		x->fRec60[0] = x->fVec19[((x->IOTA - x->iConst59) & 4095)];
		double fRec61 = (0.600000024 * fTemp22);
		x->fRec67[0] = ((x->fConst8 * x->fRec67[1]) + (x->fConst9 * (x->fRec4[1] + x->fRec4[2])));
		x->fRec66[0] = ((fSlow180 * x->fRec66[1]) + (fSlow181 * (x->fRec4[1] + (fSlow182 * x->fRec67[0]))));
		x->fVec20[(x->IOTA & 16383)] = ((0.353553385 * x->fRec66[0]) + 9.99999968e-21);
		double fTemp23 = ((fTemp20 + x->fVec20[((x->IOTA - x->iConst65) & 16383)]) - (0.600000024 * x->fRec64[1]));
		x->fVec21[(x->IOTA & 4095)] = fTemp23;
		x->fRec64[0] = x->fVec21[((x->IOTA - x->iConst66) & 4095)];
		double fRec65 = (0.600000024 * fTemp23);
		x->fRec71[0] = ((x->fConst8 * x->fRec71[1]) + (x->fConst9 * (x->fRec0[1] + x->fRec0[2])));
		x->fRec70[0] = ((fSlow189 * x->fRec70[1]) + (fSlow190 * (x->fRec0[1] + (fSlow191 * x->fRec71[0]))));
		x->fVec22[(x->IOTA & 16383)] = ((0.353553385 * x->fRec70[0]) + 9.99999968e-21);
		double fTemp24 = ((x->fVec22[((x->IOTA - x->iConst72) & 16383)] + fTemp20) - (0.600000024 * x->fRec68[1]));
		x->fVec23[(x->IOTA & 2047)] = fTemp24;
		x->fRec68[0] = x->fVec23[((x->IOTA - x->iConst73) & 2047)];
		double fRec69 = (0.600000024 * fTemp24);
		double fTemp25 = (fRec69 + fRec65);
		double fTemp26 = (fRec57 + (fRec61 + fTemp25));
		x->fRec0[0] = (x->fRec8[1] + (x->fRec44[1] + (x->fRec48[1] + (x->fRec52[1] + (x->fRec56[1] + (x->fRec60[1] + (x->fRec64[1] + (x->fRec68[1] + (fRec9 + (fRec45 + (fRec49 + (fRec53 + fTemp26))))))))))));
		x->fRec1[0] = ((x->fRec56[1] + (x->fRec60[1] + (x->fRec64[1] + (x->fRec68[1] + fTemp26)))) - (x->fRec8[1] + (x->fRec44[1] + (x->fRec48[1] + (x->fRec52[1] + (fRec9 + (fRec45 + (fRec53 + fRec49))))))));
		double fTemp27 = (fRec61 + fRec57);
		x->fRec2[0] = ((x->fRec48[1] + (x->fRec52[1] + (x->fRec64[1] + (x->fRec68[1] + (fRec49 + (fRec53 + fTemp25)))))) - (x->fRec8[1] + (x->fRec44[1] + (x->fRec56[1] + (x->fRec60[1] + (fRec9 + (fRec45 + fTemp27)))))));
		x->fRec3[0] = ((x->fRec8[1] + (x->fRec44[1] + (x->fRec64[1] + (x->fRec68[1] + (fRec9 + (fRec45 + fTemp25)))))) - (x->fRec48[1] + (x->fRec52[1] + (x->fRec56[1] + (x->fRec60[1] + (fRec49 + (fRec53 + fTemp27)))))));
		double fTemp28 = (fRec69 + fRec61);
		double fTemp29 = (fRec65 + fRec57);
		x->fRec4[0] = ((x->fRec44[1] + (x->fRec52[1] + (x->fRec60[1] + (x->fRec68[1] + (fRec45 + (fRec53 + fTemp28)))))) - (x->fRec8[1] + (x->fRec48[1] + (x->fRec56[1] + (x->fRec64[1] + (fRec9 + (fRec49 + fTemp29)))))));
		x->fRec5[0] = ((x->fRec8[1] + (x->fRec48[1] + (x->fRec60[1] + (x->fRec68[1] + (fRec9 + (fRec49 + fTemp28)))))) - (x->fRec44[1] + (x->fRec52[1] + (x->fRec56[1] + (x->fRec64[1] + (fRec45 + (fRec53 + fTemp29)))))));
		double fTemp30 = (fRec69 + fRec57);
		double fTemp31 = (fRec65 + fRec61);
		x->fRec6[0] = ((x->fRec8[1] + (x->fRec52[1] + (x->fRec56[1] + (x->fRec68[1] + (fRec9 + (fRec53 + fTemp30)))))) - (x->fRec44[1] + (x->fRec48[1] + (x->fRec60[1] + (x->fRec64[1] + (fRec45 + (fRec49 + fTemp31)))))));
		x->fRec7[0] = ((x->fRec44[1] + (x->fRec48[1] + (x->fRec56[1] + (x->fRec68[1] + (fRec45 + (fRec49 + fTemp30)))))) - (x->fRec8[1] + (x->fRec52[1] + (x->fRec60[1] + (x->fRec64[1] + (fRec9 + (fRec53 + fTemp31)))))));
		double fTemp32 = (1.0 - x->fRec12[0]);

		*output0++ = sanitize_denormal(double(((0.370000005 * (x->fRec1[0] + x->fRec2[0])) + (fSlow164 * (fTemp32 * fTemp14)))));
		*output1++ = sanitize_denormal(double(((0.370000005 * (x->fRec1[0] - x->fRec2[0])) + (fSlow11 * (fTemp32 * x->fVec6[((x->IOTA - iSlow127) & 8191)])))));
		
		x->fRec11[1] = x->fRec11[0];
		x->fRec10[1] = x->fRec10[0];
		x->IOTA = (x->IOTA + 1);
		x->fRec12[1] = x->fRec12[0];
		x->fRec20[1] = x->fRec20[0];
		x->iRec26[1] = x->iRec26[0];
		x->fRec28[1] = x->fRec28[0];
		x->fRec27[1] = x->fRec27[0];
		x->fRec29[1] = x->fRec29[0];
		x->fRec25[1] = x->fRec25[0];
		x->fRec24[1] = x->fRec24[0];
		x->fRec23[1] = x->fRec23[0];
		x->fRec22[1] = x->fRec22[0];
		x->fRec21[1] = x->fRec21[0];
		x->fVec1[1] = x->fVec1[0];
		x->fRec19[1] = x->fRec19[0];
		x->fRec18[1] = x->fRec18[0];
		x->fVec2[1] = x->fVec2[0];
		x->fRec33[1] = x->fRec33[0];
		x->fRec32[1] = x->fRec32[0];
		x->fVec3[1] = x->fVec3[0];
		x->fRec30[1] = x->fRec30[0];
		x->fRec14[1] = x->fRec14[0];
		x->fRec15[1] = x->fRec15[0];
		x->fRec13[2] = x->fRec13[1];
		x->fRec13[1] = x->fRec13[0];
		x->fVec4[1] = x->fVec4[0];
		x->fVec5[1] = x->fVec5[0];
		x->fRec43[1] = x->fRec43[0];
		x->fRec42[1] = x->fRec42[0];
		x->fRec41[1] = x->fRec41[0];
		x->fRec40[1] = x->fRec40[0];
		x->fRec39[1] = x->fRec39[0];
		x->fRec38[2] = x->fRec38[1];
		x->fRec38[1] = x->fRec38[0];
		x->fRec37[2] = x->fRec37[1];
		x->fRec37[1] = x->fRec37[0];
		x->fRec36[2] = x->fRec36[1];
		x->fRec36[1] = x->fRec36[0];
		x->fRec35[2] = x->fRec35[1];
		x->fRec35[1] = x->fRec35[0];
		x->fRec34[1] = x->fRec34[0];
		x->fRec8[1] = x->fRec8[0];
		x->fRec47[1] = x->fRec47[0];
		x->fRec46[1] = x->fRec46[0];
		x->fRec44[1] = x->fRec44[0];
		x->fRec51[1] = x->fRec51[0];
		x->fRec50[1] = x->fRec50[0];
		x->fRec48[1] = x->fRec48[0];
		x->fRec55[1] = x->fRec55[0];
		x->fRec54[1] = x->fRec54[0];
		x->fRec52[1] = x->fRec52[0];
		x->fRec59[1] = x->fRec59[0];
		x->fRec58[1] = x->fRec58[0];
		x->fRec56[1] = x->fRec56[0];
		x->fRec63[1] = x->fRec63[0];
		x->fRec62[1] = x->fRec62[0];
		x->fRec60[1] = x->fRec60[0];
		x->fRec67[1] = x->fRec67[0];
		x->fRec66[1] = x->fRec66[0];
		x->fRec64[1] = x->fRec64[0];
		x->fRec71[1] = x->fRec71[0];
		x->fRec70[1] = x->fRec70[0];
		x->fRec68[1] = x->fRec68[0];
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

double sanitize_denormal(double v) {
	if (IS_DENORM_NAN_SAMPLE(v))
		return 0.0;
	return v;
}

double mydsp_faustpower2_f(double value) {
	return (value * value);
}

void pianof_free(t_pianof* x) {
	dsp_free((t_pxobject*)x);

	sysmem_freeptr(x->fRec11);
	sysmem_freeptr(x->fRec10);
	sysmem_freeptr(x->fVec0);
	sysmem_freeptr(x->fRec20);
	sysmem_freeptr(x->iRec26);
	sysmem_freeptr(x->fRec28);
	sysmem_freeptr(x->fRec12);
	sysmem_freeptr(x->fRec27);
	sysmem_freeptr(x->fRec13);
	sysmem_freeptr(x->fRec29);
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
	sysmem_freeptr(x->fRec33); 
	sysmem_freeptr(x->fRec32);
	sysmem_freeptr(x->fRec31);
	sysmem_freeptr(x->fVec3);
	sysmem_freeptr(x->fRec30);
	sysmem_freeptr(x->fRec14);
	sysmem_freeptr(x->fRec15);
	sysmem_freeptr(x->fVec4);
	sysmem_freeptr(x->fVec5);
	sysmem_freeptr(x->fRec43);
	sysmem_freeptr(x->fRec42);
	sysmem_freeptr(x->fRec41);
	sysmem_freeptr(x->fRec40);
	sysmem_freeptr(x->fRec39);
	sysmem_freeptr(x->fRec38);
	sysmem_freeptr(x->fRec37);
	sysmem_freeptr(x->fRec36);
	sysmem_freeptr(x->fRec35);
	sysmem_freeptr(x->fRec34);
	sysmem_freeptr(x->fVec6);
	sysmem_freeptr(x->fVec7);
	sysmem_freeptr(x->fVec8);
	sysmem_freeptr(x->fRec8);
	sysmem_freeptr(x->fRec47);
	sysmem_freeptr(x->fRec46);
	sysmem_freeptr(x->fVec9);
	sysmem_freeptr(x->fVec10);
	sysmem_freeptr(x->fRec44);
	sysmem_freeptr(x->fRec51);
	sysmem_freeptr(x->fRec50);
	sysmem_freeptr(x->fVec11);
	sysmem_freeptr(x->fVec14);
	sysmem_freeptr(x->fRec52);
	sysmem_freeptr(x->fRec59);
	sysmem_freeptr(x->fRec58);
	sysmem_freeptr(x->fVec15);
	sysmem_freeptr(x->fVec16);
	sysmem_freeptr(x->fVec17);
	sysmem_freeptr(x->fRec56);
	sysmem_freeptr(x->fRec63);
	sysmem_freeptr(x->fRec62);
	sysmem_freeptr(x->fVec18);
	sysmem_freeptr(x->fVec19);
	sysmem_freeptr(x->fRec60);
	sysmem_freeptr(x->fVec21);
	sysmem_freeptr(x->fRec64);
	sysmem_freeptr(x->fRec71);
	sysmem_freeptr(x->fRec70);
	sysmem_freeptr(x->fVec22);
	sysmem_freeptr(x->fVec23);
	sysmem_freeptr(x->fRec68);
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
	sysmem_freeptr(x->fRec55);
	sysmem_freeptr(x->fRec54);
	sysmem_freeptr(x->fVec13);
	sysmem_freeptr(x->fRec67);
	sysmem_freeptr(x->fRec66);
	sysmem_freeptr(x->fVec20);

}

void setFreq(t_pianof *x, double frequency) {
	if (frequency < 20.0) x->freq = 20.0;
	else if (frequency > 20000.0) x->freq = 20000.0;
	else x->freq = frequency;
}

void playStart(t_pianof *x, double gt) {
	if (gt < 0.0) x->gate = 0.0;
	else if (gt > 1.0) x->gate = 1.0;
	else x->gate = gt;
}

void setGain(t_pianof *x, double gn) {
	if (gn < 0.0) x->gain = 0.0;
	else if (gn > 1.0) x->gain = 1.0;
	else x->gain = gn;
}

void setBrightness(t_pianof *x, double brght) {
	if (brght < 0.0) x->brght_f = 0.0;
	else if (brght > 1.0) x->brght_f = 1.0;
	else x->brght_f = brght;
}

void setDetuning(t_pianof *x, double dtng) {
	if (dtng < 0.0) x->dtng_f = 0.0;
	else if (dtng > 1.0) x->dtng_f = 1.0;
	else x->dtng_f = dtng;
}

void setHammer(t_pianof *x, double hmmr) {
	if (hmmr < 0.0) x->hmmr_hrd = 0.0;
	else if (hmmr > 1.0) x->hmmr_hrd = 1.0;
	else x->hmmr_hrd = hmmr;
}

void setStiffness(t_pianof *x, double stffn) {
	if (stffn < 0.0) x->stffn_f = 0.0;
	else if (stffn > 1.0) x->stffn_f = 1.0;
	else x->stffn_f = stffn;
}

void setReverb(t_pianof *x, double rvrbgain) {
	if (rvrbgain < 0.0) x->rvrb_gain = 0.0;
	else if (rvrbgain > 1.0) x->rvrb_gain = 1.0;
	else x->rvrb_gain = rvrbgain;
}
void setRoom(t_pianof *x, double rmSize) {
	if (rmSize < 0.01) x->roomsize = 0.01;
	else if (rmSize > 2.0) x->roomsize = 2.0;
	else x->roomsize = rmSize;
}

void setAngle(t_pianof *x, double panAngle) {
	if (panAngle < 0.0) x->pan_angle = 0.0;
	else if (panAngle > 1.0) x->pan_angle = 1.0;
	else x->pan_angle = panAngle;
}

void setWidth(t_pianof *x, double width) {
	if (width < 0.0) x->spat_width = 0.0;
	else if (width > 1.0) x->spat_width = 1.0;
	else x->spat_width = width;
}

void setDef(t_pianof *x) {
	x->gain = 1.0;
	x->gate = 0.0;
	x->brght_f = 0.0;
	x->dtng_f = 0.1;
	x->hmmr_hrd = 0.1;
	x->stffn_f = 0.28;

	x->rvrb_gain = 0.137;
	x->roomsize = 0.72;
	x->pan_angle = 0.6;
	x->spat_width = 0.5;
}

void clearall(t_pianof *x) {
	x->gate = 0.0;

	x->fRec11 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec10 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec0 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fRec20 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->iRec26 = (int *)sysmem_newptrclear(2 * sizeof(int));
	x->fRec28 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec12 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec27 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec13 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec29 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec25 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec24 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec23 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec22 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec21 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec1 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec19 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec18 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec17 = (double *)sysmem_newptrclear(8192 * sizeof(double));
	x->fVec2 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec33 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec32 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec31 = (double *)sysmem_newptrclear(8192 * sizeof(double));
	x->fVec3 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec30 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec14 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec15 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec4 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec5 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec43 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec42 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec41 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec40 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec39 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec38 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec37 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec36 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec35 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec34 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec6 = (double *)sysmem_newptrclear(8192 * sizeof(double));
	x->fVec7 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fVec8 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec8 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec47 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec46 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec9 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fVec10 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec44 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec51 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec50 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec11 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec14 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec52 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec59 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec58 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec15 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec16 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fVec17 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec56 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec63 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec62 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec18 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec19 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec60 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec21 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec64 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec71 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec70 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec22 = (double *)sysmem_newptrclear(16384 * sizeof(double));
	x->fVec23 = (double *)sysmem_newptrclear(2048 * sizeof(double));
	x->fRec68 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec0 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec1 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec2 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec3 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec4 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec5 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec6 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fRec7 = (double *)sysmem_newptrclear(3 * sizeof(double));
	x->fVec12 = (double *)sysmem_newptrclear(4096 * sizeof(double));
	x->fRec48 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec55 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec54 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec13 = (double *)sysmem_newptrclear(32768 * sizeof(double));
	x->fRec67 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fRec66 = (double *)sysmem_newptrclear(2 * sizeof(double));
	x->fVec20 = (double *)sysmem_newptrclear(16384 * sizeof(double));

	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		x->fRec11[l0] = 0.0f;
	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		x->fRec10[l1] = 0.0;
	}
	x->IOTA = 0;
	for (int l2 = 0; (l2 < 32768); l2 = (l2 + 1)) {
		x->fVec0[l2] = 0.0;
	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		x->fRec12[l3] = 0.0;
	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		x->fRec20[l4] = 0.0;
	}
	for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
		x->iRec26[l5] = 0;
	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		x->fRec28[l6] = 0.0;
	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		x->fRec27[l7] = 0.0;
	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		x->fRec29[l8] = 0.0;
	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		x->fRec25[l9] = 0.0;
	}
	for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
		x->fRec24[l10] = 0.0;
	}
	for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
		x->fRec23[l11] = 0.0;
	}
	for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
		x->fRec22[l12] = 0.0;
	}
	for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
		x->fRec21[l13] = 0.0;
	}
	for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
		x->fVec1[l14] = 0.0;
	}
	for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
		x->fRec19[l15] = 0.0;
	}
	for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
		x->fRec18[l16] = 0.0;
	}
	for (int l17 = 0; (l17 < 8192); l17 = (l17 + 1)) {
		x->fRec17[l17] = 0.0;
	}
	for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
		x->fVec2[l18] = 0.0;
	}
	for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
		x->fRec33[l19] = 0.0;
	}
	for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
		x->fRec32[l20] = 0.0;
	}
	for (int l21 = 0; (l21 < 8192); l21 = (l21 + 1)) {
		x->fRec31[l21] = 0.0;
	}
	for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
		x->fVec3[l22] = 0.0;
	}
	for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
		x->fRec30[l23] = 0.0;
	}
	for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
		x->fRec14[l24] = 0.0;
	}
	for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
		x->fRec15[l25] = 0.0;
	}
	for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
		x->fRec13[l26] = 0.0;
	}
	for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
		x->fVec4[l27] = 0.0;
	}
	for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
		x->fVec5[l28] = 0.0;
	}
	for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
		x->fRec43[l29] = 0.0;
	}
	for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
		x->fRec42[l30] = 0.0;
	}
	for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
		x->fRec41[l31] = 0.0;
	}
	for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
		x->fRec40[l32] = 0.0;
	}
	for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) {
		x->fRec39[l33] = 0.0;
	}
	for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
		x->fRec38[l34] = 0.0;
	}
	for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) {
		x->fRec37[l35] = 0.0;
	}
	for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
		x->fRec36[l36] = 0.0;
	}
	for (int l37 = 0; (l37 < 3); l37 = (l37 + 1)) {
		x->fRec35[l37] = 0.0;
	}
	for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
		x->fRec34[l38] = 0.0;
	}
	for (int l39 = 0; (l39 < 8192); l39 = (l39 + 1)) {
		x->fVec6[l39] = 0.0;
	}
	for (int l40 = 0; (l40 < 4096); l40 = (l40 + 1)) {
		x->fVec7[l40] = 0.0;
	}
	for (int l41 = 0; (l41 < 2048); l41 = (l41 + 1)) {
		x->fVec8[l41] = 0.0;
	}
	for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
		x->fRec8[l42] = 0.0;
	}
	for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
		x->fRec47[l43] = 0.0;
	}
	for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
		x->fRec46[l44] = 0.0;
	}
	for (int l45 = 0; (l45 < 32768); l45 = (l45 + 1)) {
		x->fVec9[l45] = 0.0;
	}
	for (int l46 = 0; (l46 < 4096); l46 = (l46 + 1)) {
		x->fVec10[l46] = 0.0;
	}
	for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
		x->fRec44[l47] = 0.0;
	}
	for (int l48 = 0; (l48 < 2); l48 = (l48 + 1)) {
		x->fRec51[l48] = 0.0;
	}
	for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
		x->fRec50[l49] = 0.0;
	}
	for (int l50 = 0; (l50 < 16384); l50 = (l50 + 1)) {
		x->fVec11[l50] = 0.0;
	}
	for (int l51 = 0; (l51 < 4096); l51 = (l51 + 1)) {
		x->fVec12[l51] = 0.0;
	}
	for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
		x->fRec48[l52] = 0.0;
	}
	for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
		x->fRec55[l53] = 0.0;
	}
	for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
		x->fRec54[l54] = 0.0;
	}
	for (int l55 = 0; (l55 < 32768); l55 = (l55 + 1)) {
		x->fVec13[l55] = 0.0;
	}
	for (int l56 = 0; (l56 < 4096); l56 = (l56 + 1)) {
		x->fVec14[l56] = 0.0;
	}
	for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
		x->fRec52[l57] = 0.0;
	}
	for (int l58 = 0; (l58 < 2); l58 = (l58 + 1)) {
		x->fRec59[l58] = 0.0;
	}
	for (int l59 = 0; (l59 < 2); l59 = (l59 + 1)) {
		x->fRec58[l59] = 0.0;
	}
	for (int l60 = 0; (l60 < 16384); l60 = (l60 + 1)) {
		x->fVec15[l60] = 0.0;
	}
	for (int l61 = 0; (l61 < 4096); l61 = (l61 + 1)) {
		x->fVec16[l61] = 0.0;
	}
	for (int l62 = 0; (l62 < 2048); l62 = (l62 + 1)) {
		x->fVec17[l62] = 0.0;
	}
	for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
		x->fRec56[l63] = 0.0;
	}
	for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
		x->fRec63[l64] = 0.0;
	}
	for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
		x->fRec62[l65] = 0.0;
	}
	for (int l66 = 0; (l66 < 16384); l66 = (l66 + 1)) {
		x->fVec18[l66] = 0.0;
	}
	for (int l67 = 0; (l67 < 4096); l67 = (l67 + 1)) {
		x->fVec19[l67] = 0.0;
	}
	for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
		x->fRec60[l68] = 0.0;
	}
	for (int l69 = 0; (l69 < 2); l69 = (l69 + 1)) {
		x->fRec67[l69] = 0.0;
	}
	for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
		x->fRec66[l70] = 0.0;
	}
	for (int l71 = 0; (l71 < 16384); l71 = (l71 + 1)) {
		x->fVec20[l71] = 0.0;
	}
	for (int l72 = 0; (l72 < 4096); l72 = (l72 + 1)) {
		x->fVec21[l72] = 0.0;
	}
	for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
		x->fRec64[l73] = 0.0;
	}
	for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
		x->fRec71[l74] = 0.0;
	}
	for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
		x->fRec70[l75] = 0.0;
	}
	for (int l76 = 0; (l76 < 16384); l76 = (l76 + 1)) {
		x->fVec22[l76] = 0.0;
	}
	for (int l77 = 0; (l77 < 2048); l77 = (l77 + 1)) {
		x->fVec23[l77] = 0.0;
	}
	for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
		x->fRec68[l78] = 0.0;
	}
	for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
		x->fRec0[l79] = 0.0;
	}
	for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
		x->fRec1[l80] = 0.0;
	}
	for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
		x->fRec2[l81] = 0.0;
	}
	for (int l82 = 0; (l82 < 3); l82 = (l82 + 1)) {
		x->fRec3[l82] = 0.0;
	}
	for (int l83 = 0; (l83 < 3); l83 = (l83 + 1)) {
		x->fRec4[l83] = 0.0;
	}
	for (int l84 = 0; (l84 < 3); l84 = (l84 + 1)) {
		x->fRec5[l84] = 0.0;
	}
	for (int l85 = 0; (l85 < 3); l85 = (l85 + 1)) {
		x->fRec6[l85] = 0.0;
	}
	for (int l86 = 0; (l86 < 3); l86 = (l86 + 1)) {
		x->fRec7[l86] = 0.0;
	}
}