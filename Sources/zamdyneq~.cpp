#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

static t_class *zamdyneq_class;

typedef struct _zamdyneq
{
	t_pxobject x_obj;

	double sample_rate;

	double attack, release, knee, ratio, thresdb, max, slewfactor, detectfreq, targetfreq, targetwidth, controlgain; //parameters
	double oldL_yl, oldL_y1, oldL_yg;

	double x1, x2, y1, y2;
	double x1a, x2a, y1a, y2a;
	double zln1, zln2, zld1, zld2;
	double zhn1, zhn2, zhd1, zhd2;
	double a0x, a1x, a2x, b0x, b1x, b2x, gainx;
	double a0y, a1y, a2y, b0y, b1y, b2y, gainy;

	double *Bl,*Al,*Bh,*Ah;
	double gainred;
	
	bool toglow, togpeak, toghigh;
	//bool toggles[3];
	bool *toggles;
	bool boostcut;
	short side_connected;
	bool sidechain;
	bool BYPASS;
}t_zamdyneq;

struct linear_svf {
	double g, k;
	double a[3];
	double m[3];

	double s[2][2];
};

struct linear_svf bandpass;

void* zamdyneq_new(t_symbol* s, short argc, t_atom* argv);

void zamdyneq_dsp64(t_zamdyneq *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zamdyneq_dsp(t_zamdyneq *x, t_signal **sp, short *count);

void zamdyneq_perform64(t_zamdyneq *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zamdyneq_perform(t_int *w);
void zamdyneq_free(t_zamdyneq* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void linear_svf_set_params(struct linear_svf *self, double sample_rate, double cutoff, double resonance);
void linear_svf_reset(struct linear_svf *self);
double run_linear_svf(struct linear_svf *self, int c, double in);

void set_attack(t_zamdyneq *x, double att);
void set_release(t_zamdyneq *x, double rls);
void set_threshold(t_zamdyneq *x, double v);
void set_knee(t_zamdyneq *x, double kn);
void set_ratio(t_zamdyneq *x, double rt);
void set_slew(t_zamdyneq *x, double sl);
void set_maxboostcut(t_zamdyneq *x, double mxbt);

void set_detfreq(t_zamdyneq *x, double dtfr);
void set_tarfreq(t_zamdyneq *x, double tarfr);
void set_tarwidth(t_zamdyneq *x, double tawdth);

void t_boostcut(t_zamdyneq *x, bool bstcut);
void lopehi_toggle(t_zamdyneq *x, t_symbol *msg, short argc, t_atom *argv);

//void t_low(t_zamdyneq *x, bool tlow);
//void t_peak(t_zamdyneq *x, bool tpeak);
//void t_high(t_zamdyneq *x, bool thigh);
void t_sidechn(t_zamdyneq *x, bool sd);
void zamBypass(t_zamdyneq *x, bool bypass);

void peq(double G0, double G, double GB, double w0, double Dw, double *a0, double *a1, double *a2, double *b0, double *b1, double *b2, double *gn);
void highshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]);
void lowshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]);

void run_comp(t_zamdyneq *x,double input, double* output);
void run_highshelf(t_zamdyneq *x, double input, double* output);
void run_lowshelf(t_zamdyneq *x, double input, double* output);
void run_peq2(t_zamdyneq *x, double input, double* output);
void reset_low(t_zamdyneq *x);
void reset_high(t_zamdyneq *x);
void reset_peak(t_zamdyneq *x);
void activate(t_zamdyneq *x);


void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamdyneq~", (method)zamdyneq_new, (method)zamdyneq_free, sizeof(t_zamdyneq), 0, 0);
	class_addmethod(c, (method)zamdyneq_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamdyneq_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)set_attack, "attack", A_FLOAT, 0);
	class_addmethod(c, (method)set_release, "release", A_FLOAT, 0);
	class_addmethod(c, (method)set_threshold, "thresh", A_FLOAT, 0);
	class_addmethod(c, (method)set_knee, "knee", A_FLOAT, 0);
	class_addmethod(c, (method)set_ratio, "ratio", A_FLOAT, 0);
	class_addmethod(c, (method)set_slew, "slew", A_FLOAT, 0);
	class_addmethod(c, (method)set_maxboostcut, "max", A_FLOAT, 0);

	class_addmethod(c, (method)set_detfreq, "detectfreq", A_FLOAT, 0);
	class_addmethod(c, (method)set_tarfreq, "targetfreq", A_FLOAT, 0);
	class_addmethod(c, (method)set_tarwidth, "targetwidth", A_FLOAT, 0);

	class_addmethod(c, (method)t_boostcut, "cut/boost", A_LONG, 0);
	class_addmethod(c, (method)lopehi_toggle, "low/peak/high", A_GIMME, 0);
	//class_addmethod(c, (method)t_low, "togglelow", A_LONG, 0);
	//class_addmethod(c, (method)t_high, "togglehigh", A_LONG, 0);
	//class_addmethod(c, (method)t_peak, "togglepeak", A_LONG, 0);

	class_addmethod(c, (method)t_sidechn, "sidechain", A_LONG, 0);
	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);

	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamdyneq_class = c;

	post("------------------------------------------------------------------");
	post("The Zam Dynamic Equalizer for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamdyneq_dsp64(t_zamdyneq *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	x->side_connected = count[1];

	object_method(dsp64, gensym("dsp_add64"), x,
		zamdyneq_perform64, 0, NULL);
}

void zamdyneq_perform64(t_zamdyneq *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];

	int n = vectorsize;
	double srate = x->sample_rate;

	if (!x->BYPASS) {
		double dcgain = 1.0;
		double qq2, boost2, fc2, w02, bwgain2, bw2;
		double boostl, All, bwl, bwgaindbl;
		double boosth, Ahh, bwh, bwgaindbh;
		int choose = 0;
		if (x->side_connected) {
			choose = (!x->sidechain) ? 0 : 1;
		}
		double gaincontrol = 0.;

		linear_svf_set_params(&bandpass, srate, x->detectfreq, 4.);

		
		for (int i = 0; i < n; i++) {
			double tmp, filtered, out;
			double in = input0[i];
			in = sanitize_denormal(in);
			if (choose == 0) tmp = sanitize_denormal(input0[i]);
			else if (choose == 1) tmp = sanitize_denormal(input1[i]);
			filtered = tmp;
			out = in;

			filtered = run_linear_svf(&bandpass, 0, tmp);
			filtered = run_linear_svf(&bandpass, 1, filtered);
			run_comp(x, filtered, &tmp);

			gaincontrol = tmp / filtered;
			x->controlgain = sanitize_denormal(to_dB(gaincontrol));
			if (x->boostcut) {
				x->controlgain = -(x->controlgain);
				if (x->controlgain < 0.)
					x->controlgain = 0.;
				else if (x->controlgain > x->max)
					x->controlgain = x->max;
			}
			else {
				if (x->controlgain > 0.)
					x->controlgain = 0.;
				else if (x->controlgain < -(x->max))
					x->controlgain = -(x->max);
			}

			if (x->controlgain != 0.) {
				if (x->toglow) {
					bwl = TWOPI*x->targetfreq / srate;
					boostl = from_dB(x->controlgain);
					All = sqrt(boostl);
					bwgaindbl = to_dB(All);
					lowshelfeq(0.f, x->controlgain, bwgaindbl, bwl, bwl, 0.707, x->Bl, x->Al);
					run_lowshelf(x, in, &out);
					x->toghigh = false;
					x->togpeak = false;
				}
				else if (x->togpeak) {
					qq2 = pow(2.0, 1.0 / x->targetwidth) / (pow(2.0, x->targetwidth) - 1.0); //q from octave bw
					boost2 = from_dB(x->controlgain);
					fc2 = x->targetfreq / srate;
					w02 = fc2 * TWOPI;
					bwgain2 = sqrt(boost2);
					bw2 = fc2 / qq2;
					peq(dcgain, boost2, bwgain2, w02, bw2, &x->a0y, &x->a1y, &x->a2y, &x->b0y, &x->b1y, &x->b2y, &x->gainy);
					run_peq2(x, in, &out);
					x->toglow = false;
					x->toghigh = false;
				}
				else if(x->toghigh){
					bwh = TWOPI*x->targetfreq / srate;
					boosth = from_dB(x->controlgain);
					Ahh = sqrt(boosth);
					bwgaindbh = to_dB(Ahh);
					highshelfeq(0.0, x->controlgain, bwgaindbh, bwh, bwh, 0.707, x->Bh, x->Ah);
					run_highshelf(x, in, &out);
					x->toglow = false;
					x->togpeak = false;
				}
				else {
					x->togpeak = true;

					reset_low(x);
					reset_high(x);
					reset_peak(x);
				}
			}
			output0[i] = (double)out;
			output1[i] = x->gainred;
		}
	}
	else {
		while (n--) {
			*output0++ = *input0++;
		}
	}
}

void linear_svf_set_params(struct linear_svf *self, double sample_rate, double cutoff, double resonance)
{

	self->g = tan(PI * (cutoff / sample_rate));
	self->k = 1.0 / resonance;

	self->a[0] = 1.0 / (1.0 + self->g * (self->g + self->k));
	self->a[1] = self->g * self->a[0];
	self->a[2] = self->g * self->a[1];

	self->m[0] = 0.0;
	self->m[1] = 1.0;
	self->m[2] = 0.0;
}


void linear_svf_reset(struct linear_svf *self)
{
	self->s[0][0] =
		self->s[0][1] =
		self->s[1][0] =
		self->s[1][1] = 0.0;
}

double run_linear_svf(struct linear_svf *self, int c, double in)
{
	double v[3];

	v[2] = in - self->s[c][1];
	v[0] = (self->a[0] * self->s[c][0]) + (self->a[1] * v[2]);
	v[1] = self->s[c][1] + (self->a[1] * self->s[c][0]) + (self->a[2] * v[2]);

	self->s[c][0] = (2.0 * v[0]) - self->s[c][0];
	self->s[c][1] = (2.0 * v[1]) - self->s[c][1];

	return
		(self->m[0] * in)
		+ (self->m[1] * v[0])
		+ (self->m[2] * v[1]);
}

void peq(double G0, double G, double GB, double w0, double Dw, double *a0, double *a1, double *a2, double *b0, double *b1, double *b2, double *gn) 
{

	double F, G00, F00, num, den, G1, G01, G11, F01, F11, W2, Dww, C, D, B, A;
	F = fabs(G*G - GB * GB);
	G00 = fabs(G*G - G0 * G0);
	F00 = fabs(GB*GB - G0 * G0);
	num = G0 * G0 * (w0*w0 - PI*PI)*(w0*w0 - PI*PI)
		+ G * G * F00 * PI*PI * Dw*Dw / F;
	den = (w0*w0 - PI*PI)*(w0*w0 - PI*PI)
		+ F00 * PI*PI * Dw*Dw / F;
	G1 = sqrt(num / den);
	G01 = fabs(G*G - G0 * G1);
	G11 = fabs(G*G - G1 * G1);
	F01 = fabs(GB*GB - G0 * G1);
	F11 = fabs(GB*GB - G1 * G1);
	W2 = sqrt(G11 / G00) * tan(w0 / 2.0)*tan(w0 / 2.0);
	Dww = (1.0 + sqrt(F00 / F11) * W2) * tan(Dw / 2.0);
	C = F11 * Dww*Dww - 2.0 * W2 * (F01 - sqrt(F00 * F11));
	D = 2.0 * W2 * (G01 - sqrt(G00 * G11));
	A = sqrt((C + D) / F);
	B = sqrt((G*G * C + GB * GB * D) / F);
	*gn = G1;
	*b0 = (G1 + G0 * W2 + B) / (1.0 + W2 + A);
	*b1 = -2.0*(G1 - G0 * W2) / (1.0 + W2 + A);
	*b2 = (G1 - B + G0 * W2) / (1.0 + W2 + A);
	*a0 = 1.0;
	*a1 = -2.0*(1.0 - W2) / (1.0 + W2 + A);
	*a2 = (1 + W2 - A) / (1.0 + W2 + A);

	*b1 = sanitize_denormal(*b1);
	*b2 = sanitize_denormal(*b2);
	*a0 = sanitize_denormal(*a0);
	*a1 = sanitize_denormal(*a1);
	*a2 = sanitize_denormal(*a2);
	*gn = sanitize_denormal(*gn);
	if (IS_DENORM_NAN_SAMPLE(*b0)) { *b0 = 1.0; }
}

void lowshelfeq(double, double G, double, double w0, double, double q, double B[], double A[]) {
	double alpha, b0, b1, b2, a0, a1, a2;
	G = pow(10.0, G / 20.0);
	double AA = sqrt(G);

	alpha = sin(w0) / 2.0 * sqrt((AA + 1.0 / AA)*(1.0 / q - 1.0) + 2.0);
	b0 = AA * ((AA + 1.0) - (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha);
	b1 = 2.0*AA*((AA - 1.0) - (AA + 1.0)*cos(w0));
	b2 = AA * ((AA + 1.0) - (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha);
	a0 = (AA + 1.0) + (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha;
	a1 = -2.0*((AA - 1.0) + (AA + 1.0)*cos(w0));
	a2 = (AA + 1.0) + (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha;

	B[0] = b0 / a0;
	B[1] = b1 / a0;
	B[2] = b2 / a0;
	A[0] = 1.0;
	A[1] = a1 / a0;
	A[2] = a2 / a0;
}

void highshelfeq(double, double G, double, double w0, double, double q, double B[], double A[])
{
	double alpha, b0, b1, b2, a0, a1, a2;
	G = pow(10.0, G / 20.0);
	double AA = sqrt(G);

	alpha = sin(w0) / 2.0 * sqrt((AA + 1.0 / AA)*(1.0 / q - 1.0) + 2.0);
	b0 = AA * ((AA + 1.0) + (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha);
	b1 = -2.0*AA*((AA - 1.0) + (AA + 1.0)*cos(w0));
	b2 = AA * ((AA + 1.0) + (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha);
	a0 = (AA + 1.0) - (AA - 1.0)*cos(w0) + 2.0*sqrt(AA)*alpha;
	a1 = 2.0*((AA - 1.0) - (AA + 1.0)*cos(w0));
	a2 = (AA + 1.0) - (AA - 1.0)*cos(w0) - 2.0*sqrt(AA)*alpha;

	B[0] = b0 / a0;
	B[1] = b1 / a0;
	B[2] = b2 / a0;
	A[0] = 1.0;
	A[1] = a1 / a0;
	A[2] = a2 / a0;
}

void run_lowshelf(t_zamdyneq *x, double input, double* output)
{
	double in = input;
	x->zln1 = sanitize_denormal(x->zln1);
	x->zln2 = sanitize_denormal(x->zln2);
	x->zld1 = sanitize_denormal(x->zld1);
	x->zld2 = sanitize_denormal(x->zld2);
	in = sanitize_denormal(in);

	*output = in * x->Bl[0] +
		x->zln1 * x->Bl[1] +
		x->zln2 * x->Bl[2] -
		x->zld1 * x->Al[1] -
		x->zld2 * x->Al[2] + 1e-20;

	*output = sanitize_denormal(*output);
	x->zln2 = x->zln1;
	x->zld2 = x->zld1;
	x->zln1 = in;
	x->zld1 = *output;
}

void run_highshelf(t_zamdyneq *x,double input, double* output)
{
	double in = input;
	x->zhn1 = sanitize_denormal(x->zhn1);
	x->zhn2 = sanitize_denormal(x->zhn2);
	x->zhd1 = sanitize_denormal(x->zhd1);
	x->zhd2 = sanitize_denormal(x->zhd2);
	in = sanitize_denormal(in);

	*output = in * x->Bh[0] +
		x->zhn1 * x->Bh[1] +
		x->zhn2 * x->Bh[2] -
		x->zhd1 * x->Ah[1] -
		x->zhd2 * x->Ah[2] + 1e-20;

	*output = sanitize_denormal(*output);
	x->zhn2 = x->zhn1;
	x->zhd2 = x->zhd1;
	x->zhn1 = in;
	x->zhd1 = *output;
}

void run_peq2(t_zamdyneq *x,double input, double* output)
{
	double in = input;
	x->x1a = sanitize_denormal(x->x1a);
	x->x2a = sanitize_denormal(x->x2a);
	x->y1a = sanitize_denormal(x->y1a);
	x->y2a = sanitize_denormal(x->y2a);
	in = sanitize_denormal(in);

	*output = in * x->b0y + x->x1a * x->b1y + x->x2a * x->b2y - x->y1a * x->a1y - x->y2a * x->a2y + 1e-20;

	*output = sanitize_denormal(*output);
	x->x2a = x->x1a;
	x->y2a = x->y1a;
	x->x1a = in;
	x->y1a = *output;
}

void run_comp(t_zamdyneq *x ,double input, double* output)
{
	double srate = x->sample_rate;
	double width = (6.0 * x->knee) + 0.01;
	double slewwidth = 1.8;
		double cdb = 0.0;
		double attack_coeff = exp(-1000.0 / (x->attack * srate));
		double release_coeff = exp(-1000.0 / (x->release * srate));

		int attslew = 0;
	double lgaininp = 0.0;
	double Lgain = 1.0;
		double Lxg, Lxl, Lyg, Lyl, Ly1;
		double checkwidth = 0.0;
	double in;

		in = input;
		attslew = 0;
	Lyg = 0.0;
	Lxg = (in == 0.0) ? -160.0 : to_dB(fabs(in));
		Lxg = sanitize_denormal(Lxg);

		Lyg = Lxg + (1.0 / x->ratio - 1.0)*(Lxg - x->thresdb + width / 2.0)*(Lxg - x->thresdb + width / 2.0) / (2.0*width);

	checkwidth = 2.0*fabs(Lxg - x->thresdb);
		if (2.0*(Lxg - x->thresdb) < -width) {
			Lyg = Lxg;
		}
		else if (checkwidth <= width) {
			Lyg = x->thresdb + (Lxg - x->thresdb) / x->ratio;
			Lyg = sanitize_denormal(Lyg);
			if (checkwidth <= slewwidth) {
				if (Lyg >= x->oldL_yg) {
					attslew = 1;
				}
			}
		}
		else if (2.0*(Lxg - x->thresdb) > width) {
			Lyg = x->thresdb + (Lxg - x->thresdb) / x->ratio;
			Lyg = sanitize_denormal(Lyg);
		}

		attack_coeff = attslew ? exp(-1000.0 / ((x->attack + 2.0*(x->slewfactor - 1)) * srate)) : attack_coeff;

		Lxl = Lxg - Lyg;

		x->oldL_y1 = sanitize_denormal(x->oldL_y1);
		x->oldL_yl = sanitize_denormal(x->oldL_yl);
		Ly1 = max(Lxl, release_coeff * x->oldL_y1 + (1.0 - release_coeff)*Lxl);
		Lyl = attack_coeff * x->oldL_yl + (1.0 - attack_coeff)*Ly1;
		Ly1 = sanitize_denormal(Ly1);
		Lyl = sanitize_denormal(Lyl);

		cdb = -Lyl;
		Lgain = from_dB(cdb);

		x->gainred = Lgain;

		lgaininp = Lgain * in;
		*output = lgaininp;

		x->oldL_yl = Lyl;
		x->oldL_y1 = Ly1;
		x->oldL_yg = Lyg;
}

void reset_low(t_zamdyneq *x)
{
	x->zln1 = x->zln2 = x->zld1 = x->zld2 = 0.0;
}

void reset_high(t_zamdyneq *x)
{
	x->zhn1 = x->zhn2 = x->zhd1 = x->zhd2 = 0.0;
}

void reset_peak(t_zamdyneq *x)
{
	x->x1a = x->x2a = x->y1a = x->y2a = 0.0;
}

void activate(t_zamdyneq *x)
{
	x->oldL_yl = x->oldL_y1 = x->oldL_yg = 0.0;

	x->gainy = 0.0;

	reset_low(x);
	reset_high(x);
	reset_peak(x);
	linear_svf_reset(&bandpass);
}

void* zamdyneq_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamdyneq* x = (t_zamdyneq*)object_alloc(zamdyneq_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 2);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();


	x->Bl = (double*)sysmem_newptr(3 * sizeof(double));
	x->Al = (double*)sysmem_newptr(3 * sizeof(double));
	x->Bh = (double*)sysmem_newptr(3 * sizeof(double));
	x->Ah = (double*)sysmem_newptr(3 * sizeof(double));

	x->toggles = (bool*)sysmem_newptr(3 * sizeof(bool));

	x->attack = 10.0;
	x->release = 80.0;
	x->knee = 0.0;
	x->ratio = 1.5;
	x->thresdb = 0.0;

	x->slewfactor = 1.0;
	x->sidechain = false;

	x->max = 10.0;
	x->detectfreq = 1000.0;
	x->targetfreq = 1000.0;
	x->targetwidth = 1.0;
	x->controlgain = 0.0;

	x->boostcut = false;
	x->togpeak = true;
	x->toglow = false;
	x->toghigh = false;

	x->BYPASS = false;
	/* reset filter values */
	activate(x);
	return x;
}

void zamBypass(t_zamdyneq *x, bool bypass)
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

double sanitize_denormal(double v) {
	if (IS_DENORM_NAN_SAMPLE(v))
		return 0.f;
	return v;
}

double from_dB(double gdb) {
	return (exp(gdb / 20.0*log(10.0)));
}

double to_dB(double g) {
	return (20.0*log10(g));
}

void set_attack(t_zamdyneq *x, double att)
{
	if (att > 1000.0) {
		att = 1000.0;
		post("Maximum Attack Time: 1000 ms");
	}
	else if (att < 0.1) {
		att = 0.1;
		post("Minimum Atack Time: 0.1 ms");
	}
	x->attack = att;
}

void set_release(t_zamdyneq *x, double rls)
{
	if (rls > 2000.0) {
		rls = 2000.0;
		post("Maximum Release Time: 2000 ms");
	}
	else if (rls < 1.0) {
		rls = 1.0;
		post("Minimum Release Time: 1 ms");
	}
	x->release = rls;
}

void set_knee(t_zamdyneq *x, double kn)
{
	if (kn > 12.0) {
		kn = 12.0;
		post("Maximum Knee Value: 12 db");
	}
	if (kn < 0.0) {
		kn = 0.0;
		post("Minimum Knee Value: 0 db");
	}
	x->knee = kn;
}

void set_ratio(t_zamdyneq *x, double rt)
{
	if (rt > 10.0) {
		rt = 10.0;
		post("Maximum Ratio Value: 10");
	}
	if (rt < 1.0) {
		rt = 1.0;
		post("Minimum Ratio Value: 1");
	}
	x->ratio = rt;
}

void set_threshold(t_zamdyneq *x, double v)
{
	if (v > 0.0) {
		v = 0.0;
		post("Maximum Threshold Value: 0 db");
	}
	if (v < -80.0) {
		v = -80.0;
		post("Minimum Threshold Value: -80 db");
	}
	x->thresdb = v;
}

void set_maxboostcut(t_zamdyneq *x, double mxbt)
{
	if (mxbt > 10.0) {
		mxbt = 10.0;
		post("Maximum Max Boost/Cut Value: 10 db");
	}
	if (mxbt < 0.0) {
		mxbt = 0.0;
		post("Minimum Max Boost/Cut Value: 0 db");
	}
	x->max = mxbt;
}

void set_slew(t_zamdyneq *x, double sl)
{
	if (sl > 150.0) {
		sl = 150.0;
		post("Maximum Slew Value: 150");
	}
	if (sl < 1.0) {
		sl = 1.0;
		post("Minimum Slew Value: 1");
	}
	x->slewfactor = sl;
}

void set_detfreq(t_zamdyneq *x, double dtfr)
{
	if (dtfr > 16000.0) {
		dtfr = 16000.0;
		post("Maximum Detection Frequency Value: 16000 Hz");
	}
	if (dtfr < 20.0) {
		dtfr = 20.0;
		post("Minimum Detection Frequency Value: 20 Hz");
	}
	x->detectfreq = dtfr;
}

void set_tarfreq(t_zamdyneq *x, double tarfr)
{
	if (tarfr > 16000.0) {
		tarfr = 16000.0;
		post("Maximum Detection Frequency Value: 16000 Hz");
	}
	if (tarfr < 20.0) {
		tarfr = 20.0;
		post("Minimum Detection Frequency Value: 20 Hz");
	}
	x->targetfreq = tarfr;
}

void set_tarwidth(t_zamdyneq *x, double tawdth)
{
	if (tawdth > 5.0) {
		tawdth = 5.0;
		post("Maximum Target Width Value: 5 Oct");
	}
	if (tawdth < 1.0) {
		tawdth = 1.0;
		post("Minimum Target Width Value: 1 Oct");
	}
	x->targetwidth = tawdth;
}

void t_boostcut(t_zamdyneq *x, bool bstcut)
{

	x->boostcut = bstcut;

	reset_low(x);
	reset_high(x);
	reset_peak(x);
}

void t_sidechn(t_zamdyneq *x, bool sd)
{
	bool t_sd;
	if (sd == false) t_sd = sd;
	else {
		t_sd = true;
		post("Sidechain is ON");
	}
	x->sidechain = t_sd;
}


void lopehi_toggle(t_zamdyneq *x, t_symbol *msg, short argc, t_atom *argv)
{
	short i;
	bool *toglist = x->toggles;

	for (i = 0; i < argc; i++) {
		toglist[i] = atom_getfloat(argv + i);
	}
	x->toglow = toglist[0];
	x->togpeak = toglist[1];
	x->toghigh = toglist[2];

	reset_low(x);
	reset_high(x);
	reset_peak(x);
}

void zamdyneq_free(t_zamdyneq* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->Al);
	sysmem_freeptr(x->Bl);
	sysmem_freeptr(x->Ah);
	sysmem_freeptr(x->Bh);

	sysmem_freeptr(x->toggles);
}