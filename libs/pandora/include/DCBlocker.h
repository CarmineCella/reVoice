// DCBlocker.h
// 

#ifndef DCBLOCKER_H
#define DCBLOCKER_H 

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
#ifndef TWOPI
#define TWOPI (6.28318530717958647692)
#endif

#include "SoundInfo.h"
#include "AbstractFilter.h"

//! Basic DC blocker.
class DCBlocker : public AbstractFilter {
private:
	DCBlocker& operator= (DCBlocker&);
	DCBlocker (const DCBlocker&);
public: 
	DCBlocker (SoundInfo* info, double gain = .99);
	virtual ~DCBlocker ();
	virtual void cutoff (double c);
	virtual void gain (double g);
	virtual void reset ();
	virtual double& process (double& sig) {
		m_val = sig - m_inputs + (m_gain * m_val);
		m_inputs = sig;
		return m_val;
	}
	virtual void update (Subject* s);
	
private:
	double m_gain;
	double m_cutoff;
	double m_inputs;
};

#endif	// DCBLOCKER_H 

// EOF

