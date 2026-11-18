// SpectralEstimator.h
//

#ifndef SPECTRALESTIMATOR_H
#define SPECTRALESTIMATOR_H

#include "VariState.h"
#include "Envelope.h"
#include "SoundInfo.h"
#include "fourier.h"
#include "AbstractPitchDetector.h"
#include "ZeroCrossings.h"
#include "DCBlocker.h"

//#include <vector>
#include <stdexcept>
#include <iostream>

#define SE_MIN_LEVELS 1

//! Basic pitch detector based on the Fast Fourier Transform.
class SpectralEstimator : public AbstractPitchDetector {
private:
	SpectralEstimator& operator= (SpectralEstimator&);
	SpectralEstimator (const SpectralEstimator&);
public:
	SpectralEstimator (SoundInfo* info, int size = 128, double min = 77.782,
        double max = 1396.913, int levels = 4, double split = 64,
        double bias = 1, double threshold = 0.03, double smoothingRate = 2000);
	virtual ~SpectralEstimator ();
    virtual void threshold (double t);
	virtual void reset ();
	void filterQuality (double q);
	virtual void range (double min, double max);
  	virtual void smoothingRate (double f);
  	virtual void smoothingQuality (double f);
  	virtual void split (double s);
	double& process (double& sig) {
		// Window before the FFT: leakage from a fractional-bin fundamental must
		// not be mistaken for a lower-frequency partial.
		m_data[m_index] = sig * (0.5 - 0.5 * std::cos (TWOPI * m_index / m_size));
		++m_index;

		if (m_index == m_size) {
			m_index = 0;
			fourier::realfft (m_data - 1, m_size, 1);
			fourier::rectToPolar (m_data, m_size / 2);

			const double peak = findSpectralPeak ();
			m_peak = peak * 4.0 / m_size; // amplitude scale for a Hann window
			//std::cout << "peak: " << m_peak << std::endl;

			if (m_peak >= m_threshold) {
				int fundPos = findFundamentalPos (peak / m_split);
				for (int k = 0; k < m_levels; k++) {
					m_filters[k]->cutoff (m_bin * fundPos); // * 1.4142);
				}
			}

		}

		double filtered = m_block->process (sig);
		for (int k = 0; k < m_levels; k++) {
			filtered = m_filters[k]->fastProcess (filtered);
		}
		double t = m_zero->process (filtered);
		if (t > MIN_FREQ_LIMIT && m_peak >= m_threshold) {
			m_val = m_final->fastProcess (t);
		}
		else {
			if (m_peak < m_threshold) m_val = -1;
		}

		return m_val;
	}

    virtual double get () const;
    virtual double min () const;
    virtual double max () const ;
	virtual void update (Subject* s);

private:
//	double hamming (double in, long pos, long samples) {
//		return (-.46 * cos (2. * M_PI * (double) pos / (double) samples) + .54) * in;
//	}

	double findSpectralPeak (/*int& position*/) {
		double max = 0;
//		position = m_minPos;
		for (int l = m_minPos; l < m_maxPos; l += 2) {
			double v = std::sqrt (m_data[l] * m_data[l]);
			if (v >= max) {
				max = v;
//				position = l;
			}
		}

		return max;
	}

//	int findSpectralPeakPos () {
//		float max = m_data[m_minPos];
//		int pos = m_minPos;
//		for (int l = m_minPos; l < m_maxPos; l += 2) {
//			if (m_data[l] >= max) {
//				max = m_data[l];
//				pos = l;
//			}
//		}
//
//		return pos;
//	}

	int findFundamentalPos (float peak) {
		int pos = m_minPos;
		for (int l = m_minPos; l < m_maxPos; l += 2) {
			double v = std::sqrt (m_data[l] * m_data[l]);
			const double previous = l >= 2 ? m_data[l - 2] : 0;
			const double next = l + 2 < m_size ? m_data[l + 2] : 0;
			if (v >= peak && v >= previous && v >= next) {
				pos = l;
				break;
			}
		}

		return pos;
	}
	int m_size;
	int m_size2;
	int m_index;
    double m_min, m_max;
    int m_levels;
    int m_minPos, m_maxPos;
    int m_totBins;
    double m_bin;
    double m_split;
    double m_bias;
    double m_threshold;
    double m_peak;
    double* m_data;
 //   VariState* m_lp;
    VariState* m_final;
    ZeroCrossings* m_zero;
	VariState** m_filters;
	DCBlocker* m_block;
};

#endif	// SPECTRALESTIMATOR_H

// EOF


