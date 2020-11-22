#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"
#include <string.h>
#include <math.h>
#include <assert.h>

//#define MAXCHAN 64

#ifndef MAX
#define MAX(A, B) ((A) > (B)) ? (A) : (B)
#endif

#ifndef MIN
#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#endif

static t_class *dpl_class;


typedef struct _dpl
{
	t_pxobject x_obj;
	
	double _gt;
	double _g0;
	double _g1;
	double _w3;
	double _fsamp;
	double _db;
	double _sec;
	
	int SIZE;
	int MASK;
	int    _hlen;
	int    _hold;
	int    _wind;
	double  _vmin;
	double  *_hist;
	
	int               _state;
	int               _nchan;
	int               _div1;
	int               _div2;
	int               _len1;
	int               _len2;
	int               _delay;
	int               _dsize;
	int               _dmask;
	int               _delri;
	//double           *_dbuff[MAXCHAN];
	double			  **_dbuff;

	int               _c1;
	int               _c2;
	double             _m1;
	double             _m2;
	double             _wlf;
	double             _w1;
	double             _w2;
	double             _z1;
	double             _z2;
	double             _z3;
	double             *_zlf;
	double			  _dg;
	volatile bool     _rstat;
	volatile double    _peak;
	volatile double    _gmax;
	volatile double    _gmin;
	
	bool				BYPASS;
}t_dpl;

void dpl_dsp64(t_dpl *x, t_object *dsp64, short *count,
			   double samplerate, long maxvectorsize, long flags);
//void dpl_dsp(t_dpl *x, t_signal **sp, short *count);

void dpl_perform64(t_dpl *x, t_object *dsp64,
				   double **ins, long numins, double **outs,
				   long numouts, long vectorsize, long flags,
				   void *userparam);
//t_int *dpl_perform(t_int *w);

void  dpinit(t_dpl *x, int hlen);
double write(t_dpl *x, double v);
void setThreshold(t_dpl *x, double db);
void setReltime(t_dpl *x, double msec);
void setInputgain(t_dpl *x, double db);
void dpBypass(t_dpl *x, bool bypass);
void dpl_free(t_dpl* x);
void* dpl_new(t_symbol* s, short argc, t_atom* argv);

void dplimit_assist(t_dpl *x, void *b, long msg, long arg, char *dst);

void ext_main(void *r)
{
	t_class *c;
	
	c = class_new("dplimit~", (method)dpl_new, (method)dpl_free,
				  sizeof(t_dpl), 0, A_GIMME, 0);
	//class_addmethod(c, (method)dpl_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)dpl_dsp64, "dsp64", A_CANT, 0);
	class_addmethod(c, (method)dplimit_assist, "assist", A_CANT, 0);
	
	class_addmethod(c, (method)setThreshold, "Threshold", A_FLOAT, 0);
	class_addmethod(c, (method)setReltime, "Release", A_FLOAT, 0);
	class_addmethod(c, (method)dpBypass, "Bypass", A_LONG, 0);
	class_addmethod(c, (method)setInputgain, "InputGain", A_FLOAT, 0);
	
	class_dspinit(c);
	class_register(CLASS_BOX, c);
	dpl_class = c;
	
	post("------------------------------------------------------------------");
	post("The zita-dpl1 limiter by Fons Adriaensen  <fons@linuxaudio.org>");
	post("Max object implementation(2016): Georgios Nikolopoulos ");
	post("Default values> Threshold: -0.1  Release: 10.0 ms InputGain: 0.0 dB");
	
	//return 0;
}

void dpl_dsp64(t_dpl *x, t_object *dsp64, short *count,
			   double samplerate, long maxvectorsize, long flags)
{
	if (x->_fsamp != samplerate) x->_fsamp = samplerate;

	object_method(dsp64, gensym("dsp_add64"), x,
				  dpl_perform64, 0, NULL);
}

/*
void dpl_dsp(t_dpl *x, t_signal **sp, short *count)
{
		dsp_add(dpl_perform, 5, x,
			sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,
			sp[0]->s_n);
}
*/

void dpl_perform64(t_dpl *x, t_object *dsp64,
				   double **ins, long numins, double **outs,
				   long numouts, long vectorsize, long flags,
				   void *userparam)
{
	t_double **input, **output;
	

	input = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->_nchan);
	output = (t_double * *)sysmem_newptrclear(sizeof(t_double*) * x->_nchan);

	for(int j = 0; j < x->_nchan; j++){
		input[j] = ins[j];
		output[j] = outs[j];
	}
	t_double *gr = outs[x->_nchan];


	int n = vectorsize;
	
	if (!x->BYPASS){
		int   i,j,k, n2, ri, wi;
		double g, d, h1, h2, m1, m2, x1, z, z1, z2, z3, pk, t0, t1, *p;
		
		ri = x->_delri;
		wi = (ri + x->_delay) & x->_dmask;
		h1 = 1;
		h2 = 1;
		m1 = x->_m1;
		m2 = x->_m2;
		z1 = x->_z1;
		z2 = x->_z2;
		z3 = x->_z3;
		
		
		if (x->_rstat)
		{
			x->_rstat = false;
			pk = 0;
			t0 = x->_gmax;
			t1 = x->_gmin;
		}
		else
		{
			pk = x->_peak;
			t0 = x->_gmin;
			t1 = x->_gmax;
		}

		k = 0;
		
		while (n)
		{
			
			n2 = (x->_c1 < n) ? x->_c1 : n;
			
			g = x->_g0;

			for (j = 0; j < x->_nchan; j++)
			{
				
				p = input[j] + k;
				z = x->_zlf[j];
				g = x->_g0;
				d = x->_dg;

				for (i = 0; i < n2; i++)
				{
					x1 = g * *p++;
					g += d;
					x->_dbuff[j][wi + i] = x1;
					z += x->_wlf * (x1 - z) + 1e-20f;
					x1 = fabs(x1);
					if (x1 > m1) {
						m1 = x1;
					}
					x1 = fabs(z);
					if (x1 > m2) {
						m2 = x1;
					}
				}
			x->_zlf[j] = z;
			}

			x->_g0 = g;

			x->_c1 -= n2;

			if (x->_c1 == 0)
			{
				m1 *= x->_gt;
				if (m1 > pk) pk = m1;
				h1 = (m1 > 1.0) ? 1.0 / m1 : 1.0;
				h1 = write(x, h1);
				m1 = 0;
				x->_c1 = x->_div1;
				if (--x->_c2 == 0)
				{
					m2 *= x->_gt;
					h2 = (m2 > 1.0) ? 1.0 / m2 : 1.0;
					h2 = write(x, h2);
					m2 = 0;
					x->_c2 = x->_div2;
					x->_dg = x->_g1 - x->_g0;
					
					if (fabs(x->_dg) < 1e-9f) {
						x->_g0 = x->_g1;
						x->_dg = 0;
					}
					else {
						x->_dg /= (double)x->_div1 * x->_div2;
					}
				}
			}
			
			for (i = 0; i < n2; i++)
			{
				z1 += x->_w1 * (h1 - z1);
				z2 += x->_w2 * (h2 - z2);
				z = (z2 < z1) ? z2 : z1;
				if (z < z3)  z3 += x->_w1 * (z - z3);
				else         z3 += x->_w3 * (z - z3);
				if (z3 > t1) t1 = z3;
				if (z3 < t0) t0 = z3;

				gr[k + i] = z3;

				for (j = 0; j < x->_nchan; j++)
				{
					output[j][k + i] = z3 * x->_dbuff[j][ri + i];
				}
				//*gr++ = z3;
			}
			
			wi = (wi + n2) & x->_dmask;
			ri = (ri + n2) & x->_dmask;
			k += n2;

			n -= n2;
		}
		x->_delri = ri;
		x->_m1 = m1;
		x->_m2 = m2;
		x->_z1 = z1;
		x->_z2 = z2;
		x->_z3 = z3;
		x->_peak = pk;
		x->_gmin = t0;
		x->_gmax = t1;
		
	}
	else{
		while (n--){
			for (int j = 0; j < x->_nchan; j++){
				*output[j]++ = *input[j]++;
			}
		}
	}
}

/*
t_int *dpl_perform(t_int *w)
{
	t_dpl *x = (t_dpl *)(w[1]);
	float *input = (t_float *)(w[2]);
	float *output = (t_float *)(w[3]);
	float *gr = (t_float *)(w[4]);

	int n = (t_int)w[5];
	float out;

	if (!x->BYPASS) {
		int     i, k, c1, c2, ri, wi;
		float   g1, g2, m1, m2, x1, z, z1, z2, z3, pk, t0, t1, *p;
		float  *dp[1];

		ri = x->_delri;
		wi = (ri + x->_delay) & x->_dmask;
		g1 = 1;
		c1 = x->_c1;
		m1 = (float)x->_m1;
		g2 = 1;
		c2 = x->_c2;
		m2 = (float)x->_m2;
		z1 = (float)x->_z1;
		z2 = (float)x->_z2;
		z3 = (float)x->_z3;
		//chn = 1;
		//for (j = 0; j < chn; j++) dp[j] = input;
		dp[0] = input;

		if (x->_rstat)
		{
			x->_rstat = false;
			pk = 0;
			t0 = (float)x->_gmax;
			t1 = (float)x->_gmin;
		}
		else
		{
			pk = (float)x->_peak;
			t0 = (float)x->_gmin;
			t1 = (float)x->_gmax;
		}

		while (n)
		{
			k = (c1 < n) ? c1 : n;

			//for (j = 0; j < chn; j++)
			//{
			p = dp[0];
			z = (float)x->_zlf[0];
			for (i = 0; i < k; i++)
			{
				x1 = *p++ * (float)x->in_gain;
				z += (float)x->_wlf * (x1 - z) + 1e-20f;
				x->_dbuff[0][wi + i] = x1;
				x1 = (float)fabs(x1);
				if (x1 > m1) m1 = x1;
				x1 = (float)fabs(z);
				if (x1 > m2) m2 = x1;
			}
			x->_zlf[0] = z;
			//}

			c1 -= k;
			if (c1 == 0)
			{
				m1 *= (float)x->_gt;
				if (m1 > pk) pk = m1;
				g1 = (m1 > 1.0f) ? 1.0f / m1 : 1.0f;
				g1 = (float)write(x, g1);
				c1 = x->_div1;
				m1 = 0;
				if (--c2 == 0)
				{
					m2 *= (float)x->_gt;
					g2 = (m2 > 1.0f) ? 1.0f / m2 : 1.0f;
					g2 = (float)write(x, g2);
					c2 = x->_div2;
					m2 = 0;
				}
			}

			for (i = 0; i < k; i++)
			{
				z1 += (float)x->_w1 * (g1 - z1);
				z2 += (float)x->_w2 * (g2 - z2);
				z = (z2 < z1) ? z2 : z1;
				if (z < z3)  z3 += (float)x->_w1 * (z - z3);
				else         z3 += (float)x->_w3 * (z - z3);
				if (z3 > t1) t1 = z3;
				if (z3 < t0) t0 = z3;
				//for (j = 0; j < chn; j++)
				//{
				out = *output++ = (float)(z3 * x->_dbuff[0][ri + i]);

				//*gr++ = sqrtf((float)(pow(x->_dbuff[0][ri + i], 2) - pow(out, 2)));
				*gr++ = z3;
				//dp[j][i] = z3 * _dbuff[j][ri + i];
				//}
			}

			//for (j = 0; j < chn; j++) dp[j] += k;
			dp[0] += k;
			wi = (wi + k) & x->_dmask;
			ri = (ri + k) & x->_dmask;
			n -= k;
		}

		x->_delri = ri;
		x->_c1 = c1;
		x->_m1 = m1;
		x->_c2 = c2;
		x->_m2 = m2;
		x->_z1 = z1;
		x->_z2 = z2;
		x->_z3 = z3;
		x->_peak = pk;
		x->_gmin = t0;
		x->_gmax = t1;
	}
	else {
		while (n--) {
			*output++ = *input++;
		}
	}
	return (w + 6);
}

*/
/*
t_int *dpl_perform(t_int *w)
{
	t_dpl *x = (t_dpl *)(w[1]);
	float *input[MAXCHAN], *output[MAXCHAN];
	int j,ji;
	for (j = 2,ji=0; j < x->_nchan+2; ji++) {
		input[ji] = (t_float *)(w[j]);
		output[ji] = (t_float *)(w[j+1]);
		j += 2;
	}

	float *gr = (t_float *)(w[x->_nchan + 3]);
	int n = (t_int)w[x->_nchan + 4];
	//float out;
	
	if (!x->BYPASS){
		int     i, k, c1, c2, ri, wi;
		float   g1, g2, m1, m2, x1, z, z1, z2, z3, pk, t0, t1, *p;
		float  *dp[MAXCHAN];
		
		ri = x->_delri;
		wi = (ri + x->_delay) & x->_dmask;
		g1 = 1;
		c1 = x->_c1;
		m1 = (float)x->_m1;
		g2 = 1;
		c2 = x->_c2;
		m2 = (float)x->_m2;
		z1 = (float)x->_z1;
		z2 = (float)x->_z2;
		z3 = (float)x->_z3;
		
		for (j = 0; j < x->_nchan; j++) dp[j] = input[j];
		//dp[0] = input;
		
		if (x->_rstat)
		{
			x->_rstat = false;
			pk = 0;
			t0 = (float)x->_gmax;
			t1 = (float)x->_gmin;
		}
		else
		{
			pk = (float)x->_peak;
			t0 = (float)x->_gmin;
			t1 = (float)x->_gmax;
		}
		
		while (n)
		{
			k = (c1 < n) ? c1 : n;
			
			for (j = 0; j < x->_nchan; j++){
			p = dp[j];
			z = (float)x->_zlf[j];
			for (i = 0; i < k; i++)
			{
				x1 = *p++ * (float)x->in_gain;
				z += (float)x->_wlf * (x1 - z) + 1e-20f;
				x->_dbuff[j][wi + i] = x1;
				x1 = (float)fabs(x1);
				if (x1 > m1) m1 = x1;
				x1 = (float)fabs(z);
				if (x1 > m2) m2 = x1;
			}
			x->_zlf[j] = z;
			}
			
			c1 -= k;
			if (c1 == 0)
			{
				m1 *= (float)x->_gt;
				if (m1 > pk) pk = m1;
				g1 = (m1 > 1.0f) ? 1.0f / m1 : 1.0f;
				g1 = (float)write(x, g1);
				c1 = x->_div1;
				m1 = 0;
				if (--c2 == 0)
				{
					m2 *= (float)x->_gt;
					g2 = (m2 > 1.0f) ? 1.0f / m2 : 1.0f;
					g2 = (float)write(x, g2);
					c2 = x->_div2;
					m2 = 0;
				}
			}
			
			for (i = 0; i < k; i++)
			{
				z1 += (float)x->_w1 * (g1 - z1);
				z2 += (float)x->_w2 * (g2 - z2);
				z = (z2 < z1) ? z2 : z1;
				if (z < z3)  z3 += (float)x->_w1 * (z - z3);
				else         z3 += (float)x->_w3 * (z - z3);
				if (z3 > t1) t1 = z3;
				if (z3 < t0) t0 = z3;
				for (j = 0; j < x->_nchan; j++){
				*output[j]++ = (float)(z3 * x->_dbuff[j][ri + i]);
				
				//*gr++ = sqrtf((float)(pow(x->_dbuff[0][ri + i], 2) - pow(out, 2)));
				//dp[j][i] = z3 * _dbuff[j][ri + i];
				}
				*gr++ = z3;
			}
			
			for (j = 0; j < x->_nchan; j++) dp[j] += k;
			//dp[0] += k;
			wi = (wi + k) & x->_dmask;
			ri = (ri + k) & x->_dmask;
			n -= k;
		}
		
		x->_delri = ri;
		x->_c1 = c1;
		x->_m1 = m1;
		x->_c2 = c2;
		x->_m2 = m2;
		x->_z1 = z1;
		x->_z2 = z2;
		x->_z3 = z3;
		x->_peak = pk;
		x->_gmin = t0;
		x->_gmax = t1;
	}
	else{
		while (n--){
			for (j = 0; j < x->_nchan; j++) {
				*output[j]++ = *input[j]++;
			}
		}
	}
	return (w + x->_nchan + 5);
}
*/
 
void* dpl_new(t_symbol* s, short argc, t_atom* argv)
{
	t_dpl* x = (t_dpl*)object_alloc(dpl_class);
	
	double db = -0.1, msec = 10.0;
	double numChan = 1;
	atom_arg_getdouble(&numChan, 0, argc, argv);
	atom_arg_getdouble(&db, 1, argc, argv);
	atom_arg_getdouble(&msec, 2, argc, argv);
	
	//if (numChan > MAXCHAN) numChan = MAXCHAN;

	x->_nchan = (int)numChan;
	
	dsp_setup((t_pxobject *)x, x->_nchan);

	for (int j = 0; j < x->_nchan + 1; j++){
		outlet_new((t_object *)x, "signal");
	}
	
	x->x_obj.z_misc = Z_NO_INPLACE; //force independent signal vectors
	
	/*
	for (int i = 0; i < MAXCHAN; i++){
		delete[] x->_dbuff[i];
		x->_dbuff[i] = 0;
	}
	*/
	
	x->SIZE = 32;
	x->MASK = 31;
	
	x->_fsamp = sys_getsr();
	
	x->_hist = (double*)sysmem_newptr(sizeof(double) * 32);
	x->_zlf = (double*)sysmem_newptrclear(sizeof(double) * x->_nchan);


	if (x->_fsamp > 130000) x->_div1 = 32;
	else if (x->_fsamp >  65000) x->_div1 = 16;
	else 	             x->_div1 = 8;
	x->_div2 = 8;
	x->_len1 = (int)(ceil(1.2e-3f * x->_fsamp / x->_div1));
	x->_len2 = 12;
	x->_delay = x->_len1 * x->_div1;
	for (x->_dsize = 64; x->_dsize < x->_delay + x->_div1; x->_dsize *= 2);
	x->_dmask = x->_dsize - 1;
	x->_delri = 0;

	x->_dbuff = (double **)sysmem_newptrclear(x->_nchan * sizeof(double*));

	for (int i = 0; i < x->_nchan; i++)
	{
		//x->_dbuff[i] = x->_dsize;
		//x->_dbuff[i] = (double *)sysmem_newptr(x->_dsize * sizeof(double));
		
		x->_dbuff[i] = (double*)sysmem_newptrclear(x->_dsize * sizeof(double));
		//x->_dbuff[i] = new double[x->_dsize];
		//memset(x->_dbuff[i], 0, x->_dsize * sizeof(double));
	}
	
	//x->_gt = pow(10.0, -0.05 * db);
	
	dpinit(x,x->_len1 + 1);
	dpinit(x,x->_len2);

	x->_c1 = x->_div1;
	x->_c2 = x->_div2;
	x->_m1 = 0.0;
	x->_m2 = 0.0;
	x->_wlf = 6.28 * 500.0 / x->_fsamp;
	x->_w1 = 10.0 / x->_delay;
	x->_w2 = x->_w1 / x->_div2;
	//x->_w3 = 1.0 / ((msec / 1000) * x->_fsamp);
	x->_w3 = 1.0f / (0.01 * x->_fsamp);

	for (int i = 0; i < x->_nchan; i++) x->_zlf[i] = 0.0f;
	x->_z1 = 1.0;
	x->_z2 = 1.0;
	x->_z3 = 1.0;
	x->_gt = 1.0;
	x->_g0 = 1.0;
	x->_g1 = 1.0;
	x->_dg = 0.0;
	x->_peak = 0;
	x->_gmax = 1.0;
	x->_gmin = 1.0;
	
	//x->in_gain = pow(10.0, 0.05 * 0.0); //Default Input Gain
	
	x->_rstat = true;
	
	x->BYPASS = 0;
	
	return x;
}

void dpinit(t_dpl *x, int hlen)
{	
	assert(hlen <= x->SIZE);
	x->_hlen = hlen;
	x->_hold = hlen;
	x->_wind = 0;
	x->_vmin = 1;
	for (int i = 0; i < x->SIZE; i++) x->_hist[i] = x->_vmin;
}

double write(t_dpl *x, double v)
{
	int i, j;
	
	i = x->_wind;
	x->_hist[i] = v;
	if (v <= x->_vmin)
	{
		x->_vmin = v;
		x->_hold = x->_hlen;
	}
	else if (--x->_hold == 0)
	{
		x->_vmin = v;
		x->_hold = x->_hlen;
		for (j = 1 - x->_hlen; j < 0; j++)
		{
			v = x->_hist[(i + j) & x->MASK];
			if (v < x->_vmin)
			{
				x->_vmin = v;
				x->_hold = x->_hlen + j;
			}
		}
	}
	x->_wind = ++i & x->MASK;
	
	return x->_vmin;
}

void setThreshold(t_dpl *x, double db)
{
	x->_db = db;
	if (db > 0.5){
		db = 0.5;
		post("Threshold Maximum: 0.5");
	}
	
	x->_gt = pow(10.0, -0.05 * db);
}

void setReltime(t_dpl *x, double msec)
{
	double t_sec;
	if (msec <= 0.01){
		msec = 0.01;
		post("Release time: 0.01 - 3000.0 ms");
	}
	else if (msec >= 3000.0) {
		msec = 3000.0;
		post("Release time: 0.01 - 3000.0 ms");
	}
	t_sec = msec / 1000.0;
	
	x->_w3 = 1.0 / (t_sec * x->_fsamp);
}

void setInputgain(t_dpl *x, double db){

	if (db > 30.0){
		db = 30.0;
		post("Input Gain Maximum: 30. dB");
	}
	
	x->_g1 = pow(10.0, 0.05 * db);
}

void dpBypass(t_dpl *x, bool bypass)
{
	bool t_by;
	if (bypass == false){
		t_by = bypass;
	}
	else {
		t_by = true;
		post("Bypass is ON");
	}
	x->BYPASS = t_by;
}

void dpl_free(t_dpl* x)
{
	dsp_free((t_pxobject*)x);
	sysmem_freeptr(x->_dbuff);
	sysmem_freeptr(x->_zlf);
	sysmem_freeptr(x->_hist);
}

void dplimit_assist(t_dpl *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET){

			for (int i = 0; i < x->_nchan; i++) {
				if(arg == i) sprintf(dst, "(signal) Input %d",i+1);
		}
	}
	else if (msg == ASSIST_OUTLET) {
		for (int i = 0; i < x->_nchan; i++) {
			if (arg == i) sprintf(dst, "(signal) Output %d",i+1);
		}
		if(arg == x->_nchan) sprintf(dst, "(signal) Gain Reduction");
	}
}