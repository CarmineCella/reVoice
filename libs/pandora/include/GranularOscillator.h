// GranularOscillator.h
// 

#ifndef GRANULAROSCILLATOR_H
#define GRANULAROSCILLATOR_H 

#include "Envelope.h"
#include "SignalTable.h"
#include "windowType.h"
#include "AbstractOscillator.h"
#include "LoopOscillator.h"

//! Oscillator for single-grain generation.
/*!
	Actually this class HAS a LoopOscillator object
	and an Envelope object. It generates a single grain for each event.
*/
class GranularOscillator : public AbstractOscillator {
private:
	//! Assignment operator; it is private.
	GranularOscillator& operator= (GranularOscillator&);
	//! Copy constructor; it is private.
	GranularOscillator (const GranularOscillator&);
public: 
	//! Class constructor.
	GranularOscillator (SoundInfo* info, SignalTable *tab, double amp = 1, 
        double initFreq = 440, double finalFreq = 440, double curve = 0,
        double grainDur = 1, double phi = 0);
	//! Class destructor.
	virtual ~GranularOscillator ();
	//! Sets the amplitude for the oscillator.
	virtual void amplitude (double amp) {
		m_osc->amplitude (amp);
	}
	//! Sets the frequency for the oscillator (non-glisson).
	virtual void frequency (double initFreq) {
		m_centerFreq = initFreq;
		m_deltaFreq = 0;
		m_osc->frequency (m_centerFreq);
	}
	//! Sets the frequency for the oscillator.
	virtual void frequency (double initFreq, double finalFreq) {
		m_centerFreq = initFreq;
		m_deltaFreq = (finalFreq - initFreq);
		m_osc->frequency (m_centerFreq);
	}
	//! Sets the phase for the oscillator.
	virtual void phase (double phi) {
		m_osc->phase (phi);
	}
	//! Gets the phase for the oscillator.
	virtual double phase () const;		
	//! Sets a new table for the oscillator.
	virtual void table (SignalTable* tab);
	//! Sets the duration of the grain.
	virtual void grainDuration (double dur, double attack = .5, 
		double decay = .5) {
		if (dur > 0) {
			m_env->mapTo (dur, attack, decay); 
			m_glissGen->mapTo (dur);
		}    
	}
    virtual void grainType (Pandora::windowType w, double shift = 0);
    virtual void curve (double c);
    virtual void grainDirection (int sign);
	//! Sound generation function. It produces a single grain.
	virtual double& tick () {
		double gliss = m_glissGen->tick () * m_deltaFreq;
		m_osc->frequency (m_grainDirection * (m_centerFreq + gliss));
		m_val = m_osc->tick () * m_env->tick ();
		return m_val;
	}
	//! Fast generation function for non-glisson grain.	
	virtual double& fastTick () {
		m_val = m_osc->tick () * m_env->tick ();
		return m_val;		
	}
	//! Gets the mute flag (override SoundObject).
	virtual bool mute () const {
		return m_env->mute ();
	}
	//! Sets the mute flag (override SoundObject).	
	virtual void mute (bool m) {
		m_env->mute (m);
		m_osc->mute (m);
		m_glissGen->mute (m);
	}
	virtual void update (Subject* s);
private:
    double m_centerFreq, m_deltaFreq;
    int m_grainDirection;
	LoopOscillator* m_osc;
	Envelope* m_env;
	Envelope* m_glissGen;
};

#endif	// GRANULAROSCILLATOR_H 

// EOF

