// wavelet.h
//

#ifndef WAVELET_H
#define WAVELET_H

#include <vector>
#include <stdexcept>

namespace wavelet {

	void pwtset (int n);
	void pwt (std::vector <double>& a, const int n, const int isign);
	//! Discrete wavelet transform for 1-dimensional signals.
	/*! Data are transformed in-place.
	 * The nn parameter MUST be a power of two.
	 * The isign parameter sets the direction
	 * of the operation. The last parameter set the wavelet type.
	 */
	void wt1 (std::vector <double>& a, const int isign,
			  void wtstep(std::vector <double>&, const int, const int));
	//! Discrete wavelet transform for n-dimensional signals.
	//void wtn (float a[], unsigned long nn[], int ndim, int isign,
	//	void (*wtstep)(float [], unsigned long, int));
}

#endif // WAVELET_H

// EOF

