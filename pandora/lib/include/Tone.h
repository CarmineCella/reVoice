// Tone.h
//

#ifndef TONE_H
#define TONE_H
                 
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
#ifndef TWOPI
#define TWOPI (6.28318530717958647692)
#endif

#include "SoundInfo.h"
#include "AbstractFilter.h"
#include <cmath>

//! Simple 1st order LP filter (IIR, 6dB/oct)
class Tone : public AbstractFilter {
private:
	Tone& operator= (Tone&);
	Tone (const Tone&);
public:
	Tone (SoundInfo* info, double feedback = .999);
	virtual ~Tone ();
	virtual void cutoff (double c);
	virtual void feedback (double f);
	void reset ();
	virtual double& process (double& sig);
	virtual void update (Subject* s);

protected:
	double m_cutoff;
	double m_feedback;
	double m_gain;
};

#endif	// TONE_H

// EOF


