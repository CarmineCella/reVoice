// wavelet.cpp
//

#include "../include/wavelet.h"

namespace wavelet {
	class WaveletFilter {
	private:
		std::vector <double> *cc_p,*cr_p;
	public:
		int ncof,ioff,joff;
		std::vector <double> &cc,&cr;
		WaveletFilter() : cc(*cc_p),cr(*cr_p) {}
		WaveletFilter(const double *a, const int n) :  //initialize to array
				cc_p(new std::vector <double>(n)),cr_p(new std::vector <double>(n)),
				ncof(n),ioff(-(n >> 1)),joff(-(n >> 1)),cc(*cc_p),cr(*cr_p) {
			int i;
			for (i=0; i<n; i++)
				cc[i] = *a++;
			double sig = -1.0;
			for (i=0; i<n; i++) {
				cr[n-1-i]=sig*cc[i];
				sig = -sig;
			}
		}
		~WaveletFilter() {
			if (cc_p != 0) delete cc_p;
			if (cr_p != 0) delete cr_p;
		}
	};
	
	WaveletFilter *wfilt_p;
	
	void pwtset(const int n) {
		const static double c4_d[4]= {0.4829629131445341,0.8365163037378079,
									  0.2241438680420134,-0.1294095225512604
									 };
		const static double c12_d[12]= {0.111540743350, 0.494623890398, 0.751133908021,
										0.315250351709,-0.226264693965,-0.129766867567,
										0.097501605587, 0.027522865530,-0.031582039318,
										0.000553842201, 0.004777257511,-0.001077301085
									   };
		const static double c20_d[20]= {0.026670057901, 0.188176800078, 0.527201188932,
										0.688459039454, 0.281172343661,-0.249846424327,
										-0.195946274377, 0.127369340336, 0.093057364604,
										-0.071394147166,-0.029457536822, 0.033212674059,
										0.003606553567,-0.010733175483, 0.001395351747,
										0.001992405295,-0.000685856695,-0.000116466855,
										0.000093588670,-0.000013264203
									   };
	
		if (n == 4)
			wfilt_p=new WaveletFilter(c4_d,n);
		else if (n == 12)
			wfilt_p=new WaveletFilter(c12_d,n);
		else if (n == 20)
			wfilt_p=new WaveletFilter(c20_d,n);
		else throw std::runtime_error ("invalid number of coefficients");
	}
	
	void pwt(std::vector <double>& a, const int n, const int isign) {
		double ai,ai1;
		int i,ii,j,jf,jr,k,n1,ni,nj,nh,nmod;
	
		if (n < 4) return;
		WaveletFilter &wfilt=*wfilt_p;
		std::vector <double> wksp(n);
		nmod=wfilt.ncof*n;
		n1=n-1;
		nh=n >> 1;
		for (j=0;j<n;j++) wksp[j]=0.0;
		if (isign >= 0) {
			for (ii=0,i=0;i<n;i+=2,ii++) {
				ni=i+1+nmod+wfilt.ioff;
				nj=i+1+nmod+wfilt.joff;
				for (k=0;k<wfilt.ncof;k++) {
					jf=n1 & (ni+k+1);
					jr=n1 & (nj+k+1);
					wksp[ii] += wfilt.cc[k]*a[jf];
					wksp[ii+nh] += wfilt.cr[k]*a[jr];
				}
			}
		} else {
			for (ii=0,i=0;i<n;i+=2,ii++) {
				ai=a[ii];
				ai1=a[ii+nh];
				ni=i+1+nmod+wfilt.ioff;
				nj=i+1+nmod+wfilt.joff;
				for (k=0;k<wfilt.ncof;k++) {
					jf=n1 & (ni+k+1);
					jr=n1 & (nj+k+1);
					wksp[jf] += wfilt.cc[k]*ai;
					wksp[jr] += wfilt.cr[k]*ai1;
				}
			}
		}
		for (j=0;j<n;j++) a[j]=wksp[j];
	}
	
	void wt1 (std::vector <double>& a, const int isign,
			  void wtstep(std::vector <double>&, const int, const int)) {
		int nn;
	
		int n=a.size();
		if (n < 4) return;
		if (isign >= 0) {
			for (nn=n;nn>=4;nn>>=1) wtstep(a,nn,isign);
		} else {
			for (nn=4;nn<=n;nn<<=1) wtstep(a,nn,isign);
		}
	}
}

// EOF

