// Envelope.h
// 

#ifndef ENVELOPE_H
#define ENVELOPE_H 

#include "SoundInfo.h"
#include "SoundObject.h"
#include "LoopOscillator.h"
#include "SignalTable.h"

//! Base class for the envelopes.
/*!
	It is a mixed class which inherits from SignalTable and from SoundUnit, ie. it' s a
	table with the information about the sampling rate. 
*/
class Envelope : public SignalTable, public SoundUnit {
private:
	//! Assignment operator; it is private.
	Envelope& operator= (Envelope&);
	//! Copy constructor; it is private.
	Envelope (const Envelope&);
public: 
	//! Class constructor. It needs both a pointer to a SoundInfo object and the length for the table.
	Envelope (SoundInfo* info, long len = TABLEN);
	//! Class destructor.
	virtual ~Envelope ();
	//! Rescales the duration of the table to the parameter 'dur'.
	/*!
		In this way you can map an envelope to any duration.
	*/
	void mapTo (double dur, double attack = .5, double decay = .5) {
		if (dur <= 0) {
			dur = 1;
		}
		m_dur = dur;
		long samples = (long) (m_sr * m_dur);
	
		if (attack > .5 || attack <= 0) {
			attack = .5;
		}
		m_attack = attack;
		
		if (decay > .5 || decay <= 0) {
			decay = .5;
		}
		m_decay = decay;  
		
		m_attackFreq = .5 / ((double) samples / m_sr * attack); // half table
		m_attackSamples = (long) ((double) samples * m_attack);
		   
		m_decayFreq = .5 / ((double) samples / m_sr * m_decay); // half table
		long decaySamples = (long) ((double) samples * m_decay);
		
		m_susSamples = samples - decaySamples;
		m_osc->frequency (m_attackFreq);	
	}
	//! Shift table data by the specified amount.
	void shift (double amount = 0);
	//! The usual tick () function, common to all the SoundObject<T> classes.
	/*!
		In this case the output is the element of the table with the frequency
		based on the dur - attack - decay parameters.
	*/
	virtual double& tick () {
		if (m_n == m_attackSamples) {
			m_osc->frequency (0);
		}
		if (m_n == m_susSamples) {
			m_osc->frequency (m_decayFreq);    
		}
		m_val = m_osc->tick ();
		m_n++;
		return m_val;
	}
#if 0
	//! Fast implementation (not interpolated).	
	virtual double& fastTick () {
		if (m_n == m_attackSamples) {
			m_osc->frequency (0);
		}
		if (m_n == m_susSamples) {
			m_osc->frequency (m_decayFreq);    
		}
		m_val = m_osc->fastTick ();
		m_n++;
		return m_val;
	}
#endif	
	//! Gets the mute flag (override SoundObject).
	virtual bool mute () const {
		return m_osc->mute ();
	}
	//! Sets the mute flag (override SoundObject).	
	virtual void mute (bool m) {
		m_n = 0;                          // counter cleared
		m_osc->frequency (m_attackFreq);  // new freq
		m_osc->phase (0);             // phase cleared
		m_osc->mute (m);
	}	
	void update (Subject* s);

private:
	LoopOscillator* m_osc;
	double m_sr;
	double m_dur, m_attack, m_decay;
	double m_attackFreq, m_decayFreq;
	long m_attackSamples, m_susSamples;
	long m_n;
};

#endif	// ENVELOPE_H 

// EOF

