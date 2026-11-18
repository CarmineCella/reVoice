// FMOscillator.h
// 

#ifndef FMOSCILLATOR_H
#define FMOSCILLATOR_H 

#include "AbstractOscillator.h"
#include "SoundInfo.h"
#include "LoopOscillator.h"
#include "SignalTable.h"

//! Oscillator for frequency modulation.
/*!
	Actually this class it HAS two LoopOscillator objects
	to use in FM synthesis.
*/
class FMOscillator : public AbstractOscillator {
private:
	//! Assignment operator; it is private.
	FMOscillator& operator= (FMOscillator&);
	//! Copy constructor; it is private.
	FMOscillator (const FMOscillator&);
public: 
	//! Class constructor.
	FMOscillator (SoundInfo* info, SignalTable *tab, double amp = 1, double carr = 440, 
		 double ind = 3, double mod = 40, double phi = 0);
	//! Class destructor.
	virtual ~FMOscillator ();
	//! Sets the amplitude for the oscillator.
	void amplitude (double amp);
	//! Sets the carrier frequency.
	void frequency (double carr);
	//! Sets the modulation index.
	void index (double ind);
	//! Sets the modulator frequency.
	void modulator (double mod);
	//! Sets the phase for the oscillator.
	void phase (double phi);
	//! Gets the phase for the oscillator.
	double phase () const;
	//! Sets a new table for the oscillator.
	void table (SignalTable* tab);
	//! Sound generation function.
	virtual double& tick ();
	virtual void update (Subject* s);
private:
	LoopOscillator* m_osc1;
	LoopOscillator* m_osc2;
	double m_carr, m_mod, m_ind, m_phi;
};

#endif	// FMOSCILLATOR_H 

// EOF

// FIXME: Forse non funziona correttamente!

