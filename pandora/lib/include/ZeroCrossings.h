// ZeroCrossings.h
// 

#ifndef ZEROCROSSINGS_H
#define ZEROCROSSINGS_H 

#include "SoundInfo.h"
#include "Tone.h"

//! Zero crossings counter class.
class ZeroCrossings : public Tone {
private:
	ZeroCrossings& operator= (ZeroCrossings&);
	ZeroCrossings (const ZeroCrossings&);
public: 
	ZeroCrossings (SoundInfo* info, double feedback = .8);
	virtual ~ZeroCrossings ();
	virtual void reset (double z = 0);
	virtual double& process (double& sig) {
		m_val = -1;
		if (sig >= 0.f) {
			m_posit++;
	
			if (m_negat != 0) {
				m_z = m_z * m_feedback + (double) (m_posit + m_negat) * m_gain;
				m_val = 1 / (m_z * m_rate);
				//m_val = (1 / ((double) (m_posit + m_negat) * m_rate));
				m_posit = 0;
				m_negat = 0;
			}
		}
	
		if (sig < 0.f) {
			m_negat++;
		}
		
		return m_val;
	}
//	virtual double& process (double& sig) {
//		//m_val = -1;
//		//m_z = m_z * m_feedback + (double) (m_posit + m_negat) * m_gain;
//		if (sig >= 0.f) {
//			m_posit++;
//	
//			if (m_negat != 0) {
//				//m_val = 1 / (m_z * m_rate);
//				m_val = (1 / ((double) (m_posit + m_negat) * m_rate));
//				m_posit = 0;
//				m_negat = 0;
//			}
//		}
//	
//		if (sig < 0.f) {
//			m_negat++;
//		}
//		
//		return m_val;
//	}
	
	//! This is the implementation of the update function of the Observer.
	virtual void update (Subject* s);
private:
	long m_posit;
	long m_negat;
	double m_rate;
	double m_z;
};

#endif	// ZEROCROSSINGS_H 

// EOF

