// fourier.cpp
//

#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
#ifndef TWOPI
#define TWOPI (6.28318530717958647692)
#endif
#define SWAP(a,b) tempr = (a); (a) = (b); (b) = tempr
#define SOMEOF(s) (s)
#define ACTUAL(s) ((s - 1L) * 2L)
#define MASKPHS(p,q,s) \
    q = (int)(s * p);     \
    p -= M_PI * (double)((int)((q + ((q >= 0) ? (q & 1) : -(q & 1))))); \

namespace fourier {
	void fft1 (double data[], unsigned long nn, int isign) {
		unsigned long n, mmax, m, j, istep, i;
		double wtemp, wr, wpr, wpi, wi, theta; 
		double tempr, tempi;
		n = nn << 1;
		j = 1;
		for (i = 1; i < n; i += 2) {
			if (j > i) {
				SWAP(data[j], data[i]);
				SWAP(data[j + 1], data[i + 1]);
			}
	
			m = n >> 1;
		
			while (m >= 2 && j > m) {
				j -= m;
				m >>= 1;
			}
			
			j += m;
		}
	
		mmax = 2;
		
		while (n > mmax) {
			istep = mmax << 1;
			theta = isign * (6.28318530717959 / mmax);
			wtemp = sin (0.5 * theta);
			wpr = -2.0 * wtemp * wtemp;
		
			wpi = sin (theta);
			wr = 1.0;
			wi = 0.0;
			for (m = 1; m < mmax; m += 2) {
				for (i = m; i <= n; i += istep) {
					j = i + mmax;
					tempr = wr * data[j] - wi * data[j + 1];
					tempi = wr * data[j + 1] + wi * data[j];
					data[j] = data[i] - tempr;
					data[j + 1] = data[i + 1] - tempi;
					data[i] += tempr;
					data[i + 1] += tempi;
				}
		
				wr = (wtemp = wr) * wpr - wi * wpi + wr;
				wi = wi * wpr + wtemp * wpi + wi;
			}
		
			mmax=istep;
		}
	}
	void realfft (double data[], unsigned long n, int isign) {
		unsigned long i, i1, i2, i3, i4, np3;
		double c1 = 0.5, c2, h1r, h1i, h2r, h2i;
		double wr, wi, wpr, wpi, wtemp, theta;
		
		theta = 3.141592653589793 / (double) (n >> 1);
		if (isign == 1) {
			c2 = -0.5;
			fft1 (data, n >> 1, 1); 
		} else {
			c2 = 0.5;
			theta = -theta;
		}
	
		wtemp = sin (0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin (theta);
		wr = 1.0 + wpr;
		wi = wpi;
		np3 = n + 3;
	
		for (i = 2; i <= (n >> 2); i++) {
			i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
			h1r = c1 * (data[i1] + data[i3]);
			h1i = c1 * (data[i2] - data[i4]);
			h2r = -c2 * (data[i2] + data[i4]);
			h2i = c2 * (data[i1] - data[i3]);
			data[i1] = h1r + wr * h2r - wi * h2i;
			data[i2] = h1i + wr * h2i + wi * h2r;
			data[i3] = h1r - wr * h2r + wi * h2i;
			data[i4] = -h1i + wr * h2i + wi * h2r;
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		
		if (isign == 1) {
			data[1] = (h1r = data[1]) + data[2];
			data[2] = h1r - data[2];
			//fft1 (data, n >> 1, 1); // produce risultati assurdi: capisce F0?
		} else {
			data[1] = c1 * ((h1r = data[1]) + data[2]);
			data[2] = c1 * (h1r - data[2]);
			fft1 (data, n >> 1, -1);
		}
	}	
	void cmult (double* c, double* a, double* b, long n) {
		long i, j;
		double re, im;
		
		c[0] = (a[0] * b[0]);
		c[1] = (a[1] * b[1]);
		
		for (i = 2, j = 3; i < n; i += 2, j += 2) {
			re = ((a[i] * b[i]) - (a[j] * b[j]));
			im = ((a[i] * b[j]) + (a[j] * b[i]));
			c[i] = re;
			c[j] = im;
		}
	}
	void rectToPolar (double* buffer, long size) {
		double *real,*imag;
		double re, im;
		double mag;
		
		real = buffer;
		imag = buffer + 1;
		for (long i = 0; i < SOMEOF (size); ++i) {
		  re = (double) real[2L * i];
		  im = (double) imag[2L * i];
		  real[2L * i] = mag = (double) sqrt (re * re + im * im);
		  if (mag == 0.0)
			imag[2L * i] = 0.0;
		  else
			imag[2L * i] = (double) atan2 (im, re);
		}
	}
	void polarToRect (double* buffer, long size) {
		double *magn, *phas;
		double mag;
		double pha;
		
		magn = buffer;
		phas = buffer + 1;
		for (long i = 0; i < SOMEOF (size); ++i) {
			mag = magn[2L * i];
			pha = (double) phas[2L * i];
			magn[2L * i] = mag * (double) cos (pha);
			phas[2L * i] = mag * (double) sin (pha);
		}
	}	
	void realToComplex (double* dst, long size) {
		double* d2;
	
		d2 = dst + 2 * size - 1;
		dst += size - 1;
		++size;
		while (--size) {
			*d2-- = 0.0;
			*d2-- = *dst--;
		}
	}
	void complexToReal (double* buffer, long size) {
		double *b2 = buffer;
	
		++size;
		while (--size) {
		  *b2++ = *buffer++;
		  ++buffer;
		}
	}
	void unwrapPhase (double* buf, long size, double* oldPh) {
		double *pha;
		double p, oneOnPi;
		int z;
	
		pha = buf + 1;
		oneOnPi = 1.0 / M_PI;
		for (long i = 0; i < SOMEOF (size); ++i) {
			p = pha[2L * i];
			p -= oldPh[i];
			MASKPHS (p, z, oneOnPi);
			oldPh[i] = pha[2L * i];
			pha[2L * i] = p;
		}
	}
	void rewrapPhase (double *buf, long size, double* oldPh) {
		double* pha;
		double p, oneOnPi;
		int z;
	
		pha = buf + 1;
		oneOnPi = 1.0 / M_PI;
		for (long i = 0; i < SOMEOF (size); ++i) {
			p = (pha[2L * i] + oldPh[i]);
			MASKPHS (p, z, oneOnPi);
			oldPh[i] = pha[2L * i] = p;
		}
	}
	void phaseToFreq (double* buf, long size, double incr, double sampRate) {
		double *pha, p, q, oneOnPi;
		int z;
		double srOn2pi, binMidFrq, frqPerBin;
		double expectedDphas, eDphIncr;
	
		pha = buf + 1;
		srOn2pi = sampRate / (2.0* M_PI * incr);
		frqPerBin = sampRate / ((double) ACTUAL (size));
		binMidFrq = 0.0;
		expectedDphas = 0.0;
		eDphIncr = 2.0 * M_PI * incr / ((double) ACTUAL (size));
		oneOnPi = 1.0 / M_PI;
		for (long i = 0; i < SOMEOF (size); ++i) {
				q = p = pha[2L * i] - expectedDphas;
				MASKPHS (p,z,oneOnPi);
				pha[2L * i] = p;
				pha[2L * i] *= srOn2pi;
				pha[2L * i] += binMidFrq;
	
				expectedDphas += eDphIncr;
				expectedDphas -= TWOPI * (double)((int)
					(expectedDphas * oneOnPi));
				binMidFrq += frqPerBin;
		}
	}
	void freqToPhase (double* buf, long size, double incr, double sampRate, 
		double fixUp) {
		double* pha;
		double twoPiOnSr, binMidFrq, frqPerBin;
		double expectedDphas, eDphIncr;
		double p;
		int j;
		double oneOnPi;
	
		oneOnPi = 1.0 / M_PI;
		pha = buf + 1;
		twoPiOnSr = 2.0 * M_PI * ((double) incr) / sampRate;
		frqPerBin = sampRate / ((double) ACTUAL (size));
		binMidFrq = 0.0;
		expectedDphas = 0.0;
		eDphIncr = 2.0 * M_PI * ((incr) / ((double) ACTUAL (size)) + fixUp);
		for (long i = 0; i < SOMEOF (size); ++i) {
			p = pha[2L * i];
			p -= binMidFrq;
			p *= twoPiOnSr;
			p += expectedDphas;
			MASKPHS (p, j, oneOnPi);
			pha[2L * i] = p;
			expectedDphas += eDphIncr;
			expectedDphas -= TWOPI * (double)((int)(expectedDphas * oneOnPi));
			binMidFrq += frqPerBin;
		}
	}
	void preWarpSpec (double* spec, double* env, long size, double warpFactor)	{
		double eps, slope;
		double mag, lastmag, nextmag, pkOld;
		long pkcnt;

		eps = -64.0 / size;
		lastmag = *spec;
		mag = spec[2 * 1];
		pkOld = lastmag;
		env[0] = pkOld;
		pkcnt = 1;
	
		for (long i = 1; i < SOMEOF (size); i++) {
			if (i < SOMEOF (size) - 1)
				nextmag = spec[2 * (i + 1)];
			else nextmag = 0.0;
	
			if (pkOld != 0.0)
				slope = ((double) (mag - pkOld) / (pkOld * pkcnt));
			else
				slope = -10.0;
	
			if ((mag >= lastmag) && (mag > nextmag) && (slope > eps)) {
				env[i] = mag;
				pkcnt--;
				for (long j = 1; j <= pkcnt; j++) {
					env[i - pkcnt + j - 1] = pkOld * (1.0 + slope * j);
				}
				pkOld = mag;
				pkcnt = 1;
			}
			else
				pkcnt++;
	
			lastmag = mag;
			mag = nextmag;
		}
	
		if (pkcnt > 1) {
			mag = spec[2 * (size / 2)];
			slope = ((double) (mag - pkOld) / pkcnt);
			env[size / 2] = mag;
			pkcnt--;
			for (long j = 1; j <= pkcnt; j++) {
				env[size / 2 - pkcnt + j - 1] = pkOld + slope * j;
			}
		}
	
		for (long i = 0; i < SOMEOF (size); i++) {
			long j = (long)((double) i * warpFactor);
			mag = spec[2 * i];
			if ((j < SOMEOF (size)) && (env[i] != 0.0))
				spec[2 * i] *= env[j] / env[i];
			else
				spec[2 * i] = 0.0;
		}
	}
}

// EOF

