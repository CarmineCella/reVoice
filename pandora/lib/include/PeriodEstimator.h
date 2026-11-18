// PeriodEstimator.h
//

#ifndef PERIODESTIMATOR_H
#define PERIODESTIMATOR_H

#include "VariState.h"
#include "LoopOscillator.h"
#include "ZeroCrossings.h"
#include "SignalTable.h"
#include "SoundInfo.h"
#include "AbstractPitchDetector.h"
#include "EnvelopeFollower.h"
#include "DCBlocker.h"
#include <vector>
#include <iostream>

#define PE_MIN_LEVELS 1
#define PE_MIN_SPLIT 2

//! Basic pitch detector based on the Fundamental Period Mesurement.
/*!
    This class detects the pitch of a SignalTable (single voice), using a bank of LP
    filters and zero crossing. The frequency range defaults to human voice.
*/
class PeriodEstimator : public AbstractPitchDetector {
private:
	PeriodEstimator& operator= (PeriodEstimator&);
	PeriodEstimator (const PeriodEstimator&);
public:
	PeriodEstimator (SoundInfo* info, double min = 77.782,
        double max = 1396.913, int levels = 6, int split = 30,
        double threshold = .03, double smoothingRate = 2000);
	virtual ~PeriodEstimator ();
    virtual void threshold (double t);
	virtual void reset ();
    virtual void split (double s);
    virtual void filterQuality (double q);
	virtual void scaleBinsTo (double m);
  	virtual void feedback (double f);
  	virtual void smoothingRate (double f);
  	virtual void smoothingQuality (double f);
	virtual double& process (double& sig) {
		double dc = m_block->process (sig);

		for (int i = 0; i < m_num; i++) {
			m_filtered[i] = dc;
			for (int k = 0; k < m_levels; k++) {
				m_filtered[i] =
					m_filters[(i * m_levels) + k]->fastProcess (m_filtered[i]);
			}
			m_followed[i] = m_envFollowers[i]->process (m_filtered[i]);
			m_periods[i] = m_zeroes[i]->process (m_filtered[i]);
		}
		double max = findMax (m_followed, m_num);
		//std::cout << "max : " << max << std::endl;
		int filt = 0;
		if (max < m_threshold) {
			m_val = -1;
		}
		else {
			double maxSplit = max / m_split;
			m_val = -1;
			while (filt < m_num) {
				if (m_followed[filt] >= maxSplit) {
					if (m_periods[filt] <= m_bins[filt] && m_periods[filt] >= m_min) {
							m_val = m_final->process (m_periods[filt]);
						break;
					}
					else if (filt < m_num - 1 && m_periods[filt] <= m_bins[filt + 1]
						&& m_periods[filt] >= m_min) {
							m_val = m_final->process (m_periods[filt]);
						break;
					}
					else {
					   break;
					}
				}
				filt++;
			}
		}

		return m_val;
	}
//	virtual double& process (double& sig) { // UNDERSAMPLED VERSION
//		sig = m_block->process (sig);
//
//		for (int i = 0; i < m_num; i++) {
//			m_filtered[i] = sig;
//			for (int k = 0; k < m_levels; k++) {
//				m_filtered[i] =
//					m_filters[(i * m_levels) + k]->fastProcess (m_filtered[i]);
//			}
//			m_followed[i] = m_envFollowers[i]->process (m_filtered[i]);
//			m_periods[i] = m_zeroes[i]->process (m_filtered[i]);
//		}
//
//		if (m_counter >= m_period) {
//			double max = findMax (m_followed, m_num);
//			int filt = 0;
//			if (max < m_threshold) {
//				m_val = -1;
//			}
//			else {
//				double maxSplit = max / m_split;
//				m_val = -1;
//				while (filt < m_num) {
//					if (m_followed[filt] >= maxSplit) {
//						if (m_periods[filt] != -1 && m_periods[filt] <= m_bins[filt]) {
//							if (m_periods[filt] >= m_min) {
//								m_val = m_final->process (m_periods[filt]);
//								m_period = (long) (m_sr / m_val);
//								m_counter = 0;
//								//TRACE (m_period)
//							}
//							break;
//						}
//						else if (m_periods[filt] != -1 && filt < m_num - 1 &&
//							m_periods[filt] <= m_bins[filt + 1]) {
//							if (m_periods[filt] >= m_min) {
//								m_val = m_final->process (m_periods[filt]);
//								m_period = (long) (m_sr / m_val);
//								m_counter = 0;
//								//TRACE (m_period)
//							}
//							break;
//						}
//						else {
//						   break;
//						}
//					}
//					filt++;
//				}
//			}
//		}
//		m_counter++;
//		return m_val;
//	}
	virtual double get () const { return m_val; }
    virtual double min () const;
    virtual double max () const ;
	virtual void update (Subject* s);

private:
	double findMax (double* array, int len) {
		double max = 0;
		for (int i = 0; i < len; i++) {
			if (max < array[i]) {
				max = array[i];
			}
		}
		return max;
	}

    double m_min, m_max;
    int m_levels;
    double m_split;
    double m_threshold;
    long m_num;
    VariState** m_filters;
    ZeroCrossings** m_zeroes;
    EnvelopeFollower** m_envFollowers;
    VariState* m_final;
    DCBlocker* m_block;
    //VariState* m_block;
    double* m_followed;
    double* m_filtered;
    double* m_periods;
    std::vector<double> m_bins;
    long m_counter, m_period;
    double m_sr;
};

#endif	// PERIODESTIMATOR_H

// EOF


