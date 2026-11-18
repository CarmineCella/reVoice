// Renoise.h
// created: Mon Mar 26 16:17:06 2007
// 

#include "SoundEffect.h"
#include "VariState.h"
#include "RandomGenerators.h"
#include <cmath>

#ifndef RENOISE_H
#define RENOISE_H 

//! Noise modulation effect.
/*!
	Adds to the signal a band-limited noise to create
	dynamic effects.
*/
class Renoise : public SoundEffect {
private:
	Renoise& operator= (Renoise&);
	Renoise (const Renoise&);
public: 
	Renoise (SoundInfo* info, double band, double gain);
	virtual ~Renoise ();
	void band (double b);
	void gain (double g);
	virtual double& process (double& sig) {
		double r = m_rand->tick ();
		r = m_var->process (r);
		if (fabs (sig + (r * sig * m_gain)) > 1) {
			m_val = sig;
		}
		else {
			m_val = sig + (r * sig * m_gain);
		}
		return m_val;
	}
	void update (Subject* s) {}	
private:
	PRNG_31bit m_rng;
	VariState* m_var;
	RangedRandomNumberGenerator* m_rand;
	double m_gain;
};

#endif	// RENOISE_H 

// EOF

