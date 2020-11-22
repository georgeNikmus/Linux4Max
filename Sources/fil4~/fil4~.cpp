#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include "filters.h"
#include "iir.h"
#include "hip.h"
#include "lop.h"

#define NSECT (4)

static t_class *fil4_class;

typedef struct fil4 {
	t_pxobject	  x_obj;
	double         rate;
	double         below_nyquist;
	int			n_channels;

	Fil4Paramsect **_sect;
	
	HighPass      *hip;
	LowPass       *lop;

	IIRProc       *iir_lowshelf;
	IIRProc       *iir_highshelf;

	int           *_fade;
	double         _gain;

	double 	IIR_LS_EN, IIR_LS_FREQ, IIR_LS_Q, IIR_LS_GAIN,
		IIR_HS_EN, IIR_HS_FREQ, IIR_HS_Q, IIR_HS_GAIN;

	double 	FIL_HIPASS, FIL_HIFREQ, FIL_HIQ,
		FIL_LOPASS, FIL_LOFREQ, FIL_LOQ;

	double *FREQ, *BAND, *GAIN;
	double *hipass_list, *lopass_list, *hishelf_list, *loshelf_list;

	double *sfreq, *sband, *sgain;
	double TOT_GAIN;

	t_double** signal;
	t_double** ptr;
	int* toj;
	bool Bypass;
} t_fil4;

void* fil4_new(t_symbol* s, short argc, t_atom* argv);

void fil4_dsp64(t_fil4 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags);
void fil4_perform64(t_fil4 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam);

void fil4_free(t_fil4* x);

void freq_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);
void band_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);
void gain_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);

void hip_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);
void loshelf_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);
void lop_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);
void hishelf_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv);

void reset(t_fil4* x);

void set_tgain(t_fil4* x, double v);
void setBypass(t_fil4* x, bool bypass);

double exp2ap(double x);

void ext_main(void *r) {
	t_class *c;

	c = class_new("fil4~", (method)fil4_new, (method)fil4_free, sizeof(t_fil4), 0, A_GIMME, 0);
	class_addmethod(c, (method)fil4_dsp64, "dsp64", A_CANT, 0);

	class_addmethod(c, (method)freq_list, "freqs", A_GIMME, 0);
	class_addmethod(c, (method)band_list, "bands", A_GIMME, 0);
	class_addmethod(c, (method)gain_list, "gains", A_GIMME, 0);
	
	class_addmethod(c, (method)hip_list, "hipass", A_GIMME, 0);
	class_addmethod(c, (method)loshelf_list, "loshelf", A_GIMME, 0);
	class_addmethod(c, (method)lop_list, "lopass", A_GIMME, 0);
	class_addmethod(c, (method)hishelf_list, "hishelf", A_GIMME, 0);

	class_addmethod(c, (method)set_tgain, "Output", A_GIMME, 0);
	class_addmethod(c, (method)setBypass, "Bypass", A_LONG, 0);

	class_addmethod(c, (method)reset, "init", 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	fil4_class = c;

	post("------------------------------------------------------------------");
	post("The fil4 4 band filters by Robin Gareus for Max");
	post("Max object implementation(2019): Georgios Nikolopoulos ");
	//return 0;
}

void fil4_dsp64(t_fil4 *x, t_object *dsp64, short *count,
	double samplerate, long maxvectorsize, long flags)
{
	if (x->rate != samplerate) x->rate = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
		fil4_perform64, 0, NULL);
}

void fil4_perform64(t_fil4 *x, t_object *dsp64,
	double **ins, long numins, double **outs,
	long numouts, long vectorsize, long flags,
	void *userparam)
{
	t_double** aip, ** aop;
	aip = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->n_channels);
	aop = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->n_channels);

	for (int j = 0; j < x->n_channels; j++) {
		aip[j] = ins[j];
		aop[j] = outs[j];
	}

	int n = vectorsize;

	//if (!x->Bypass) {
		/* localize variables */
		const double ls_gain = x->IIR_LS_EN > 0.0 ? pow(10.0, .05 * x->IIR_LS_GAIN) : 1.0;
		const double hs_gain = x->IIR_HS_EN > 0.0 ? pow(10.0, .05 * x->IIR_HS_GAIN) : 1.0;
		const double ls_freq = x->IIR_LS_FREQ;
		const double hs_freq = x->IIR_HS_FREQ;
		// map [2^-4 .. 4] to [2^(-3/2) .. 2]
		const double ls_q = .2129 + x->IIR_LS_Q / 2.25;
		const double hs_q = .2129 + x->IIR_HS_Q / 2.25;
		const bool  hipass = x->FIL_HIPASS > 0.0 ? true : false;
		const bool  lopass = x->FIL_LOPASS > 0.0 ? true : false;
		double hifreq = x->FIL_HIFREQ;
		double hi_q = x->FIL_HIQ;
		double lofreq = x->FIL_LOFREQ;
		double lo_q = x->FIL_LOQ;

		//double sfreq[NSECT];
		//double sband[NSECT];
		//double sgain[NSECT];

		double *sfreq = x->sfreq;
		double *sband = x->sband;
		double *sgain = x->sgain;
		/* clamp inputs to legal range - see lv2ttl/fil4.ports.ttl.in */
		if (lofreq > x->below_nyquist) lofreq = x->below_nyquist;
		if (lofreq < 630) lofreq = 630.0;
		if (lofreq > 20000) lofreq = 20000.0;
		if (lo_q < 0.0625) lo_q = 0.0625;
		if (lo_q > 4.0)    lo_q = 4.0;

		if (hifreq > x->below_nyquist) hifreq = x->below_nyquist;
		if (hifreq < 10) hifreq = 10.0;
		if (hifreq > 1000) hifreq = 1000.0;
		if (hi_q < 0.0625) hi_q = 0.0625;
		if (hi_q > 4.0)    hi_q = 4.0;

		// shelf-filter freq,q is clamped in src/iir.h


		/* calculate target values, parameter smoothing */
		const double fgain = exp2ap(0.1661 * x->TOT_GAIN);

		for (int j = 0; j < NSECT; ++j) {
			double t = x->FREQ[j] / x->rate;
			if (t < 0.0002) t = 0.0002;
			if (t > 0.4998) t = 0.4998;

			sfreq[j] = t;
			sband[j] = x->BAND[j];

			sgain[j] = exp2ap(0.1661 * x->GAIN[j]);
		}

		while (n) {
			int i;
			//double sig[][48];
			t_double **sig = x->signal;
			//t_double* p =  NULL ;
			t_double **p = x->ptr;
			const int k = (n > 48) ? 32 : n;
			
			for (int cn = 0; cn < x->n_channels; cn++) {
			
				double t = fgain;
				double g = x->_gain;
				if (t > 1.25 * g) t = 1.25 * g;
				else if (t < 0.80 * g) t = 0.80 * g;
				x->_gain = t;
				double d = (t - g) / k;

				/* apply gain */
				for (i = 0; i < k; i++) {
					g += d;
					sig[cn][i] = g * aip[cn][i];
				}

			/* update IIR */
			if (iir_interpolate(&x->iir_lowshelf[cn], ls_gain, ls_freq, ls_q)) {
				iir_calc_lowshelf(&x->iir_lowshelf[cn]);
			}
			if (iir_interpolate(&x->iir_highshelf[cn], hs_gain, hs_freq, hs_q)) {
				iir_calc_highshelf(&x->iir_highshelf[cn]);
			}

			if (hip_interpolate(&x->hip[cn], hipass, hifreq, hi_q)) {
			}
			if (lop_interpolate(&x->lop[cn], lopass, lofreq, lo_q)) {
			}

			/* run filters */
			hip_compute(&x->hip[cn], k, sig[cn]);
			lop_compute(&x->lop[cn], k, sig[cn]);

			for (int j = 0; j < NSECT; ++j) {
				x->_sect[cn][j].proc(k, sig[cn], sfreq[j], sband[j], sgain[j]);
			}

			iir_compute(&x->iir_lowshelf[cn], k, sig[cn]);
			iir_compute(&x->iir_highshelf[cn], k, sig[cn]);
			
			/* fade 16 * 32 samples when enable changes */
			int *j = x->toj;
			j[cn] = x->_fade[cn];

			g = j[cn] / 16.0;
			if (x->Bypass == false) {
				if (j[cn] == 16) p[cn] = sig[cn];
				else ++j[cn];
			}
			else
			{
				if (j[cn] == 0) p[cn] = aip[cn];
				else --j[cn];
			}
			
			x->_fade[cn] = j[cn];

			if (p[cn] && x->Bypass == false) {
			/* active or bypassed */
				if (aop[cn] != p[cn]) { // no in-place bypass
					//memcpy(aop[cn], p[cn], k * sizeof(t_double));
					sysmem_copyptr(p[cn], aop[cn], k * sizeof(t_double));
				}
			}
			else {
				/* fade in/out */
				d = (j[cn] / 16.0 - g) / k;
				for (int i = 0; i < k; ++i) {
					g += d;
					aop[cn][i] =  g * sig[cn][i] + (1 - g) * aip[cn][i];
				}
			}
			aip[cn] += k;
			aop[cn] += k;
			}		
			n -= k;
		}
	//}

	/*else {
		while (n--) {
			for (int cn = 0; cn < x->n_channels; cn++)	*aop[cn]++ = *aip[cn]++;
		}
	}
	*/
}

void* fil4_new(t_symbol* s, short argc, t_atom* argv)
{
	t_fil4* x = (t_fil4*)object_alloc(fil4_class);

	double numChan = 1.0;
	atom_arg_getdouble(&numChan, 0, argc, argv);
	x->n_channels = (int)numChan;

	dsp_setup((t_pxobject*)x, x->n_channels);

	for (int cn = 0; cn < x->n_channels; cn++) {
		outlet_new((t_object*)x, "signal");
	}

	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors

	x->rate = sys_getsr();
	x->below_nyquist = x->rate * 0.4998;

	x->FREQ = (double*)sysmem_newptr(NSECT * sizeof(double));
	x->BAND = (double*)sysmem_newptr(NSECT * sizeof(double));
	x->GAIN = (double*)sysmem_newptr(NSECT * sizeof(double));

	x->sfreq = (double*)sysmem_newptr(NSECT * sizeof(double));
	x->sband = (double*)sysmem_newptr(NSECT * sizeof(double));
	x->sgain = (double*)sysmem_newptr(NSECT * sizeof(double));

	x->_sect = (Fil4Paramsect**)sysmem_newptr(x->n_channels * sizeof(Fil4Paramsect*));

	for (int j = 0; j < x->n_channels; j++) {
		x->_sect[j] = (Fil4Paramsect *)sysmem_newptr(NSECT * sizeof(Fil4Paramsect));
	}
	
	x->hipass_list = (double*)sysmem_newptr(3 * sizeof(double));
	x->lopass_list = (double*)sysmem_newptr(3 * sizeof(double));
	x->loshelf_list = (double*)sysmem_newptr(4 * sizeof(double));
	x->hishelf_list = (double*)sysmem_newptr(4 * sizeof(double));

	x->_fade = (int*)sysmem_newptr(x->n_channels * sizeof(int));
	x->toj = (int*)sysmem_newptr(x->n_channels * sizeof(int));

	x->hip = (HighPass*)sysmem_newptr(x->n_channels * sizeof(HighPass));
	x->lop = (LowPass*)sysmem_newptr(x->n_channels * sizeof(LowPass));
	x->iir_lowshelf = (IIRProc*)sysmem_newptr(x->n_channels * sizeof(IIRProc));
	x->iir_highshelf = (IIRProc*)sysmem_newptr(x->n_channels * sizeof(IIRProc));

	x->signal = (t_double * *)sysmem_newptr(x->n_channels * sizeof(t_double*));
	x->ptr = (t_double**)sysmem_newptrclear(x->n_channels * sizeof(t_double*));

	for (int j = 0; j < x->n_channels; j++) {
		x->signal[j] = (double*)sysmem_newptr(48 * sizeof(double));
		x->ptr[j] = NULL;
	}

	//for (int cn = 0; cn < x->n_channels; cn++) x->_fade[cn] = 0 ;
	for (int cn = 0; cn < x->n_channels; cn++) {
		x->_fade[cn] = 0;
		for (int j = 0; j < NSECT; ++j) {
			x->_sect[cn][j].init();
		}
	}

	x->_gain = 1.0;

	for (int cn = 0; cn < x->n_channels; cn++) {
		iir_init(&x->iir_lowshelf[cn], x->rate);
		iir_init(&x->iir_highshelf[cn], x->rate);

		x->iir_lowshelf[cn].freq = 50.0;
		x->iir_highshelf[cn].freq = 8000.0;

		iir_calc_lowshelf(&x->iir_lowshelf[cn]);
		iir_calc_highshelf(&x->iir_highshelf[cn]);

		hip_setup(&x->hip[cn], x->rate, 20.0, .7);
		lop_setup(&x->lop[cn], x->rate, 10000.0, .7);
	}
	x->FIL_HIPASS = 0.0;
	x->FIL_LOPASS = 0.0;
	x->IIR_LS_EN = 0.0;
	x->IIR_HS_EN = 0.0;

	x->FIL_HIFREQ = 20.0;
	x->IIR_LS_FREQ = 80.0;
	x->FREQ[0] = 160.0;
	x->FREQ[1] = 397.0;
	x->FREQ[2] = 1250.0;
	x->FREQ[3] = 2500.0;
	x->IIR_HS_FREQ = 8000.0;
	x->FIL_LOFREQ = 20000.0;

	for (int i = 0; i < NSECT; i++) x->GAIN[i] = 0.0;

	for (int i = 0; i < NSECT; i++) x->BAND[i] = 0.60;

	x->FIL_HIQ = 0.7;
	x->FIL_LOQ = 1.0;
	x->IIR_LS_Q = 1.0;
	x->IIR_HS_Q = 1.0;
	x->IIR_LS_GAIN = 0.0;
	x->IIR_HS_GAIN = 0.0;

	x->TOT_GAIN = 0.0;

	x->Bypass = false;

	return x;
}

double exp2ap(double x) {
	int i;

	i = (int)(floor(x));
	x -= i;
	return ldexp(1 + x * (0.6930 + x * (0.2416 + x * (0.0517 + x * 0.0137))), i);
}

void freq_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* frequencies = x->FREQ;

	for (i = 0; i < argc; i++) {
		frequencies[i] = atom_getfloat(argv + i);
		if (frequencies[i] < 20.0) {
			frequencies[i] = 20.0;
			post("Lowest Frequency: 20 Hz");
		}
		else if (frequencies[i] > 20000.0) {
			frequencies[i] = 20000.0;
			post("Highest Frequency: 20000 Hz");
		}
	}
}

void band_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* bands = x->BAND;

	for (i = 0; i < argc; i++) {
		bands[i] = atom_getfloat(argv + i);
		if (bands[i] < 0.01) {
			bands[i] = 0.01;
			post("Minimum Q value: 0.01");
		}
		else if (bands[i] > 6.0) {
			bands[i] = 6.0;
			post("Maximum Q value: 6.0");
		}
	}
}

void gain_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* gains = x->GAIN;

	for (i = 0; i < argc; i++) {
		gains[i] = atom_getfloat(argv + i);
	}
}

void hip_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* hipass = x->hipass_list;

	for (i = 0; i < argc; i++) {
		hipass[i] = atom_getfloat(argv + i);
	}
	x->FIL_HIPASS = hipass[0];
	x->FIL_HIFREQ = hipass[1];
	x->FIL_HIQ = hipass[2];
}

void loshelf_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* loshelf = x->loshelf_list;

	for (i = 0; i < argc; i++) {
		loshelf[i] = atom_getfloat(argv + i);
	}
	x->IIR_LS_EN = loshelf[0];
	x->IIR_LS_FREQ = loshelf[1];
	x->IIR_LS_Q = loshelf[2];
	x->IIR_LS_GAIN = loshelf[3];
}

void hishelf_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* hishelf = x->hishelf_list;

	for (i = 0; i < argc; i++) {
		hishelf[i] = atom_getfloat(argv + i);
	}
	x->IIR_HS_EN = hishelf[0];
	x->IIR_HS_FREQ = hishelf[1];
	x->IIR_HS_Q = hishelf[2];
	x->IIR_HS_GAIN = hishelf[3];
}

void lop_list(t_fil4* x, t_symbol* msg, short argc, t_atom* argv)
{
	short i;
	double* lopass = x->lopass_list;

	for (i = 0; i < argc; i++) {
		lopass[i] = atom_getfloat(argv + i);
	}
	x->FIL_LOPASS = lopass[0];
	x->FIL_LOFREQ = lopass[1];
	x->FIL_LOQ = lopass[2];
}

void set_tgain(t_fil4* x, double v)
{
	x->TOT_GAIN = v;
}

void setBypass(t_fil4* x, bool bypass) {
	x->Bypass = bypass;
}

void reset(t_fil4* x) {
	for (int cn = 0; cn < x->n_channels; cn++) {
		x->_fade[cn] = 0;
		for (int j = 0; j < NSECT; ++j) {
			x->_sect[cn][j].init();
		}
	}

	for (int cn = 0; cn < x->n_channels; cn++) {
		iir_init(&x->iir_lowshelf[cn], x->rate);
		iir_init(&x->iir_highshelf[cn], x->rate);

		x->iir_lowshelf[cn].freq = 50.0;
		x->iir_highshelf[cn].freq = 8000.0;

		iir_calc_lowshelf(&x->iir_lowshelf[cn]);
		iir_calc_highshelf(&x->iir_highshelf[cn]);

		hip_setup(&x->hip[cn], x->rate, 20.0, .7);
		lop_setup(&x->lop[cn], x->rate, 10000.0, .7);
	}
}

void fil4_free(t_fil4* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->FREQ);
	sysmem_freeptr(x->BAND);
	sysmem_freeptr(x->GAIN);
	sysmem_freeptr(x->_sect);
	sysmem_freeptr(x->_fade);
	sysmem_freeptr(x->signal);
	sysmem_freeptr(x->ptr);
	sysmem_freeptr(x->toj);

	sysmem_freeptr(x->hipass_list);
	sysmem_freeptr(x->lopass_list);
	sysmem_freeptr(x->loshelf_list);
	sysmem_freeptr(x->hishelf_list);

	sysmem_freeptr(x->hip);
	sysmem_freeptr(x->lop);
	sysmem_freeptr(x->iir_lowshelf);
	sysmem_freeptr(x->iir_highshelf);

	sysmem_freeptr(x->sfreq);
	sysmem_freeptr(x->sband);
	sysmem_freeptr(x->sgain);
}