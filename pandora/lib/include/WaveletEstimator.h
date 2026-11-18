// WaveletEstimator.h
// 

#ifndef WAVELETESTIMATOR_H
#define WAVELETESTIMATOR_H

#include "VariState.h"
#include "Envelope.h"
#include "SoundInfo.h"
#include "fourier.h"
#include "AbstractPitchDetector.h"
#include "ZeroCrossings.h"
#include "DCBlocker.h"
#include "EnvelopeFollower.h"
#include "wavelet.h"

//#include <vector>
#include <stdexcept>
//#include <fstream>
#include <iostream>

#define WE_MIN_LEVELS 1

//! Pitch detector based on the discrete wavelet transform (DWT).
/*! 
	This class uses a DWT transformation in combination
	with a lowpass filter bank.
*/
class WaveletEstimator : public AbstractPitchDetector {    
private:
	WaveletEstimator& operator= (WaveletEstimator&);
	WaveletEstimator (const WaveletEstimator&);
public: 
	WaveletEstimator (SoundInfo* info, int size = 2048, double min = 77.782, 
        double max = 1396.913, int levels = 3, int coefficients = 20,
        double bias = 1, double threshold = 0.03, double smoothingRate = 2000);
	virtual ~WaveletEstimator ();
    virtual void threshold (double t);
	virtual void reset ();
	void filterQuality (double q);
	virtual void range (double min, double max);
  	virtual void smoothingRate (double f);
  	virtual void smoothingQuality (double f);
	double& process (double& sig) {
		m_peak = m_follow->process (sig);
		//std::cout << "peak    : " << m_peak << std::endl;
					
		m_data[m_index] = hamming (sig, m_index, m_size);
		++m_index;		
			
		double filtered = m_block->process (sig);
		for (int k = 0; k < m_levels; k++) {
			filtered = m_filters[k]->fastProcess (filtered);
		}		
		
		if (m_index == m_size) {	
			m_index = 0;
			wavelet::wt1 (m_data, 1, wavelet::pwt);
			waveletSpectrum ();
//			m_peak = findSpectralPeak () / m_totBand;

			if (m_peak >= m_threshold) {
				double c = linearCentroid () / m_totBand;
				//std::cout << "centroid: " << c << std::endl;
				for (int k = 0; k < m_levels; k++) {
					m_filters[k]->cutoff (c * m_bias);
				}
			}
		}
	
		double t = m_zero->process (filtered);
		if (t > MIN_FREQ_LIMIT && m_peak >= m_threshold) {
			m_val = m_final->fastProcess (t);
		}
		else {
			m_val = -1;
		}
			
		return m_val;
	}  	

    virtual double get () const;
    virtual double min () const;
    virtual double max () const ;
	virtual void update (Subject* s);

private:	
	double hamming (double in, long pos, long samples) {
		return (-.46 * cos (2. * M_PI * (double) pos / (double) samples) + .54) * in;
	}
	
	double findSpectralPeak () {
		double max = m_bands[m_minBand];
		for (int l = m_minBand; l < m_maxBand; ++l) {
			if (m_bands[l] >= max) {
				max = m_bands[l];
			}
		}
		
		return max;
	}	
	void waveletSpectrum () {
	    int endp = 1;
	    int start = 1;
	    int exp = 0;
	    while (endp <= m_size) {
	        double power = 0;
	        for (int i = start; i < endp; ++i) {
	        	power += (m_data[i] * m_data[i]);
	        }

	        if (0 < power) m_bands[exp] = std::sqrt (power);
	        else m_bands[exp] = 0;
	        
	        start = endp;
	        endp *= 2;
	        ++exp;    
	    }
	}
	double linearCentroid () {
	    double sumWeigth = 0;
	    double sumWeigthDistance = 0;
	    double distance = 0;
	    double weigth = 0;
	    double weigthDistance = 0;
	    
	    for (int index = m_minBand; index < m_maxBand; ++index) {
	        distance = m_bins[index];
	        weigth = m_bands[index];
	        weigthDistance = weigth * distance;
	        sumWeigth += weigth;
	        sumWeigthDistance += weigthDistance;
	    }		
	    
	    return (sumWeigth != 0 ? sumWeigthDistance / sumWeigth : 0);
	}		
	int m_size;
	int m_exponent;
	int m_index;
    double m_min, m_max;
	int m_minBand;
	int m_maxBand;    
	int m_totBand;
    int m_levels;
	int m_coefficients;
    double m_bias;
    double m_threshold;
    double m_peak;
    std::vector <double> m_data; 
    double* m_bands;
    double* m_bins;
 //   VariState* m_lp;
    VariState* m_final;
    EnvelopeFollower* m_follow;
    ZeroCrossings* m_zero;
	VariState** m_filters;
	DCBlocker* m_block;
};

#endif	// WAVELETESTIMATOR_H 

// EOF


