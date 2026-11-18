// CorrelationEstimator.h
//

#ifndef CORRELATIONESTIMATOR_H
#define CORRELATIONESTIMATOR_H

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

#define CE_MIN_LEVELS 1

//! Basic pitch detector based on autocorrelation
class CorrelationEstimator : public AbstractPitchDetector {
private:
    CorrelationEstimator& operator= (CorrelationEstimator&);
    CorrelationEstimator (const CorrelationEstimator&);
public:
    CorrelationEstimator (SoundInfo* info, int size = 1024, double min = 77.782,
                          double max = 1396.913, int levels = 6,
                          double threshold = 0.007, double smoothingRate = 2500);
    virtual ~CorrelationEstimator ();
    virtual void threshold (double t);
    virtual void reset ();
    void filterQuality (double q);
    virtual void range (double min, double max);
    virtual void smoothingRate (double f);
    virtual void smoothingQuality (double f);
    double& process (double& sig) {
        m_data[m_index] = (double) sig;
        ++m_index;

        //double t = -1;
        if (m_index == m_size) {
            m_index = 0;
            float period = autocorr <double> (m_size, m_data, m_back);
            float power = std::sqrt (m_back[0] / m_size);
			float pitch = 0;

            if (period > 0) {
            	pitch = m_info->samplingRate () / period;
            }

            //std::cout << period << ", " << power << ", " << pitch << std::endl;
			if (power >= m_threshold) {
				if (pitch > 0) {
					m_val = pitch;
//					for (int k = 0; k < m_levels; k++) {
//						m_filters[k]->cutoff (pitch);
//					}
					//std::cout << "pitch: " << m_val << std::endl;
				}
			}
        }

//	   	double filtered = m_block->process (sig);
//		for (int k = 0; k < m_levels; k++) {
//			filtered = m_filters[k]->fastProcess (filtered);
//		}
//        double t = m_zero->process (filtered);
        if (m_val >= m_min && m_val <= m_max) {
            m_val = m_final->fastProcess (m_val);
        } else {
            m_val = -1;
        }

        return m_val;
    }
    double& fastBlockProcess (const float* buffer, int size) {
		float period = autocorr <float> (size, buffer, m_back);
		float power = std::sqrt (m_back[0] / size);
		double pitch = 0;

		if (period > 0) {
			pitch = m_info->samplingRate () / period;
		}

		m_val = -1.;
		if (power >= m_threshold) {
			if (pitch > 0) {
				m_val = pitch;
			}
		}

        return m_val;
    }
    virtual double get () const;
    virtual double min () const;
    virtual double max () const ;
    virtual void update (Subject* s);

private:
	template <typename T>
    float autocorr (long size, const T* data, float *result) {
        long i, j, k;
        double temp, norm;

        long size2 = size / 2;
        long size4 = size / 4;

        for (i = 0; i < size2; i++) {
            result[i] = 0.0;
            for (j = 0; j < size - i - 1; j++)	{
                result[i] += data[i+j] * data[j];
            }
        }
        temp = result[0];
        j = (long) size * 0.02;
        while (result[j] < temp && j < size2)	{
            temp = result[j];
            j += 1;
        }
        temp = 0.0;
        for (i = j; i < size2; i++) {
            if (result[i]>temp) {
                j = i;
                temp = result[i];
            }
        }
        norm = 1.0 / size;
        k = size2;
        for (i = 0; i < size2; i++) {
            result[i] *=  (k - i) * norm;
        }
        if (result[j] == 0) j = 0;
        else if ((result[j] / result[0]) < 0.4) j = 0;
        else if (j > size4) j = 0;
        return (float) j;
    }
    int m_size;

    int m_index;
    double m_min, m_max;
    int m_levels;
    double m_threshold;
    double* m_data;
    float* m_back;
//   VariState* m_lp;
    VariState* m_final;
    ZeroCrossings* m_zero;
    VariState** m_filters;
    DCBlocker* m_block;
};

#endif	// CORRELATIONESTIMATOR_H

// EOF


