// AdaptiveEstimator.h
//

#ifndef ADAPTIVEESTIMATOR_H
#define ADAPTIVEESTIMATOR_H

#define AT_MIN_LEVELS 1

#include "SoundInfo.h"
#include "VariState.h"
#include "EnvelopeFollower.h"
#include "AbstractPitchDetector.h"
#include "ZeroCrossings.h"
#include "DCBlocker.h"
#include <cmath>

#include <iostream>

//! Pitch detector based on adaptive tuning.
/*!
	This class uses a center filter in combination
	with a lowpass filter bank. The center frequency
	of the center filter is 'tuned' in realtime till it
	reaches the input signal frequency.
*/
class AdaptiveEstimator : public AbstractPitchDetector {
private:
	AdaptiveEstimator& operator= (AdaptiveEstimator&);
	AdaptiveEstimator (const AdaptiveEstimator&);
public:
	AdaptiveEstimator (SoundInfo* info, double min = 77.782,
        double max = 1396.913, int levels = 2, double bias = 0.75,
        double threshold = .09,
        double smoothingRate = 2000); //, double speed = 2); // speed not used!
	virtual ~AdaptiveEstimator ();
    virtual void threshold (double t);
    virtual void reset ();
    //virtual void speed (double s);
    virtual void filterQuality (double q);
	virtual void range (double min, double max);
  	virtual void smoothingRate (double f);
  	virtual void smoothingQuality (double f);

	double& process (double& sig) {
	   	double filtered = m_block->process (sig);
		for (int k = 0; k < m_levels; k++) {
			filtered = m_lps[k]->fastProcess (filtered);
		}
		double t = m_zero->process (filtered);
		double max = m_follow->process (sig);
		//std::cout << "max: " << max << std::endl;
		if (t > m_min && t < m_max &&
			 max >= m_threshold) {
			//m_bp->cutoff (m_center);
			m_val = m_final->fastProcess (t);
			for (int i = 0; i < m_levels; i++) {
				m_lps[i]->cutoff (t * m_bias);
			}
		}
		else {
			m_val = -1;
		}

	//	if (m_follow->process (sig) > m_threshold) {	 4
	//		double bandpassed = m_bp->process (filtered);

	//		double phi = m_oldIn * bandpassed - m_oldOut * filtered;
	//		double sign = -1;
	//		if (phi >= 0) {
	//			sign = 1;
	//		}

	//		double deltaFreq = m_speed * fabs (filtered - bandpassed) * sign;
	//		m_center += deltaFreq;

	//		if (m_center > m_max || m_center < m_min) {
	//			m_center = m_middle;
	//		}

	//		else {
	//			m_val = -1;
	//		}

//			m_oldIn = filtered;
//			m_oldOut = bandpassed;
//		}
		return m_val;
	}
	virtual double get () const;
    virtual double min () const;
    virtual double max () const;
	virtual void update (Subject* s);

private:
//    VariState* m_bp;
    VariState** m_lps;
    VariState* m_final;
    EnvelopeFollower* m_follow;
    ZeroCrossings* m_zero;
    DCBlocker* m_block;
    double m_min, m_max; //, m_middle;
    int m_levels;
    double m_bias;
    double m_threshold;
//    double m_speed;
//    double m_oldIn;
//    double m_oldOut;
//    double m_center;

	double m_old;
};

#endif	// ADAPTIVEESTIMATOR_H

// EOF


