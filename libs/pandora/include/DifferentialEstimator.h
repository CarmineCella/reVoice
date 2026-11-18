// DifferentialEstimator.h
//

#ifndef DIFFERENTIALESTIMATOR_H
#define DIFFERENTIALESTIMATOR_H

#include "VariState.h"
#include "SoundInfo.h"
#include "AbstractPitchDetector.h"
#include "EnvelopeFollower.h"
#include "ZeroCrossings.h"
#include "DCBlocker.h"

#include <stdexcept>
#include <iostream>

#define DE_MIN_LEVELS 1

//! Basic pitch detector based on derivatives
class DifferentialEstimator : public AbstractPitchDetector {
private:
    DifferentialEstimator& operator= (DifferentialEstimator&);
    DifferentialEstimator (const DifferentialEstimator&);
public:
    DifferentialEstimator (SoundInfo* info, int size = 2048, double min = 77.782,
                          double max = 1396.913, int levels = 4,
                          double threshold = 0.3, double ampThreshold = 0.01,
                          double smoothingRate = 2500);
    virtual ~DifferentialEstimator ();
    virtual void threshold (double t);
    virtual void ampThreshold (double t);
    virtual void range (double min, double max);
    virtual void smoothingRate (double f);
    virtual void smoothingQuality (double f);
    double& process (double& sig) {
        double follow = m_follow->process (sig);
		double filtered = m_block->process (sig);
		for (int k = 0; k < m_levels; k++) {
			filtered = m_filters[k]->fastProcess (filtered);
		}

    	m_data[m_index] = (double) filtered;
        ++m_index;

        if (m_index == m_size) {
            m_index = 0;
            int s2 = m_size / 2;
            double prevDiff = 0;
			double prevDx = 0;
			double maxDiff = 0;

			int sampleLen = 0;

			for (int i = 0; i < s2; i++) {
				double diff = 0;
				for (int j = 0; j < s2; j++) {
					diff += fabs (m_data[j] - m_data[i + j]);
				}

				double dx = prevDiff - diff;

				// change of sign in dx
				if (dx < 0 && prevDx > 0) {
					if (diff < (m_threshold * maxDiff)) {
						if (sampleLen == 0) {
							sampleLen = i - 1;
						}
					}
				}

				prevDx = dx;
				prevDiff = diff;
				maxDiff = std::max (diff, maxDiff);

//				std::cout << prevDx << ", " << prevDiff << ", " << maxDiff << std::endl;
			}

			if (sampleLen > 0 ) {
				double x = (m_info->samplingRate () / sampleLen);

				while (x < m_min) {
					x *= 2;
				}
				while (x > m_max) {
					x *= .5;
				}
				std::cout << "\tx = " << x << std::endl;
				for (int k = 0; k < m_levels; k++) {
					m_filters[k]->cutoff (x); // * 1.4142);
				}
			}
        }

		double t = m_zero->process (filtered);
		if (t > m_min && t < m_max && follow >= m_threshold) {
			m_val = m_final->fastProcess (t);
			std::cout << m_val << std::endl;
		}
		else {
			m_val = -1;
		}
		//std::cout << follow << ", " << filtered << ", " << t << "; " << m_val << std::endl;
//        if (m_val >= m_min && m_val <= m_max) {
//            m_val = m_final->fastProcess (m_val);
//        } else {
//            m_val = -1;
//        }

        return m_val;
    }
    virtual float fastBlockProcess (const float* buffer, int size) {
        int s2 = size / 2;
        double prevDiff = 0;
		double prevDx = 0;
		double maxDiff = 0;
		double fol = 0;

		int sampleLen = 0;

		for (int i = 0; i < s2; i++) {
			double diff = 0;
			for (int j = 0; j < s2; j++) {
				diff += fabs (buffer[j] - buffer[i + j]);
			}

			double dx = prevDiff - diff;

			// change of sign in dx
			if (dx < 0 && prevDx > 0) {
				if (diff < (m_threshold * maxDiff)) {
					if (sampleLen == 0) {
						sampleLen = i - 1;
					}
				}
			}

			prevDx = dx;
			prevDiff = diff;
			maxDiff = std::max (diff, maxDiff);
			double v = (double) buffer[i];
			fol = m_follow->process (v);
		}

		if (sampleLen > 0 && fol > m_ampThreshold) {
			m_val = (m_info->samplingRate () / sampleLen);
//			while (t < m_min) {
//				m_val *= 2;
//			}
//			while (t > m_max) {
//				m_val *= .5;
//			}
		}

		if (m_val >= m_min && m_val <= m_max) {
            m_val = m_final->fastProcess (m_val);
        } else {
            m_val = -1;
        }
		return m_val;
    }
    virtual double get () const;
    virtual double min () const;
    virtual double max () const ;
    void reset ();
    virtual void update (Subject* s);

private:
    int m_size;
    int m_index;
    int m_levels;

    double m_min, m_max;
    double m_threshold;
    double m_ampThreshold;

    double* m_data;
    VariState* m_final;
    EnvelopeFollower* m_follow;
    ZeroCrossings* m_zero;
	VariState** m_filters;
	DCBlocker* m_block;

};

#endif	// DIFFERENTIALESTIMATOR_H

// EOF


