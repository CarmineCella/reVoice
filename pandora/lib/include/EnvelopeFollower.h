// EnvelopeFollower.h
//

#ifndef ENVELOPEFOLLOWER_H
#define ENVELOPEFOLLOWER_H

#include "SoundInfo.h"
#include "Tone.h"
#include <cmath>

//! Basic envelope follower with adjustable feedback.
class EnvelopeFollower : public Tone {
private:
	//! Assignment operator; it is private.
	EnvelopeFollower& operator= (EnvelopeFollower&);
	//! Copy constructor; it is private.
	EnvelopeFollower (const EnvelopeFollower&);
public:
	EnvelopeFollower (SoundInfo* info, double feedback = .999);
	virtual ~EnvelopeFollower ();
	virtual double& process (double& sig) {
		double abs = fabs (sig);
		if (abs > m_val) {
			m_val = abs;
		}
		else {
			m_val = m_val * m_feedback + abs * m_gain;
		}
		return m_val;
	}
	virtual double get () const;
    virtual void update (Subject* s);
};

#endif	// ENVELOPEFOLLOWER_H

// EOF


