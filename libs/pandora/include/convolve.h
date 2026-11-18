// convolve.h
// 

#ifndef CONVOLVE_H
#define CONVOLVE_H 

namespace convolve {
	//! Slow convolution.
	/*! 
		This implementation DOES NOT use the FFT and is very slow. If in d
		you have n samples and in f you have m samples then in r you must
		provide room for (n + m) - 1 samples.
	*/
	void conv (double d[], double f[], double r[], long c[]);
	
	//! Slow correlation function.
	void corr (double d[], double f[], double r[], long c[]);
}

#endif	// CONVOLVE_H 

// EOF

