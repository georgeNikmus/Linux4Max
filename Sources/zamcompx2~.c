#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define STEREOLINK_AVERAGE 0
#define STEREOLINK_MAX 1
#define STEREOLINK_UNCOUPLED 2

static t_class *zamcompx2_class;

typedef struct _zamcompx2
{
	t_pxobject x_obj;

	double sample_rate;

	double attack, release, knee, ratio, thresdb, makeup, gainred, slewfactor; //parameters
	double oldL_yl, oldL_y1, oldR_yl, oldR_y1, oldL_yg, oldR_yg;

	short side_connected;
	bool sidechain;
	bool stereodet;
	bool BYPASS;
}t_zamcompx2;

void* zamcompx2_new(t_symbol* s, short argc, t_atom* argv);

void zamcompx2_dsp64(t_zamcompx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
//void zamcompx2_dsp(t_zamcompx2 *x, t_signal **sp, short *count);

void zamcompx2_perform64(t_zamcompx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);
//t_int *zamcompx2_perform(t_int *w);
void zamcompx2_free(t_zamcompx2* x);

double sanitize_denormal(double v);
double from_dB(double gdb);
double to_dB(double g);

void activate(t_zamcompx2 *x);

void set_attack(t_zamcompx2 *x, double att);
void set_release(t_zamcompx2 *x, double rls);
void set_threshold(t_zamcompx2 *x, double v);
void set_knee(t_zamcompx2 *x, double kn);
void set_ratio(t_zamcompx2 *x, double rt);
void set_makeup(t_zamcompx2 *x, double mk);
void set_slew(t_zamcompx2 *x, double sl);

void t_stereodet(t_zamcompx2 *x, bool st);
void t_sidechn(t_zamcompx2 *x, bool sd);
void zamBypass(t_zamcompx2 *x, bool bypass);

void ext_main(void *r)
{
	t_class *c;

	c = class_new("zamcompx2~", (method)zamcompx2_new, (method)zamcompx2_free, sizeof(t_zamcompx2), 0, 0);
	class_addmethod(c, (method)zamcompx2_dsp64, "dsp64", A_CANT, 0);
	//class_addmethod(c, (method)zamcompx2_assist, "assist", A_CANT, 0);

	class_addmethod(c, (method)set_attack, "attack", A_FLOAT, 0);
	class_addmethod(c, (method)set_release, "release", A_FLOAT, 0);
	class_addmethod(c, (method)set_threshold, "thresh", A_FLOAT, 0);
	class_addmethod(c, (method)set_knee, "knee", A_FLOAT, 0);
	class_addmethod(c, (method)set_ratio, "ratio", A_FLOAT, 0);
	class_addmethod(c, (method)set_slew, "slew", A_FLOAT, 0);
	class_addmethod(c, (method)set_makeup, "makeup", A_FLOAT, 0);

	class_addmethod(c, (method)t_sidechn, "sidechain", A_LONG, 0);
	class_addmethod(c, (method)t_stereodet, "avg/max", A_LONG, 0);
	class_addmethod(c, (method)zamBypass, "Bypass", A_LONG, 0);

	class_addmethod(c, (method)activate, "reset", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	zamcompx2_class = c;

	post("------------------------------------------------------------------");
	post("The Zam Stereo Compressor for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void zamcompx2_dsp64(t_zamcompx2 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->sample_rate != samplerate) x->sample_rate = samplerate;

	x->side_connected = count[2];

	object_method(dsp64, gensym("dsp_add64"), x,
		zamcompx2_perform64, 0, NULL);
}

void zamcompx2_perform64(t_zamcompx2 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double *input0 = ins[0];
	t_double *input1 = ins[1];
	t_double *input2 = ins[2];
	t_double *output0 = outs[0];
	t_double *output1 = outs[1];
	t_double *output2 = outs[2];

	int n = vectorsize;
	double srate = x->sample_rate;
	bool usesidechain = false;

	if (!x->BYPASS) {
		double width = (6.0 * x->knee) + 0.01;
		double slewwidth = 1.8;
		double cdb = 0.0;
		double attack_coeff = exp(-1000.0 / (x->attack * srate));
		double release_coeff = exp(-1000.0 / (x->release * srate));
		
		int stereo = (!x->stereodet) ? STEREOLINK_AVERAGE : STEREOLINK_MAX;
		if(x->side_connected) 
			usesidechain = (!x->sidechain) ? false : true;

		int attslew = 0;
		double max = 0.0;
		double lgaininp = 0.0;
		double rgaininp = 0.0;
		double Lgain = 1.0;
		double Rgain = 1.0;
		double Lxg, Lxl, Lyg, Lyl;
		double Rxg, Rxl, Ryg, Ryl;
		double checkwidth = 0.0;
		int i;
		double in0;
		double in1;
		double ingain;

		for (i = 0; i < n; i++) {
			in0 = input0[i];
			in1 = input1[i];
			ingain = usesidechain ? input2[i] : max(in0, in1);
			attslew = 0;
			Lyg = Ryg = 0.0;
			if (usesidechain) {
				Lxg = (ingain == 0.0) ? -160.0 : to_dB(fabs(ingain));
				Rxg = Lxg;
			}
			else {
				Lxg = (in0 == 0.0) ? -160.0 : to_dB(fabs(in1));
				Rxg = (in1 == 0.0) ? -160.0 : to_dB(fabs(in1));
			}
			Lxg = sanitize_denormal(Lxg);
			Rxg = sanitize_denormal(Rxg);

			Lyg = Lxg + (1.0 / x->ratio - 1.0)*(Lxg - x->thresdb + width / 2.0)*(Lxg - x->thresdb + width / 2.0) / (2.0*width);
			Ryg = Rxg + (1.0 / x->ratio - 1.0)*(Rxg - x->thresdb + width / 2.0)*(Rxg - x->thresdb + width / 2.0) / (2.0*width);

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
			checkwidth = 2.0*fabs(Rxg - x->thresdb);

			if (2.f*(Rxg - x->thresdb) < -width) {
				Ryg = Rxg;
			}
			else if (checkwidth <= width) {
				Ryg = x->thresdb + (Rxg - x->thresdb) / x->ratio;
				Ryg = sanitize_denormal(Ryg);
				if (checkwidth <= slewwidth) {
					if (Ryg >= x->oldR_yg) {
						attslew = 1;
					}
				}
			}
			else if (2.0*(Rxg - x->thresdb) > width) {
				Ryg = x->thresdb + (Rxg - x->thresdb) / x->ratio;
				Ryg = sanitize_denormal(Ryg);
			}

			attack_coeff = attslew ? exp(-1000.f / ((x->attack + 2.0*(x->slewfactor - 1)) * srate)) : attack_coeff;
			// Don't slew on release
			if (stereo == STEREOLINK_UNCOUPLED) {
				Lxl = Lxg - Lyg;
				Rxl = Rxg - Ryg;
			}
			else if (stereo == STEREOLINK_MAX) {
			Lxl = Rxl = max(Lxg - Lyg, Rxg - Ryg);
			}
			else {
				Lxl = Rxl = (Lxg - Lyg + Rxg - Ryg) / 2.0;
				Lxl = sanitize_denormal(Lxl);
			}

			x->oldL_yl = sanitize_denormal(x->oldL_yl);
			if (Lxl < x->oldL_yl) {
				Lyl = release_coeff * x->oldL_yl + (1.0 - release_coeff)*Lxl;
			}
			else if (Lxl > x->oldL_yl) {
				Lyl = attack_coeff * x->oldL_yl + (1.0 - attack_coeff)*Lxl;
			}
			else {
				Lyl = Lxl;
			}
			Lyl = sanitize_denormal(Lyl);

			cdb = -Lyl;
			Lgain = from_dB(cdb);

			x->gainred = Lyl;

			x->oldR_y1 = sanitize_denormal(x->oldR_y1);
			if (Rxl < x->oldR_yl) {
				Ryl = release_coeff * x->oldR_yl + (1.0 - release_coeff)*Rxl;
			}
			else if (Rxl > x->oldR_yl) {
				Ryl = attack_coeff * x->oldR_yl + (1.0 - attack_coeff)*Rxl;
			}
			else {
				Ryl = Rxl;
			}
			Ryl = sanitize_denormal(Ryl);

			cdb = -Ryl;
			Rgain = from_dB(cdb);

			lgaininp = in0 * Lgain;
			rgaininp = in1 * Rgain;
			output0[i] = lgaininp * from_dB(x->makeup);
			output1[i] = rgaininp * from_dB(x->makeup);
			output2[i] = (Lgain + Rgain) / 2.0;

			//max = (fabs(max(output0[i], output1[i])) > max) ? fabs(max(output0[i], output1[i])) : sanitize_denormal(max);

			x->oldL_yl = Lyl;
			x->oldR_yl = Ryl;
			x->oldL_yg = Lyg;
			x->oldR_yg = Ryg;
		}
		//x->outlevel = (max == 0.f) ? -45.f : to_dB(max); // relative to - thresdb;
	}
	else {
		while (n--) {
			*output0++ = *input0++;
			*output1++ = *input1++;
		}
	}
}

void* zamcompx2_new(t_symbol* s, short argc, t_atom* argv)
{
	t_zamcompx2* x = (t_zamcompx2*)object_alloc(zamcompx2_class);

	//atom_arg_getdouble(&numChan, 0, argc, argv);

	dsp_setup((t_pxobject *)x, 3);
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");
	outlet_new((t_object *)x, "signal");

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->sample_rate = sys_getsr();

	x->attack = 10.0;
	x->release = 80.0;
	x->knee = 0.0;
	x->ratio = 4.0;
	x->thresdb = 0.0;
	x->makeup = 0.0;
	x->gainred = 0.0;
	x->slewfactor = 1.0;
	x->sidechain = false;
	x->stereodet = false;
	//x->outlevel = -45.0;

	/* reset filter values */
	activate(x);

	x->BYPASS = 0;
	return x;
}

void activate(t_zamcompx2 *x)
{
	x->gainred = 0.0f;
	//x->outlevel = -45.0f;
	x->oldL_yl = x->oldL_y1 = x->oldR_yl = x->oldR_y1 = x->oldL_yg = x->oldR_yg = 0.f;
}

void zamBypass(t_zamcompx2 *x, bool bypass)
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

void set_attack(t_zamcompx2 *x, double att)
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

void set_release(t_zamcompx2 *x, double rls)
{
	if (rls > 3000.0) {
		rls = 3000.0;
		post("Maximum Release Time: 3000 ms");
	}
	else if (rls < 1.0) {
		rls = 1.0;
		post("Minimum Release Time: 1 ms");
	}
	x->release = rls;
}

void set_threshold(t_zamcompx2 *x, double v)
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

void set_knee(t_zamcompx2 *x, double kn)
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

void set_ratio(t_zamcompx2 *x, double rt)
{
	if (rt > 30.0) {
		rt = 30.0;
		post("Maximum Ratio Value: 30");
	}
	if (rt < 1.0) {
		rt = 1.0;
		post("Minimum Ratio Value: 1");
	}
	x->ratio = rt;
}

void set_makeup(t_zamcompx2 *x, double mk)
{
	if (mk < 0.0) {
		mk = 0.0;
		post("Minimum Makeup Value: 0 db");
	}
	if (mk > 30.0) {
		mk = 30.0;
		post("Maximum Makeup Value: 30 db");
	}
	x->makeup = mk;
}

void set_slew(t_zamcompx2 *x, double sl)
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

void t_sidechn(t_zamcompx2 *x, bool sd)
{
	bool t_sd;
	if (sd == false) {
		t_sd = sd;
	}
	else {
		t_sd = true;
		post("Sidechain is ON");
	}
	x->sidechain = t_sd;
}

void t_stereodet(t_zamcompx2 *x, bool st) {
	x->stereodet = st;
}

void zamcompx2_free(t_zamcompx2* x)
{
	dsp_free((t_pxobject*)x);
}