#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

//#define MAXCHAN 64

static t_class *darc_class;

typedef struct _darc
{
	t_pxobject x_obj;

	double sample_rate;

	int n_channels;
	double norm_input;

	double ratio;
	double p_rat;

	bool hold;

	double igain;
	double p_ign;
	double l_ign;

	double p_thr;
	double l_thr;

	double w_att;
	double w_rel;

	double t_att;
	double t_rel;

	double za1;
	double zr1;
	double zr2;

	bool  newg;
	double gmax;
	double gmin;

	double rms;
	double w_rms;
	double w_lpf;

	bool BYPASS;
}t_darc;

void darc_dsp64(t_darc *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void darc_dsp(t_darc *x, t_signal **sp, short *count);

void darc_perform64(t_darc *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *darc_perform(t_int *w);

//void Dyncomp_init(t_darc* x, double sample_rate, int n_channels);
//void Dyncomp_get_gain(t_darc* x, float* gmin, float* gmax, float* rms);
void Dyncomp_set_inputgain(t_darc* x, double g);
void Dyncomp_set_attack(t_darc* x, double a);
void Dyncomp_set_release(t_darc* x, double r);
void Dyncomp_set_ratio(t_darc* x, double r);
void Dyncomp_set_threshold(t_darc* x, double t);
void Dyncomp_set_hold(t_darc* x, bool hold);
void Dyncomp_reset(t_darc* x);

void darcBypass(t_darc *x, bool bypass);
void darc_free(t_darc* x);
void* darc_new(t_symbol* s, short argc, t_atom* argv);

void darc_assist(t_darc *x, void *b, long msg, long arg, char *dst);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("darc~", (method)darc_new, (method)darc_free, sizeof(t_darc), 0, A_GIMME, 0);
	//class_addmethod(c, (method)darc_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)darc_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(c, (method)darc_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)Dyncomp_set_inputgain, "InputGain", A_FLOAT, 0);
	class_addmethod(c, (method)Dyncomp_set_threshold, "Threshold", A_FLOAT, 0);
	class_addmethod(c, (method)Dyncomp_set_ratio, "Ratio", A_FLOAT, 0);

	class_addmethod(c, (method)Dyncomp_set_release, "Release", A_FLOAT, 0);
	class_addmethod(c, (method)Dyncomp_set_attack, "Attack", A_FLOAT, 0);

	class_addmethod(c, (method)darcBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)Dyncomp_set_hold, "Hold", A_LONG, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	darc_class = c;

	post("------------------------------------------------------------------");
	post("The darc compressor by Robin Gareus");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void darc_dsp64(t_darc *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		darc_perform64, 0, NULL);
}

/*
void darc_dsp(t_darc *x, t_signal **sp, short *count)
{
	dsp_add(darc_perform, 5, x,
		sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,
		sp[0]->s_n);
}
*/

void darc_perform64(t_darc *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	double gmin, gmax;

	t_double **input, **output;
	//double out[MAXCHAN];
	int n = vectorsize;
	
	input = (t_double**)sysmem_newptrclear(sizeof(t_double *) * x->n_channels);
	output = (t_double**)sysmem_newptrclear(sizeof(t_double *) * x->n_channels);

	/*
	for (int j = 0; j < x->n_channels; j++) {
		input[j] = (t_double *)sysmem_newptrclear(sizeof(t_double) * n);
		output[j] = (t_double*)sysmem_newptrclear(sizeof(t_double) * n);
	}
	*/

	for (int j = 0; j < x->n_channels; j++) {
		input[j] = ins[j];
		output[j] = outs[j];
	}
	t_double *gr = outs[x->n_channels];

	
	if (!x->BYPASS) {
		/* reset min/max gain report */
		if (x->newg) {
			gmax = -100.0;
			gmin = 100.0;
			x->newg = false;
		}
		else {
			gmax = x->gmax;
			gmin = x->gmin;
		}
		/* interpolate input gain */
		double       g = x->igain;
		const double g1 = x->p_ign;
		double       dg = g1 - g;
		if (fabs(dg) < 1e-5 || (g > 1.0 && fabs(dg) < 1e-3)) {
			g = g1;
			dg = 0;
		}

		/* interpolate ratio */
		double r = x->ratio;
		const double r1 = x->p_rat;
		double dr = r1 - r;
		if (fabs(dr) < 1e-5) {
			r = r1;
			dr = 0;
		}

		/* localize variables */
		double za1 = x->za1;
		double zr1 = x->zr1;
		double zr2 = x->zr2;

		double rms = x->rms;

		const double w_rms = x->w_rms;
		const double w_lpf = x->w_lpf;
		const double w_att = x->w_att;
		const double w_rel = x->w_rel;
		const double p_thr = x->p_thr;

		const double p_hold = x->hold ? 2.0 * p_thr : 0.0;

		const int nc = x->n_channels;
		const double n_1 = x->norm_input;
		
		for (int i = 0; i < n; ++i) {
		//while(n--){
			/* update input gain */
			if (dg != 0) {
				g += w_lpf * (g1 - g);
			}
			/* Input/Key RMS */
			double v = 0.0;
			for (int j = 0; j < nc; ++j) {
				const double xi = g * input[j][i];
				v += xi * xi;
			}

			v *= n_1;

			rms += w_rms * (v - rms);
			za1 += w_att * (p_thr + v - za1);

			/* hold release */
			const bool hold = (za1 < p_hold);

			/* Note: za1 >= p_thr; so zr1, zr2 can't become denormal */
			if (zr1 < za1) {
				zr1 = za1;
			}
			else if (!hold) {
				zr1 -= w_rel * zr1;
			}

			if (zr2 < za1) {
				zr2 = za1;
			}
			else if (!hold) {
				zr2 += w_rel * (zr1 - zr2);
			}

			/* update ratio */
			if (dr != 0) {
				r += w_lpf * (r1 - r);
			}

			/* Note: expf (a * logf (b)) == powf (b, a);
			 * however powf() is significantly slower
			 *
			 * Effective gain is  (zr2) ^ (-ratio).
			 *
			 * with 0 <= ratio <= 0.5 and
			 * zr2 being low-pass filtered RMS of the key-signal.
			 */

			double pg = -r * log(20.0 * zr2);

			gmax = max(gmax, pg);
			gmin = min(gmin, pg);

			pg = g * exp(pg);

			gr[i] = pg;

			/* apply gain factor to all channels */
			for (int j = 0; j < nc; ++j) {
				output[j][i] = pg * input[j][i];
				
				/*
				if (rms > 1e-8f) {
					gr[i] = 10.f * log10f(2.f * rms * x->norm_input);
				}
				else {
					gr[i] = -80;
				}*/
			}
		}

		/* copy back variables */
		x->za1 = za1;
		x->zr1 = zr1;
		x->zr2 = zr2;
		x->igain = g;
		x->ratio = r;
		x->gmax = gmax;
		x->gmin = gmin;
		x->rms = rms + 1e-12; // + denormal protection
	}
	else {
		while (n--) {
			for (int j = 0; j < x->n_channels; j++) {
				*output[j]++ = *input[j]++;
			}
		}
	}
}

void* darc_new(t_symbol* s, short argc, t_atom* argv)
{
	t_darc* x = (t_darc*)object_alloc(darc_class);

	double numChan = 1;
	atom_arg_getdouble(&numChan, 0, argc, argv);

	//if (numChan > MAXCHAN) numChan = MAXCHAN;

	x->n_channels = (int)numChan;

	dsp_setup((t_pxobject *)x, x->n_channels);

	for (int j = 0; j < x->n_channels + 1; j++) {
		outlet_new((t_object *)x, "signal");
	}

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors
	
	x->sample_rate = sys_getsr();

	x->norm_input = 1.0 / x->n_channels;

	x->ratio = 0.;
	x->p_rat = 0.;

	x->igain = 1.0;
	x->p_ign = 1.0;
	x->l_ign = 0.;

	x->l_thr = -10.0;
	x->p_thr = 0.05;

	x->hold = false;

	x->t_att = 0.;
	x->t_rel = 0.;

	x->w_rms = 5.0 / x->sample_rate;
	x->w_lpf = 160.0 / x->sample_rate;

	Dyncomp_set_attack(x, 10.0);
	Dyncomp_set_release(x, 300.0);
	Dyncomp_reset(x);

	x->BYPASS = false;

	return x;
}


void Dyncomp_reset(t_darc* x)
{
	x->za1 = 0.;
	x->zr1 = 0.;
	x->zr2 = 0.;
	x->rms = 0.;
	x->gmin = 0.;
	x->gmax = 0.;
	x->newg = true;
}

void Dyncomp_set_inputgain(t_darc* x, double g)
{
	if (g > 30.0) {
		g = 30.0;
		post("setting input gain to 30");
	}
	if (g < -50.0) {
		g = -50.0;
		post("setting input gain to -50.0");
	}

	if (g == x->l_ign) {
		return;
	}
	x->l_ign = g;
	x->p_ign = pow(10.0, 0.05 * g);
}

void Dyncomp_set_threshold(t_darc* x, double t)
{

	if (t > 0.5) {
		t = 0.5;
		post("setting threshold to 0.5");
	}
	if (t < -70.0) {
		t = -70.0;
		post("setting threshold to -70.0");
	}

	if (t == x->l_thr) {
		return;
	}
	x->l_thr = t;
	x->p_thr = 0.5 * pow(10.0, 0.1 * t);
}

void Dyncomp_set_ratio(t_darc* x, double r)
{
	if (r < 1.0) {
		r = 1.0;
		post("setting ratio to 1.0");
	}
	else if (r >= 144.0) {
		r = 144.0;
		post("setting ratio to infinity");
	}
	double ratio = sqrt(r/ 72.0);
	ratio = min(ratio, 2.01);
	x->p_rat = 0.5 * ratio;
}

void Dyncomp_set_attack(t_darc* x, double a)
{
	if (a < 1.0) {
		a = 1.0;
		post("setting attack to 1.0 ms");
	}
	double atsec = a / 1000.0;

	if (atsec == x->t_att) {
		return;
	}
	x->t_att = atsec;
	x->w_att = 0.5 / (x->sample_rate * atsec);
}

void Dyncomp_set_release(t_darc* x, double r)
{
	if (r < 1.0) {
		r = 1.0;
		post("setting release to 1.0 ms");
	}
	double rlssec = r / 1000.0;

	if (rlssec == x->t_rel) {
		return;
	}
	x->t_rel = rlssec;
	x->w_rel = 3.5 / (x->sample_rate * rlssec);
}

void Dyncomp_set_hold(t_darc* x, bool hold)
{
	x->hold = hold;
}

void darcBypass(t_darc *x, bool bypass)
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

void darc_free(t_darc* x)
{
	dsp_free((t_pxobject*)x);

}

void darc_assist(t_darc *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {

		for (int i = 0; i < x->n_channels; i++) {
			if (arg == i) sprintf(dst, "(signal) Input %d", i + 1);
		}
	}
	else if (msg == ASSIST_OUTLET) {
		for (int i = 0; i < x->n_channels; i++) {
			if (arg == i) sprintf(dst, "(signal) Output %d", i + 1);
		}
		if (arg == x->n_channels) sprintf(dst, "(signal) Gain Reduction");
	}
}