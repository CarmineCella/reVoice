// convolve.cpp
//

namespace convolve {
	void conv (double d[], double f[], double r[], long c[]) {
		long	nv, nc;
		long	n1, n2;
	
		nv = c[0];
		nc = c[1];
	
		for (n1 = 0; n1 <= (nv + nc - 2); ++n1) {
			r[n1] = 0;
		}
			
		for (n1 = 0; n1 <= (nv - 1); ++n1) {
			for (n2 = 0; n2 <= (nc - 1); ++n2) {
				r[n1 + n2] = r[n1 + n2]  +  d[n1] * f[n2];
			}
		}
	
		return;
	}

	void corr (double d[], double f[], double r[], long c[]) {
		long	nv, nc;
		long	n1, n2;
	
		nv = c[0];
		nc = c[1];
	
		for (n1 = 0; n1 <= (nv + nc - 2); ++n1) {
			r[n1] = 0;
		}
			
		for (n1 = 0; n1 <= (nv - 1); ++n1) {
			for (n2 = 0; n2 <= (nc - 1); ++n2) {
				r[n1 + n2] = r[n1 + n2]  +  d[n1] * f[(nc - 1) - n2];
			}
		}
	
		return;
	}
}

// EOF

