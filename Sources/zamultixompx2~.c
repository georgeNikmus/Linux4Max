#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define MAX_FILT 4
#define MAX_COMP 3
#define STEREOLINK_MAX 1
#define STEREOLINK_AVERAGE 0
#define DANGER 100000.0

static t_class *zamulticompx2_class;

struct linear_svf {
	double k;
	double g;
	double s[2];
};

typedef struct _zamulticompx2
{
	t_pxobject x_obj;

	double sample_rate;
	double *attack, *release, *knee, *ratio, *thresdb, *makeup, globalgain;
	double *gainr, *toggle, *listen, maxL, maxR, outl, outr, xover1, xover2;
	double **old_yl, **old_y1, **old_yg;
	double *old_ll, *old_l1;

	double* outL, * outR;
	double oldxover1, oldxover2;
	bool stereodet;

	struct linear_svf **simper;
}t_zamulticompx2;



void* zamulticompx2_new(t_symbol* s, short argc, t_atom* argv);

void zamulticompx2_dsp64(t_zamulticompx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void zamulticompx2_perform64(t_zamulticompx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
void zamulticompx2_free(t_zamulticompx2* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void activate(t_zamulticompx2 *x);

void run_comp(t_zamulticompx2 *x, int k, double inL, double inR, double *outL, double *outR);
void calc_lr4(t_zamulticompx2 *x, double f, int i);
void run_lr4(t_zamulticompx2 *x,int i, double in, double *outlo, double *outhi);
//void linear_svf_reset(struct linear_svf *self);
void linear_svf_set_xover(struct linear_svf *self, double sample_rate, double cutoff, double resonance);
double run_linear_svf_xover(struct linear_svf *self, double in, double mixlow, double mixhigh);

void att_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void rel_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void knee_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void ratio_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void thresh_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void makeup_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void locut(t_zamulticompx2 *x, double f1);
void hicut(t_zamulticompx2 *x, double f2);
void bypass_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void master(t_zamulticompx2 *x, double v);
void listen_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv);
void stereolink(t_zamulticompx2 *x, bool v);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamulticompx2~", (method)zamulticompx2_new, (method)zamulticompx2_free, sizeof(t_zamulticompx2), 0, 0);
	class_addmethod(c, (method)zamulticompx2_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamulticompx2_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)att_list, "attacks", A_GIMME, 0);
	class_addmethod(c, (method)rel_list, "releases", A_GIMME, 0);
	class_addmethod(c, (method)knee_list, "knees", A_GIMME, 0);
	class_addmethod(c, (method)ratio_list, "ratios", A_GIMME, 0);
	class_addmethod(c, (method)thresh_list, "threshs", A_GIMME, 0);
	class_addmethod(c, (method)makeup_list, "makeups", A_GIMME, 0);
	class_addmethod(c, (method)locut, "locut", A_FLOAT, 0);
	class_addmethod(c, (method)hicut, "hicut", A_FLOAT, 0);
	class_addmethod(c, (method)bypass_list, "bypass_lst", A_GIMME, 0);
	class_addmethod(c, (method)master, "master", A_FLOAT, 0);
	class_addmethod(c, (method)listen_list, "soloband", A_GIMME, 0);
	class_addmethod(c, (method)stereolink, "stereolink", A_GIMME, 0);

	class_addmethod(c, (method)activate, "flreset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamulticompx2_class = c;

	post("------------------------------------------------------------------");
	post("The Zam Multiband(3 bands) Stereo Compressor for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamulticompx2_dsp64(t_zamulticompx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		zamulticompx2_perform64, 0, NULL);
}

void zamulticompx2_perform64(t_zamulticompx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	t_double *output2 = outs[2];
	t_double *output3 = outs[3];
	t_double *output4 = outs[4];
	int n = vectorsize;

	double maxxL = 0.0;
	double maxxR = 0.0;
	int i;

	int tog1 = (x->toggle[0] > 0.5) ? 0 : 1;
	int tog2 = (x->toggle[1] > 0.5) ? 0 : 1;
	int tog3 = (x->toggle[2] > 0.5) ? 0 : 1;

	int listen1 = (x->listen[0] > 0.5) ? 1 : 0;
	int listen2 = (x->listen[1] > 0.5) ? 1 : 0;
	int listen3 = (x->listen[2] > 0.5) ? 1 : 0;

	if (x->oldxover1 != x->xover1) {
		calc_lr4(x, x->xover1, 0);
		calc_lr4(x, x->xover1, 1);
		x->oldxover1 = x->xover1;
	}

	if (x->oldxover2 != x->xover2) {
		calc_lr4(x, x->xover2, 2);
		calc_lr4(x, x->xover2, 3);
		x->oldxover2 = x->xover2;
	}

	for (i = 0; i < n; ++i) {
		double tmp1[2], tmp2[2], tmp3[2];
		double fil1[2], fil2[2], fil3[2], fil4[2];
		//double outL[MAX_COMP + 1] = { 0.0 };
		//double outR[MAX_COMP + 1] = { 0.0 };
		double* outL = x->outL;
		double* outR = x->outR;

		double inl = sanitize_denormal(input0[i]);
		double inr = sanitize_denormal(input1[i]);
		inl = (fabs(inl) < DANGER) ? inl : 0.0;
		inr = (fabs(inr) < DANGER) ? inr : 0.0;

		int listenmode = 0;

		// Interleaved channel processing
		run_lr4(x, 0, inl, &fil1[0], &fil2[0]);
		run_lr4(x, 1, inr, &fil1[1], &fil2[1]);
		run_lr4(x, 2, fil2[0], &fil3[0], &fil4[0]);
		run_lr4(x, 3, fil2[1], &fil3[1], &fil4[1]);

		//pushsample(x, x->outlevelold[0], max(fil1[0], fil1[1]), 0);
		//x->outlevel[0] = averageabs(outlevelold[0]);
		//x->outlevel[0] = (x->outlevel[0] == 0.f) ? -45.0 : to_dB(x->outlevel[0]);
		if (tog1)
			run_comp(x, 0, fil1[0], fil1[1], &outL[0], &outR[0]);

		tmp1[0] = tog1 ? outL[0] * from_dB(x->makeup[0]) : fil1[0];
		tmp1[1] = tog1 ? outR[0] * from_dB(x->makeup[0]) : fil1[1];

		//pushsample(x, x->outlevelold[1], max(fil3[0], fil3[1]), 1);
		//x->outlevel[1] = averageabs(x->outlevelold[1]);
		//x->outlevel[1] = (x->outlevel[1] == 0.f) ? -45.0 : to_dB(x->outlevel[1]);
		if (tog2)
			run_comp(x, 1, fil3[0], fil3[1], &outL[1], &outR[1]);

		tmp2[0] = tog2 ? outL[1] * from_dB(x->makeup[1]) : fil3[0];
		tmp2[1] = tog2 ? outR[1] * from_dB(x->makeup[1]) : fil3[1];

		//pushsample(x, x->outlevelold[2], max(fil4[0], fil4[1]), 2);
		//x->outlevel[2] = averageabs(outlevelold[2]);
		//x->outlevel[2] = (x->outlevel[2] == 0.f) ? -45.0 : to_dB(x->outlevel[2]);
		if (tog3)
			run_comp(x, 2, fil4[0], fil4[1], &outL[2], &outR[2]);

		tmp3[0] = tog3 ? outL[2] * from_dB(x->makeup[2]) : fil4[0];
		tmp3[1] = tog3 ? outR[2] * from_dB(x->makeup[2]) : fil4[1];

		output0[i] = output1[i] = 0.0;
		if (listen1) {
			listenmode = 1;
			output0[i] += outL[0] * tog1*from_dB(x->makeup[0])
				+ (1. - tog1) * tmp1[0];
			output1[i] += outR[0] * tog1*from_dB(x->makeup[0])
				+ (1. - tog1) * tmp1[1];
		}
		if (listen2) {
			listenmode = 1;
			output0[i] += outL[1] * tog2*from_dB(x->makeup[1])
				+ (1. - tog2) * tmp2[0];
			output1[i] += outR[1] * tog2*from_dB(x->makeup[1])
				+ (1. - tog2) * tmp2[1];
		}
		if (listen3) {
			listenmode = 1;
			output0[i] += outL[2] * tog3*from_dB(x->makeup[2])
				+ (1. - tog3) * tmp3[0];
			output1[i] += outR[2] * tog3*from_dB(x->makeup[2])
				+ (1. - tog3) * tmp3[1];
		}
		if (!listenmode) {
			output0[i] = tmp1[0] + tmp2[0] + tmp3[0];
			output1[i] = tmp1[1] + tmp2[1] + tmp3[1];
		}
		output0[i] = sanitize_denormal(output0[i]);
		output1[i] = sanitize_denormal(output1[i]);
		output0[i] *= from_dB(x->globalgain);
		output1[i] *= from_dB(x->globalgain);
		output2[i] = x->gainr[0];
		output3[i] = x->gainr[1];
		output4[i] = x->gainr[2];

		maxxL = (fabs(output0[i]) > maxxL) ? fabs(output0[i]) : sanitize_denormal(maxxL);
		maxxR = (fabs(output1[i]) > maxxR) ? fabs(output1[i]) : sanitize_denormal(maxxR);
	}
	//x->outl = (maxxL == 0.f) ? -160.f : to_dB(maxxL);
	//x->outr = (maxxR == 0.f) ? -160.f : to_dB(maxxR);
}

void* zamulticompx2_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamulticompx2* x = (t_zamulticompx2*)object_alloc(zamulticompx2_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();

	x->attack = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->release = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->knee = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->ratio = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->thresdb = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->makeup = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->gainr = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->toggle = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->listen = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->old_l1 = (double*)sysmem_newptr(2 * sizeof(double));
	x->old_ll = (double*)sysmem_newptr(2 * sizeof(double));

	x->simper = (struct linear_svf**)sysmem_newptr(2 * sizeof(struct linear_svf*));
	x->old_y1 = (double**)sysmem_newptr(2 * sizeof(double*));
	x->old_yl = (double**)sysmem_newptr(2 * sizeof(double*));
	x->old_yg = (double**)sysmem_newptr(2 * sizeof(double*));

	for (int i = 0; i < 2; i++) {
		x->old_y1[i] = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
		x->old_yl[i] = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
		x->old_yg[i] = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
		x->simper[i] = (struct linear_svf*)sysmem_newptr(MAX_FILT * sizeof(struct linear_svf));
	}

	x->outL = (double*)sysmem_newptrclear((MAX_COMP + 1) * sizeof(double));
	x->outR = (double*)sysmem_newptrclear((MAX_COMP + 1) * sizeof(double));

	x->attack[0] = 10.0;
	x->attack[1] = 10.0;
	x->attack[2] = 10.0;
	x->release[0] = 80.0;
	x->release[1] = 80.0;
	x->release[2] = 80.0;
	x->knee[0] = 0.0;
	x->knee[1] = 0.0;
	x->knee[2] = 0.0;
	x->ratio[0] = 4.0;
	x->ratio[1] = 4.0;
	x->ratio[2] = 4.0;
	x->thresdb[0] = -20.0;
	x->thresdb[1] = -18.0;
	x->thresdb[2] = -16.0;
	x->makeup[0] = 0.0;
	x->makeup[1] = 0.0;
	x->makeup[2] = 0.0;
	x->xover1 = 160.0;
	x->xover2 = 1400.0;
	x->gainr[0] = 0.0;
	x->gainr[1] = 0.0;
	x->gainr[2] = 0.0;
	x->toggle[0] = 0.0;
	x->toggle[1] = 0.0;
	x->toggle[2] = 0.0;
	x->listen[0] = 0.0;
	x->listen[1] = 0.0;
	x->listen[2] = 0.0;
	x->stereodet = true;
	x->globalgain = 0.0;

	/* Default variable values */
	x->maxL = 0.f;
	x->maxR = 0.f;

	/* reset filter values */
	activate(x);

	return x;
}

void calc_lr4(t_zamulticompx2 *x, double f, int i)
{
	double srate = x->sample_rate;

	linear_svf_set_xover(&x->simper[0][i], srate, f, 0.25);
	linear_svf_set_xover(&x->simper[1][i], srate, f, 0.25);
}

void run_lr4(t_zamulticompx2 *x, int i, double in, double *outlo, double *outhi)
{
	*outlo = run_linear_svf_xover(&x->simper[0][i], in, 1., 0.);
	*outhi = run_linear_svf_xover(&x->simper[1][i], in, 0., 1.);
}

void linear_svf_set_xover(struct linear_svf *self, double sample_rate, double cutoff, double resonance)
{
	double w;

	self->k = 2. - 2. * resonance;
	w = PI * cutoff / sample_rate;
	self->g = tan(w);
}

/*
void linear_svf_reset(struct linear_svf *self)
{
	self->s[0] = self->s[1] = 0.f;
}
*/

double run_linear_svf_xover(struct linear_svf *self, double in, double mixlow, double mixhigh)
{
	double v[3];
	double g = self->g;
	double k = self->k;
	double s0 = self->s[0];
	double s1 = self->s[1];
	double g2 = g * g;
	double vhigh = in * mixhigh;
	double vband = in * 0.75;
	double vlow = in * mixlow;

	v[0] = in;
	v[1] = -1. / (1. + g2 + g * k) * (-s0 + g * s1 - g * k*s0 + g2 * vband + g * vhigh - g * vlow - g2 * k*vlow);
	v[2] = -1. / (1. + g2 + g * k) * (-g * s0 - s1 - g * vband + g2 * vhigh + g * k*vhigh - g2 * vlow);
	self->s[0] = 2. * v[1] - s0;
	self->s[1] = 2. * v[2] - s1;

	return (double)(vhigh + v[2]);
}

void activate(t_zamulticompx2 *x)
{
	int i, j;
	for (i = 0; i < MAX_COMP; i++)
		for (j = 0; j < 2; j++)
			x->old_yl[j][i] = x->old_y1[j][i] = x->old_yg[j][i] = 0.0;

	for (j = 0; j < 2; j++)
		x->old_ll[j] = x->old_l1[j] = 0.0;

	for (j = 0; j < 2; j++) {
		for (i = 0; i < MAX_FILT; i++) {
			x->simper[j][i].k = 0.0;
			x->simper[j][i].g = 0.0;
			x->simper[j][i].s[0] = 0.0;
			x->simper[j][i].s[1] = 0.0;
		}
	}
	x->maxL = x->maxR = 0.0;

	x->oldxover1 = 0.0;
	x->oldxover2 = 0.0;
}

double sanitize_denormal(double v) {
	if (IS_DENORM_NAN_SAMPLE(v))
		return 0.0;
	return v;
}

double from_dB(double gdb) {
	return (exp(gdb / 20.0*log(10.0)));
}

double to_dB(double g) {
	return (20.0*log10(g));
}

void run_comp(t_zamulticompx2 *x, int k, double inL, double inR, double *outL, double *outR)
{
	double srate = x->sample_rate;
	double width = (6.0 * x->knee[k]) + 0.01;
	double attack_coeff = exp(-1000.0 / (x->attack[k] * srate));
	double release_coeff = exp(-1000.0 / (x->release[k] * srate));
	int stereolink = (x->stereodet) ? STEREOLINK_MAX : STEREOLINK_AVERAGE;
	double checkwidth = 0.0;
	double cdb = 0.0;
	double Lgain = 1.0;
	double Rgain = 1.0;
	double Lxg, Lyg;
	double Rxg, Ryg;
	double Lxl, Lyl;
	double Rxl, Ryl;

	Lyg = Ryg = 0.0;
	inL = sanitize_denormal(inL);
	inR = sanitize_denormal(inR);
	Lxg = (inL == 0.0) ? -160.0 : to_dB(fabs(inL));
	Rxg = (inR == 0.0) ? -160.0 : to_dB(fabs(inR));
	Lxg = sanitize_denormal(Lxg);
	Rxg = sanitize_denormal(Rxg);

	checkwidth = 2.0*fabs(Lxg - x->thresdb[k]);
	if (2.0*(Lxg - x->thresdb[k]) < -width) {
		Lyg = Lxg;
	}
	else if (checkwidth <= width) {
		Lyg = Lxg + (1.0 / x->ratio[k] - 1.0)*(Lxg - x->thresdb[k] + width / 2.0)*(Lxg - x->thresdb[k] + width / 2.0) / (2.0*width);
		Lyg = sanitize_denormal(Lyg);
	}
	else if (2.0*(Lxg - x->thresdb[k]) > width) {
		Lyg = x->thresdb[k] + (Lxg - x->thresdb[k]) / x->ratio[k];
		Lyg = sanitize_denormal(Lyg);
	}

	checkwidth = 2.0*fabs(Rxg - x->thresdb[k]);
	if (2.0*(Rxg - x->thresdb[k]) < -width) {
		Ryg = Rxg;
	}
	else if (checkwidth <= width) {
		Ryg = Rxg + (1.0 / x->ratio[k] - 1.0)*(Rxg - x->thresdb[k] + width / 2.0)*(Rxg - x->thresdb[k] + width / 2.0) / (2.0*width);
		Ryg = sanitize_denormal(Ryg);
	}
	else if (2.0*(Rxg - x->thresdb[k]) > width) {
		Ryg = x->thresdb[k] + (Rxg - x->thresdb[k]) / x->ratio[k];
		Ryg = sanitize_denormal(Ryg);
	}

	if (stereolink == STEREOLINK_MAX) {
		Lxl = Rxl = max(Lxg - Lyg, Rxg - Ryg);
	}
	else {
		Lxl = Rxl = (Lxg - Lyg + Rxg - Ryg) / 2.0;
	}

	x->old_yl[0][k] = sanitize_denormal(x->old_yl[0][k]);
	x->old_yl[1][k] = sanitize_denormal(x->old_yl[1][k]);


	if (Lxl < x->old_yl[0][k]) {
		Lyl = release_coeff * x->old_yl[0][k] + (1.0 - release_coeff)*Lxl;
	}
	else if (Lxl > x->old_yl[0][k]) {
		Lyl = attack_coeff * x->old_yl[0][k] + (1.0 - attack_coeff)*Lxl;
	}
	else {
		Lyl = Lxl;
	}
	Lyl = sanitize_denormal(Lyl);

	cdb = -Lyl;
	Lgain = from_dB(cdb);

	if (Rxl < x->old_yl[1][k]) {
		Ryl = release_coeff * x->old_yl[1][k] + (1.0 - release_coeff)*Rxl;
	}
	else if (Rxl > x->old_yl[1][k]) {
		Ryl = attack_coeff * x->old_yl[1][k] + (1.0 - attack_coeff)*Rxl;
	}
	else {
		Ryl = Rxl;
	}
	Ryl = sanitize_denormal(Ryl);

	cdb = -Ryl;
	Rgain = from_dB(cdb);

	if (stereolink == STEREOLINK_MAX)
		//x->gainr[k] = max(Lyl, Ryl);
		x->gainr[k] = max(Lgain, Rgain);
	else
		//x->gainr[k] = (Lyl + Ryl) / 2.f;
		x->gainr[k] = (Lgain + Rgain) / 2.f;

	*outL = inL * Lgain;
	*outR = inR * Rgain;

	x->old_yl[0][k] = Lyl;
	x->old_yl[1][k] = Ryl;
	x->old_yg[0][k] = Lyg;
	x->old_yg[1][k] = Ryg;
}


void att_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *attacks = x->attack;

	for (i = 0; i < argc; i++) {
		attacks[i] = atom_getfloat(argv + i);
		if (attacks[i] < 0.1) {
			attacks[i] = 0.1;
			post("Minimum Attack Time: 0.1 ms");
		}
		else if (attacks[i] > 1000.0) {
			attacks[i] = 1000.0;
			post("Maximum Attack Time: 1000 ms");
		}
	}
}

void rel_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *releases = x->release;

	for (i = 0; i < argc; i++) {
		releases[i] = atom_getfloat(argv + i);
			if (releases[i] < 1.0) {
				releases[i] = 1.0;
				post("Minimum Release Time: 1 ms");
			}
			else if (releases[i] > 1000.0) {
				releases[i] = 1000.0;
				post("Maximum Release Time: 1000 ms");
			}
		}
}

void knee_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *knees = x->knee;

	for (i = 0; i < argc; i++) {
		knees[i] = atom_getfloat(argv + i);
		if (knees[i] < 0.0) {
			knees[i] = 0.0;
			post("Minimum Knee Value: 0 db");
		}
		else if (knees[i] > 18.0) {
			knees[i] = 18.0;
			post("Maximum Knee Value: 18 db");
		}
	}
}

void ratio_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *ratios = x->ratio;

	for (i = 0; i < argc; i++) {
		ratios[i] = atom_getfloat(argv + i);
		if (ratios[i] < 1.0) {
			ratios[i] = 1.0;
			post("Minimum Ratio Value: 1.0");
		}
		else if (ratios[i] > 30.0) {
			ratios[i] = 30.0;
			post("Maximum Ratio Value: 30.0");
		}
	}
}

void thresh_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *threshs = x->thresdb;

	for (i = 0; i < argc; i++) {
		threshs[i] = atom_getfloat(argv + i);
		if (threshs[i] < -70.0) {
			threshs[i] = -70.0;
			post("Minimum Threshold Value: -70 db");
		}
		else if (threshs[i] > 0.0) {
			threshs[i] = 0.0;
			post("Maximum Threshold Value: 0 db");
		}
	}
}

void makeup_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *makeups = x->makeup;

	for (i = 0; i < argc; i++) {
		makeups[i] = atom_getfloat(argv + i);
		if (makeups[i] < 0.0) {
			makeups[i] = 0.0;
			post("Minimum Makeup Value: 0 db");
		}
		else if (makeups[i] > 30.0) {
			makeups[i] = 30.0;
			post("Maximum Makeup Value: 30 db");
		}
	}
}

void locut(t_zamulticompx2 *x, double hz1)
{

	if (hz1 > 1400.0) {
		hz1 = 1400.0;
		post("Low Crossover Maximum: 1400.0");
	}
	else if (hz1 < 20.0) {
		hz1 = 20.0;
		post("Low Crossover Minimum: 20.0");
	}
	x->xover1 = hz1;
}

void hicut(t_zamulticompx2 *x, double hz2)
{

	if (hz2 < 1400.0) {
		hz2 = 1400.0;
		post("High Crossover Minimum: 1400.0");
	}
	x->xover2 = hz2;
}

void bypass_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *toggles = x->toggle;

	for (i = 0; i < argc; i++) {
		toggles[i] = atom_getfloat(argv + i);
	}
}

void stereolink(t_zamulticompx2 *x, bool v )
{
	x->stereodet = v;
}

void master(t_zamulticompx2 *x, double v)
{

	if (v > 30.0) {
		v = 30.0;
		post("Maximum Master Trim: 30.0");
	}
	if (v < -30.0) {
		v = -30.0;
		post("Minimum Master Trim: -30.0");
	}
	x->globalgain = v;
}

void listen_list(t_zamulticompx2 *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *listen = x->listen;

	for (i = 0; i < argc; i++) {
		listen[i] = atom_getfloat(argv + i);
	}
}

void zamulticompx2_free(t_zamulticompx2* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->attack);
	sysmem_freeptr(x->release);
	sysmem_freeptr(x->knee);
	sysmem_freeptr(x->ratio);
	sysmem_freeptr(x->thresdb);
	sysmem_freeptr(x->makeup);
	sysmem_freeptr(x->gainr);
	sysmem_freeptr(x->toggle);
	sysmem_freeptr(x->listen);
	//sysmem_freeptr(x->outlevel);
	sysmem_freeptr(x->old_yl);
	sysmem_freeptr(x->old_y1);
	sysmem_freeptr(x->old_yg);
	//sysmem_freeptr(x->pos);
	//sysmem_freeptr(x->outlevelold);
	sysmem_freeptr(x->old_ll);
	sysmem_freeptr(x->old_l1);
	sysmem_freeptr(x->simper);
	sysmem_freeptr(x->outL);
	sysmem_freeptr(x->outR);
}