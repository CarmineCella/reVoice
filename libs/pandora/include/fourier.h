// fourier.h
//

#ifndef FOURIER_H
#define FOURIER_H

namespace fourier {
	//! Fast Fourier transform for 1-dimensional signals
	/*! Data are transformed in-place and are 1-offset.
	 * The nn parameter MUST be the double of the FFT window
	 * (2 * WINDOW). The isign parameter sets the direction 
	 * of the operation.
	 */
	void fft1 (double data[], unsigned long nn, int isign);
	void realfft (double data[], unsigned long n, int isign);
	void cmult (double* c, double* a, double* b, long n);
	void rectToPolar (double* buffer, long size);
	void polarToRect (double* buffer, long size);
	void realToComplex (double* dst, long size);
	void complexToReal (double* buffer, long size);	
	void unwrapPhase (double* buf, long size, double* oldPh);
	void rewrapPhase (double *buf, long size, double* oldPh);
	void phaseToFreq (double* buf, long size, double incr, double sampRate);
	void freqToPhase (double* buf, long size, double incr, double sampRate, 
		double fixUp);
	void preWarpSpec (double* spec, double* env, long size, double warpFactor);
}

#endif // FOURIER_H

// EOF
