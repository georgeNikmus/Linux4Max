#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define MAX_FILT 2
#define MAX_COMP 3
#define DANGER 100000.0

static t_class *zamulticomp_class;

struct linear_svf {
	double k;
	double g;
	double s[2];
};

typedef struct _zamulticomp
{
	t_pxobject x_obj;

	double sample_rate;

	/*
	double attack[MAX_COMP];
	double release[MAX_COMP];
	double knee[MAX_COMP];
	double ratio[MAX_COMP];
	double thresdb[MAX_COMP];
	double makeup[MAX_COMP];
	double gainr[MAX_COMP];
	double toggle[MAX_COMP];
	double listen[MAX_COMP];
	*/
	double *attack, *release, *knee, *ratio, *thresdb, *makeup, *gainr, *toggle, *listen;

	double xover1;
	double xover2;
	double globalgain;
	double max;
	double limit;

	//double old_yl[MAX_COMP], old_y1[MAX_COMP], old_yg[MAX_COMP];
	double *old_yl, *old_y1, *old_yg;

	double oldxover1, oldxover2;

	double *outL;
	bool reset;
	//struct linear_svf simper[2][MAX_FILT];
	struct linear_svf **simper;

}t_zamulticomp;

void* zamulticomp_new(t_symbol* s, short argc, t_atom* argv);

void zamulticomp_dsp64(t_zamulticomp *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void zamulticomp_perform64(t_zamulticomp *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

void zamulticomp_free(t_zamulticomp* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void activate(t_zamulticomp *x);
void run_comp(t_zamulticomp *x, int k, double in, double *out);
void calc_lr4(t_zamulticomp *x, double f, int i);
void run_lr4(t_zamulticomp *x, int i, double in, double *outlo, double *outhi);
//void linear_svf_reset(struct linear_svf *self);
void linear_svf_set_xover(struct linear_svf *self, double sample_rate, double cutoff, double resonance);
double run_linear_svf_xover(struct linear_svf *self, double in, double mixlow, double mixhigh);

void att_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void rel_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void knee_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void ratio_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void thresh_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void makeup_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void locut(t_zamulticomp *x, double f1); 
void hicut(t_zamulticomp *x, double f2);
void bypass_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);
void master(t_zamulticomp *x, double v);
void listen_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamulticomp~", (method)zamulticomp_new, (method)zamulticomp_free, sizeof(t_zamulticomp), 0, 0);
	class_addmethod(c, (method)zamulticomp_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamulticomp_assist, "assist", A_CANT, 0);

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

	class_addmethod(c, (method)activate, "flreset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamulticomp_class = c;

	post("------------------------------------------------------------------");
	post("The Zam Multiband(3 bands) Mono Compressor for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamulticomp_dsp64(t_zamulticomp *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		zamulticomp_perform64, 0, NULL);
}

void zamulticomp_perform64(t_zamulticomp *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	t_double *output2 = outs[2];
	t_double *output3 = outs[3];
	int n = vectorsize;

	double maxx = x->max;

	int tog1 = (x->toggle[0] > 0.5) ? 0 : 1;
	int tog2 = (x->toggle[1] > 0.5) ? 0 : 1;
	int tog3 = (x->toggle[2] > 0.5) ? 0 : 1;

	int listen1 = (x->listen[0] > 0.5) ? 1 : 0;
	int listen2 = (x->listen[1] > 0.5) ? 1 : 0;
	int listen3 = (x->listen[2] > 0.5) ? 1 : 0;

	if (x->oldxover1 != x->xover1) {
		// recalculate coeffs
		calc_lr4(x, x->xover1, 0);
		x->oldxover1 = x->xover1;
	}
	if (x->oldxover2 != x->xover2) {
		// recalculate coeffs
		calc_lr4(x, x->xover2, 1);
		x->oldxover2 = x->xover2;
	}

	for (int i = 0; i < n; ++i) {
		double tmp1[2], tmp2[2], tmp3[2];
		double fil1[2], fil2[2], fil3[2], fil4[2];
		//double outL[MAX_COMP + 1] = { 0.0 };
		double* outL = x->outL;

		double inl = sanitize_denormal(input0[i]);
		inl = (fabs(inl) < DANGER) ? inl : 0.;

		int listenmode = 0;

		// Interleaved channel processing
		run_lr4(x, 0, inl, &fil1[0], &fil2[0]);
		run_lr4(x, 1, fil2[0], &fil3[0], &fil4[0]);

		//pushsample(x, x->outlevelold[0], fil1[0], 0);
		//x->outlevel[0] = averageabs(x->outlevelold[0]);
		//x->outlevel[0] = (x->outlevel[0] == 0.f) ? -45.0 : to_dB(x->outlevel[0]);
		if (tog1)
			run_comp(x, 0, fil1[0], &outL[0]);

		tmp1[0] = tog1 ? outL[0] * from_dB(x->makeup[0]) : fil1[0];

		//pushsample(x, x->outlevelold[1], fil3[0], 1);
		//x->outlevel[1] = averageabs(x->outlevelold[1]);
		//x->outlevel[1] = (x->outlevel[1] == 0.f) ? -45.0 : to_dB(x->outlevel[1]);
		if (tog2)
			run_comp(x, 1, fil3[0], &outL[1]);

		tmp2[0] = tog2 ? outL[1] * from_dB(x->makeup[1]) : fil3[0];

		//pushsample(x,x->outlevelold[2], fil4[0], 2);
		//x->outlevel[2] = averageabs(x->outlevelold[2]);
		//x->outlevel[2] = (x->outlevel[2] == 0.f) ? -45.0 : to_dB(x->outlevel[2]);
		if (tog3)
			run_comp(x, 2, fil4[0], &outL[2]);

		tmp3[0] = tog3 ? outL[2] * from_dB(x->makeup[2]) : fil4[0];

		output0[i] = 0.0;
		if (listen1) {
			listenmode = 1;
			output0[i] += outL[0] * tog1*from_dB(x->makeup[0])
				+ (1. - tog1) * tmp1[0];
		}
		if (listen2) {
			listenmode = 1;
			output0[i] += outL[1] * tog2*from_dB(x->makeup[1])
				+ (1. - tog2) * tmp2[0];
		}
		if (listen3) {
			listenmode = 1;
			output0[i] += outL[2] * tog3*from_dB(x->makeup[2])
				+ (1. - tog3) * tmp3[0];
		}
		if (!listenmode) {
			output0[i] = tmp1[0] + tmp2[0] + tmp3[0];
		}
		output0[i] = sanitize_denormal(output0[i]);
		output0[i] *= from_dB(x->globalgain);

		output1[i] = x->gainr[0];
		output2[i] = x->gainr[1];
		output3[i] = x->gainr[2];

		if (x->reset) {
			x->max = fabs(output0[i]);
			x->reset = false;
		}
		else {
			maxx = (fabs(output0[i]) > maxx) ? fabs(output0[i]) : sanitize_denormal(maxx);
		}
	}
	//x->out = (maxx <= 0.f) ? -160.f : to_dB(maxx);
}

void* zamulticomp_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamulticomp* x = (t_zamulticomp*)object_alloc(zamulticomp_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 1);
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
	x->old_y1 = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->old_yl = (double*)sysmem_newptr(MAX_COMP * sizeof(double));
	x->old_yg = (double*)sysmem_newptr(MAX_COMP * sizeof(double));

	x->simper = (struct linear_svf**)sysmem_newptr(2 * sizeof(struct linear_svf*));

	for (int i = 0; i < 2; i++) {
		x->simper[i] = (struct linear_svf*)sysmem_newptr(MAX_FILT * sizeof(struct linear_svf));
	}

	x->outL = (double*)sysmem_newptrclear((MAX_COMP + 1) * sizeof(double));

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
	x->globalgain = 0.0;
	/* Default variable values */
	x->max = 0.0;
	x->limit = 0.0;
	/* reset filter values */
	activate(x);
	return x;
}

void calc_lr4(t_zamulticomp *x, double f, int i)
{
	double srate = x->sample_rate;

	linear_svf_set_xover(&x->simper[0][i], srate, f, 0.25);
	linear_svf_set_xover(&x->simper[1][i], srate, f, 0.25);
}

void run_lr4(t_zamulticomp *x, int i, double in, double *outlo, double *outhi)
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
	self->s[0] = self->s[1] = 0.0;
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
	v[1] = -1. / (1. + g2 + g*k) * (-s0 + g*s1 - g*k*s0 + g2*vband + g*vhigh - g*vlow - g2*k*vlow);
	v[2] = -1. / (1. + g2 + g*k) * (-g*s0 - s1 - g*vband + g2*vhigh + g*k*vhigh - g2*vlow);
	self->s[0] = 2. * v[1] - s0;
	self->s[1] = 2. * v[2] - s1;

	return (double)(vhigh + v[2]);
}

void activate(t_zamulticomp *x)
{
	int i;
	for (i = 0; i < MAX_COMP; i++)
		x->old_yl[i] = x->old_y1[i] = x->old_yg[i] = 0.0;

	//x->old_ll = x->old_l1 = 0.0;
	
	for (i = 0; i < MAX_FILT; i++) {
		x->simper[0][i].k = 0.0;
		x->simper[0][i].g = 0.0;
		x->simper[0][i].s[0] = 0.0;
		x->simper[0][i].s[1] = 0.0;

		x->simper[1][i].k = 0.0;
		x->simper[1][i].g = 0.0;
		x->simper[1][i].s[0] = 0.0;
		x->simper[1][i].s[1] = 0.0;
	}
	
	x->max = 0.0;

	x->oldxover1 = 0.0;
	x->oldxover2 = 0.0;
	x->reset = true;
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

void run_comp(t_zamulticomp *x, int k, double in, double *out)
{
	double srate = x->sample_rate;
	double width = (6.0 * x->knee[k]) + 0.01;
	double attack_coeff = exp(-1000.0 / (x->attack[k] * srate));
	double release_coeff = exp(-1000.0 / (x->release[k] * srate));

	double checkwidth = 0.0;
	double cdb = 0.0;
	double Lgain = 1.0;
	double Lxg, Lyg;
	double Lxl, Lyl;

	Lyg = 0.0;
	in = sanitize_denormal(in);
	Lxg = (in == 0.0) ? -160.0 : to_dB(fabs(in));
	Lxg = sanitize_denormal(Lxg);

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

	Lxl = Lxg - Lyg;

	x->old_yl[k] = sanitize_denormal(x->old_yl[k]);

	if (Lxl < x->old_yl[k]) {
		Lyl = release_coeff * x->old_yl[k] + (1.0 - release_coeff)*Lxl;
	}
	else if (Lxl > x->old_yl[k]) {
		Lyl = attack_coeff * x->old_yl[k] + (1.0 - attack_coeff)*Lxl;
	}
	else {
		Lyl = Lxl;
	}
	Lyl = sanitize_denormal(Lyl);

	cdb = -Lyl;
	Lgain = from_dB(cdb);

	//x->gainr[k] = Lyl;
	x->gainr[k] = Lgain;

	*out = in * Lgain;

	x->old_yl[k] = Lyl;
	x->old_yg[k] = Lyg;
}

void att_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void rel_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void knee_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void ratio_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void thresh_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void makeup_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
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

void locut(t_zamulticomp *x, double hz1)
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

void hicut(t_zamulticomp *x, double hz2)
{

	if (hz2 < 1400.0) {
		hz2 = 1400.0;
		post("High Crossover Minimum: 1400.0");
	}
	x->xover2 = hz2;
}

void bypass_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *toggles = x->toggle;

	for (i = 0; i < argc; i++) {
		toggles[i] = atom_getfloat(argv + i);
	}
}

void master(t_zamulticomp *x, double v)
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

void listen_list(t_zamulticomp *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	double *listen = x->listen;

	for (i = 0; i < argc; i++) {
		listen[i] = atom_getfloat(argv + i);
	}
}

void zamulticomp_free(t_zamulticomp* x)
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
	sysmem_freeptr(x->simper);
	sysmem_freeptr(x->outL);
}